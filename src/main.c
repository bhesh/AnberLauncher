/**
 * AnberDriver
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

#include <stdio.h>
#include <unistd.h>

#include "rg351.h"

static int select = 0;
static int start = 0;

int main(int argc, char **argv) {

    struct rg_controls rg_dev;
    struct rg_input_event event;

    if (rg_controls_create(RG351_INPUT_DEV, &rg_dev) != 0) {
        printf("Error reading input device\n");
        return -1;
    }

    while (rg_controls_get_event(&rg_dev, &event) == 0 && \
            // Press SELECT and START to exit
            (select != RG_SHORTPRESS || start != RG_SHORTPRESS)) {
        printf("Key ID: %04x -- Action Type: %d -- Value: %d\n", event.keyid, event.type, event.value);
        if (event.keyid == RG_BTN_SELECT)
            select = event.type;
        else if (event.keyid == RG_BTN_START)
            start = event.type;
        usleep(1000); // Don't print too fast
    }

    rg_controls_destroy(&rg_dev);

    return 0;
}
