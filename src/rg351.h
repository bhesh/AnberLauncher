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

#ifndef ANBERDRIVER_RG351_H
#define ANBERDRIVER_RG351_H

#define RG351_INPUT_DEV "/dev/input/by-path/platform-ff300000.usb-usb-0:1.2:1.0-event-joystick"

// Button IDs
#define RG_BTN_A       1
#define RG_BTN_B       2
#define RG_BTN_X       3
#define RG_BTN_Y       4
#define RG_BTN_L1      5
#define RG_BTN_R1      6
#define RG_BTN_START   7
#define RG_BTN_SELECT  8
#define RG_BTN_L3      9
#define RG_BTN_R3     10
#define RG_BTN_L2     11
#define RG_BTN_R2     12

// D-Pad IDs
#define RG_DPAD_X     13
#define RG_DPAD_Y     14

// Analog IDs
#define RG_L_ANALOG_X 15
#define RG_L_ANALOG_Y 16
#define RG_R_ANALOG_X 17
#define RG_R_ANALOG_Y 18

// Event Types
#define RG_RELEASE    0
#define RG_SHORTPRESS 1
#define RG_LONGPRESS  2
#define RG_DPAD       3
#define RG_ANALOG     4

struct rg_controls {
    int fd;
    struct libevdev *device;
};

struct rg_input_event {
    int keyid;
    int type;
    int value; // Analog-only
};

int rg_controls_create(const char *, struct rg_controls *);
void rg_controls_destroy(struct rg_controls *);
int rg_controls_get_event(const struct rg_controls *, struct rg_input_event *);

#endif // ANBERDRIVER_RG351_H
