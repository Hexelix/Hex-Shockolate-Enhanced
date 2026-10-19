#include "Prefs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
uchar popup_cursors, olh_active;
bool DoubleSize, SkipLines;
int _fr_global_detail;
short mode_id, global_fov, saved_fov;
uchar curr_vol_lev, curr_sfx_vol, curr_alog_vol, audiolog_setting;

static void prefs(int value) {
    FILE *file = fopen("prefs.txt", "w");
    assert(file);
    fprintf(file,"shadow-lift = %d\n",value);
    fclose(file);
    assert(LoadPrefs()==0);
}

int main(void) {
    char original[PATH_MAX];
    assert(getcwd(original,sizeof(original)));
    char folder[]="/tmp/hex-shadow-prefs-XXXXXX";
    assert(mkdtemp(folder) && chdir(folder)==0);
    SetDefaultPrefs();
    assert(gShockPrefs.doShadowLift==12);
    prefs(27); assert(gShockPrefs.doShadowLift==27);
    assert(SavePrefs()==0);
    gShockPrefs.doShadowLift=0;
    assert(LoadPrefs()==0 && gShockPrefs.doShadowLift==27);
    prefs(99); assert(gShockPrefs.doShadowLift==30);
    prefs(-20); assert(gShockPrefs.doShadowLift==0);
    unlink("prefs.txt");
    assert(chdir(original)==0 && rmdir(folder)==0);
    puts("PASS shadow preferences: default lift, save/reload and 0-30 percent limits");
}
