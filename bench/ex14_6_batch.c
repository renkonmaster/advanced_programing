#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef BENCHMARK_MAIN
#error "BENCHMARK_MAIN must name the renamed ex14 main function"
#endif

int BENCHMARK_MAIN(int argc, char *argv[]);

static uint64_t nanoseconds(struct timespec value) {
  return (uint64_t)value.tv_sec * UINT64_C(1000000000) +
         (uint64_t)value.tv_nsec;
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr, "usage: %s repetitions file N exponent\n", argv[0]);
    return 2;
  }

  errno = 0;
  char *end = NULL;
  unsigned long repetitions = strtoul(argv[1], &end, 10);
  if (errno != 0 || end == argv[1] || *end != '\0' || repetitions == 0) {
    return 2;
  }

  char *inner_argv[] = {argv[0], argv[2], argv[3], argv[4], NULL};
  struct timespec before;
  struct timespec after;
  if (clock_gettime(CLOCK_MONOTONIC, &before) != 0) {
    return 2;
  }

  for (unsigned long i = 0; i < repetitions; i++) {
    if (BENCHMARK_MAIN(4, inner_argv) != 0) {
      return 1;
    }
  }

  if (clock_gettime(CLOCK_MONOTONIC, &after) != 0) {
    return 2;
  }
  uint64_t elapsed = nanoseconds(after) - nanoseconds(before);
  fprintf(stderr, "runs=%lu elapsed_ns=%" PRIu64 " average_ns=%.3f\n",
          repetitions, elapsed, (double)elapsed / (double)repetitions);
  return 0;
}
