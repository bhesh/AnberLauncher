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
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libevdev-1.0/libevdev/libevdev.h>

#include "rg351-input.h"

#define RG351_INPUT_DEV "/dev/input/by-path/platform-ff300000.usb-usb-0:1.2:1.0-event-joystick"

// D-Pad is 0x10 (horizontal) and 0x11 (vertical)
#define RG_DPAD_MASK 0x10

// Offsets
#define RG_BTN_OFFSET RG_BTN_A - 304

// Analog codes
#define RG_DPAD_X 16
#define RG_DPAD_Y 17
#define RG_L_ANALOG_X 2
#define RG_L_ANALOG_Y 3
#define RG_R_ANALOG_X 4
#define RG_R_ANALOG_Y 5
#define ANALOG_CENTER 2048

// Event Types
#define RG_BTN_EVENT    1
#define RG_ANALOG_EVENT 3

struct map_item {
    short mapping;
    short threshold;
};

#define MAP_SIZE 25
struct map_item KEY_MAP[MAP_SIZE];

int L3_R3_QUIT = 0;
int SELECT_START_QUIT = 0;

int RG_DEV_FD;
struct libevdev* RG_DEV;
int EV_FD;

int L3_R3_STATE = 0;
#define L3_PRESSED 1
#define R3_PRESSED 2

int SELECT_START_STATE = 0;
#define SELECT_PRESSED 1
#define START_PRESSED 2

int __rg_dev_init() {
    int err;
    if ((RG_DEV_FD = open(RG351_INPUT_DEV, O_RDONLY|O_NONBLOCK)) <= 0)
        return RG_DEV_FD;
    if ((err = libevdev_new_from_fd(RG_DEV_FD, &RG_DEV)) != 0)
        return err;
    return 0;
}

void __rg_dev_clean() {
    libevdev_free(RG_DEV);
    close(RG_DEV_FD);
}

int __ev_dev_init() {
    // 351ELEC toolchain does not like struct uinput_setup so we
    // need to use the old version
    struct uinput_user_dev usetup;

    EV_FD = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (EV_FD < 0)
        return -1;

    // Enable the device to pass all key and mouse events
    ioctl(EV_FD, UI_SET_EVBIT, EV_KEY);
    ioctl(EV_FD, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(EV_FD, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(EV_FD, UI_SET_EVBIT, EV_REL);
    ioctl(EV_FD, UI_SET_RELBIT, REL_X);
    ioctl(EV_FD, UI_SET_RELBIT, REL_Y);
    for (int i = 0; i < 256; i++)
        ioctl(EV_FD, UI_SET_KEYBIT, i);

    // Intialize the device
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;  /* sample vendor */
    usetup.id.product = 0x5678; /* sample product */
    strncpy(usetup.name, "RG351 Virtual Pad", UINPUT_MAX_NAME_SIZE);
    usetup.id.version = 1;

    // Register/create the device
    write(EV_FD, &usetup, sizeof(usetup));
    if (ioctl(EV_FD, UI_DEV_CREATE) != 0)
        return -1;

    return 0;
}

void __ev_dev_clean() {
    ioctl(EV_FD, UI_DEV_DESTROY);
    close(EV_FD);
}

int rg_init() {
    int err;
    memset(&KEY_MAP, 0, sizeof(KEY_MAP));
    if ((err = __rg_dev_init()) != 0)
        return -1;
    if ((err = __ev_dev_init()) != 0) {
        __rg_dev_clean();
        return -1;
    }
    return 0;
}

int __rg_register_anything(int key_id, struct map_item mapping) {
    if (key_id <= 0 || key_id >= MAP_SIZE)
        return -1;
    KEY_MAP[key_id] = mapping;
    return 0;
}

int rg_register_key(int key_id, short mapping) {
    struct map_item item;
    item.mapping = mapping;
    item.threshold = 0;
    return __rg_register_anything(key_id, item);
}

int rg_register_analog(int analog_id, short threshold, short mapping) {
    struct map_item item;
    item.mapping = mapping;
    item.threshold = threshold;
    return __rg_register_anything(analog_id, item);
}

int rg_unregister(int key_id) {
    struct map_item item;
    item.mapping = 0;
    item.threshold = 0;
    return __rg_register_anything(key_id, item);
}

void rg_set_l3_r3_quit(int val) {
    L3_R3_QUIT = val;
}

void rg_set_select_start_quit(int val) {
    SELECT_START_QUIT = val;
}

void __emit(int type, int code, int value) {

    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = value;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    write(EV_FD, &ie, sizeof(ie));
}

void __map_analog_to_key(short value, int negative, int positive) {
    struct map_item neg = KEY_MAP[negative];
    struct map_item pos = KEY_MAP[positive];
    if (value < (ANALOG_CENTER - neg.threshold)) {
        if (neg.mapping) {
            __emit(EV_KEY, neg.mapping, 1);
            __emit(EV_SYN, SYN_REPORT, 0);
        }
    } else if (value > (ANALOG_CENTER + pos.threshold)) {
        if (pos.mapping) {
            __emit(EV_KEY, pos.mapping, 1);
            __emit(EV_SYN, SYN_REPORT, 0);
        }
    } else {
        if (neg.mapping) {
            __emit(EV_KEY, neg.mapping, 0);
            __emit(EV_SYN, SYN_REPORT, 0);
        }
        if (pos.mapping) {
            __emit(EV_KEY, pos.mapping, 0);
            __emit(EV_SYN, SYN_REPORT, 0);
        }
    }
}

int rg_map() {

    struct input_event rg_event;
    int err;
    int rg_code;
    struct map_item item;

    // Read all the events in the queue
    err = libevdev_next_event(RG_DEV, LIBEVDEV_READ_FLAG_NORMAL, &rg_event);
    while (err == LIBEVDEV_READ_STATUS_SUCCESS) {
        if (rg_event.type == RG_BTN_EVENT) {

            // Translate the button code
            rg_code = rg_event.code + RG_BTN_OFFSET;
            item = KEY_MAP[rg_code];

            // Released
            if (rg_event.value == 0) {
                switch(rg_code) {
                    case RG_BTN_L3:
                        L3_R3_STATE &= ~L3_PRESSED;
                        break;
                    case RG_BTN_R3:
                        L3_R3_STATE &= ~R3_PRESSED;
                        break;
                    case RG_BTN_SELECT:
                        SELECT_START_STATE &= ~SELECT_PRESSED;
                        break;
                    case RG_BTN_START:
                        SELECT_START_STATE &= ~START_PRESSED;
                        break;
                }
                if (item.mapping) {
                    __emit(EV_KEY, item.mapping, 0);
                    __emit(EV_SYN, SYN_REPORT, 0);
                }
            }

            // Pressed
            else {
                switch(rg_code) {
                    case RG_BTN_L3:
                        L3_R3_STATE |= L3_PRESSED;
                        break;
                    case RG_BTN_R3:
                        L3_R3_STATE |= R3_PRESSED;
                        break;
                    case RG_BTN_SELECT:
                        SELECT_START_STATE |= SELECT_PRESSED;
                        break;
                    case RG_BTN_START:
                        SELECT_START_STATE |= START_PRESSED;
                        break;
                }
                if (item.mapping) {
                    __emit(EV_KEY, item.mapping, 1);
                    __emit(EV_SYN, SYN_REPORT, 0);
                }
            }
        } else if (rg_event.type == RG_ANALOG_EVENT) {

            switch(rg_event.code) {
                case RG_DPAD_X: {
                    struct map_item left = KEY_MAP[RG_DPAD_LEFT];
                    struct map_item right = KEY_MAP[RG_DPAD_RIGHT];
                    if (rg_event.value == 0) {
                        if (left.mapping) {
                            __emit(EV_KEY, left.mapping, 0);
                            __emit(EV_SYN, SYN_REPORT, 0);
                        }
                        if (right.mapping) {
                            __emit(EV_KEY, right.mapping, 0);
                            __emit(EV_SYN, SYN_REPORT, 0);
                        }
                    } else if (left.mapping && rg_event.value < 0) {
                        __emit(EV_KEY, left.mapping, 1);
                        __emit(EV_SYN, SYN_REPORT, 0);
                    } else if (right.mapping) {
                        __emit(EV_KEY, right.mapping, 1);
                        __emit(EV_SYN, SYN_REPORT, 0);
                    }
                    break;
                }
                case RG_DPAD_Y: {
                    struct map_item up = KEY_MAP[RG_DPAD_UP];
                    struct map_item down = KEY_MAP[RG_DPAD_DOWN];
                    if (rg_event.value == 0) {
                        if (up.mapping) {
                            __emit(EV_KEY, up.mapping, 0);
                            __emit(EV_SYN, SYN_REPORT, 0);
                        }
                        if (down.mapping) {
                            __emit(EV_KEY, down.mapping, 0);
                            __emit(EV_SYN, SYN_REPORT, 0);
                        }
                    } else if (up.mapping && rg_event.value < 0) {
                        __emit(EV_KEY, up.mapping, 1);
                        __emit(EV_SYN, SYN_REPORT, 0);
                    } else if (down.mapping) {
                        __emit(EV_KEY, down.mapping, 1);
                        __emit(EV_SYN, SYN_REPORT, 0);
                    }
                    break;
                }
                case RG_L_ANALOG_X:
                    __map_analog_to_key(rg_event.value, RG_L_ANALOG_RIGHT, RG_L_ANALOG_LEFT);
                    // TODO: optionally map to mouse
                    break;
                case RG_L_ANALOG_Y:
                    __map_analog_to_key(rg_event.value, RG_L_ANALOG_DOWN, RG_L_ANALOG_UP);
                    // TODO: optionally map to mouse
                    break;
                case RG_R_ANALOG_X:
                    __map_analog_to_key(rg_event.value, RG_R_ANALOG_LEFT, RG_R_ANALOG_RIGHT);
                    // TODO: optionally map to mouse
                    break;
                case RG_R_ANALOG_Y:
                    __map_analog_to_key(rg_event.value, RG_R_ANALOG_UP, RG_R_ANALOG_DOWN);
                    // TODO: optionally map to mouse
                    break;
            }
        }

        // Get the next event
        err = libevdev_next_event(RG_DEV, LIBEVDEV_READ_FLAG_NORMAL, &rg_event);
    }

    // Check for quit status
    if (L3_R3_QUIT && L3_R3_STATE == (L3_PRESSED | R3_PRESSED))
        return QUIT_STATUS;
    if (SELECT_START_QUIT && SELECT_START_STATE == (SELECT_PRESSED | START_PRESSED))
        return QUIT_STATUS;

    // These return values are (more or less) expected
    if (err == LIBEVDEV_READ_STATUS_SYNC || err == LIBEVDEV_READ_STATUS_SUCCESS || err == -EAGAIN)
        return 0;
    return -1;
}

void rg_clean() {
    __rg_dev_clean();
    __ev_dev_clean();
}
