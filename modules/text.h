#ifndef DIX_TEXT_H
#define DIX_TEXT_H

#include "../dix.h"

view_t *create_text_view(UINT x, UINT y, UINT width, UINT height);
void text_view_text(view_t *view, WCHAR *text);

#endif //DIX_TEXT_H
