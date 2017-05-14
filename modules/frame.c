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

#include "frame.h"

typedef struct {
	WCHAR borderchar;
	WCHAR left, top, right, bottom;
	WCHAR top_left, top_right, bottom_right, bottom_left;
	border_style_t border_style;
	boolean border;
	UINT child_count;
	view_t **children;
} frame_data_t;

void render_frame(view_t *view, render_buf_t *out) {
	frame_data_t *data;
	UINT actual_width, actual_height;
	UINT child_width, child_height;
	UINT x, y;
	size_t i;
	render_buf_t ch;

	data = view_get_data(view);

	if(view->x >= out->width ||
	   view->y >= out->height) {
		return;
	}

	child_width = actual_width = min(out->width - view->x, view->width);
	child_height = actual_height = min(out->height - view->y, view->height);

	x = view->x;
	y = view->y;


	/*
	 * Border rendering ladder ifs, heh
	 * TODO: Implement corner borders
	 */
	if(data->border) {
		if(data->border_style == ONECHAR) {
			wmemset(out->buff + x + out->width * y, data->borderchar, actual_width);
			for(i = 1; i < actual_height - 1; i++) {
				out->buff[x + (i + y) * out->width] = data->borderchar;
			}

			child_width--;
			child_height--;

			if(child_height != 0) {
				wmemset(out->buff + x + out->width * (y + child_height), data->borderchar, actual_width);

				child_height--;
			}

			if(child_width != 0) {
				for(i = 1; i < actual_height - 1; i++) {
					out->buff[x + (i + y) * out->width + child_width] = data->borderchar;
				}

				child_width--;
			}

			x++;
			y++;
		}
		else {
			wmemset(out->buff + x + out->width * y, data->top, actual_width);
			for(i = 1; i < actual_height - 1; i++) {
				out->buff[x + (i + y) * out->width] = data->left;
			}

			child_width--;
			child_height--;

			if(child_height != 0) {
				wmemset(out->buff + x + out->width * (y + child_height), data->bottom, actual_width);

				child_height--;
			}

			if(child_width != 0) {
				for(i = 1; i < actual_height - 1; i++) {
					out->buff[x + (i + y) * out->width + child_width] = data->right;
				}

				child_width--;
			}

			x++;
			y++;
		}
	}

	if(child_height != 0 && child_width != 0 && data->child_count > 0) {
		ch.height = child_height;
		ch.width = child_width;
		ch.buff = malloc(child_height * child_width * sizeof *ch.buff);
		wmemset(ch.buff, L' ', child_height * child_width);

		for(i = 0; i < data->child_count; i++) {
			render_view(data->children[i], &ch);
		}

		for(i = 0; i < child_height; i++) {
			memcpy(out->buff + x + (y + i) * out->width, ch.buff + (y * ch.width),
				   child_width * sizeof *ch.buff);
		}

		free(ch.buff);
	}
}

void destroy_frame(void *data) {
	frame_data_t *vdata;
	UINT i;

	vdata = data;
	for(i = 0; i < vdata->child_count; i++) {
		destroy_view(vdata->children[i]);
	}
	free(vdata->children);
	free(data);
}

view_t *create_frame_view(UINT x, UINT y, UINT width, UINT height) {
	view_t *view = create_view(x, y, width, height);
	frame_data_t *data = calloc(1, sizeof *data);

	data->children = malloc(sizeof(view_t));

	view_set_data(view, data);
	view_set_render_function(view, render_frame);
	view_set_destroy_function(view, destroy_frame);

	return view;
}

void frame_view_border(view_t *view, boolean enable) {
	((frame_data_t *) view_get_data(view))->border = enable;
}

void frame_view_border_style(view_t *view, border_style_t style) {
	((frame_data_t *) view_get_data(view))->border_style = style;
}

void frame_view_border_char(view_t *view, WCHAR character) {
	((frame_data_t *) view_get_data(view))->borderchar = character;
}

void frame_view_border_sides(view_t *view, WCHAR left, WCHAR top, WCHAR right, WCHAR bottom) {
	frame_data_t *data = view_get_data(view);

	data->left = left;
	data->top = top;
	data->right = right;
	data->bottom = bottom;
}

void frame_view_border_corners(view_t *view, WCHAR top_left, WCHAR top_right, WCHAR bottom_right, WCHAR bottom_left) {
	frame_data_t *data = view_get_data(view);

	data->top_left = top_left;
	data->top_right = top_right;
	data->bottom_right = bottom_right;
	data->bottom_left = bottom_left;
}

void frame_view_appendv(view_t *view, view_t *child) {
	frame_data_t *data;

	data = view_get_data(view);

	data->child_count++;
	data->children = realloc(data->children, data->child_count * sizeof *data->children);
	data->children[data->child_count - 1] = child;
	child->parent = view;
}

BOOLEAN frame_view_removev(view_t *view, view_t *child) {
	frame_data_t *data;
	UINT i;
	BOOLEAN removed;

	data = view_get_data(view);

	for(i = 0, removed = FALSE; i < data->child_count; i++) {
		if(data->children[i] == child) {
			removed = TRUE;
			while(i < data->child_count - 1) {
				data->children[i] = data->children[++i];
			}
			data->child_count--;
			data->children = realloc(data->children, data->child_count * sizeof *data->children);
			break;
		}
	}

	return removed;
}

BOOLEAN frame_view_destroyv(view_t *view, view_t *child) {
	BOOLEAN removed;

	if((removed = frame_view_removev(view, child)) == TRUE) {
		destroy_view(child);
	}

	return removed;
}

void frame_view_removev_all(view_t *view) {
	frame_data_t *data;

	data = view_get_data(view);

	data->children = realloc(data->children, sizeof *data->children);
	data->child_count = 0;
}

void frame_view_destroyv_all(view_t *view) {
	frame_data_t *data;
	UINT i;

	data = view_get_data(view);

	for(i = 0; i < data->child_count; i++) {
		destroy_view(data->children[i]);
	}

	data->child_count = 0;
	data->children = realloc(data->children, sizeof *data->children);
}