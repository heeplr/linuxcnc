/******************************************************************************
 *
 * Copyright (C) 2023 Daniel Hiepler <d-linuxcnc AT coderdu DOT de>
 * License: GPL Version 2
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General
 * Public License as published by the Free Software Foundation.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * THE AUTHORS OF THIS LIBRARY ACCEPT ABSOLUTELY NO LIABILITY FOR
 * ANY HARM OR LOSS RESULTING FROM ITS USE.  IT IS _EXTREMELY_ UNWISE
 * TO RELY ON SOFTWARE ALONE FOR SAFETY.  Any machinery capable of
 * harming persons must have provisions for completely removing power
 * from all motors, etc, before persons enter any danger area.  All
 * machinery must be designed to comply with local and national safety
 * codes, and the authors of this software can not, and do not, take
 * any responsibility for such compliance.
 *
 * This code was written as part of the EMC HAL project.  For more
 * information, go to www.linuxcnc.org.
 *
 ******************************************************************************/

/***
 * driver using the pigpio library to interface raspberry pi GPIOs via DMA faster than hal_pi_gpio
 ***/

#include <pigpiod_if2.h>
#include "rtapi.h"		// RTAPI realtime OS API.
#include "rtapi_app.h"		// RTAPI realtime module decls.
#include "hal.h"		// HAL public API decls.

// Module information.
MODULE_AUTHOR("Daniel Hiepler");
MODULE_DESCRIPTION("Driver for raspberry pi GPIOs using libpigpio");
MODULE_LICENSE("GPL");


/* maximum amount of raspberry pi's supported - currently 1 since pigpiod isn't used */
#define MAX_PIGPIO_DEVICES 1

/* descriptor for one raspberry pi */
typedef struct { // Raspberry
    /* pigpio id for this raspberry pi */
    int         pi;
    /* pigpio hw_revision & lib version exported as RO parameters */
    hal_u32_t   hw_revision;
    hal_u32_t   pigpio_version;
} Raspberry;

/* our descriptor to hold all necessary info for linuxcnc/HAL and libpigpio */
typedef struct {
    /* HAL component ID */
    int					component_id;
    /* device descriptors when using multiple raspberry pi's */
    Raspberry				*device[MAX_PIGPIO_DEVICES];
} Driver;

static Driver				driver;


/******************************************************************************
 * INIT AND EXIT CODE
 ******************************************************************************/

int rtapi_app_main(void)
{
    int res = 0;
    Raspberry *device;  // current raspberry pi

    /* connect to the HAL */
    if((driver.component_id = hal_init("hal_pigpio")) < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR, "FAST_PI_GPIO: ERROR: hal_init() failed\n");
	return -EINVAL;
    }

    /* allocate memory for device object */
    if((device = hal_malloc(sizeof(Raspberry))) == 0) {
	rtapi_print_msg(RTAPI_MSG_ERR, "pigpio: ERROR: hal_malloc() failed.\n");
        res = -ENOMEM;
        goto ram_error;
    }
    /* store device in driver descriptor */
    driver.device[0] = device;

    /* initialise pigpio */
    if ((device->pi = pigpio_start(NULL, NULL)) < 0) {
        rtapi_print_msg(RTAPI_MSG_ERR, "pigpio: ERROR: could not connect to pigpiod\n");
        res = -EINVAL;
        goto ram_error;
    }

    /* export hardware revision */
    if(hal_param_u32_newf(HAL_RO, &(device->hw_revision), driver.component_id, "pigpio.hw_revision") < 0) {
        rtapi_print_msg(RTAPI_MSG_ERR, "pigpio: ERROR: hal_param_new() failed\n");
        res = -EINVAL;
        goto ram_error;
    }
    device->hw_revision = get_hardware_revision(device->pi);

    /* export library version */
    if(hal_param_u32_newf(HAL_RO, &(device->pigpio_version), driver.component_id, "pigpio.lib_version") < 0) {
        rtapi_print_msg(RTAPI_MSG_ERR, "pigpio: ERROR: hal_param_new() failed\n");
        res = -EINVAL;
        goto ram_error;
    }
    device->lib_version = get_pigpio_version(device->pi);


    /* signal HAL that we're ready for operation */
    hal_ready(driver.component_id);
    return 0;

ram_error:
    hal_exit(driver.component_id);
    return res;
}

void rtapi_app_exit(void)
{
    /* disconnect from HAL */
    hal_exit(driver.component_id);
    /* deinitialize all raspbery pi's */
    int n;
    for(n = 0; driver.device[n]; n++) {
        /* deinitialize pigpio */
        pigpio_stop(driver.device[n]->pi);
    }
}
