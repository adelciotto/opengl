#include "sdl2_impl.h"

struct GLState
{
    GLenum      errFlag;

    GLint       viewportOX;
    GLint       viewportOY;
    GLsizei     viewportW;
    GLsizei     viewportH;
    GLclampf    viewportN;
    GLclampf    viewportF;

    GLclampf    clearColorR;
    GLclampf    clearColorG;
    GLclampf    clearColorB;
    GLclampf    clearColorA;
};

INTERNAL GLContext *context = NULL;

GLState *createGLState(int framebufferWidth, int framebufferHeight)
{
    GLState *state = malloc(sizeof(GLState));
    if (state == NULL)
    {
        return NULL;
    }

    state->errFlag = GL_NO_ERROR;

    state->viewportOX = framebufferWidth / 2;
    state->viewportOY = framebufferHeight / 2;
    state->viewportW = framebufferWidth;
    state->viewportH = framebufferHeight;
    state->viewportN = 0.0f;
    state->viewportF = 1.0f;

    state->clearColorR = 0.0f;
    state->clearColorG = 0.0f;
    state->clearColorB = 0.0f;
    state->clearColorA = 1.0f;

    return state;
}

void setGLContext(GLContext *_context)
{
    assert(_context != NULL);

    context = _context;
}

INTERNAL void setErrFlag(GLenum errFlag)
{
    if (context->state->errFlag == GL_NO_ERROR)
    {
        context->state->errFlag = errFlag;
    }
}

INTERNAL GLclampf clampf(GLclampf val, GLclampf min, GLclampf max)
{
    const GLclampf t = val < min ? min : val;

    return t > max ? max : t;
}

GLenum APIENTRY glGetError(void)
{
    if (context == NULL) return GL_NO_ERROR;

    GLenum lastErr = context->state->errFlag;

    context->state->errFlag = GL_NO_ERROR;

    return lastErr;
}

void APIENTRY glClear(GLbitfield mask)
{
    if (context == NULL) return;

    if (mask & GL_COLOR_BUFFER_BIT)
    {
        GLubyte red = context->state->clearColorR * 255;
        GLubyte green = context->state->clearColorG * 255;
        GLubyte blue = context->state->clearColorB * 255;

        int size = context->size;
        int components = context->components;
        GLubyte *colorBuf = context->colorBuffer;

        for (int i = 0; i < size; i += components)
        {
            colorBuf[i] = blue;
            colorBuf[i+1] = green;
            colorBuf[i+2] = red;
            colorBuf[i+3] = 255;
        }
    }
    else
    {
        setErrFlag(GL_INVALID_VALUE);

        return;
    }
}

void APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    if (context == NULL) return;

    context->state->clearColorR = clampf(red, 0.0f, 1.0f);
    context->state->clearColorG = clampf(green, 0.0f, 1.0f);
    context->state->clearColorB = clampf(blue, 0.0f, 1.0f);
    context->state->clearColorA = clampf(alpha, 0.0f, 1.0f);
}

void APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    if (context == NULL) return;

    if (width < 0 || height < 0)
    {
        setErrFlag(GL_INVALID_VALUE);

        return;
    }

    width = width < context->maxDeviceWidth ? width : context->maxDeviceWidth;
    height = height < context->maxDeviceHeight ? height : context->maxDeviceHeight;

    context->state->viewportOX = x + width / 2;
    context->state->viewportOY = y + height / 2;

    if (width != context->state->viewportW || height != context->state->viewportH)
    {
        resizeGLFramebuffer(width, height);
    }

    context->state->viewportW = width;
    context->state->viewportH = height;
}

void APIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                           GLenum format, GLenum type, GLvoid *pixels)
{
}
