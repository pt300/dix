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

#ifndef DIX_TEXT_H
#define DIX_TEXT_H

#include "../dix.h"

typedef enum {
	LEFT,
	CENTER,
	RIGHT
} text_gravity_t;

view_t *create_text_view(UINT x, UINT y, UINT width, UINT height);
void text_view_text(view_t *view, WCHAR *text);
void text_view_gravity(view_t *view, text_gravity_t gravity);
void text_view_wrap(view_t *view, BOOLEAN wrap);

#endif //DIX_TEXT_H
