#include "buffer.h"

typedef struct {
	WCHAR *data;
	UINT width, height;
} buffer_data_t;

void render_buffer(view_t *view, render_buf_t *out) {
	buffer_data_t *data;
	UINT x, y, max_x, max_y, width, height, i;

	data = view_get_data(view);
	max_x = min(view->width + view->x, out->width);
	max_y = min(view->height + view->y, out->height);
	x = view->x;
	y = view->y;
	width = max_x - x;
	height = max_y - y;

	if(max_x - x > data->width) {
		x += (width - data->width) / 2;
		width = data->width;
	}

	if(max_y - y > data->height) {
		y += (height - data->height) / 2;
		height = data->height;
	}

	for(i = 0; i < height; i++) {
		wmemcpy(out->buff + x + (i + y) * out->width, data->data + data->width * i, width);
	}

}

void destroy_buffer(void *data) {
	free(((buffer_data_t *) data)->data);
	free(data);
}

view_t *create_buffer_view(UINT x, UINT y, UINT width, UINT height) {
	view_t *view = create_view(x, y, width, height);
	buffer_data_t *data;

	data = calloc(1, sizeof *data);
	data->width = width;
	data->height = height;
	data->data = calloc(width * height, sizeof *data->data);

	view_set_data(view, data);
	view_set_render_function(view, render_buffer);
	view_set_destroy_function(view, destroy_buffer);

	return view;
}

/*
 * it's all dumb
 */

void buffer_view_set_size(view_t *view, UINT width, UINT height) {
	buffer_data_t *data;

	data = view_get_data(view);
	data->width = width;
	data->height = height;
	data->data = realloc(data->data, width * height * sizeof *data->data);
}

void buffer_view_set_data(view_t *view, WCHAR *data) {
	buffer_data_t *vdata;

	vdata = view_get_data(view);
	wmemcpy(vdata->data, data, vdata->width * vdata->height);
}