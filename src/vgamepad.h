/**
 * Virtual Gamepad
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

#ifndef ANBERDRIVER_VGAMEPAD_H
#define ANBERDRIVER_VGAMEPAD_H

typedef int VGAMEPAD;

VGAMEPAD vgamepad_create(const char *name);
void vgamepad_destroy(VGAMEPAD fd);
void vgamepad_send_event(VGAMEPAD fd, int type, int code, int value);

#endif // ANBERDRIVER_VGAMEPAD_H
