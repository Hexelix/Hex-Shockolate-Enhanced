// Regression test linked against the real EDMS interfac.cc implementation.
#include "edms_int.h"
#include "idof.h"
#include <cassert>
#include <cstdio>
extern "C" void EDMS_rotate_player_yaw(physics_handle, fix);
Q S[MAX_OBJ][DOF][DOF_DERIVS];
Q I[MAX_OBJ][DOF_MAX];
Q PELVIS = 5;
int main() {
    const int ph = 4, on = 7;
    ph2on[ph] = on;
    I[on][IDOF_MODEL] = PELVIS;
    fix before[MAX_OBJ][DOF][DOF_DERIVS];
    for (int o=0;o<MAX_OBJ;++o)
        for (int d=0;d<DOF;++d)
            for (int v=0;v<DOF_DERIVS;++v) {
                before[o][d][v] = 1000 + o*100 + d*10 + v;
                S[o][d][v].fix_to(before[o][d][v]);
            }
    // A full sequence of turns must preserve every position, velocity,
    // acceleration and other object's state except the player's yaw.
    for (int n=0;n<1000;++n) EDMS_rotate_player_yaw(ph, 123);
    for (int o=0;o<MAX_OBJ;++o)
        for (int d=0;d<DOF;++d)
            for (int v=0;v<DOF_DERIVS;++v)
                assert(S[o][d][v].to_fix() == before[o][d][v] + ((o==on && d==3 && v==0) ? 123000 : 0));
    EDMS_rotate_player_yaw(ph, -123000);
    assert(S[on][3][0].to_fix() == before[on][3][0]);
    EDMS_rotate_player_yaw(-1, 100);
    EDMS_rotate_player_yaw(MAX_OBJ, 100);
    ph2on[ph]=-1; EDMS_rotate_player_yaw(ph,100);
    ph2on[ph]=MAX_OBJ; EDMS_rotate_player_yaw(ph,100);
    ph2on[ph]=on; I[on][IDOF_MODEL]=3; EDMS_rotate_player_yaw(ph,100);
    assert(S[on][3][0].to_fix() == before[on][3][0]);
    puts("PASS yaw: 1000 turns preserve lean/stance derivatives; invalid/non-pelvis handles ignored");
}
