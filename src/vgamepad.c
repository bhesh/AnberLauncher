/**
 * Virtual Gamepad
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

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <linux/uinput.h>

#include "vgamepad.h"

VGAMEPAD vgamepad_create(const char *name) {

    VGAMEPAD fd;
    // 351ELEC toolchain does not like struct uinput_setup so we
    // need to use the old version
    struct uinput_user_dev usetup;

    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0)
        return -1;

    // Enable the device to pass all key and mouse events
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);
    for (int i = 0; i < 256; i++)
        ioctl(fd, UI_SET_KEYBIT, i);

    // Intialize the device
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;  /* sample vendor */
    usetup.id.product = 0x5678; /* sample product */
    strncpy(usetup.name, name, UINPUT_MAX_NAME_SIZE);
    usetup.id.version = 1;

    // Register/create the device
    write(fd, &usetup, sizeof(usetup));
    if (ioctl(fd, UI_DEV_CREATE) != 0)
        return -1;

    return fd;
}

void vgamepad_destroy(VGAMEPAD fd) {
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}

void vgamepad_send_event(VGAMEPAD fd, int type, int code, int value) {

    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = value;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    write(fd, &ie, sizeof(ie));
}
