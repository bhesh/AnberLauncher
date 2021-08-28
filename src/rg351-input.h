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

#include <linux/uinput.h>

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
#define RG_DPAD_LEFT  13
#define RG_DPAD_RIGHT 14
#define RG_DPAD_UP    15
#define RG_DPAD_DOWN  16

// Analog IDs
#define RG_L_ANALOG_LEFT  17
#define RG_L_ANALOG_RIGHT 18
#define RG_L_ANALOG_UP    19
#define RG_L_ANALOG_DOWN  20
#define RG_R_ANALOG_LEFT  21
#define RG_R_ANALOG_RIGHT 22
#define RG_R_ANALOG_UP    23
#define RG_R_ANALOG_DOWN  24

// Quit signal
#define QUIT_STATUS 2

int rg_init();
int rg_register_key(int key_id, short mapping);
int rg_register_analog(int analog_id, short threshold, short mapping);
int rg_unregister(int key_id);
void rg_set_l3_r3_quit(int val);
void rg_set_select_start_quit(int val);
int rg_map();
void rg_clean();

#endif // ANBERDRIVER_RG351_H
