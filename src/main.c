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

#include "config.h"
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

int main(int argc, char **argv) {

    // Argument parsing
    int opt;
    char *configfile;
    char *signal_str;
    char *cmd;
    char **cmd_line;

    // Process handling
    int err;
    int pid;
    int status;
    int stop_process;

    // Allow for overrides
    signal_str = 0;

    // Enforce POSIXLY_CORRECT so it stops parsing at the first non-option argument
    setenv("POSIXLY_CORRECT", "1", 1);
    while ((opt = getopt(argc, argv, "hf:t:s:")) != -1) {
        switch (opt) {
            case 'h':
                print_usage(stdout);
                return 0;
            case 'f':
                configfile = optarg;
                break;
            case 't':
                // TODO: joypad type
                break;
            case 's':
                signal_str = optarg;
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

    // Load the configuration
    if (load_config(configfile) != 0) {
        fprintf(stderr, "%s: error reading config file. Loading default", argv[0]);
        default_config();
    }

    // Override signal if the option was presented
    if (signal_str && (err = signal_from_string(signal_str, &STOP_SIGNAL)) != 0) {
        fprintf(stderr, "%s: invalid signal\n"
                        "error: use `-h` for more information\n", argv[0]);
        return -1;
    }

    // Fork and execute the child process
    pid = fork();
    if (pid ==  0) {
        err = execv(cmd, cmd_line);
        if (err) {
            fprintf(stderr, "%s: failed to start the child process\n", argv[0]);
            return -1;
        }
    }

    // Keep mapping input until the child proc ends (or is killed).
    stop_process = 0;
    while (1) {
        if (waitpid(pid, &status, WNOHANG) != 0)
            break;

        // A quit signal was sent (or the rg_dev died), kill the child proc
        if (stop_process == 0 && (stop_process = rg_map()) != 0)
            kill(pid, STOP_SIGNAL);

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
