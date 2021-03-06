/* -*- mode:c -*-
 *
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Interrupts */
GPIO_INT(AP_IN_SUSPEND,        PIN(E, 9),  GPIO_INT_BOTH,                   power_signal_interrupt)

/* Outputs */
GPIO(SH_EC_SIGNAL,         PIN(A, 7),  GPIO_OUT_LOW)
GPIO(SH_IRQ_L,             PIN(A, 11), GPIO_OUT_LOW)

/* Inputs */
GPIO(LID_CLOSED,           PIN(A,  2), GPIO_INPUT)
GPIO(BASE_PRESENT,         PIN(A,  3), GPIO_INPUT)

#if 0
/* Alternate functions */
GPIO(UART_TX,              PIN(A,  9), GPIO_OUT_LOW)
GPIO(UART_RX,              PIN(A, 10), GPIO_OUT_LOW)
#endif

/* Needed to bypass flash write protection */
UNIMPLEMENTED(ENTERING_RW)
UNIMPLEMENTED(WP)

/*
 * I2C pins should be configured as inputs until I2C module is
 * initialized. This will avoid driving the lines unintentionally.
 */
GPIO(SLAVE_I2C_SCL,        PIN(B,  6), GPIO_INPUT)
GPIO(SLAVE_I2C_SDA,        PIN(B,  7), GPIO_INPUT)

ALTERNATE(PIN_MASK(A, 0x0600), 1, MODULE_UART,   0) /* USART1: PA9/PA10 */
ALTERNATE(PIN_MASK(B, 0x00C0), 1, MODULE_I2C,    0) /* I2C SLAVE:PB6/7 */
