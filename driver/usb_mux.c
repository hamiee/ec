/* Copyright 2015 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* USB mux high-level driver. */

#include "common.h"
#include "console.h"
#include "usb_mux.h"
#include "util.h"

#define CPRINTS(format, args...) cprints(CC_USBCHARGE, format, ## args)
#define CPRINTF(format, args...) cprintf(CC_USBCHARGE, format, ## args)

static int enable_debug_prints;

void usb_mux_init(int port)
{
	const struct usb_mux *mux = &usb_muxes[port];
	int res;

	ASSERT(port >= 0 && port < CONFIG_USB_PD_PORT_COUNT);
	res = mux->driver->init(mux->port_addr);
	if (res)
		CPRINTS("Error initializing mux port(%d): %d", port, res);
}

/*
 * TODO(crbug.com/505480): Setting muxes often involves I2C transcations,
 * which can block. Consider implementing an asynchronous task.
 */
void usb_mux_set(int port, enum typec_mux mux_mode,
		 enum usb_switch usb_mode, int polarity)
{
	const struct usb_mux *mux = &usb_muxes[port];
	int res;
	mux_state_t mux_state;

	/* Configure USB2.0 */
	board_set_usb_switches(port, usb_mode);

	/* Configure superspeed lanes */
	mux_state = polarity ? mux_mode | MUX_POLARITY_INVERTED : mux_mode;
	res = mux->driver->set(mux->port_addr, mux_state);
	if (res) {
		CPRINTS("Error setting mux port(%d): %d", port, res);
		return;
	}

	if (enable_debug_prints)
		CPRINTS(
		     "usb/dp mux: port(%d) typec_mux(%d) usb2(%d) polarity(%d)",
		     port, mux_mode, usb_mode, polarity);
}

int usb_mux_get(int port, const char **dp_str, const char **usb_str)
{
	const struct usb_mux *mux = &usb_muxes[port];
	int res;
	mux_state_t mux_state;
	const char *dp, *usb;

	res = mux->driver->get(mux->port_addr, &mux_state);
	if (res) {
		CPRINTS("Error setting mux port(%d): %d", port, res);
		return 0;
	}

	dp = mux_state & MUX_POLARITY_INVERTED ? "DP2" : "DP1";
	usb = mux_state & MUX_POLARITY_INVERTED ? "USB2" : "USB1";

	*dp_str = mux_state & MUX_DP_ENABLED ? dp : NULL;
	*usb_str = mux_state & MUX_USB_ENABLED ? usb : NULL;

	return *dp_str || *usb_str;
}

void usb_mux_flip(int port)
{
	const struct usb_mux *mux = &usb_muxes[port];
	int res;
	mux_state_t mux_state;

	res = mux->driver->get(mux->port_addr, &mux_state);
	if (res) {
		CPRINTS("Error getting mux port(%d): %d", port, res);
		return;
	}

	if (mux_state & MUX_POLARITY_INVERTED)
		mux_state &= ~MUX_POLARITY_INVERTED;
	else
		mux_state |= MUX_POLARITY_INVERTED;

	res = mux->driver->set(mux->port_addr, mux_state);
	if (res)
		CPRINTS("Error setting mux port(%d): %d", port, res);
}

#ifdef CONFIG_CMD_TYPEC
static int command_typec(int argc, char **argv)
{
	const char * const mux_name[] = {"none", "usb", "dp", "dock"};
	char *e;
	int port;
	enum typec_mux mux = TYPEC_MUX_NONE;
	int i;

	if (argc == 2 && !strcasecmp(argv[1], "debug")) {
		enable_debug_prints = 1;
		return EC_SUCCESS;
	}

	if (argc < 2)
		return EC_ERROR_PARAM_COUNT;

	port = strtoi(argv[1], &e, 10);
	if (*e || port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_ERROR_PARAM1;

	if (argc < 3) {
		const char *dp_str, *usb_str;
		ccprintf("Port C%d: polarity:CC%d\n",
			port, pd_get_polarity(port) + 1);
		if (usb_mux_get(port, &dp_str, &usb_str))
			ccprintf("Superspeed %s%s%s\n",
				 dp_str ? dp_str : "",
				 dp_str && usb_str ? "+" : "",
				 usb_str ? usb_str : "");
		else
			ccprintf("No Superspeed connection\n");

		return EC_SUCCESS;
	}

	for (i = 0; i < ARRAY_SIZE(mux_name); i++)
		if (!strcasecmp(argv[2], mux_name[i]))
			mux = i;
	usb_mux_set(port, mux, mux == TYPEC_MUX_NONE ?
				      USB_SWITCH_DISCONNECT :
				      USB_SWITCH_CONNECT,
			  pd_get_polarity(port));
	return EC_SUCCESS;
}
DECLARE_CONSOLE_COMMAND(typec, command_typec,
			"[port|debug] [none|usb|dp|dock]",
			"Control type-C connector muxing",
			NULL);
#endif
