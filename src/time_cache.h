#ifndef TIME_CACHE_H
#define TIME_CACHE_H

#include "time_def.h"

#define SLOTS_PER_HOUR 4

#define TIMESLOTS_IN_A_DAY (HOURS_IN_A_DAY * SLOTS_PER_HOUR)
#define MILLISEC_PER_SLOT (MILLISEC_IN_A_DAY / TIMESLOTS_IN_A_DAY)

typedef unsigned char DayCache[TIMESLOTS_IN_A_DAY];

typedef unsigned int SlotIndex;

SlotIndex time_to_slot(const msec time, const msec ref_time = 0);
void add_activity_to_slot(DayCache &cache, const SlotIndex index);
void age_cache(DayCache &cache);
bool is_timeslot_active(DayCache &cache, const SlotIndex index);

#endif