#include "hugeint_fast.h"

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

static huge_fast_status file_to_huge_nat(huge_nat *result,
                                         const char *filename) {
  FILE *stream = fopen(filename, "rb");
  if (stream == NULL) {
    return HUGE_FAST_EINVAL;
  }

  unsigned char bytes[BUFFER_SIZE];
  size_t used = 0;
  bytes[used++] = 0x01;

  int ch;
  while ((ch = fgetc(stream)) != EOF) {
    if (used >= BUFFER_SIZE - 1) {
      fclose(stream);
      return HUGE_FAST_ERANGE;
    }
    bytes[used++] = (unsigned char)ch;
  }
  if (ferror(stream)) {
    fclose(stream);
    return HUGE_FAST_EINVAL;
  }
  bytes[used++] = 0x02;

  if (fclose(stream) != 0) {
    return HUGE_FAST_EINVAL;
  }
  return huge_nat_from_bytes_le(result, bytes, used);
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    return 1;
  }

  huge_nat message = {0};
  huge_nat modulus = {0};
  huge_nat exponent = {0};
  huge_nat encrypted = {0};
  int exit_status = 1;

  huge_fast_status status = file_to_huge_nat(&message, argv[1]);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  status = huge_nat_from_decimal(&modulus, argv[2]);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  status = huge_nat_from_decimal(&exponent, argv[3]);
  if (status != HUGE_FAST_OK || huge_nat_is_zero(&modulus)) {
    goto cleanup;
  }

  /*
   * Keep the same two-line normal output as ex14-6.c so stdout can be
   * compared byte-for-byte during validation and benchmarking.
   */
  status = huge_nat_print_decimal(stdout, &message);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }

  if (huge_nat_compare(&message, &modulus) >= 0) {
    puts("Too big.");
    goto cleanup;
  }

  status =
      huge_nat_modular_power(&encrypted, &message, &exponent, &modulus);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  status = huge_nat_print_decimal(stdout, &encrypted);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }

  exit_status = 0;

cleanup:
  huge_nat_clear(&message);
  huge_nat_clear(&modulus);
  huge_nat_clear(&exponent);
  huge_nat_clear(&encrypted);
  return exit_status;
}
