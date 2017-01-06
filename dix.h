#ifndef DIX_H
#define DIX_H

#include <windows.h>
#include <wchar.h>

#define SCREEN (void *)1 //flawless

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
dix_status_t set_screen_size(SHORT width, SHORT height);
void set_screen_title(WCHAR *title);
view_t *attach_to_screen(view_t *view);

view_t *create_view(UINT x, UINT y, UINT width, UINT height);
void *view_get_data(view_t *view);
void view_set_data(view_t *view, void *data);
void view_set_render_function(view_t *view, view_render_func module_render);
void view_set_destroy_function(view_t *view, view_destroy_func module_destroy);
void view_set_pos(view_t *view, UINT x, UINT y);
void view_set_pos_relative(view_t *view, int x, int y);
void destroy_view(view_t *view);
void render_view(view_t *view, render_buf_t *out);

void render(view_t *view);

//basic modules
#include "modules/frame.h"
#include "modules/text.h"

#endif //DIX_H
