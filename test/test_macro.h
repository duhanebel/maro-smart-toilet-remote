#ifndef TEST_MACRO_H
#define TEST_MACRO_H

typedef const char *test_result;

#define mu_assert(test, message)                                               \
  do {                                                                         \
    if (!(test))                                                               \
      return message;                                                          \
  } while (0)

#define mu_run_test(test)                                                      \
  do {                                                                         \
    printf("Running: ");                                                       \
    printf("%s\n", #test);                                                     \
    const char *message = test();                                              \
    tests_run++;                                                               \
    if (message)                                                               \
      return message;                                                          \
  } while (0)
extern int tests_run;

#define mu_run_tests(all_tests_fn)                                             \
  int tests_run = 0;                                                           \
  int main(int argc, char **argv) {                                            \
    const char *result = all_tests_fn();                                       \
    if (result != 0) {                                                         \
      printf("%s\n", result);                                                  \
    } else {                                                                   \
      printf("\nALL TESTS PASSED\n");                                          \
    }                                                                          \
    printf("Tests run: %d\n", tests_run);                                      \
                                                                               \
    return result != 0;                                                        \
  }

#endif