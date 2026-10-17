#include "player.h"
#include "drugs.h"
#include "faketime.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
Player player_struct;
extern void drug_berserk_use(void);
extern bool drug_berserk_expired(void), drug_should_decay(int drug, int decay);
int main(void) {
    player_struct.game_time=100;
    player_struct.drug_status[DRUG_LSD]=4;
    drug_berserk_use();
    for (int sec=0;sec<60;++sec) {
        player_struct.game_time=100+sec*CIT_CYCLE;
        assert(!drug_berserk_expired());
        for(int seed=0;seed<100;++seed) {
            srand(seed);
            assert(!drug_should_decay(DRUG_LSD,100000));
        }
    }
    player_struct.game_time=100+60*CIT_CYCLE-1; assert(!drug_berserk_expired());
    ++player_struct.game_time; assert(drug_berserk_expired());
    drug_berserk_use(); assert(!drug_berserk_expired()); // dose refreshes at current time
    player_struct.game_time+=60*CIT_CYCLE; assert(drug_berserk_expired());
    player_struct.game_time=UINT32_MAX-100; drug_berserk_use();
    player_struct.game_time+=60*CIT_CYCLE-1; assert(!drug_berserk_expired());
    ++player_struct.game_time; assert(drug_berserk_expired());
    player_struct.drug_status[DRUG_LSD]=0; assert(!drug_berserk_expired());
    assert(drug_should_decay(DRUG_MEDIC,16)); // other patches keep their decay path
    puts("PASS berserk: 60-second boundary, refreshed doses, clock wrap, no random/fatigue decay");
}
