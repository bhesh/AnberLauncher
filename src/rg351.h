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

#ifndef ANBERDRIVER_RG351_H
#define ANBERDRIVER_RG351_H

#define RG351_JOYPAD_DEV "/dev/input/by-path/platform-ff300000.usb-usb-0:1.2:1.0-event-joystick"

// Key IDs
#define RG_A      304
#define RG_B      305
#define RG_X      306
#define RG_Y      307
#define RG_L1     308
#define RG_R1     309
#define RG_START  310
#define RG_SELECT 311
#define RG_L3     312
#define RG_R3     313
#define RG_L2     314
#define RG_R2     315

// Joystick IDs
#define RG_LSTICK_HORIZONTAL 2
#define RG_LSTICK_VERTICAL   3
#define RG_RSTICK_HORIZONTAL 4
#define RG_RSTICK_VERTICAL   5
#define RG_DPAD_HORIZONTAL   16
#define RG_DPAD_VERTICAL     17

// Event Types
#define RG_RELEASED   0
#define RG_SHORTPRESS 1
#define RG_LONGPRESS  2
#define RG_ANALOG     3

// Analog Threshold
static int RG_LSTICK_THRESHOLD_X = 256;
static int RG_LSTICK_THRESHOLD_Y = 256;
static int RG_RSTICK_THRESHOLD_X = 256;
static int RG_RSTICK_THRESHOLD_Y = 256;

struct rg351_joypad {
    int fd;
    struct libevdev *device;
};

struct rg351_event {
    int keyid;
    int type;
    int value; // Analog-only
};

int joypad_create(const char *, struct rg351_joypad *);
void joypad_destroy(struct rg351_joypad *);
int joypad_get_event(const struct rg351_joypad *, struct rg351_event *);

#endif // ANBERDRIVER_RG351_H
