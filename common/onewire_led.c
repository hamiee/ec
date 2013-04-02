/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Power LED control for Chrome EC */

#include "charge_state.h"
#include "console.h"
#include "hooks.h"
#include "onewire.h"
#include "timer.h"
#include "util.h"

#define POWERLED_RETRIES 10

enum powerled_color {
	POWERLED_OFF = 0,
	POWERLED_RED,
	POWERLED_YELLOW,
	POWERLED_GREEN,
	POWERLED_COLOR_COUNT  /* Number of colors, not a color itself */
};

static const uint8_t led_masks[POWERLED_COLOR_COUNT] = {0xff, 0xfe, 0xfc, 0xfd};
static const char * const color_names[POWERLED_COLOR_COUNT] = {
	"off", "red", "yellow", "green"};

/**
 * Set the power LED GPIO controller outputs
 *
 * @param mask		Mask of outputs to enable
 *
 * @return EC_SUCCESS, or non-zero if error.
 */
static int powerled_set_mask(int mask)
{
	int rv;

	/* Reset the 1-wire bus */
	rv = onewire_reset();
	if (rv)
		return rv;

	/* Skip ROM, since only one device */
	onewire_write(0xcc);

	/* Write and turn on the LEDs */
	onewire_write(0x5a);
	onewire_write(mask);
	onewire_write(~mask);  /* Repeat inverted */

	rv = onewire_read();   /* Confirmation byte */
	if (rv != 0xaa)
		return EC_ERROR_UNKNOWN;

	/* The next byte is a read-back of the chip status.  Since we're only
	 * using lines as outputs, we can ignore it. */
	return EC_SUCCESS;
}

static int powerled_set(enum powerled_color color)
{
	int rv = EC_SUCCESS;
	int i;

	/*
	 * 1-wire communication can fail for timing reasons in the current
	 * system.  We have a limited timing window to send/receive bits, but
	 * we can't disable interrupts for the rest of the system to guarantee
	 * we hit that window.  Instead, simply retry the low-level command a
	 * few times.
	 */
	for (i = 0; i < POWERLED_RETRIES; i++) {
		rv = powerled_set_mask(led_masks[color]);
		if (rv == EC_SUCCESS)
			break;

		/*
		 * Sleep for a bit between tries.  This gives the 1-wire GPIO
		 * chip time to recover from the failed attempt, and allows
		 * lower-priority tasks a chance to run.
		 */
		usleep(100);
	}

	return rv;
}

/*****************************************************************************/
/* Hooks */

static void powerled_tick(void)
{
	static enum powerled_color current_color = POWERLED_COLOR_COUNT;
	static int tick_count;

	enum powerled_color new_color = POWERLED_OFF;
	uint32_t chflags = charge_get_flags();

	tick_count++;

	if (!(chflags & CHARGE_FLAG_EXTERNAL_POWER)) {
		/* AC isn't present, so the power LED on the AC plug is off */
		current_color = POWERLED_OFF;
		return;
	}

	/* Translate charge state to LED color */
	switch (charge_get_state()) {
	case PWR_STATE_IDLE:
		if (chflags & CHARGE_FLAG_FORCE_IDLE)
			new_color = ((tick_count & 1) ?
				     POWERLED_GREEN : POWERLED_OFF);
		else
			new_color = POWERLED_GREEN;
		break;
	case PWR_STATE_CHARGE:
		new_color = POWERLED_YELLOW;
		break;
	case PWR_STATE_CHARGE_NEAR_FULL:
		new_color = POWERLED_GREEN;
		break;
	case PWR_STATE_ERROR:
		new_color = POWERLED_RED;
		break;
	default:
		/* Other states don't change LED color */
		break;
	}

	/*
	 * The power adapter on link can partially unplug and lose its LED
	 * state.  There's no way to detect this, so just assume it forgets its
	 * state every 10 seconds.
	 */
	if (!(tick_count % 10))
		current_color = POWERLED_COLOR_COUNT;

	/* If current color is still correct, leave now */
	if (new_color == current_color)
		return;

	/* Update LED */
	if (!powerled_set(new_color))
		current_color = new_color;
}
DECLARE_HOOK(HOOK_SECOND, powerled_tick, HOOK_PRIO_DEFAULT);

/*****************************************************************************/
/* Console commands */

static int command_powerled(int argc, char **argv)
{
	int i;

	/* Pick a color, any color... */
	for (i = 0; i < POWERLED_COLOR_COUNT; i++) {
		if (!strcasecmp(argv[1], color_names[i]))
			return powerled_set(i);
	}
	return EC_ERROR_PARAM1;
}
DECLARE_CONSOLE_COMMAND(powerled, command_powerled,
			"<off | red | yellow | green>",
			"Set power LED color",
			NULL);