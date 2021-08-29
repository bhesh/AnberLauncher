/**
 * AnberLauncher Configuration
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
#include <string.h>
#include <sys/wait.h>

#include "config.h"
#include "ezini.h"
#include "rg351-input.h"

int JOYPAD_TYPE = RG_KEYBOARD;
int STOP_SIGNAL = SIGTERM;

ini_entry_t INI_ENTRY;
ini_entry_list_t INI_LIST;

const char *INI_FIELDS[] = {
    "AnberLauncher",
    "KeyBindings"
};

#define MAX_SECTION_LENGTH 255
#define MAX_KEY_LENGTH      64
#define MAX_VALUE_LENGTH   255

const char *RGNAMES[] = {
    0, "RG_BTN_A", "RG_BTN_B", "RG_BTN_X", "RG_BTN_Y", "RG_BTN_L1", "RG_BTN_R1", "RG_BTN_START",
    "RG_BTN_SELECT", "RG_BTN_L3", "RG_BTN_R3", "RG_BTN_L2", "RG_BTN_R2", "RG_DPAD_LEFT", "RG_DPAD_RIGHT",
    "RG_DPAD_UP", "RG_DPAD_DOWN", "RG_L_ANALOG_LEFT", "RG_L_ANALOG_RIGHT", "RG_L_ANALOG_UP", "RG_L_ANALOG_DOWN",
    "RG_R_ANALOG_LEFT", "RG_R_ANALOG_RIGHT", "RG_R_ANALOG_UP", "RG_R_ANALOG_DOWN"
};
#define RGNAMES_LENGTH sizeof(RGNAMES) / sizeof(char *)

#define KEY_OFFSET 0
const char *KEYNAMES[] = {
    "KEY_RESERVED", "KEY_ESC", "KEY_1", "KEY_2", "KEY_3", "KEY_4", "KEY_5", "KEY_6", "KEY_7", "KEY_8", "KEY_9",
    "KEY_0", "KEY_MINUS", "KEY_EQUAL", "KEY_BACKSPACE", "KEY_TAB", "KEY_Q", "KEY_W", "KEY_E", "KEY_R", "KEY_T",
    "KEY_Y", "KEY_U", "KEY_I", "KEY_O", "KEY_P", "KEY_LEFTBRACE", "KEY_RIGHTBRACE", "KEY_ENTER", "KEY_LEFTCTRL",
    "KEY_A", "KEY_S", "KEY_D", "KEY_F", "KEY_G", "KEY_H", "KEY_J", "KEY_K", "KEY_L", "KEY_SEMICOLON", "KEY_APOSTROPHE",
    "KEY_GRAVE", "KEY_LEFTSHIFT", "KEY_BACKSLASH", "KEY_Z", "KEY_X", "KEY_C", "KEY_V", "KEY_B", "KEY_N", "KEY_M",
    "KEY_COMMA", "KEY_DOT", "KEY_SLASH", "KEY_RIGHTSHIFT", "KEY_KPASTERISK", "KEY_LEFTALT", "KEY_SPACE",
    "KEY_CAPSLOCK", "KEY_F1", "KEY_F2", "KEY_F3", "KEY_F4", "KEY_F5", "KEY_F6", "KEY_F7", "KEY_F8", "KEY_F9",
    "KEY_F10", "KEY_NUMLOCK", "KEY_SCROLLLOCK", "KEY_KP7", "KEY_KP8", "KEY_KP9", "KEY_KPMINUS", "KEY_KP4", "KEY_KP5",
    "KEY_KP6", "KEY_KPPLUS", "KEY_KP1", "KEY_KP2", "KEY_KP3", "KEY_KP0", "KEY_KPDOT", 0, "KEY_ZENKAKUHANKAKU",
    "KEY_102ND", "KEY_F11", "KEY_F12", "KEY_RO", "KEY_KATAKANA", "KEY_HIRAGANA", "KEY_HENKAN", "KEY_KATAKANAHIRAGANA",
    "KEY_MUHENKAN", "KEY_KPJPCOMMA", "KEY_KPENTER", "KEY_RIGHTCTRL", "KEY_KPSLASH", "KEY_SYSRQ", "KEY_RIGHTALT",
    "KEY_LINEFEED", "KEY_HOME", "KEY_UP", "KEY_PAGEUP", "KEY_LEFT", "KEY_RIGHT", "KEY_END", "KEY_DOWN", "KEY_PAGEDOWN",
    "KEY_INSERT", "KEY_DELETE", "KEY_MACRO", "KEY_MUTE", "KEY_VOLUMEDOWN", "KEY_VOLUMEUP", "KEY_POWER", "KEY_KPEQUAL",
    "KEY_KPPLUSMINUS", "KEY_PAUSE", "KEY_SCALE", "KEY_KPCOMMA", "KEY_HANGEUL", "KEY_HANJA", "KEY_YEN", "KEY_LEFTMETA",
    "KEY_RIGHTMETA", "KEY_COMPOSE", "KEY_STOP", "KEY_AGAIN", "KEY_PROPS", "KEY_UNDO", "KEY_FRONT", "KEY_COPY",
    "KEY_OPEN", "KEY_PASTE", "KEY_FIND", "KEY_CUT", "KEY_HELP", "KEY_MENU", "KEY_CALC", "KEY_SETUP", "KEY_SLEEP",
    "KEY_WAKEUP", "KEY_FILE", "KEY_SENDFILE", "KEY_DELETEFILE", "KEY_XFER", "KEY_PROG1", "KEY_PROG2", "KEY_WWW",
    "KEY_MSDOS", "KEY_SCREENLOCK", "KEY_ROTATE_DISPLAY", "KEY_CYCLEWINDOWS", "KEY_MAIL", "KEY_BOOKMARKS",
    "KEY_COMPUTER", "KEY_BACK", "KEY_FORWARD", "KEY_CLOSECD", "KEY_EJECTCD", "KEY_EJECTCLOSECD", "KEY_NEXTSONG",
    "KEY_PLAYPAUSE", "KEY_PREVIOUSSONG", "KEY_STOPCD", "KEY_RECORD", "KEY_REWIND", "KEY_PHONE", "KEY_ISO",
    "KEY_CONFIG", "KEY_HOMEPAGE", "KEY_REFRESH", "KEY_EXIT", "KEY_MOVE", "KEY_EDIT", "KEY_SCROLLUP", "KEY_SCROLLDOWN",
    "KEY_KPLEFTPAREN", "KEY_KPRIGHTPAREN", "KEY_NEW", "KEY_REDO", "KEY_F13", "KEY_F14", "KEY_F15", "KEY_F16",
    "KEY_F17", "KEY_F18", "KEY_F19", "KEY_F20", "KEY_F21", "KEY_F22", "KEY_F23", "KEY_F24", 0, 0, 0, 0, 0,
    "KEY_PLAYCD", "KEY_PAUSECD", "KEY_PROG3", "KEY_PROG4", "KEY_DASHBOARD", "KEY_SUSPEND", "KEY_CLOSE", "KEY_PLAY",
    "KEY_FASTFORWARD", "KEY_BASSBOOST", "KEY_PRINT", "KEY_HP", "KEY_CAMERA", "KEY_SOUND", "KEY_QUESTION", "KEY_EMAIL",
    "KEY_CHAT", "KEY_SEARCH", "KEY_CONNECT", "KEY_FINANCE", "KEY_SPORT", "KEY_SHOP", "KEY_ALTERASE", "KEY_CANCEL",
    "KEY_BRIGHTNESSDOWN", "KEY_BRIGHTNESSUP", "KEY_MEDIA", "KEY_SWITCHVIDEOMODE", "KEY_KBDILLUMTOGGLE",
    "KEY_KBDILLUMDOWN", "KEY_KBDILLUMUP", "KEY_SEND", "KEY_REPLY", "KEY_FORWARDMAIL", "KEY_SAVE", "KEY_DOCUMENTS",
    "KEY_BATTERY", "KEY_BLUETOOTH", "KEY_WLAN", "KEY_UWB", "KEY_UNKNOWN", "KEY_VIDEO_NEXT", "KEY_VIDEO_PREV",
    "KEY_BRIGHTNESS_CYCLE", "KEY_BRIGHTNESS_AUTO", "KEY_DISPLAY_OFF", "KEY_WWAN", "KEY_RFKILL", "KEY_MICMUTE"
};
#define KEYNAMES_LENGTH sizeof(KEYNAMES) / sizeof(char *)

#define BTN_OFFSET 0x100
const char *BTNNAMES[] = {
    "BTN_0", "BTN_1", "BTN_2", "BTN_3", "BTN_4", "BTN_5", "BTN_6", "BTN_7", "BTN_8", "BTN_9", 0, 0, 0, 0, 0, 0,
    "BTN_LEFT", "BTN_RIGHT", "BTN_MIDDLE", "BTN_SIDE", "BTN_EXTRA", "BTN_FORWARD", "BTN_BACK", "BTN_TASK",
    0, 0, 0, 0, 0, 0, 0, 0, "BTN_TRIGGER", "BTN_THUMB", "BTN_THUMB2", "BTN_TOP", "BTN_TOP2", "BTN_PINKIE",
    "BTN_BASE", "BTN_BASE2", "BTN_BASE3", "BTN_BASE4", "BTN_BASE5", "BTN_BASE6", 0, 0, 0, "BTN_DEAD", "BTN_SOUTH",
    "BTN_EAST", "BTN_C", "BTN_NORTH", "BTN_WEST", "BTN_Z", "BTN_TL", "BTN_TR", "BTN_TL2", "BTN_TR2", "BTN_SELECT",
    "BTN_START", "BTN_MODE", "BTN_THUMBL", "BTN_THUMBR", 0, "BTN_TOOL_PEN", "BTN_TOOL_RUBBER", "BTN_TOOL_BRUSH",
    "BTN_TOOL_PENCIL", "BTN_TOOL_AIRBRUSH", "BTN_TOOL_FINGER", "BTN_TOOL_MOUSE", "BTN_TOOL_LENS", "BTN_TOOL_QUINTTAP",
    "BTN_STYLUS3", "BTN_TOUCH", "BTN_STYLUS", "BTN_STYLUS2", "BTN_TOOL_DOUBLETAP", "BTN_TOOL_TRIPLETAP",
    "BTN_TOOL_QUADTAP", "BTN_GEAR_DOWN", "BTN_GEAR_UP"
};
#define BTNNAMES_LENGTH sizeof(BTNNAMES) / sizeof(char *)

int signal_from_string(const char *signal, int *sig) {
    if (strncmp("INT", signal, 10) == 0) {
        *sig = SIGINT;
        return 0;
    } else if (strncmp("TERM", signal, 10) == 0) {
        *sig = SIGTERM;
        return 0;
    } else if (strncmp("KILL", signal, 10) == 0) {
        *sig = SIGKILL;
        return 0;
    }
    return 1;
}

int __get_rg_key(const char *name, int *value) {
    long unsigned int i;
    for (i = 0; i < RGNAMES_LENGTH; ++i) {
        if (RGNAMES[i] && strncmp(RGNAMES[i], name, MAX_KEY_LENGTH) == 0) {
            *value = i;
            return 1;
        }
    }
    return 0;
}

int __get_map_key(const char *name, int *value) {
    long unsigned int i;
    for (i = 0; i < KEYNAMES_LENGTH; ++i) {
        if (KEYNAMES[i] && strncmp(KEYNAMES[i], name, MAX_VALUE_LENGTH) == 0) {
            *value = i;
            return 1;
        }
    }
    for (i = 0; i < BTNNAMES_LENGTH; ++i) {
        if (BTNNAMES[i] && strncmp(BTNNAMES[i], name, MAX_VALUE_LENGTH) == 0) {
            *value = i;
            return 1;
        }
    }
    return 0;
}

int load_config(const char *filename) {
    
    FILE *fp;
    int res;

    int rg_key;
    int map_key;

    if ((fp = fopen(filename, "r")) == 0) {
        return 1;
    }

    while ((res = GetEntryFromFile(fp, &INI_ENTRY)) > 0) {
        if (strncmp("AnberLauncher", INI_ENTRY.section, MAX_SECTION_LENGTH) == 0) {
            if (strncmp("L3_R3_QUIT", INI_ENTRY.key, MAX_KEY_LENGTH) == 0) {
                if (strncmp("1", INI_ENTRY.value, MAX_VALUE_LENGTH) == 0) {
#ifdef DEBUG
                    printf("Set L3+R3 quit\n");
#endif
                    rg_set_l3_r3_quit(1);
                }
            }
            else if (strncmp("SELECT_START_QUIT", INI_ENTRY.key, MAX_KEY_LENGTH) == 0) {
                if (strncmp("1", INI_ENTRY.value, MAX_VALUE_LENGTH) == 0) {
#ifdef DEBUG
                    printf("Set Select+Start quit\n");
#endif
                    rg_set_select_start_quit(1);
                }
            }
        } else if (strncmp("KeyBindings", INI_ENTRY.section, MAX_SECTION_LENGTH) == 0) {
            if (__get_rg_key(INI_ENTRY.key, &rg_key) == 0)
                continue;
            if (__get_map_key(INI_ENTRY.value, &map_key) == 0)
                continue;

#ifdef DEBUG
            // Print
            if (map_key < 256)
                printf("Mapping %s ==> %s\n", RGNAMES[rg_key], KEYNAMES[map_key]);
            else
                printf("Mapping %s ==> %s\n", RGNAMES[rg_key], BTNNAMES[map_key]);
#endif

            // Map
            if (rg_key >= RG_L_ANALOG_LEFT) {
                rg_register_analog(rg_key, 256, map_key);
            } else {
                rg_register_key(rg_key, map_key);
            }
        }
    }

    return res;
}

void default_config() {
    JOYPAD_TYPE = RG_KEYBOARD;
    STOP_SIGNAL = SIGTERM;
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
