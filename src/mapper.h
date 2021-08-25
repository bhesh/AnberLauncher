/**
 * Key mapper
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

#ifndef ANBERDRIVER_MAPPER_H
#define ANBERDRIVER_MAPPER_H

#include <linux/uinput.h>

#include "rg351.h"
#include "vgamepad.h"

// Buttons
#define MAP_A               1
#define MAP_B               2
#define MAP_X               3
#define MAP_Y               4
#define MAP_L1              5
#define MAP_R1              6
#define MAP_START           7
#define MAP_SELECT          8
#define MAP_L3              9
#define MAP_R3             10
#define MAP_L2             11
#define MAP_R2             12

// D-Pad
#define MAP_LEFT           13
#define MAP_RIGHT          14
#define MAP_UP             15
#define MAP_DOWN           16

// Analogs
#define MAP_L_ANALOG_LEFT  17
#define MAP_L_ANALOG_RIGHT 18
#define MAP_L_ANALOG_UP    19
#define MAP_L_ANALOG_DOWN  20
#define MAP_R_ANALOG_LEFT  21
#define MAP_R_ANALOG_RIGHT 22

// Settings
#define L_ANALOG_MOUSE 0x1
#define R_ANALOG_MOUSE 0x2
#define DPAD_MOUSE     0x3

struct keymap {
    struct rg_controls rg_dev;
    VGAMEPAD v_dev;
    int[23] map;
    int settings;
}

static int L_ANALOG_THRESHOLD_X = 256;
static int L_ANALOG_THRESHOLD_Y = 256;
static int R_ANALOG_THRESHOLD_X = 256;
static int R_ANALOG_THRESHOLD_Y = 256;

int keymap_create(struct keymap *map, int settings);
void keymap_destroy(struct keymap *map);
int map_keys(struct keymap *map);

#endif // ANBERDRIVER_MAPPER_H
