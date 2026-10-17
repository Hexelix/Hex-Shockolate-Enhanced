/*

Copyright (C) 2015-2018 Night Dive Studios, LLC.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// Delay population-based reinforcement triggers, not scripted monster creation.
#include <stdlib.h>
#include <string.h>
#include "ecology.h"
#include "faketime.h"
#include "map.h"
#include "objclass.h"
#include "objgame.h"
#include "otrip.h"
#include "player.h"
#include "trigger.h"

extern uchar trigger_check;
extern ObjID current_trap;
errtype grind_trap(char type, int p1, int p2, int p3, int p4, ubyte *destroy_count, ObjID id);

static bool ecology_trigger_valid(ObjID id) {
    return id > OBJ_NULL && id < NUM_OBJECTS && objs[id].active &&
        ID2TRIP(id) == ECOLOGY_TRIG_TRIPLE &&
        objs[id].specID > OBJ_SPEC_NULL && objs[id].specID < NUM_OBJECTS_TRAP &&
        objTraps[objs[id].specID].id == id;
}

static bool ecology_needs_reinforcements(ObjID id) {
    if (!ecology_trigger_valid(id)) return false;
    uint32_t comparator = objTraps[objs[id].specID].comparator;
    int quota = comparator >> 24;
    int triple = comparator & 0xFFFFFF;
    int cl = (comparator >> 16) & 0xFF;
    if (cl >= NUM_CLASSES || objSpecHeaders[cl].data == NULL) return false;

    ObjSpecHeader *header = &objSpecHeaders[cl];
    ObjSpecID next = ((ObjSpec *)header->data)->headused;
    int count = 0;
    while (next != OBJ_SPEC_NULL) {
        ObjSpec *spec = (ObjSpec *)(header->data + next * header->struct_size);
        if (ID2TRIP(spec->bits.id) == triple && ++count >= quota) return false;
        next = spec->next;
    }
    return count < quota;
}

static errtype ecology_queue_reinforcement(ObjID id) {
    TrapSchedEvent event = {0};
    // One randomly selected whole-second delay, plus scheduler rounding.
    unsigned seconds = 30 + rand() % 91;
    event.timestamp = TICKS2TSTAMP(player_struct.game_time + seconds * CIT_CYCLE) + 1;
    event.type = ECOLOGY_SCHED_EVENT;
    event.target_id = id;
    event.source_id = objs[id].specID;
    return schedule_event(&global_fullmap->sched[MAP_SCHEDULE_GAMETIME], (SchedEvent *)&event);
}

static bool ecology_reinforcement_pending(ObjID id) {
    PQueue *queue = &global_fullmap->sched[MAP_SCHEDULE_GAMETIME].queue;
    for (int i = 0; i < queue->fullness; ++i) {
        TrapSchedEvent event;
        memcpy(&event, queue->vec + i * queue->elemsize, sizeof event);
        if (event.type == ECOLOGY_SCHED_EVENT && event.target_id == id &&
            event.source_id == objs[id].specID)
            return true;
    }
    return false;
}

errtype do_ecology_triggers(void) {
    ObjSpecID spec = objTraps[0].id;
    while (spec != OBJ_SPEC_NULL) {
        ObjID id = objTraps[spec].id;
        if (ecology_needs_reinforcements(id) && !ecology_reinforcement_pending(id))
            ecology_queue_reinforcement(id);
        spec = objTraps[spec].next;
    }
    return OK;
}

void ecology_event_handler(Schedule *schedule, SchedEvent *raw) {
    TrapSchedEvent event;
    memcpy(&event, raw, sizeof event);
    ObjID id = event.target_id;
    // A removed or repurposed trap must not fire a stale event. Recount too:
    // another source may have restored the population during the wait.
    if (!ecology_trigger_valid(id) || objs[id].specID != event.source_id ||
        !ecology_needs_reinforcements(id))
        return;

    ObjSpecID spec = objs[id].specID;
    ubyte remaining_uses = objTraps[spec].destroy_count;
    uchar old_check = trigger_check;
    ObjID old_trap = current_trap;
    trigger_check = FALSE;
    current_trap = id; // monster/scheduler traps may use their source's location
    grind_trap(objTraps[spec].trap_type, objTraps[spec].p1, objTraps[spec].p2,
               objTraps[spec].p3, objTraps[spec].p4, &objTraps[spec].destroy_count, id);
    current_trap = old_trap;
    trigger_check = old_check;

    // Retry a failed placement or a remaining deficit after another fresh delay.
    // The scheduler still holds the firing event until this handler returns, so
    // enqueue directly rather than treating that event as a pending retry.
    if (remaining_uses != 1 && ecology_needs_reinforcements(id))
        ecology_queue_reinforcement(id);
}
