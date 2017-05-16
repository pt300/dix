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

#include "text.h"

typedef struct {
	text_gravity_t gravity;
	BOOLEAN wrap;
	WCHAR *string;
} text_data_t;

static inline size_t llen(WCHAR *str) {
	size_t ret;

	for(ret = 0; *str != L'\0' && *str != L'\n'; str++, ret++);

	return ret;
}

static inline size_t gravity_shift(text_gravity_t gravity, size_t width, size_t length) {
	size_t shift;

	if(gravity == CENTER) {
		shift = width - length;
		shift /= 2;
	}
	else if(gravity == RIGHT) {
		shift = width - length;
	}
	else {
		shift = 0;
	}

	return shift;
}

void render_text(view_t *view, render_buf_t *out) {
	text_data_t *data = view_get_data(view);
	WCHAR *str = data->string;
	UINT x, y, width, height;
	size_t iy, iw, tw, line_len;

	if(str == NULL ||
	   view->x >= out->width ||
	   view->y >= out->height) {
		return;
	}

	width = min(out->width - view->x, view->width);
	height = min(out->height - view->y, view->height);
	x = view->x;
	y = view->y;

	iy = 0;
	if(*str == L'\n') {
		iy = 1;
	}

	while(*str != L'\0') {
		if(*str == L'\n') {
			str++;
		}
		line_len = llen(str);

		if(line_len > width) {
			if(data->wrap) {
				for(iw = 0, tw = line_len / width; iy != height && iw != tw; iy++, iw++) {
					wmemcpy(out->buff + x + (y + iy) * out->width, str + iw * width, width);
				}
				if(line_len % width != 0 && iy != height) {
					wmemcpy(out->buff + x + gravity_shift(data->gravity, width, line_len - iw * width) + (y + iy) * out->width,
							str + iw * width, line_len - iw * width);
				}
				else {
					iy--;
				}
			}
			else {
				wmemcpy(out->buff + x + (y + iy) * out->width, str, width);
			}
		}
		else {
			wmemcpy(out->buff + x + gravity_shift(data->gravity, width, line_len) + (y + iy) * out->width, str,
					line_len);
		}

		if(++iy == height) {
			return;
		}
		str += line_len;
	}
}

void text_view_wrap(view_t *view, BOOLEAN wrap) {
	((text_data_t *) view_get_data(view))->wrap = wrap;
}

void text_view_gravity(view_t *view, text_gravity_t gravity) {
	((text_data_t *) view_get_data(view))->gravity = gravity;
}


void destroy_text(void *data) {
	text_data_t *tdata = data;

	if(tdata->string) {
		free(tdata->string);
	}
	free(data);
}

view_t *create_text_view(UINT x, UINT y, UINT width, UINT height) {
	view_t *view = create_view(x, y, width, height);
	text_data_t *data = calloc(1, sizeof *data);

	view_set_data(view, data);
	view_set_render_function(view, render_text);
	view_set_destroy_function(view, destroy_text);

	return view;
}

void text_view_text(view_t *view, WCHAR *text) {
	text_data_t *data = view_get_data(view);

	if(data->string != NULL) {
		free(data->string);
	}

	if(text != NULL) {
		data->string = malloc((wcslen(text) + 1) * sizeof *data->string);
		wcscpy(data->string, text);
	}
	else {
		data->string = NULL;
	}
}