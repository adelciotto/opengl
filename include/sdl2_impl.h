#ifndef SDL2_IMPL_H
#define SDL2_IMPL_H

#include "glut.h"

#include <assert.h>
#include <stdlib.h>

#define INTERNAL static

typedef struct GLState GLState;

GLState *createGLState(int framebufferWidth, int framebufferHeight);

typedef struct
{
    int             framebufferWidth;
    int             framebufferHeight;
    int             maxDeviceWidth;
    int             maxDeviceHeight;
    int             refreshRate;
    unsigned char   *colorBuffer;
    int             components;
    int             stride;
    int             size;
    float           *depthBuffer;
    GLState         *state;
} GLContext;

void setGLContext(GLContext *context);

void resizeGLFramebuffer(int width, int height);

#endif // SDL2_IMPL_H
