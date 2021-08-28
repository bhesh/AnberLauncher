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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rg351-input.h"

void default_mapping() {
    rg_register_key(RG_BTN_A, KEY_ENTER);
    rg_register_key(RG_BTN_B, KEY_ESC);
    rg_register_key(RG_BTN_X, KEY_C);
    rg_register_key(RG_BTN_Y, KEY_A);
    rg_register_key(RG_BTN_L1, KEY_RIGHTSHIFT);
    rg_register_key(RG_BTN_L2, BTN_LEFT);
    rg_register_key(RG_BTN_R1, KEY_LEFTSHIFT);
    rg_register_key(RG_BTN_R2, BTN_RIGHT);
    rg_register_key(RG_BTN_SELECT, KEY_ESC);
    rg_register_key(RG_BTN_START, KEY_ENTER);
    rg_register_key(RG_DPAD_LEFT, KEY_LEFT);
    rg_register_key(RG_DPAD_RIGHT, KEY_RIGHT);
    rg_register_key(RG_DPAD_UP, KEY_UP);
    rg_register_key(RG_DPAD_DOWN, KEY_DOWN);
    rg_register_analog(RG_L_ANALOG_LEFT, 256, KEY_LEFT);
    rg_register_analog(RG_L_ANALOG_RIGHT, 256, KEY_RIGHT);
    rg_register_analog(RG_L_ANALOG_UP, 256, KEY_UP);
    rg_register_analog(RG_L_ANALOG_DOWN, 256, KEY_DOWN);
    rg_set_select_start_quit(1);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: anberdriver <cmd> [args...]\n");
        return 1;
    }

    // Setup the key mapping
    if (rg_init() != 0) {
        printf("Error setting up rg351 input\n");
        return 1;
    }
    default_mapping();

    int pid;
    int err;
    int quit_status = 0;
    int stat;

    pid = fork();
    if (pid ==  0) {
        // Start the process
        err = execv(argv[1], argv + 1);
        if (err) {
            printf("Failed to start the child process\n");
            return err;
        }
    }

    // Map the input until the process exits
    while (1) {
        if (waitpid(pid, &stat, WNOHANG) != 0)
            break;
        // A quit signal was sent (or the rg_dev died), kill the child proc
        if (quit_status == 0 && (quit_status = rg_map()) != 0)
            kill(pid, SIGKILL);
        usleep(100);
    }
    rg_clean();

    // Ensure the uinput stuff is flushed through before exiting
    sleep(1);
    return 0;
}
