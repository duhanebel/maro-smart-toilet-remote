#include "../src/aging_policy.h"
#include "test_macro.h"
#include <stdio.h>

test_result creation() {
  aging_policy policy = make_day_aging_policy();
  int millis_in_a_day = 86400000;
  mu_assert(policy.interval = millis_in_a_day, "Wrong number on interval");
  mu_assert(policy.current_period == 0, "current period is not init to zero");
  return 0;
}

test_result same_period() {
  aging_policy policy;
  policy.interval = 15;
  policy.current_period = 6;
  mu_assert(is_within_same_period(policy, (15 * 6) == true),
            "Policy period calculation error");
  mu_assert(is_within_same_period(policy, (15 * 6 + 7) == true),
            "Policy period calculation error");
  mu_assert(is_within_same_period(policy, (15 * 6 + 15) == false),
            "Policy period calculation error");
  return 0;
}

test_result set_new_period() { return 0; }

test_result policy_all_tests() {
  mu_run_test(creation);
  mu_run_test(same_period);
  mu_run_test(set_new_period);
  return 0;
}
