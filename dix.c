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
	UINT width, height;

	if((screen.console = GetConsoleWindow()) == NULL ||
	   (screen.old_buffer = GetStdHandle(STD_OUTPUT_HANDLE)) == NULL) {
		return RIP;
	}

	GetConsoleScreenBufferInfo(screen.old_buffer, &info);

	width = (UINT) (info.srWindow.Right - info.srWindow.Left + 1);
	height = (UINT) (info.srWindow.Bottom - info.srWindow.Top + 1);

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

/*
 * So, the thing is
 * width and height of buffer cannot be less than the width and height of the console window
 */
dix_status_t set_screen_size(UINT width, UINT height) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	COORD size, max;
	SMALL_RECT rekt = {.Left = 0, .Top = 0};


	if(width < 2 ||
	   height < 2) {
		return RIP;
	}

	max = GetLargestConsoleWindowSize(screen.buffer);

	size.X = min(width, max.X);
	rekt.Right = (SHORT) (size.X - 1);
	screen.width = (UINT) size.X;

	GetConsoleScreenBufferInfo(screen.buffer, &info);

	size.Y = info.dwSize.Y;
	rekt.Bottom = (SHORT) (info.dwSize.Y - 1);

	if(screen.width > info.dwSize.X) { //if buffer will be larger
		SetConsoleScreenBufferSize(screen.buffer, size);
		SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);
	}
	else if(screen.width != info.dwSize.X) { //and if smaller
		SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);
		SetConsoleScreenBufferSize(screen.buffer, size);
	}

	size.Y = min(height, max.Y);
	rekt.Bottom = (SHORT) (size.Y - 1);
	screen.height = (UINT) size.Y;

	if(screen.height > info.dwSize.Y) {
		SetConsoleScreenBufferSize(screen.buffer, size);
		SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);
	}
	else if(screen.height != info.dwSize.Y) {
		SetConsoleWindowInfo(screen.buffer, TRUE, &rekt);
		SetConsoleScreenBufferSize(screen.buffer, size);
	}

	return OK;
}

void set_cursor_position(UINT x, UINT y) {
	COORD pos = {.X = (SHORT) x, .Y = (SHORT) y};

	SetConsoleCursorPosition(screen.buffer, pos);
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
	view_t *view = malloc(sizeof *view);

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
		view->view_destroy(view->data);
	}
	free(view);
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

	width = (SHORT) min(out.width, screen.width - x);
	height = (SHORT) min(out.height, screen.height - y);

	for(row = 0; row < height; row++) {
		cursor.X = x;
		cursor.Y = y + row;

		WriteConsoleOutputCharacterW(screen.buffer, out.buff + row * out.width, (DWORD) width, cursor, &DONTCARE);
	}

	free(out.buff);
}