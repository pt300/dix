#include <stdio.h>
#include <wchar.h>
#include "dix.h"

typedef struct {
	HWND console;
	HANDLE buffer;
	view_t *child;
	UINT width, height;
	/*Data to restore*/
	HANDLE old_buffer;
	WCHAR title[250];
} screen_t;

screen_t screen = {};

dix_status_t init_dix() {
	CONSOLE_SCREEN_BUFFER_INFO info;
	SHORT width, height;

	if((screen.console = GetConsoleWindow()) == NULL ||
	   (screen.old_buffer = GetStdHandle(STD_OUTPUT_HANDLE)) == NULL) {
		return RIP;
	}

	GetConsoleScreenBufferInfo(screen.old_buffer, &info);

	width = (SHORT) (info.srWindow.Right - info.srWindow.Left + 1);
	height = (SHORT) (info.srWindow.Bottom - info.srWindow.Top + 1);

	screen.buffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(screen.buffer);
	SetStdHandle(STD_OUTPUT_HANDLE, screen.buffer);

	set_screen_size(width, height);

	GetConsoleTitleW(screen.title, 250);

	return OK;
}

void deinit_dix() {
	SetConsoleActiveScreenBuffer(screen.old_buffer);
	SetConsoleTitleW(screen.title);
	SetStdHandle(STD_OUTPUT_HANDLE, screen.old_buffer);

	CloseHandle(screen.buffer);
	CloseHandle(screen.old_buffer);
}

static void normalize_size(COORD *max, SMALL_RECT *rekt, SHORT desired_x, SHORT desired_y) {
	rekt->Right = (SHORT) (desired_x < max->X ? desired_x : max->X);
	rekt->Bottom = (SHORT) (desired_y < max->Y ? desired_y : max->Y);
}

/*
 * do something
 */
dix_status_t set_screen_size(SHORT width, SHORT height) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	COORD size = {.X = width, .Y = height};
	COORD max;
	SMALL_RECT rekt = {.Left = 0, .Top = 0};

	if(width < 2 ||
	   height < 2) {
		return RIP;
	}

	screen.width = width;
	screen.height = height;

	max = GetLargestConsoleWindowSize(screen.buffer);
	GetConsoleScreenBufferInfo(screen.buffer, &info);
	if(width > info.dwSize.X) {
		if(height > info.dwSize.Y) {
			SetConsoleScreenBufferSize(screen.buffer, size);
			normalize_size(&max, &rekt, (SHORT) (width - 1), (SHORT) (height - 1));
			SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);
		}
		else {
			size.Y = info.dwSize.Y;
			rekt.Bottom = (SHORT) (info.dwSize.Y - 1);
			SetConsoleScreenBufferSize(screen.buffer, size);
			normalize_size(&max, &rekt, (SHORT) (width - 1), (SHORT) (info.dwSize.Y - 1));
			SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);

			size.Y = height;
			rekt.Bottom = (SHORT) (height - 1);
			normalize_size(&max, &rekt, (SHORT) (width - 1), (SHORT) (height - 1));
			SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);
			SetConsoleScreenBufferSize(screen.buffer, size);
		}
	}
	else {
		if(height < info.dwSize.Y) {
			normalize_size(&max, &rekt, (SHORT) (width - 1), (SHORT) (height - 1));
			SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);
			SetConsoleScreenBufferSize(screen.buffer, size);
		}
		else {
			size.X = info.dwSize.X;
			rekt.Right = (SHORT) (info.dwSize.X - 1);
			SetConsoleScreenBufferSize(screen.buffer, size);
			normalize_size(&max, &rekt, (SHORT) (info.dwSize.X - 1), (SHORT) (height - 1));
			SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);

			size.X = width;
			rekt.Right = (SHORT) (width - 1);
			normalize_size(&max, &rekt, (SHORT) (width - 1), (SHORT) (height - 1));
			SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);
			SetConsoleScreenBufferSize(screen.buffer, size);
		}
	}

	return OK;
}

void set_screen_title(WCHAR *title) {
	SetConsoleTitleW(title);
}

view_t *attach_to_screen(view_t *view) {
	view_t *previous;

	previous = screen.child;
	screen.child = view;
	if(view != NULL && view != SCREEN) {
		view->parent = SCREEN;
	}

	return previous;
}

void view_set_pos(view_t *view, UINT x, UINT y) {
	view->x = x;
	view->y = y;
}

void view_set_pos_relative(view_t *view, int x, int y) {
	view->x += x;
	view->y += y;
}

view_t *create_view(UINT x, UINT y, UINT width, UINT height) {
	view_t *view = calloc(1, sizeof *view);

	view->x = x;
	view->y = y;
	view->width = width;
	view->height = height;
	view->data = NULL;
	view->view_render = NULL;
	view->view_destroy = NULL;

	return view;
}

void *view_get_data(view_t *view) {
	return view->data;
}

void view_set_data(view_t *view, void *data) {
	view->data = data;    //datdaatdatadata
}

void view_set_render_function(view_t *view, view_render_func view_render) {
	view->view_render = view_render;
}

void view_set_destroy_function(view_t *view, view_destroy_func view_destroy) {
	view->view_destroy = view_destroy;
}

void destroy_view(view_t *view) {
	if(view->view_destroy != NULL) {
		view->view_destroy(view);
	}
}

void render_view(view_t *view, render_buf_t *out) {
	if(view->view_render != NULL) {
		view->view_render(view, out);
	}
}

void render(view_t *view) {
	render_buf_t out;
	view_t *win;    //amazing naming
	COORD cursor;
	SHORT x, y, width, height;
	SHORT row;
	DWORD DONTCARE;

	set_screen_size(screen.width, screen.height); //just to be sure I guess

	if(view == NULL || view == SCREEN || view->parent == SCREEN) {
		view = screen.child;
		if(view == NULL) {
			return;
		}
		out.buff = malloc(screen.width * screen.height * sizeof *out.buff);
		out.width = screen.width;
		out.height = screen.height;
		wmemset(out.buff, L' ', screen.width * screen.height);
	}
	else {
		out.buff = malloc(view->parent->width * view->parent->height * sizeof *out.buff);
		out.width = view->parent->width;
		out.height = view->parent->height;
		wmemset(out.buff, L' ', view->parent->width * view->parent->height);
	}

	render_view(view, &out);

	for(win = view->parent, x = 0, y = 0; win != SCREEN; win = view->parent) {
		x += win->x;
		y += win->y;
	}

	width = min(out.width, screen.width - x);
	height = min(out.height, screen.height - y);

	for(row = 0; row < height; row++) {
		cursor.X = x;
		cursor.Y = y + row;
		//SetConsoleCursorPosition(screen->buffer, cursor);
		//WriteConsole(screen->buffer, out.buff + row * out.width, width, &DONTCARE, NULL);
		WriteConsoleOutputCharacterW(screen.buffer, out.buff + row * out.width, width, cursor, &DONTCARE);
	}

	free(out.buff);
}