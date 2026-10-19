// Exercise real SDL screen resizing, registered UI scales, mode descriptors,
// and the same 3D zoom calculation used by software and OpenGL vertices.
#include "Shock.h"
#include "ShockBitmap.h"
#include "InitMac.h"
#include "gr2ss.h"
#include <assert.h>
#include <math.h>
#include <string.h>

SDL_Window *window;
SDL_Renderer *renderer;
bool fullscreenActive;
intptr_t *gScreenAddress;
int32_t gScreenRowbytes, gScreenWide, gScreenHigh;
grs_drvcap grd_mode_cap;
grs_drvcap *grd_cap = &grd_mode_cap;
fix pixel_ratio;
extern SDL_Surface *drawSurface, *offscreenDrawSurface;
#define MODE_ID(w,h,id) id,
static const int ids[] = { SHOCK_VIDEO_MODES(MODE_ID) };
#undef MODE_ID

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    assert(SDL_setenv("SDL_VIDEODRIVER", "dummy", 1) == 0);
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);
    window = SDL_CreateWindow("Video test", 0, 0, 640, 480, SDL_WINDOW_HIDDEN);
    assert(window);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    assert(renderer);
    assert(SHOCK_VIDEO_MODE_COUNT < MAX_USE_MODES);
    assert(shock_video_sizes[SHOCK_DECK_MODE][0] == 1280);
    assert(shock_video_sizes[SHOCK_DECK_MODE][1] == 800);
    assert(!shock_valid_video_mode(-1) && !shock_valid_video_mode(SHOCK_VIDEO_MODE_COUNT));
    gr2ss_register_init(0, 320, 200);
    for (int i = 1; i < SHOCK_VIDEO_MODE_COUNT; ++i)
        gr2ss_register_mode(0, shock_video_sizes[i][0], shock_video_sizes[i][1]);
    grd_mode_cap.aspect = FIX_UNIT;
    for (int pass = 0; pass < 2; ++pass) {
        for (int i = 0; i < SHOCK_VIDEO_MODE_COUNT; ++i) {
            int w = shock_video_sizes[i][0], h = shock_video_sizes[i][1];
            assert(grd_mode_info[ids[i]].w == w && grd_mode_info[ids[i]].h == h);
            ChangeScreenSize(w, h);
            assert(gScreenWide == w && gScreenHigh == h);
            assert(drawSurface && offscreenDrawSurface);
            assert(drawSurface->w == w && drawSurface->h == h);
            assert(offscreenDrawSurface->w == w && offscreenDrawSurface->h == h);
            memset(drawSurface->pixels, 42, drawSurface->pitch * h);
            memset(offscreenDrawSurface->pixels, 24, offscreenDrawSurface->pitch * h);
            assert(((unsigned char *)drawSurface->pixels)[drawSurface->pitch * h - 1] == 42);
            int lw, lh;
            SDL_RenderGetLogicalSize(renderer, &lw, &lh);
            assert(lw == w && lh == h);
            convert_use_mode = i;
            assert(abs(SCONV_X(320) - w) <= 1 && abs(SCONV_Y(200) - h) <= 1);
            for (int degrees = SHOCK_FOV_MIN; degrees <= SHOCK_FOV_MAX; ++degrees) {
                fix zoom = g3_get_zoom('X', (65536 * degrees) / 360, w, h);
                double expected = tan(degrees * 3.141592653589793 / 360) * (w >= h ? (double)h/w : 1);
                assert(fabs((double)zoom/FIX_UNIT - expected) < .003);
                assert(shock_fov_from_slider(shock_fov_to_slider(degrees)) == degrees);
            }
        }
    }
    assert(shock_fov_from_slider(-1) == SHOCK_FOV_MIN);
    assert(shock_fov_from_slider(101) == SHOCK_FOV_MAX);
    SDL_FreeSurface(drawSurface);
    SDL_FreeSurface(offscreenDrawSurface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    puts("PASS video: 13 modes through 4K, Deck native, repeated SDL surfaces/scales, horizontal FOV and slider round trips");
}
