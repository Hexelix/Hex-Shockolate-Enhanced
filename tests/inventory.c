// Include the production implementation so hit-testing uses its actual display type.
#include "../src/GameSrc/invent.c"
#include <assert.h>
#include <stdio.h>
short _current_loop;
uint _change_flag;
Player player_struct;
int main(void) {
    assert(NUM_GENERAL_SLOTS==28 && NUM_WEAPON_SLOTS==9);
    inventory_page=INV_GENERAL_PAGE;
    assert(inventory_subpage_count(inventory_page)==2);
    assert(inventory_subpage(inventory_page)==0);
    assert(inventory_turn_page(1)); assert(inventory_subpage(inventory_page)==1);
    assert(inventory_turn_page(1)); assert(inventory_subpage(inventory_page)==0);
    assert(inventory_turn_page(-1)); assert(inventory_subpage(inventory_page)==1);
    inv_display dp={0}; dp.top=TOP_MARGIN; dp.pgsize=ITEMS_PER_PAGE; dp.listlen=NUM_GENERAL_SLOTS;
    for(int column=0;column<4;++column) {
        dp.relnum=column;
        for(int row=0;row<7;++row)
            assert(get_item_at_pixrow(&dp,INVENTORY_PANEL_Y+TOP_MARGIN+Y_STEP+row*Y_STEP)==7*column+row);
    }
    inventory_page=INV_MAIN_PAGE;
    assert(inventory_subpage_count(inventory_page)==2);
    inventory_reveal_slot(INV_MAIN_PAGE,8); assert(inventory_subpage(inventory_page)==1);
    inventory_reveal_slot(INV_MAIN_PAGE,0); assert(inventory_subpage(inventory_page)==0);
    dp.relnum=1; dp.listlen=NUM_WEAPON_SLOTS;
    for(int row=0;row<7;++row)
        assert(get_item_at_pixrow(&dp,INVENTORY_PANEL_Y+TOP_MARGIN+Y_STEP+row*Y_STEP)==(row<2 ? 7+row : -1));
    inventory_reveal_slot(INV_GENERAL_PAGE,0); assert(inventory_subpage(INV_GENERAL_PAGE)==0);
    inventory_reveal_slot(INV_GENERAL_PAGE,27); assert(inventory_subpage(INV_GENERAL_PAGE)==1);
    for(int i=0;i<NUM_WEAPON_SLOTS;++i) player_struct.weapons[i].type=0;
    assert(inventory_next_weapon(0,-1)==8);
    assert(inventory_next_weapon(8,1)==0);
    assert(inventory_next_weapon(6,1)==7 && inventory_next_weapon(7,1)==8);
    player_struct.weapons[7].type=player_struct.weapons[8].type=EMPTY_WEAPON_SLOT;
    assert(inventory_next_weapon(0,-1)==6 && inventory_next_weapon(6,1)==0);
    inventory_page=INV_SOFTWARE_PAGE; assert(!inventory_turn_page(1));
    puts("PASS inventory: two pages, all 28 row mappings, extra weapons, empty-row bounds and selected-slot reveal");
}
