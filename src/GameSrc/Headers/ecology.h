#ifndef HEX_ECOLOGY_H
#define HEX_ECOLOGY_H

#include "schedule.h"

errtype do_ecology_triggers(void);
void ecology_event_handler(Schedule *schedule, SchedEvent *event);

#endif
