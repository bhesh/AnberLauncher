/* Copyright (c) 2021
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301 USA
*
* Authored by: Brian Hession <github@bhmail.me>
*
* RG351 key/event structures
*/

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <libevdev-1.0/libevdev/libevdev.h>
//#include <libevdev-1.0/libevdev/libevdev-uinput.h>

#include "rg351.h"

// Raw Key IDs
#define RG_A_RAW      304
#define RG_B_RAW      305
#define RG_X_RAW      306
#define RG_Y_RAW      307
#define RG_L1_RAW     308
#define RG_R1_RAW     309
#define RG_START_RAW  310
#define RG_SELECT_RAW 311
#define RG_L3_RAW     312
#define RG_R3_RAW     313
#define RG_L2_RAW     314
#define RG_R2_RAW     315

// Raw Joystick IDs
#define RG_LSTICK_HORIZONTAL_RAW 2
#define RG_LSTICK_VERTICAL_RAW   3
#define RG_RSTICK_HORIZONTAL_RAW 4
#define RG_RSTICK_VERTICAL_RAW   5
#define RG_DPAD_HORIZONTAL_RAW   16
#define RG_DPAD_VERTICAL_RAW     17

/**
 * Raw Button Masks
 *
 * So all buttons are masked with 0x130, analog with 0x00, and dpad with 0x10.
 * They do not overlap, so it is possible to grab the offset with a XOR.
 */
#define RG_BUTTON_MASK_RAW 0x130
#define RG_ANALOG_MASK_RAW 0x00
#define RG_DPAD_MASK_RAW   0x10

// Event Types
#define RG_BUTTON_EVENT 1
#define RG_ANALOG_EVENT 3

int joypad_create(const char *filename, struct rg351_joypad *dev) {
    if (dev == 0) return -1;

    int dev_fd;
    struct libevdev* device;
    int err;

    dev->fd = 0;
    dev->device = 0;

	if ((dev_fd = open(filename, O_RDONLY|O_NONBLOCK)) <= 0) {
        return dev_fd;
    }
	if ((err = libevdev_new_from_fd(dev_fd, &device)) != 0) {
        return err;
    }

    dev->fd = dev_fd;
    dev->device = device;

    return 0;
}

void joypad_destroy(struct rg351_joypad *dev) {
    if (dev == 0) return;
    if (dev->device != 0)
        libevdev_free(dev->device);
    if (dev->fd > 0)
        close(dev->fd);
}

/**
 * This just grabs the next libevdev event and translates it to a joypad_event
 *
 * Returns 0 on success, -1 on failure
 */
int joypad_get_event(const struct rg351_joypad *dev, struct joypad_event *event) {
    if (dev == 0) return -1;

    struct input_event rg_event;
    int err;
    
    err = libevdev_next_event(dev->device, LIBEVDEV_READ_FLAG_NORMAL, &rg_event);
    if (err == LIBEVDEV_READ_STATUS_SUCCESS) {
        if (rg_event.type == RG_BUTTON_EVENT) {
            event->keyid = (char)((rg_event.code ^ RG_BUTTON_MASK_RAW) + 1) | RG_BUTTON_MASK;
            event->type = (char)rg_event.value;
            event->value = 0;
        } else if (rg_event.type == RG_ANALOG_EVENT) {
            // The D-Pad is considered an analog event of range [-1,1]
            if ((rg_event.code & RG_DPAD_MASK_RAW) > 0) {
                event->keyid = (char)((rg_event.code ^ RG_DPAD_MASK_RAW) + 1) | RG_DPAD_MASK;
                event->type = RG_DPAD;
            // The basic analog is range [0,4096]
            } else {
                event->keyid = (char)((rg_event.code ^ RG_ANALOG_MASK_RAW) - 1) | RG_ANALOG_MASK;
                event->type = RG_ANALOG;
            }
            event->value = rg_event.value;
        }
    }

    // These return values are (more or less) expected
	if (err == LIBEVDEV_READ_STATUS_SYNC || err == LIBEVDEV_READ_STATUS_SUCCESS || err == -EAGAIN)
        return 0;
    return -1;
}
