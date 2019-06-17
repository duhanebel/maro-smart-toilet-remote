#include "cache_tests.h"
#include "../src/time_cache.h"
#include <stdio.h>

test_result testTimeToSlot() {
  int millis_in_15_minutes = 900000;
  mu_assert(time_to_slot(800000) == 0,
            "Wrong msec calculation (800000 not timeslot 0)");
  mu_assert(time_to_slot(950000) == 1,
            "Wrong msec calculation (950000 not timeslot 1)");
  return 0;
}

test_result testTimePastADay() {
  int millis_in_a_day = 86400000;
  mu_assert(time_to_slot(millis_in_a_day + 1) == 0,
            "Wrong msec calculation (millis_in_a_day + 1 not timeslot 0");
  return 0;
}

test_result add_to_cache() {
  DayCache cache = {0};
  add_activity_to_slot(cache, 0);
  add_activity_to_slot(cache, 23);
  mu_assert(cache[0] == 2, "item not added to cache (0)");
  mu_assert(cache[23] == 2, "item not added to cache (23)");
  for (int i = 0; i < TIMESLOTS_IN_A_DAY; i++) {
    if (i == 0 || i == 23)
      continue;
    mu_assert(cache[i] == 0, "added to cache where it shouldn't have");
  }
  return 0;
}

test_result add_past_max() {
  DayCache cache = {0};
  for (int i = 0; i < 300; i++) {
    add_activity_to_slot(cache, 0);
  }
  mu_assert(cache[0] == 254, "Added past max value to cache");

  add_activity_to_slot(cache, 1);
  age_cache(cache);
  for (int i = 0; i < 300; i++) {
    add_activity_to_slot(cache, 1);
  }
  mu_assert(cache[1] == 255, "Added past max value to cache");

  return 0;
}

test_result age_cache() {
  DayCache cache = {0};
  add_activity_to_slot(cache, 5);
  add_activity_to_slot(cache, 5);
  add_activity_to_slot(cache, 5);
  add_activity_to_slot(cache, 5);
  add_activity_to_slot(cache, 65);
  add_activity_to_slot(cache, 65);

  age_cache(cache);

  mu_assert(cache[5] == 4, "Cache[5] should be 4");
  mu_assert(cache[65] == 2, "Cache[100] should be 2");

  age_cache(cache);
  age_cache(cache);

  mu_assert(cache[5] == 1, "Cache[5] should be 1");
  mu_assert(cache[65] == 0, "Cache[100] should be 0");

  return 0;
}

test_result test_timeslot_active() {
  DayCache cache = {0};
  add_activity_to_slot(cache, 5);
  add_activity_to_slot(cache, 5);
  mu_assert(is_timeslot_active(cache, 5) == true, "slot 5 should be active!");
  for (int i = 0; i < TIMESLOTS_IN_A_DAY; i++) {
    if (i == 5)
      continue;
    mu_assert(is_timeslot_active(cache, i) == false,
              "Other timeslots shoudln't be active");
  }
}

test_result cache_all_tests() {
  mu_run_test(testTimeToSlot);
  mu_run_test(testTimePastADay);
  mu_run_test(add_to_cache);
  mu_run_test(add_past_max);
  mu_run_test(age_cache);
  mu_run_test(test_timeslot_active);
  return 0;
}
