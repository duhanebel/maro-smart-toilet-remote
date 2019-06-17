#ifndef AGING_POLICY_H
#define AGING_POLICY_H

#include "time_def.h"

typedef struct _aging_policy {
  unsigned int current_period;
  msec interval;
} aging_policy;

bool is_within_same_period(aging_policy &policy, const msec time_now);
aging_policy make_day_aging_policy();
void set_period(aging_policy &policy, const msec time_now);

#endif