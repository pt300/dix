#ifndef DIX_FRAME_H
#define DIX_FRAME_H

#include "../dix.h"

/*
 * implement
 */
typedef enum {
	ONECHAR,
	SIDES,
	SIDESANDCORNERS
} border_style_t;

view_t *create_frame_view(UINT x, UINT y, UINT width, UINT height);
void frame_view_border(view_t *view, boolean enable);
void frame_view_border_char(view_t *view, WCHAR character);
void frame_view_appendv(view_t *view, view_t *child);
boolean frame_view_removev(view_t *view, view_t *child);
boolean frame_view_destroyv(view_t *view, view_t *child);
void frame_view_removev_all(view_t *view);
void frame_view_destroyv_all(view_t *view);

#endif //DIX_FRAME_H
