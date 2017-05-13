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

#ifndef DIX_BUFFER_H
#define DIX_BUFFER_H

#include "../dix.h"

view_t *create_buffer_view(UINT x, UINT y, UINT width, UINT height);
void buffer_view_set_size(view_t *view, UINT width, UINT height);
void buffer_view_set_data(view_t *view, WCHAR *data);
void buffer_view_clear(view_t *view);

#endif //DIX_BUFFER_H
