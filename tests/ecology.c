// Isolate the production ecology logic; model the saved scheduler and spawn callback.
#include "../src/GameSrc/ecology.c"
#include <assert.h>
#include <stdio.h>

Obj objs[NUM_OBJECTS];
ObjTrap objTraps[NUM_OBJECTS_TRAP];
ObjSpecHeader objSpecHeaders[NUM_CLASSES];
Player player_struct;
static FullMap map;
FullMap *global_fullmap=&map;
uchar trigger_check=TRUE;
ObjID current_trap=99;
static SchedEvent queue_storage[64];
static ObjSpec population[4];
static int fires, spawn_success, fail_queue;

errtype schedule_event(Schedule *s, SchedEvent *ev) {
    if (fail_queue) return ERR_NOMEM;
    assert(s->queue.fullness<64);
    memcpy(s->queue.vec+s->queue.fullness++*sizeof *ev,ev,sizeof *ev);
    return OK;
}
errtype grind_trap(char type,int p1,int p2,int p3,int p4,ubyte *remaining,ObjID id) {
    assert(current_trap==id && !trigger_check);
    ++fires;
    if (*remaining) --*remaining;
    if (spawn_success) { population[0].headused=1; population[1].bits.id=20; }
    return OK;
}
static void setup(void) {
    memset(&map,0,sizeof map);memset(objs,0,sizeof objs);memset(objTraps,0,sizeof objTraps);
    memset(population,0,sizeof population); memset(objSpecHeaders,0,sizeof objSpecHeaders);
    PQueue *q=&map.sched[MAP_SCHEDULE_GAMETIME].queue;
    q->vec=(char *)queue_storage;q->elemsize=sizeof(SchedEvent);q->size=64;
    objs[10].active=TRUE;objs[10].obclass=CLASS_TRAP;objs[10].subclass=TRIP2SC(ECOLOGY_TRIG_TRIPLE);objs[10].info.type=TRIP2TY(ECOLOGY_TRIG_TRIPLE);objs[10].specID=1;
    assert(ID2TRIP(10)==ECOLOGY_TRIG_TRIPLE);
    objTraps[0].id=1;objTraps[1].id=10;
    objTraps[1].comparator=(1u<<24)|MAKETRIP(CLASS_CRITTER,0,0);
    objs[20].obclass=CLASS_CRITTER;
    objSpecHeaders[CLASS_CRITTER].data=(char *)population;
    objSpecHeaders[CLASS_CRITTER].size=4; objSpecHeaders[CLASS_CRITTER].struct_size=sizeof(ObjSpec);
    player_struct.game_time=10000;fires=0;spawn_success=1;fail_queue=0;trigger_check=TRUE;current_trap=99;
}
static SchedEvent first(void) { SchedEvent e;memcpy(&e,queue_storage,sizeof e);return e; }
int main(void) {
    setup();
    assert(sizeof(TrapSchedEvent)==sizeof(SchedEvent));
    for(int seed=0;seed<1000;++seed) {
        map.sched[0].queue.fullness=0;srand(seed);do_ecology_triggers();
        SchedEvent e=first();
        unsigned ticks=(ushort)(e.timestamp-TICKS2TSTAMP(player_struct.game_time))*16u-(player_struct.game_time&15u);
        assert(ticks>=30*CIT_CYCLE && ticks<=120*CIT_CYCLE+16);
        assert(e.type==ECOLOGY_SCHED_EVENT && fires==0);
    }
    SchedEvent original=first();
    for(int i=0;i<100;++i) do_ecology_triggers();
    assert(map.sched[0].queue.fullness==1 && memcmp(&original,queue_storage,sizeof original)==0);
    // Saving/loading the level stores these exact event records, without rerolling.
    SchedEvent saved=first();memset(queue_storage,0,sizeof queue_storage);memcpy(queue_storage,&saved,sizeof saved);
    do_ecology_triggers();assert(map.sched[0].queue.fullness==1);
    player_struct.game_time=TSTAMP2TICKS(saved.timestamp)+16;
    ecology_event_handler(&map.sched[0],&saved);
    assert(fires==1 && trigger_check && current_trap==99);
    assert(map.sched[0].queue.fullness==1); // no retry when population recovered

    setup();do_ecology_triggers();saved=first();
    population[0].headused=1;population[1].bits.id=20;
    ecology_event_handler(&map.sched[0],&saved);assert(fires==0); // quota restored while waiting
    setup();do_ecology_triggers();saved=first();objs[10].active=FALSE;
    ecology_event_handler(&map.sched[0],&saved);assert(fires==0);
    setup();do_ecology_triggers();saved=first();objs[10].specID=2;
    ecology_event_handler(&map.sched[0],&saved);assert(fires==0);
    setup();objs[10].info.type=0;do_ecology_triggers();assert(map.sched[0].queue.fullness==0); // not an ecology trigger

    setup();spawn_success=0;do_ecology_triggers();saved=first();
    player_struct.game_time=TSTAMP2TICKS(saved.timestamp)+16;
    ecology_event_handler(&map.sched[0],&saved);
    assert(fires==1 && map.sched[0].queue.fullness==2);
    unsigned until_retry=(ushort)(queue_storage[1].timestamp-TICKS2TSTAMP(player_struct.game_time))*16u;
    assert(until_retry>=30*CIT_CYCLE && until_retry<=120*CIT_CYCLE+16);
    setup();spawn_success=0;objTraps[1].destroy_count=1;do_ecology_triggers();saved=first();
    ecology_event_handler(&map.sched[0],&saved);assert(fires==1 && map.sched[0].queue.fullness==1);
    setup();fail_queue=1;do_ecology_triggers();assert(fires==0 && map.sched[0].queue.fullness==0);
    fail_queue=0;do_ecology_triggers();assert(map.sched[0].queue.fullness==1);
    setup();player_struct.game_time=TSTAMP2TICKS(65530);do_ecology_triggers();saved=first();
    assert((ushort)(saved.timestamp-65530)>=30*CIT_CYCLE/16); // timestamp wraps safely
    puts("PASS ecology: 30-120s range, deduplication, saved timers, population recheck, stale traps, retries, finite uses and wraparound");
}
