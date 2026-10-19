#include "../src/GameSrc/physics.c"
#include <assert.h>
#include <stdio.h>

int main(void) {
    int banks;
    physics_zero_all_controls();
    player_controls[KEYBD_CONTROL_BANK][CONTROL_XYROT] = -100;
    assert(physics_combined_axis(CONTROL_XYROT, FALSE, &banks) == -100 && banks == 1);
    assert(physics_combined_axis(CONTROL_XYROT, TRUE, &banks) == 0 && banks == 0);
    player_controls[MOUSE_CONTROL_BANK][CONTROL_XYROT] = 50;
    assert(physics_combined_axis(CONTROL_XYROT, TRUE, &banks) == 50 && banks == 1);
    assert(physics_combined_axis(CONTROL_XYROT, FALSE, &banks) == -25 && banks == 2);
    player_controls[KEYBD_CONTROL_BANK][CONTROL_XYROT] = 100;
    assert(physics_combined_axis(CONTROL_XYROT, TRUE, &banks) == 50);
    for (int axis = 0; axis < DEGREES_OF_FREEDOM; ++axis) {
        if (axis == CONTROL_XYROT) continue;
        player_controls[KEYBD_CONTROL_BANK][axis] = 80;
        assert(physics_combined_axis(axis, TRUE, &banks) == 80);
    }
    puts("PASS cyber controls: keyboard turn rerouted, mouse turning preserved, other axes and realspace averaging unchanged");
}
