#include "aging_policy.h"

bool is_within_same_period(aging_policy &policy, const msec time_now) {
  unsigned char current_day = time_now / policy.interval;
  return (current_day == policy.current_period);
}

aging_policy make_day_aging_policy() {
  aging_policy policy;
  policy.current_period = 0;
  policy.interval = MILLISEC_IN_A_DAY;
  return policy;
}

void set_period(aging_policy &policy, const msec time_now) {
  unsigned char new_interval = time_now / policy.interval;
  policy.current_period = new_interval;
}