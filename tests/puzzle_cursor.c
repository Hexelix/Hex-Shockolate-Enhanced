// Real resource cursor loading/scaling, with a synthetic 9x9 puzzle symbol.
#include "Shock.h"
#include "gr2ss.h"
#include "citres.h"
#include "frprotox.h"
#include "frtypes.h"
#include <assert.h>
#include <stdio.h>

int memcount;
int32_t gScreenRowbytes;
static fauxrend_context frame;
fauxrend_context *_fr = &frame;
static struct { FrameDesc frame; uchar pixels[81]; } resource;
static int missing;
void *RefGet(Ref ref) { (void)ref; return missing ? NULL : &resource; }
errtype uiMakeBitmapCursor(LGCursor *cursor, grs_bitmap *bm, LGPoint hotspot) {
    cursor->state = bm; cursor->w = bm->w; cursor->h = bm->h;
    cursor->hotspot = hotspot;
    return OK;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    grs_canvas root;
    uchar pixel = 0;
    gr_init_canvas(&root, &pixel, BMT_FLAT8, 1, 1);
    gr_set_canvas(&root);
    grd_screen_canvas = &root;
    gr_init_bm(&resource.frame.bm, resource.pixels, BMT_FLAT8, BMF_TRANS, 9, 9);
    resource.frame.anchorArea.ul.x = resource.frame.anchorArea.ul.y = 4;
    memset(resource.pixels, 42, sizeof(resource.pixels));
    gr2ss_register_init(0, 320, 200);
    for (int i=1; i<SHOCK_VIDEO_MODE_COUNT; ++i)
        gr2ss_register_mode(0, shock_video_sizes[i][0], shock_video_sizes[i][1]);
    grs_bitmap bitmap = {0};
    LGCursor cursor = {0};
    for (int pass=0; pass<3; ++pass) {
        for (int i=0; i<SHOCK_VIDEO_MODE_COUNT; ++i) {
            convert_use_mode = i;
            gr2ss_override = OVERRIDE_NONE;
            assert(replace_res_bitmap_cursor(&cursor, &bitmap, MKREF(1,0)) == OK);
            assert(bitmap.w == SCONV_X(9) && bitmap.h == SCONV_Y(9));
            assert(cursor.state == &bitmap && cursor.w == bitmap.w && cursor.h == bitmap.h);
            assert(bitmap.row == bitmap.w && bitmap.bits);
            if (i == SHOCK_DECK_MODE)
                assert(bitmap.w * bitmap.h == 1296); // old buffer held only 1016
            if (i == 5)
                assert(MODE_SCONV_X(9,2) == SCONV_X(9) && MODE_SCONV_Y(9,2) == SCONV_Y(9));
            for (int j=0; j<bitmap.w*bitmap.h; ++j) assert(bitmap.bits[j] == 42);
            assert(convert_use_mode == i && gr2ss_override == OVERRIDE_NONE);
            assert(grd_canvas == &root);
            uchar *old_bits = bitmap.bits;
            missing = 1;
            assert(replace_res_bitmap_cursor(&cursor, &bitmap, MKREF(1,0)) == ERR_FREAD);
            assert(bitmap.bits == old_bits && cursor.state == &bitmap);
            assert(convert_use_mode == i && gr2ss_override == OVERRIDE_NONE);
            missing = 0;
        }
    }
    free(bitmap.bits);
    puts("PASS puzzle cursor: repeated owned bitmap replacement, all 13 resolutions through 4K, failed loads preserve cursor and render state");
}
