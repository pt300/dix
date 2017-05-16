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

#ifndef DIX_H
#define DIX_H

#include <windows.h>
#include <wchar.h>

#define SCREEN (void *)1 //flawless

#define IS_BETWEENI(v, min, max) (v >= min && v <= max)
#define IS_BETWEENE(v, min, max) (v > min && v < max)

typedef enum {
	OK, //is okay
	GUESS, //hell
	RIP
} dix_status_t;

typedef struct {
	WCHAR *buff;
	UINT width, height;
} render_buf_t;

typedef struct view_struct view_t;

typedef void (*view_render_func)(view_t *view, render_buf_t *buf);
typedef void (*view_destroy_func)(void *data);

struct view_struct {
	UINT width, height;
	UINT x, y;
	view_t *parent;
	void *data;
	view_render_func view_render;
	view_destroy_func view_destroy;
};

dix_status_t init_dix();
void deinit_dix();
dix_status_t set_screen_size(UINT width, UINT height);
void set_cursor_position(UINT x, UINT y);
void set_screen_title(WCHAR *title);
view_t *attach_to_screen(view_t *view);

view_t *create_view(UINT x, UINT y, UINT width, UINT height);
void *view_get_data(view_t *view);
void view_set_data(view_t *view, void *data);
void view_set_render_function(view_t *view, view_render_func module_render);
void view_set_destroy_function(view_t *view, view_destroy_func module_destroy);
void view_set_size(view_t *view, UINT width, UINT height);
void view_set_pos(view_t *view, UINT x, UINT y);
void view_set_pos_relative(view_t *view, int x, int y);
void destroy_view(view_t *view);
void render_view(view_t *view, render_buf_t *out);

void render(view_t *view);

//basic modules
#include "modules/frame.h"
#include "modules/text.h"
#include "modules/buffer.h"

#endif //DIX_H
