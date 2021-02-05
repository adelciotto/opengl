#include "sdl2_impl.h"

#include <SDL2/SDL.h>

#define SDL_LOG_ERROR(f_, ...) \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, (f_), ##__VA_ARGS__)

typedef struct
{
    const char      *title;
    int             xPos;
    int             yPos;
    int             width;
    int             height;
    int             isOpen;
    int             redraw;
    void            (*displayFunc)();
    void            (*reshapeFunc)(int width, int height);
    void            (*idleFunc)();
    int             initialized;
    GLContext       context;
    SDL_Window      *sdlWindow;
    SDL_Renderer    *sdlRenderer;
    SDL_Texture     *sdlTexture;
} Window;

INTERNAL Window window = {0};

INTERNAL int initialWindowX         = SDL_WINDOWPOS_UNDEFINED;
INTERNAL int initialWindowY         = SDL_WINDOWPOS_UNDEFINED;
INTERNAL int initialWindowWidth     = 300;
INTERNAL int initialWindowHeight    = 300;

void glutInit(int *pargc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_LOG_ERROR("Unable to initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void glutInitWindowPosition(int x, int y)
{
    initialWindowX = x;
    initialWindowY = y;
}

void glutInitWindowSize(int width, int height)
{
    initialWindowWidth = width;
    initialWindowHeight = height;
}

INTERNAL int initGLContext(GLContext *context)
{
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
    {
        SDL_LOG_ERROR("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        return -1;
    }

    unsigned char *colorBuffer = NULL;
    float *depthBuffer = NULL;
    GLState *state = NULL;

    int components = 4;
    int stride = window.width * components;
    int size = window.height * stride;

    colorBuffer = calloc(size, 1);
    if (colorBuffer == NULL)
    {
        SDL_LOG_ERROR("calloc failed for GLContext colorBuffer");
        goto error;
    }

    depthBuffer = calloc(window.width * window.height, sizeof(float));
    if (depthBuffer == NULL)
    {
        SDL_LOG_ERROR("calloc failed for GLContext depthBuffer");
        goto error;
    }

    state = createGLState(window.width, window.height);
    if (state == NULL)
    {
        SDL_LOG_ERROR("failed to allocate new GLState");
        goto error;
    }

    context->framebufferWidth = window.width;
    context->framebufferHeight = window.height;
    context->maxDeviceWidth = dm.w;
    context->maxDeviceHeight = dm.h;
    context->refreshRate = dm.refresh_rate;
    context->colorBuffer = colorBuffer;
    context->components = components;
    context->stride = stride;
    context->size = size;
    context->depthBuffer = depthBuffer;
    context->state = state;

    return 0;

error:
    if (depthBuffer != NULL)
    {
        free(depthBuffer);
    }

    if (colorBuffer != NULL)
    {
        free(colorBuffer);
    }

    return -1;
}

INTERNAL void glutQuit()
{
    if (window.context.state != NULL)
    {
        free(window.context.state);
    }

    if (window.context.depthBuffer != NULL)
    {
        free(window.context.depthBuffer);
    }

    if (window.context.colorBuffer != NULL)
    {
        free(window.context.colorBuffer);
    }

    if (window.sdlTexture != NULL)
    {
        SDL_DestroyTexture(window.sdlTexture);
    }

    if (window.sdlRenderer != NULL)
    {
        SDL_DestroyRenderer(window.sdlRenderer);
    }

    if (window.sdlWindow != NULL)
    {
        SDL_DestroyWindow(window.sdlWindow);
    }

    SDL_Quit();
}

int glutCreateWindow(const char *title)
{
    if (window.initialized)
    {
        return 1;
    }

    int width = initialWindowWidth;
    int height = initialWindowHeight;
    int x = initialWindowX;
    int y = initialWindowY;

    window.sdlWindow = SDL_CreateWindow(title, x, y, width, height,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window.sdlWindow == NULL)
    {
        SDL_LOG_ERROR("Unable to create SDL_Window: %s", SDL_GetError());
        goto error;
    }

    window.sdlRenderer = SDL_CreateRenderer(window.sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    if (window.sdlRenderer == NULL)
    {
        SDL_LOG_ERROR("Unable to create SDL_Renderer: %s", SDL_GetError());
        goto error;
    }

    window.sdlTexture = SDL_CreateTexture(window.sdlRenderer, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING, width, height);
    if (window.sdlTexture == NULL)
    {
        SDL_LOG_ERROR("Unable to create SDL_Texture: %s", SDL_GetError());
        goto error;
    }

    window.width = width;
    window.height = height;
    window.xPos = x;
    window.yPos = y;

    if (initGLContext(&window.context) != 0)
    {
        SDL_LOG_ERROR("Unable to initialize GLContext");
        goto error;
    }

    setGLContext(&window.context);

    window.initialized = 1;
    window.redraw = 1;
    window.isOpen = 1;

    return 1;

error:
    glutQuit();

    exit(EXIT_FAILURE);
}

void glutSetWindowData(void *data)
{

}

void *glutGetWindowData()
{

}

void glutReshapeFunc(void (*func)(int width, int height))
{
    assert(window.initialized);
    if (!window.initialized)
    {
        return;
    }

    window.reshapeFunc = func;
}

void glutDisplayFunc(void (*func)())
{
    assert(window.initialized);
    if (!window.initialized)
    {
        return;
    }

    window.displayFunc = func;
}

void glutIdleFunc(void (*func)())
{
    assert(window.initialized);
    if (!window.initialized)
    {
        return;
    }

    window.idleFunc = func;
}

void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y))
{

}

void glutSpecialFunc(void (*func)(int key, int x, int y))
{

}

void glutPassiveMotionFunc(void (*func)(int x, int y))
{

}

void glutSwapBuffers()
{
    assert(window.initialized);
    if (!window.initialized)
    {
        return;
    }

    SDL_UpdateTexture(window.sdlTexture, NULL, window.context.colorBuffer, window.context.stride);

    SDL_RenderClear(window.sdlRenderer);

    SDL_Rect rect = {0, 0, window.context.framebufferWidth, window.context.framebufferHeight};
    SDL_RenderCopy(window.sdlRenderer, window.sdlTexture, NULL, &rect);

    SDL_RenderPresent(window.sdlRenderer);

}

void glutPostRedisplay()
{
    assert(window.initialized);
    if (!window.initialized)
    {
        return;
    }

    window.redraw = 1;
}

int glutGet(int query)
{

}

INTERNAL void handleWindowResize(int width, int height)
{
    window.width = width;
    window.height = height;

    if (window.reshapeFunc != NULL)
    {
        window.reshapeFunc(window.width, window.height);
    }
}

void glutMainLoop()
{
    assert(window.initialized);
    if (!window.initialized)
    {
        return;
    }

    while (window.isOpen)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
               window.isOpen = 0;
            }

            if (event.type == SDL_WINDOWEVENT)
            {
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                        handleWindowResize(event.window.data1, event.window.data2);
                        break;
                }

                // Window events will re-trigger the display func
                glutPostRedisplay();
            }
        }

        if (window.idleFunc != NULL)
        {
            window.idleFunc();
        }

        if (window.redraw && window.displayFunc != NULL)
        {
            window.displayFunc();
            window.redraw = 0;
        }
    }

    glutQuit();
}

void resizeGLFramebuffer(int width, int height)
{
    assert(window.initialized);

    SDL_Texture *sdlTexture = NULL;
    unsigned char *colorBuffer = NULL;
    float *depthBuffer = NULL;

    sdlTexture = SDL_CreateTexture(window.sdlRenderer, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING, width, height);
    if (sdlTexture == NULL)
    {
        SDL_LOG_ERROR("Unable to resize GL framebuffer: %s", SDL_GetError());
        goto error;
    }

    int stride = width * window.context.components;
    int size = height * stride;

    colorBuffer = calloc(size, 1);
    if (colorBuffer == NULL)
    {
        SDL_LOG_ERROR("Unable to resize GL framebuffer: calloc failed for colorBuffer");
        goto error;
    }

    depthBuffer = calloc(width * height, sizeof(float));
    if (depthBuffer == NULL)
    {
        SDL_LOG_ERROR("Unable to resize GL framebuffer: calloc failed for depthBuffer");
        goto error;
    }

    free(window.context.depthBuffer);
    free(window.context.colorBuffer);
    SDL_DestroyTexture(window.sdlTexture);

    window.sdlTexture = sdlTexture;
    window.context.framebufferWidth = width;
    window.context.framebufferHeight = height;
    window.context.colorBuffer = colorBuffer;
    window.context.stride = stride;
    window.context.size = size;
    window.context.depthBuffer = depthBuffer;

    return;

error:
    if (colorBuffer != NULL)
    {
        free(colorBuffer);
    }

    if (sdlTexture != NULL)
    {
        SDL_DestroyTexture(sdlTexture);
    }
}

