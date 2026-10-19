// Exercise real drug_use/drugs_update and the real callback table; stub only UI/audio.
#include "player.h"
#include "drugs.h"
#include "faketime.h"
#include "mainloop.h"
#include "Prefs.h"
#include "mfdext.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
Player player_struct;
ShockPrefs gShockPrefs;
short _current_loop;
uint _change_flag;
char reflex_remainder;
uchar ppall[768];
static int palette_restores;
void gr_set_pal(int start,int count,uchar *data) { if(start==0 && count==256) ++palette_restores; }
void gamma_dealfunc(ushort gamma) {}
char *get_object_short_name(int triple,char *buf,int size) { snprintf(buf,size,"Berserk");return buf; }
char *get_string(int ref,char *buf,int size) { snprintf(buf,size," applied");return buf; }
errtype message_info(const char *message) { return OK; }
void mfd_gridpanel_set_winmove(uchar check) {}
void mfd_notify_func(ubyte func,ubyte slot,uchar grab,MFD_Status stat,uchar redraw) {}
int play_digi_fx_master(int sfx,int loops,ObjID id,ushort x,ushort y) { return 0; }
void set_global_lighting(short level) {}
int main(void) {
    for(int fatigue=0;fatigue<=1000000;fatigue+=1000000) {
        memset(&player_struct,0,sizeof player_struct);palette_restores=0;
        player_struct.game_time=80000;player_struct.drugs[DRUG_LSD]=3;
        player_struct.fatigue=fatigue;
        drug_use(DRUG_LSD);
        assert(player_struct.berserk_started_at==80000);
        assert(player_struct.drug_status[DRUG_LSD]>0);
        // Frame-by-frame path, not the standalone expiration/decay helpers.
        for(int tick=1;tick<60*CIT_CYCLE;++tick) {
            player_struct.game_time=80000+tick;drugs_update();
            assert(player_struct.drug_status[DRUG_LSD]>0 && palette_restores==0);
        }
        player_struct.game_time=80000+60*CIT_CYCLE;drugs_update();
        assert(player_struct.drug_status[DRUG_LSD]==0 && palette_restores==1);
        drug_use(DRUG_LSD);
        player_struct.game_time+=30*CIT_CYCLE;drugs_update();
        drug_use(DRUG_LSD);
        player_struct.game_time+=30*CIT_CYCLE;drugs_update();
        assert(player_struct.drug_status[DRUG_LSD]>0);
        player_struct.game_time+=30*CIT_CYCLE;drugs_update();
        assert(player_struct.drug_status[DRUG_LSD]==0);
    }
    puts("PASS integration: actual drug use, callback table and update loop last 60 seconds at normal/high fatigue; doses refresh");
}
