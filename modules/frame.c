#include "frame.h"

typedef struct {
	WCHAR borderchar;
	boolean border;
	UINT child_count;
	view_t **children;
} frame_data_t;

void render_frame(view_t *view, render_buf_t *out) {
	frame_data_t *data;
	UINT inner_w, inner_h, start_x, start_y;
	UINT y, max_x, max_y;
	int i;
	render_buf_t ch;

	data = view_get_data(view);

	max_x = min(view->width + view->x, out->width);
	max_y = min(view->height + view->y, out->height);
	start_x = view->x;
	start_y = view->y;
	inner_w = max_x - start_x;
	inner_h = max_y - start_y;

	/*
	 * What if start is out of view?
	 */
	if(inner_w == 0 || inner_h == 0) {
		return;
	}

	if(data->border) { //eh
		wmemset(out->buff + start_x + out->width * start_y, data->borderchar, inner_w);
		wmemset(out->buff + start_x + out->width * (start_y + inner_h - 1), data->borderchar, inner_w);
		inner_h -= 2;
		inner_w -= 2;
		for(i = 0; i < inner_h; i++) {
			out->buff[start_x + (i + start_y + 1) * out->width] = data->borderchar;
			out->buff[start_x + (i + start_y + 1) * out->width + inner_w + 1] = data->borderchar;
		}
		start_x++;
		start_y++;
	}

	if(inner_h != 0 && inner_w != 0 && data->child_count > 0) {
		ch.height = inner_h;
		ch.width = inner_w;
		ch.buff = malloc(inner_h * inner_w * sizeof *ch.buff);
		wmemset(ch.buff, L' ', inner_h * inner_w);

		for(i = 0; i < data->child_count; i++) {
			render_view(data->children[i], &ch);
		}

		for(y = 0; y < inner_h - 1; y++) {
			memcpy(out->buff + start_x + (start_y + y) * out->width, ch.buff + (y * ch.width),
				   inner_w * sizeof *ch.buff);
		}

		free(ch.buff);
	}
}

void destroy_frame(void *data) {
	free(((frame_data_t *) data)->children);
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

void frame_view_border_char(view_t *view, WCHAR character) {
	((frame_data_t *) view_get_data(view))->borderchar = character;
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
	frame_data_t *data;
	BOOLEAN removed;

	data = view_get_data(view);

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

	for(i = data->child_count; i != 0; i--) {
		destroy_view(data->children[i]);
	}

	data->child_count = 0;
	data->children = realloc(data->children, sizeof *data->children);
}