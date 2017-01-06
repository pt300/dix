#ifndef DIX_FRAME_H
#define DIX_FRAME_H

#include "../dix.h"

view_t *create_frame_view(UINT x, UINT y, UINT width, UINT height);
void frame_view_border(view_t *view, boolean enable);
void frame_view_border_char(view_t *view, WCHAR character);
void frame_view_appendv(view_t *frame, view_t *view);
boolean frame_view_removev(view_t *frame, view_t *child);
boolean frame_view_destroyv(view_t *frame, view_t *child);
void frame_view_removev_all(view_t *frame);
void frame_view_destroyv_all(view_t *frame);

#endif //DIX_FRAME_H
