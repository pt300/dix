#ifndef DIX_BUFFER_H
#define DIX_BUFFER_H

#include "../dix.h"

view_t *create_buffer_view(UINT x, UINT y, UINT width, UINT height);
void buffer_view_set_size(view_t *view, UINT width, UINT height);
void buffer_view_set_data(view_t *view, WCHAR *data);


#endif //DIX_BUFFER_H
