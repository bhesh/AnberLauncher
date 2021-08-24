/* Copyright (c) 2021
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
#
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
#
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301 USA
#
* Authored by: Brian Hession <github@bhmail.me>
#
* RG351 key/event values
*/

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <libevdev-1.0/libevdev/libevdev.h>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>

#include "rg351.h"

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
 * This just grabs the next libevdev event and translates it to a joypad event
 */
int joypad_get_event(const struct rg351_joypad *dev, struct rg351_event *event) {
    if (dev == 0) return -1;

    struct input_event rg_event;
    int err;
    
    err = libevdev_next_event(dev->device, LIBEVDEV_READ_FLAG_NORMAL, &rg_event);
    if (err == LIBEVDEV_READ_STATUS_SUCCESS) {
        if (rg_event.type == RG_BUTTON_EVENT) {
            event->keyid = rg_event.code;
            event->type = rg_event.value;
            event->value = 0;
        } else if (rg_event.type == RG_ANALOG_EVENT) {
            event->keyid = rg_event.code;
            event->type = RG_ANALOG;
            event->value = rg_event.value;
        }
    }

    // These return values are (more or less) expected
	if (err == LIBEVDEV_READ_STATUS_SYNC || err == LIBEVDEV_READ_STATUS_SUCCESS || err == -EAGAIN)
        return 0;
    return -1;
}
