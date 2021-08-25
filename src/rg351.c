/**
 * RG351 input devices
 *
 * Copyright (C) 2021 Brian Hession
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 
 * 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public 
 * License along with this program. If not, see 
 * <https://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <libevdev-1.0/libevdev/libevdev.h>
//#include <libevdev-1.0/libevdev/libevdev-uinput.h>

#include "rg351.h"

// D-Pad is 0x10 (horizontal) and 0x11 (vertical)
#define RG_DPAD_MASK 0x10

// Offsets
#define RG_BTN_OFFSET    304 - RG_BTN_A
#define RG_DPAD_OFFSET    17 - RG_DPAD_X
#define RG_ANALOG_OFFSET   2 - RG_L_ANALOG_X

// Event Types
#define RG_BTN_EVENT    1
#define RG_ANALOG_EVENT 3

int rg_controls_create(const char *filename, struct rg_controls *dev) {
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

void rg_controls_destroy(struct rg_controls *dev) {
    if (dev == 0) return;
    if (dev->device != 0)
        libevdev_free(dev->device);
    if (dev->fd > 0)
        close(dev->fd);
}

/**
 * This just grabs the next libevdev event and translates it to a rg_input_event
 *
 * Returns 0 on success, -1 on failure
 */
int rg_controls_get_event(const struct rg_controls *dev, struct rg_input_event *event) {
    if (dev == 0) return -1;

    struct input_event rg_event;
    int err;
    
    err = libevdev_next_event(dev->device, LIBEVDEV_READ_FLAG_NORMAL, &rg_event);
    if (err == LIBEVDEV_READ_STATUS_SUCCESS) {
        if (rg_event.type == RG_BTN_EVENT) {
            event->keyid = rg_event.code + RG_BTN_OFFSET;
            event->type = rg_event.value;
            event->value = 0;
        } else if (rg_event.type == RG_ANALOG_EVENT) {
            // The D-Pad is considered an analog event of range [-1,1]
            if ((rg_event.code & RG_DPAD_MASK) > 0) {
                event->keyid = rg_event.code + RG_DPAD_OFFSET;
                event->type = RG_DPAD;
            // The basic analog is range [0,4096]
            } else {
                event->keyid = rg_event.code + RG_ANALOG_OFFSET;
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
