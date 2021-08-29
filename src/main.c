/**
 * AnberLauncher
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
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rg351-input.h"

void print_usage(FILE *ofile) {
    fprintf(ofile, "Usage anberlauncher [options] <cmd> [arguments ...]\n"
                   "\n"
                   "OPTIONS\n"
                   "  -h          print this message\n"
                   "  -f <config> configuration file\n"
                   "  -t <type>   joypad type\n"
                   "  -s <signal> exit signal to send to child process\n"
                   "              accepted: INT, TERM, and KILL (default: TERM)\n");
}

int signal_from_string(const char *signal, int *sig) {
    if (strncmp(signal, "INT", 10) == 0) {
        *sig = SIGINT;
        return 0;
    } else if (strncmp(signal, "TERM", 10) == 0) {
        *sig = SIGTERM;
        return 0;
    } else if (strncmp(signal, "KILL", 10) == 0) {
        *sig = SIGKILL;
        return 0;
    }
    return 1;
}

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

    int err;
    int opt;
    int stop_signal;
    int pid;
    int quit_signal;
    int status;
    char *cmd;
    char **cmd_line;

    // Defaults
    stop_signal = SIGTERM;

    // Enforce POSIXLY_CORRECT so it stops parsing at the first non-option argument
    setenv("POSIXLY_CORRECT", "1", 1);
    while ((opt = getopt(argc, argv, "hf:t:s:")) != -1) {
        switch (opt) {
            case 'h':
                print_usage(stdout);
                return 0;
            case 'f':
                // TODO: config file
                break;
            case 't':
                // TODO: joypad type
                break;
            case 's':
                if ((err = signal_from_string(optarg, &stop_signal)) != 0) {
                    fprintf(stderr, "%s: invalid signal\n"
                                    "error: use `-h` for more information\n", argv[0]);
                    return -1;
                }
                break;
            case ':':
            case '?':
                fprintf(stderr, "error: use `-h` for more information\n");
                return -1;
            default:
                fprintf(stderr, "%s: unknown error\n", argv[0]);
                return -1;
        }
    }

    // Get the cmd and cmd line
    cmd = argv[optind];
    cmd_line = argv + optind;
    if ((argc - optind) < 1) {
        fprintf(stderr, "%s: required positional argument\n"
                        "error: use `-h` for more information\n", argv[0]);
        return -1;
    }

    // Setup the key mapping
    if (rg_init() != 0) {
        fprintf(stderr, "%s: error setting up rg351 input\n", argv[0]);
        return -1;
    }
    default_mapping(); // TODO: config file

    pid = fork();
    if (pid ==  0) {
        // Start the process
        err = execv(cmd, cmd_line);
        if (err) {
            fprintf(stderr, "%s: failed to start the child process\n", argv[0]);
            return -1;
        }
    }

    // Keep mapping input until the child proc ends (or is killed).
    quit_signal = 0;
    while (1) {
        if (waitpid(pid, &status, WNOHANG) != 0)
            break;

        // A quit signal was sent (or the rg_dev died), kill the child proc
        if (quit_signal == 0 && (quit_signal = rg_map()) != 0)
            kill(pid, stop_signal);

        // Don't lock the CPU
        usleep(100);
    }
    rg_clean();

    // Ensure the uinput stuff is flushed through before exiting
    sleep(1);

    // Raise the child proc's exit status
    if (WIFEXITED(status)) {
        err = WEXITSTATUS(status);
        return err;
    }
    return 0;
}
