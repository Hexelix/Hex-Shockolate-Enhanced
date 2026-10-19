// Real offscreen OpenGL shaders/framebuffers; no game data is needed.
#include "../src/MacSrc/OpenGL.cc"
#include <cassert>
extern "C" {
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Palette *sdlPalette;
errtype render_run(void) { return OK; }
}
uchar wrapper_screenmode_hack;

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    SDL_setenv("SDL_VIDEODRIVER", "offscreen", 1);
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    window = SDL_CreateWindow("GL test", 0, 0, 1280, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    assert(window);
    SDL_GLContext initial = SDL_GL_CreateContext(window);
    assert(initial);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    assert(renderer);
    SDL_RenderSetLogicalSize(renderer, 1280, 800);
    SDL_GL_MakeCurrent(window, initial);
    assert(init_opengl() == 0);
    for (int pass = 0; pass < 2; ++pass) {
        for (int i = 5; i < SHOCK_VIDEO_MODE_COUNT; ++i) {
            int w = shock_video_sizes[i][0], h = shock_video_sizes[i][1];
            SDL_RenderSetLogicalSize(renderer, w, h);
            // Physical size deliberately stays fixed, like desktop fullscreen.
            opengl_resize(1280, 800);
            assert(backupBuffer.width == w && backupBuffer.height == h);
            GLuint buffer = backupBuffer.frameBuffer;
            opengl_resize(1280, 800);
            assert(backupBuffer.frameBuffer == buffer);
            BindFrameBuffer(&backupBuffer);
            assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
            glClearColor(1, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            unsigned char pixel[4] = {};
            glReadPixels(w-1, h-1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
            assert(pixel[0] == 255 && pixel[1] == 0 && pixel[2] == 0);
            assert(glGetError() == GL_NO_ERROR);
            BindFrameBuffer(nullptr);
        }
    }
    SDL_FreePalette(opaquePalette);
    SDL_FreePalette(transparentPalette);
    SDL_GL_DeleteContext(context);
    SDL_GL_DeleteContext(initial);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    puts("PASS OpenGL: real shaders and repeated widescreen/4K framebuffer allocation, fullscreen logical resizing and pixel readback");
}
