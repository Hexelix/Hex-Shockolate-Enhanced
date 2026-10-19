// Shared resolution order. The first five indices retain their legacy meaning.
#ifndef SHOCK_VIDEO_CONFIG_H
#define SHOCK_VIDEO_CONFIG_H
#define SHOCK_WIDE_MODES(X) \
    X(1280,720,GRM_1280x720x8) \
    X(1280,800,GRM_1280x800x8) \
    X(1600,900,GRM_1600x900x8) \
    X(1920,1080,GRM_1920x1080x8) \
    X(1920,1200,GRM_1920x1200x8) \
    X(2560,1440,GRM_2560x1440x8) \
    X(2560,1600,GRM_2560x1600x8) \
    X(3840,2160,GRM_3840x2160x8)
#define SHOCK_VIDEO_MODES(X) \
    X(320,200,GRM_320x200x8) \
    X(320,400,GRM_320x400x8) \
    X(640,400,GRM_640x400x8) \
    X(640,480,GRM_640x480x8) \
    X(1024,768,GRM_1024x768x8) \
    SHOCK_WIDE_MODES(X)
#define SHOCK_VIDEO_MODE_COUNT 13
#define SHOCK_DECK_MODE 6
#define SHOCK_FOV_MIN 70
#define SHOCK_FOV_MAX 135
#define SHOCK_MODE_SIZE(w,h,id) {w,h},
static const short shock_video_sizes[SHOCK_VIDEO_MODE_COUNT][2] = {
    SHOCK_VIDEO_MODES(SHOCK_MODE_SIZE)
};
#undef SHOCK_MODE_SIZE
static inline int shock_valid_video_mode(int mode) {
    return mode >= 0 && mode < SHOCK_VIDEO_MODE_COUNT;
}
static inline short shock_fov_clamp(int degrees) {
    return degrees < SHOCK_FOV_MIN ? SHOCK_FOV_MIN :
           degrees > SHOCK_FOV_MAX ? SHOCK_FOV_MAX : degrees;
}
static inline short shock_fov_from_slider(int value) {
    if (value < 0) value = 0;
    if (value > 100) value = 100;
    return SHOCK_FOV_MIN + ((SHOCK_FOV_MAX - SHOCK_FOV_MIN) * value + 50) / 100;
}
static inline short shock_fov_to_slider(int degrees) {
    return ((shock_fov_clamp(degrees) - SHOCK_FOV_MIN) * 100 +
            (SHOCK_FOV_MAX - SHOCK_FOV_MIN) / 2) / (SHOCK_FOV_MAX - SHOCK_FOV_MIN);
}
#endif
