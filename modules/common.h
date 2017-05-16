/*
 * Dix, library for creating console UI for Windows
 * Copyright (C) 2017 P4t
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef DIX_COMMON_H
#define DIX_COMMON_H

/*
 * This header file will contain enums and other things that aren't directly part of dix
 * but are used or would be useful for other modules.
 */

typedef enum {
	LEFT,
	CENTERH,
	RIGHT
} gravity_horizontal_t;

typedef enum {
	TOP,
	CENTERV,
	BOTTOM
} gravity_vertical_t;

#endif //DIX_COMMON_H
