#ifndef HUGEINT_FAST_H
#define HUGEINT_FAST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Go's math/big package separates an unsigned, normalized magnitude ("nat")
 * from the sign of Int.  huge_nat is the corresponding unsigned core used by
 * ex14-6-fast.c.  The all-zero initializer is a valid value representing 0.
 */
typedef struct {
  uint64_t *word; /* little-endian, base 2^64 */
  size_t len;     /* number of words in use */
  size_t cap;     /* number of allocated words */
} huge_nat;

typedef enum {
  HUGE_FAST_OK = 0,
  HUGE_FAST_ENOMEM,
  HUGE_FAST_EDIVZERO,
  HUGE_FAST_EINVAL,
  HUGE_FAST_ERANGE
} huge_fast_status;

void huge_nat_init(huge_nat *x);
void huge_nat_clear(huge_nat *x);
void huge_nat_swap(huge_nat *a, huge_nat *b);

bool huge_nat_is_zero(const huge_nat *x);
size_t huge_nat_bit_length(const huge_nat *x);
int huge_nat_compare(const huge_nat *a, const huge_nat *b);

huge_fast_status huge_nat_set_u64(huge_nat *z, uint64_t value);
huge_fast_status huge_nat_copy(huge_nat *z, const huge_nat *x);
huge_fast_status huge_nat_from_bytes_le(huge_nat *z,
                                        const unsigned char *bytes,
                                        size_t byte_count);
huge_fast_status huge_nat_from_decimal(huge_nat *z, const char *text);
huge_fast_status huge_nat_print_decimal(FILE *stream, const huge_nat *x);

/* Result operands may alias their inputs. */
huge_fast_status huge_nat_add(huge_nat *z, const huge_nat *x,
                              const huge_nat *y);
huge_fast_status huge_nat_subtract(huge_nat *z, const huge_nat *x,
                                   const huge_nat *y);
huge_fast_status huge_nat_multiply(huge_nat *z, const huge_nat *x,
                                   const huge_nat *y);
/* q and r must be different objects; either may alias x or y. */
huge_fast_status huge_nat_divide(huge_nat *q, huge_nat *r,
                                 const huge_nat *x, const huge_nat *y);

/*
 * Computes base^exponent mod modulus.  Odd moduli use 64-bit Montgomery
 * multiplication and a fixed 4-bit window; even moduli use the generic
 * division fallback.
 */
huge_fast_status huge_nat_modular_power(huge_nat *z, const huge_nat *base,
                                        const huge_nat *exponent,
                                        const huge_nat *modulus);

#endif
