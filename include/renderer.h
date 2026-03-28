#ifndef RENDERER_H
#define RENDERER_H

#include "display.h"
#include <stdint.h>

// forward defining the struct
// its content is not defined yet
// it is defined in renderer
typedef struct Renderer Renderer;

// wrong way of initialzing the renderer which i learned the hard way
// int renderer_init(Renderer *r, int scale);
//
Renderer *renderer_create(int scale);
void renderer_clear(Renderer *r);
void renderer_draw(Display *disp, Renderer *r);
void renderer_present(Renderer *r);
void renderer_destroy(Renderer *r);

#endif
