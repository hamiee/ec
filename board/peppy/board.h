/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Configuration for Peppy mainboard */

#ifndef __BOARD_H
#define __BOARD_H

/* Debug features */
#define CONFIG_ASSERT_HELP
#define CONFIG_CONSOLE_CMDHELP
#define CONFIG_PANIC_HELP
#define CONFIG_TASK_PROFILING

/* Optional features */
/*HEY #define CONFIG_SMART_BATTERY */
/*HEY #define CONFIG_BATTERY_LINK */
/*HEY #define CONFIG_CHARGER_BQ24725 */
#ifdef HAS_TASK_CHIPSET
#define CONFIG_CHIPSET_X86_HASWELL
#endif
#define CONFIG_CUSTOM_KEYSCAN
#define CONFIG_EXTPOWER_GPIO
#ifdef HAS_TASK_KEYPROTO
#define CONFIG_KEYBOARD_PROTOCOL_8042
#endif
#define CONFIG_LID_SWITCH
#define CONFIG_LPC
#define CONFIG_PECI
#define CONFIG_POWER_BUTTON
#define CONFIG_PWM_FAN
#define CONFIG_TEMP_SENSOR
#define CONFIG_USB_PORT_POWER_DUMB
#define CONFIG_WP_ACTIVE_HIGH

#ifndef __ASSEMBLER__

/* PWM channels */
#define FAN_CH_CPU         2  /* CPU fan */
#define FAN_CH_BL_DISPLAY  4  /* LVDS backlight (from PCH, cleaned by EC) */

/* I2C ports */
#define I2C_PORT_BATTERY 0
#define I2C_PORT_CHARGER 0
#define I2C_PORT_THERMAL 2
/* There are only two I2C ports used because battery and charger share a port */
#define I2C_PORTS_USED 2

/* 13x8 keyboard scanner uses an entire GPIO bank for row inputs */
#define KB_SCAN_ROW_IRQ  LM4_IRQ_GPIOK
#define KB_SCAN_ROW_GPIO LM4_GPIO_K

/* Host connects to keyboard controller module via LPC */
#define HOST_KB_BUS_LPC

/* USB ports */
#define USB_PORT_COUNT 2

/* GPIOs for second UART port */
#define CONFIG_HOST_UART 2
#define CONFIG_HOST_UART_IRQ LM4_IRQ_UART2
#define CONFIG_HOST_UART2_GPIOS_PG4_5

/* GPIO signal definitions. */
enum gpio_signal {
	/* Inputs with interrupt handlers are first for efficiency */
	GPIO_POWER_BUTTON_L = 0,   /* Power button */
	GPIO_LID_OPEN,             /* Lid switch */
	GPIO_AC_PRESENT,           /* AC power present */
	GPIO_PCH_BKLTEN,           /* Backlight enable signal from PCH */
	GPIO_PCH_SLP_S0_L,         /* SLP_S0# signal from PCH */
	GPIO_PCH_SLP_S3_L,         /* SLP_S3# signal from PCH */
	GPIO_PCH_SLP_S5_L,         /* SLP_S5# signal from PCH */
	GPIO_PCH_SLP_SUS_L,        /* SLP_SUS# signal from PCH */
	GPIO_PCH_SUSWARN_L,        /* SUSWARN# signal from PCH */
	GPIO_PP1050_PGOOD,         /* Power good on 1.05V */
	GPIO_PP1350_PGOOD,         /* Power good on 1.35V (DRAM) */
	GPIO_PP5000_PGOOD,         /* Power good on 5V */
	GPIO_VCORE_PGOOD,          /* Power good on core VR */
	GPIO_PCH_EDP_VDD_EN,       /* PCH wants EDP enabled */
	GPIO_RECOVERY_L,           /* Recovery signal from servo */
	GPIO_WP,                   /* Write protect input */

	/* Other inputs */
	GPIO_FAN_ALERT_L,          /* From thermal sensor */
	GPIO_USB1_OC_L,            /* USB port overcurrent warning */
	GPIO_USB2_OC_L,            /* USB port overcurrent warning */
	GPIO_BOARD_VERSION1,       /* Board version stuffing resistor 1 */
	GPIO_BOARD_VERSION2,       /* Board version stuffing resistor 2 */
	GPIO_BOARD_VERSION3,       /* Board version stuffing resistor 3 */
	GPIO_CPU_PGOOD,            /* Power good to the CPU */

	/* Outputs */
	GPIO_CPU_PROCHOT,          /* Force CPU to think it's overheated */
	GPIO_PP1350_EN,            /* Enable 1.35V supply */
	GPIO_PP3300_DSW_GATED_EN,  /* Enable DSW rails */
	GPIO_PP3300_DX_EN,         /* Enable power to lots of peripherals */
	GPIO_PP3300_LTE_EN,        /* Enable LTE radio */
	GPIO_PP3300_WLAN_EN,       /* Enable WiFi radio */
	GPIO_SUSP_VR_EN,           /* Enable 1.05V regulator */
	GPIO_VCORE_EN,             /* Stuffing option - not connected */
	GPIO_PP5000_EN,            /* Enable 5V supply */
	GPIO_SYS_PWROK,            /* EC thinks everything is up and ready */
	GPIO_WLAN_OFF_L,           /* Disable WiFi chip? Or just the radio? */
	GPIO_CHARGE_L,             /* Allow battery to charge when on AC */

	GPIO_ENABLE_BACKLIGHT,     /* Enable backlight power */
	GPIO_ENABLE_TOUCHPAD,      /* Enable touchpad power */
	GPIO_ENTERING_RW,          /* Indicate when EC is entering RW code */
	GPIO_PCH_DPWROK,           /* Indicate when VccDSW is good */

	GPIO_PCH_HDA_SDO,          /* HDA_SDO signal to PCH; when high, ME
				    * ignores security descriptor */
	GPIO_PCH_WAKE_L,           /* Wake signal from EC to PCH */
	GPIO_PCH_NMI_L,            /* Non-maskable interrupt pin to PCH */
	GPIO_PCH_PWRBTN_L,         /* Power button output to PCH */
	GPIO_PCH_PWROK,            /* PWROK / APWROK signals to PCH */
	GPIO_PCH_RCIN_L,           /* RCIN# line to PCH (for 8042 emulation) */
	GPIO_PCH_RSMRST_L,         /* Reset PCH resume power plane logic */
	GPIO_PCH_SMI_L,            /* System management interrupt to PCH */
	GPIO_TOUCHSCREEN_RESET_L,  /* Reset touch screen */
	GPIO_EC_EDP_VDD_EN,        /* Enable EDP (passthru from PCH) */
	GPOI_LPC_CLKRUN_L,         /* Dunno. Probably important, though. */

	GPIO_USB1_ENABLE,          /* USB port 1 output power enable */
	GPIO_USB2_ENABLE,          /* USB port 2 output power enable */

	GPIO_PCH_SUSACK_L,         /* Acknowledge PCH SUSWARN# signal */
	GPIO_PCH_RTCRST_L,         /* Not supposed to be here */
	GPIO_PCH_SRTCRST_L,        /* Not supposed to be here */

	/* Number of GPIOs; not an actual GPIO */
	GPIO_COUNT
};

enum adc_channel {
	/* EC internal die temperature in degrees K. */
	ADC_CH_EC_TEMP = 0,

	/* HEY: Peppy MB has only one discrete thermal sensor, but it has two
	 * values (one internal and one external). Both should be here.
	 * HEY: There may be a BAT_TEMP sensor on the battery pack too.
	 */

	/* HEY: Be prepared to read this (ICMNT). */
	/* Charger current in mA. */
	ADC_CH_CHARGER_CURRENT,

	ADC_CH_COUNT
};

enum temp_sensor_id {
	/* HEY - need two I2C sensor values */

	/* EC internal temperature sensor */
	TEMP_SENSOR_EC_INTERNAL,
	/* CPU die temperature via PECI */
	TEMP_SENSOR_CPU_PECI,

	TEMP_SENSOR_COUNT
};

/* HEY: The below stuff is for Link. Pick a different pin for Peppy */
/* Target value for BOOTCFG. This is set to PE2/USB1_CTL1, which has an external
 * pullup. If this signal is pulled to ground when the EC boots, the EC will get
 * into the boot loader and we can recover bricked EC. */
#define BOOTCFG_VALUE 0x7fff88fe

/* Known board versions for system_get_board_version(). */
enum board_version {
	BOARD_VERSION_PROTO1 = 0,
	BOARD_VERSION_EVT = 1,
};


#endif /* !__ASSEMBLER__ */

#endif /* __BOARD_H */