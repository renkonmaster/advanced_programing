#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef union {
  size_t requested;
  max_align_t alignment;
} allocation_header;

void *__real_malloc(size_t size);
void *__real_realloc(void *pointer, size_t size);
void __real_free(void *pointer);

static size_t malloc_calls;
static size_t realloc_calls;
static size_t free_calls;
static size_t requested_bytes;
static size_t live_bytes;
static size_t peak_live_bytes;

static void record_growth(size_t old_size, size_t new_size) {
  live_bytes -= old_size;
  live_bytes += new_size;
  requested_bytes += new_size;
  if (live_bytes > peak_live_bytes) {
    peak_live_bytes = live_bytes;
  }
}

void *__wrap_malloc(size_t size) {
  malloc_calls++;
  if (size > SIZE_MAX - sizeof(allocation_header)) {
    return NULL;
  }
  allocation_header *header =
      __real_malloc(sizeof(*header) + size);
  if (header == NULL) {
    return NULL;
  }
  header->requested = size;
  record_growth(0, size);
  return header + 1;
}

void __wrap_free(void *pointer) {
  if (pointer == NULL) {
    return;
  }
  allocation_header *header =
      (allocation_header *)pointer - 1;
  live_bytes -= header->requested;
  free_calls++;
  __real_free(header);
}

void *__wrap_realloc(void *pointer, size_t size) {
  if (pointer == NULL) {
    return __wrap_malloc(size);
  }
  if (size == 0) {
    __wrap_free(pointer);
    return NULL;
  }
  realloc_calls++;
  if (size > SIZE_MAX - sizeof(allocation_header)) {
    return NULL;
  }

  allocation_header *old_header =
      (allocation_header *)pointer - 1;
  size_t old_size = old_header->requested;
  allocation_header *new_header =
      __real_realloc(old_header, sizeof(*new_header) + size);
  if (new_header == NULL) {
    return NULL;
  }
  new_header->requested = size;
  record_growth(old_size, size);
  return new_header + 1;
}

__attribute__((destructor)) static void report_allocations(void) {
  fprintf(stderr,
          "malloc=%zu realloc=%zu free=%zu requested=%zu "
          "peak_live=%zu final_live=%zu\n",
          malloc_calls, realloc_calls, free_calls, requested_bytes,
          peak_live_bytes, live_bytes);
}
