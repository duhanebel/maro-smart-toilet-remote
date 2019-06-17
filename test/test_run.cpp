#include "cache_tests.h"
#include "policy_tests.h"
#include "test_macro.h"
#include <stdio.h>

test_result all_tests() {
  printf("Running cache tests:\n");
  cache_all_tests();
  printf("\nRunning policy tests:\n");
  policy_all_tests();
}

mu_run_tests(all_tests);