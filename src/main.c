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
* AnberJoyCon
*/

#include <stdio.h>
#include <unistd.h>

#include "rg351.h"

void handle_event(const struct rg351_event *event) {
    printf("Key ID: %d -- Action Type: %d -- Value: %d\n", event->keyid, event->type, event->value);
}

int main(int argc, char **argv) {

    struct rg351_joypad rg_joypad;
    struct rg351_event rg_event;

    if (joypad_create(RG351_JOYPAD_DEV, &rg_joypad) != 0) {
        printf("Error creating joypad\n");
        return -1;
    }

    while (joypad_get_event(&rg_joypad, &rg_event) == 0) {
        handle_event(&rg_event);
        usleep(1000);
    }

    joypad_destroy(&rg_joypad);

    return 0;
}
