// Exercise the actual resource encoder/decoder with the expanded player layout.
#include "player.h"
#include "res.h"
#include "faketime.h"
#include "drugs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PL_MFD_PUZZLE_SIZE 64
#include "playerlayout.h"
#undef PL_MFD_PUZZLE_SIZE
int main(void) {
    Player src={0};
    src.version_num=PLAYER_VERSION_NUMBER;
    src.game_time=100000; src.berserk_started_at=src.game_time-30*CIT_CYCLE;
    src.drug_status[DRUG_LSD]=4;
    for(int i=0;i<NUM_GENERAL_SLOTS;++i) src.inventory[i]=1000+i;
    for(int i=0;i<NUM_WEAPON_SLOTS;++i) {
        src.weapons[i].type=i%6;
        src.weapons[i].subtype=i%5;
        src.weapons[i].ammo=20+i;
        src.weapons[i].ammo_type=i%3;
        src.weapons[i].make_info=70+i;
    }
    src.hit_points=99; src.hit_points_regen=645; src.actives[ACTIVE_WEAPON]=9;
    src.actives[ACTIVE_GENERAL]=27; src.pad[8]=123;
    // Detect declared disc-size mistakes even if they don't crash the allocator.
    size_t field_bytes=0;
    for(const ResField *f=PlayerLayout_M64.fields;f->type!=RFFT_END;++f)
        field_bytes+=f->type>RFFT_BIN_BASE ? f->type-RFFT_BIN_BASE :
            f->type==RFFT_UINT8 ? 1 : f->type==RFFT_UINT16 ? 2 :
            f->type==RFFT_PAD ? f->offset : 4;
    assert(field_bytes==PlayerLayout_M64.dsize);
    assert(field_bytes==1476);
    size_t size=sizeof src;
    void *raw=ResEncode(&src,&size,(UserDecodeData)&PlayerLayout_M64);
    assert(size==field_bytes);
    Player *dst=ResDecode(raw,&size,(UserDecodeData)&PlayerLayout_M64);
    assert(size==sizeof src);
    assert(memcmp(src.inventory,dst->inventory,sizeof src.inventory)==0);
    assert(memcmp(src.weapons,dst->weapons,sizeof src.weapons)==0);
    assert(src.berserk_started_at==dst->berserk_started_at && src.game_time==dst->game_time);
    assert(dst->drug_status[DRUG_LSD]==4 && dst->actives[ACTIVE_GENERAL]==27 && dst->actives[ACTIVE_WEAPON]==9);
    assert(dst->hit_points==99 && dst->hit_points_regen==645 && dst->pad[8]==123);
    free(dst);free(raw);
    puts("PASS save: all 28 items, ten weapons, active selections and remaining berserk time survive encode/decode");
}
