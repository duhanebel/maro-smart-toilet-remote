#include "time_cache.h"

SlotIndex time_to_slot(const msec time, const msec ref_time) {
  msec relative_day_time = (time - ref_time) % MILLISEC_IN_A_DAY;

  return (relative_day_time / MILLISEC_PER_SLOT);
}

void add_activity_to_slot(DayCache &cache, const SlotIndex index) {
  if (cache[index] >= __UINT8_MAX__ - 1) {
    return;
  }
  cache[index] += 2;
}

void age_cache(DayCache &cache) {
  for (int i = 0; i < TIMESLOTS_IN_A_DAY; ++i) {
    cache[i] >>= 1;
  }
}

bool is_timeslot_active(DayCache &cache, const SlotIndex index) {
  return cache[index] > 0;
}
