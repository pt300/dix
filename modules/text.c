#include "text.h"

/*
 * TODO: text position
 */
typedef struct {
	WCHAR *string;
} text_data_t;

void render_text(view_t *view, render_buf_t *out) {
	WCHAR *str = ((text_data_t *) view->data)->string;
	UINT x, y, max_x, max_y;

	if(str == NULL) {
		return;
	}

	max_x = min(view->width + view->x, out->width);
	max_y = min(view->height + view->y, out->height);
	x = view->x;
	y = view->y;

	while(*str != L'\0' && y < max_y) {
		if(*str == L'\n') {
			y++;
			x = view->x;
		}
		else {
			if(x < max_x) {
				out->buff[x++ + y * out->width] = *str;
			}
		}
		str++;
	}
}

void destroy_text(void *data) {
	if(((text_data_t *) data)->string) {
		free(((text_data_t *) data)->string);
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
	text_data_t *data = view->data;

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