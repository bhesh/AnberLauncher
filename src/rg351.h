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

#ifndef ANBERDRIVER_RG351_H
#define ANBERDRIVER_RG351_H

#define RG351_JOYPAD_DEV "/dev/input/by-path/platform-ff300000.usb-usb-0:1.2:1.0-event-joystick"

// Key IDs
#define RG_A      0x01
#define RG_B      0x02
#define RG_X      0x03
#define RG_Y      0x04
#define RG_L1     0x05
#define RG_R1     0x06
#define RG_START  0x07
#define RG_SELECT 0x08
#define RG_L3     0x09
#define RG_R3     0x0a
#define RG_L2     0x0b
#define RG_R2     0x0c

// Joystick IDs
#define RG_LSTICK_HORIZONTAL 0x11
#define RG_LSTICK_VERTICAL   0x12
#define RG_RSTICK_HORIZONTAL 0x13
#define RG_RSTICK_VERTICAL   0x14
#define RG_DPAD_HORIZONTAL   0x21
#define RG_DPAD_VERTICAL     0x22

// Button Masks
#define RG_BUTTON_MASK 0x00
#define RG_ANALOG_MASK 0x10
#define RG_DPAD_MASK   0x20

// Event Types
#define RG_RELEASE    0
#define RG_SHORTPRESS 1
#define RG_LONGPRESS  2
#define RG_ANALOG     3
#define RG_DPAD       4

struct rg351_joypad {
    int fd;
    struct libevdev *device;
};

struct joypad_event {
    char keyid;
    char type;
    int value; // Analog-only
};

int joypad_create(const char *, struct rg351_joypad *);
void joypad_destroy(struct rg351_joypad *);
int joypad_get_event(const struct rg351_joypad *, struct joypad_event *);

#endif // ANBERDRIVER_RG351_H
