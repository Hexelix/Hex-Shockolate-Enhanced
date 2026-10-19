#include "../src/Libraries/EDMS/Source/MODELS/d_f_2.cc"
#include "idof.h"
#include <cassert>
#include <cstdio>
extern "C" void EDMS_set_Dirac_strafe(physics_handle, fix);
extern "C" void EDMS_control_Dirac_frame(physics_handle, fix, fix, fix, fix);
Q S[MAX_OBJ][DOF][DOF_DERIVS];
Q args_storage[MAX_OBJ][DOF][DOF_DERIVS];
EDMS_Argblock_Pointer A = args_storage;
Q I[MAX_OBJ][DOF_MAX];
Q D_FRAME = 8;
object_number ph2on[MAX_OBJ];
physics_handle on2ph[MAX_OBJ];
TerrainData terrain_info = {};
int32_t ss_edms_bcd_flags;
static bool blocked;
TerrainHit indoor_terrain(Q, Q, Q, Q, physics_handle, TFType) {
    terrain_info.wx = blocked ? FIX_UNIT : 0;
    return MISS;
}

int main() {
    const int ph=2, on=3;
    ph2on[ph]=on; on2ph[on]=ph;
    I[on][IDOF_MODEL]=D_FRAME;
    A[on][3][0]=1;
    EDMS_control_Dirac_frame(ph, 0, 0, 0, 0);
    EDMS_set_Dirac_strafe(ph, 50*FIX_UNIT);
    assert(I[on][4].to_fix() == 150*FIX_UNIT);
    Q F[3]={0,0,0}, T[3]={0,0,0};
    dirac_mechanicals(on,F,T);
    assert(F[0]==0 && F[1]<0 && F[2]==0);
    assert(T[0]==0 && T[1]==0 && T[2]==0);
    mech_globalize(F[0],F[1],F[2]);
    assert(F[0]==0 && F[1]<0 && F[2]==0);
    EDMS_set_Dirac_strafe(ph, -50*FIX_UNIT);
    F[0]=F[1]=F[2]=T[0]=T[1]=T[2]=0;
    dirac_mechanicals(on,F,T);
    assert(F[0]==0 && F[1]>0 && F[2]==0);
    EDMS_set_Dirac_strafe(ph, 50*FIX_UNIT);
    // Face +Y: body-right must become world +X, not remain on a world axis.
    A[on][3][0]=.70710678; A[on][6][0]=.70710678;
    F[0]=F[1]=F[2]=T[0]=T[1]=T[2]=0;
    dirac_mechanicals(on,F,T);
    mech_globalize(F[0],F[1],F[2]);
    assert(F[0]>0 && abs(F[1].to_fix())<100 && F[2]==0);
    // Touching terrain suppresses thrust through the existing collision model.
    blocked=true;
    F[0]=F[1]=F[2]=T[0]=T[1]=T[2]=0;
    dirac_mechanicals(on,F,T);
    assert(F[1]==0);
    EDMS_control_Dirac_frame(ph, 0, 0, 0, 0);
    assert(I[on][4]==0); // release/pause cannot leave stale lateral thrust
    EDMS_set_Dirac_strafe(-1,FIX_UNIT);
    EDMS_set_Dirac_strafe(MAX_OBJ,FIX_UNIT);
    assert(I[on][4]==0);
    puts("PASS cyber strafe: real body-relative forces, no torque, terrain suppression, pause reset and handle checks");
}
