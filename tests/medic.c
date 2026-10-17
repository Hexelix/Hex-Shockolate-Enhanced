// Regression test linked against the real drugs.c implementation.
#include "player.h"
#include "drugs.h"
#include "mainloop.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
Player player_struct;
uint _change_flag;
extern void drug_medic_use(void), drug_medic_effect(void), drug_medic_wearoff(void);
int main(void) {
    const int old_rates[] = {5,5,25,25,25,30,50,55,100,110};
    memset(&player_struct,0,sizeof(player_struct));
    // Preserve an unrelated regeneration source during patch cleanup.
    player_struct.hit_points_regen=7;
    drug_medic_use();
    int expected=430;
    assert(player_struct.hit_points_regen==7+(3*expected+1)/2);
    for(int n=9;n>=0;--n) {
        drug_medic_effect(); expected-=old_rates[n];
        assert(player_struct.hit_points_regen==7+(3*expected+1)/2);
    }
    assert(player_struct.drug_intensity[DRUG_MEDIC]==0);
    assert(player_struct.hit_points_regen==7);
    drug_medic_use(); drug_medic_effect(); drug_medic_use();
    drug_medic_wearoff();
    assert(player_struct.hit_points_regen==7);
    assert(player_struct.drug_intensity[DRUG_MEDIC]==0);
    puts("PASS medic: every stage is 1.5x (rounded); single and overlapping doses clean up fully");
}
