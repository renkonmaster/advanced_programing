#include "hugeint_fast.h"

#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__SIZEOF_INT128__)
#error "hugeint_fast.c requires a compiler with unsigned __int128"
#endif

__extension__ typedef unsigned __int128 huge_double_word;

#define HUGE_DECIMAL_BLOCK UINT64_C(10000000000000000000)
#define HUGE_DECIMAL_BLOCK_DIGITS 19U

static void huge_nat_normalize(huge_nat *x) {
  while (x->len > 0 && x->word[x->len - 1] == 0) {
    x->len--;
  }
}

static huge_fast_status huge_nat_reserve(huge_nat *x, size_t need) {
  const size_t max_words = SIZE_MAX / sizeof(*x->word);
  if (need > max_words || x->cap > max_words) {
    return HUGE_FAST_ERANGE;
  }
  if (need <= x->cap) {
    return need == 0 || x->word != NULL ? HUGE_FAST_OK
                                        : HUGE_FAST_EINVAL;
  }

  size_t next = x->cap == 0 ? 1 : x->cap;
  while (next < need) {
    if (next > max_words / 2) {
      next = need;
      break;
    }
    next *= 2;
  }

  uint64_t *grown = realloc(x->word, next * sizeof(*grown));
  if (grown == NULL) {
    return HUGE_FAST_ENOMEM;
  }
  x->word = grown;
  x->cap = next;
  return HUGE_FAST_OK;
}

void huge_nat_init(huge_nat *x) {
  if (x != NULL) {
    x->word = NULL;
    x->len = 0;
    x->cap = 0;
  }
}

void huge_nat_clear(huge_nat *x) {
  if (x != NULL) {
    free(x->word);
    huge_nat_init(x);
  }
}

void huge_nat_swap(huge_nat *a, huge_nat *b) {
  huge_nat temporary = *a;
  *a = *b;
  *b = temporary;
}

bool huge_nat_is_zero(const huge_nat *x) {
  return x == NULL || x->len == 0;
}

size_t huge_nat_bit_length(const huge_nat *x) {
  if (huge_nat_is_zero(x)) {
    return 0;
  }
  uint64_t most_significant = x->word[x->len - 1];
  unsigned leading = (unsigned)__builtin_clzll(most_significant);
  return (x->len - 1) * 64 + (64U - leading);
}

int huge_nat_compare(const huge_nat *a, const huge_nat *b) {
  if (a->len < b->len) {
    return -1;
  }
  if (a->len > b->len) {
    return 1;
  }
  for (size_t i = a->len; i > 0; i--) {
    if (a->word[i - 1] < b->word[i - 1]) {
      return -1;
    }
    if (a->word[i - 1] > b->word[i - 1]) {
      return 1;
    }
  }
  return 0;
}

huge_fast_status huge_nat_set_u64(huge_nat *z, uint64_t value) {
  if (z == NULL) {
    return HUGE_FAST_EINVAL;
  }
  if (value == 0) {
    z->len = 0;
    return HUGE_FAST_OK;
  }
  huge_fast_status status = huge_nat_reserve(z, 1);
  if (status != HUGE_FAST_OK) {
    return status;
  }
  z->word[0] = value;
  z->len = 1;
  return HUGE_FAST_OK;
}

huge_fast_status huge_nat_copy(huge_nat *z, const huge_nat *x) {
  if (z == NULL || x == NULL) {
    return HUGE_FAST_EINVAL;
  }
  if (z == x) {
    return HUGE_FAST_OK;
  }
  huge_fast_status status = huge_nat_reserve(z, x->len);
  if (status != HUGE_FAST_OK) {
    return status;
  }
  if (x->len > 0) {
    memcpy(z->word, x->word, x->len * sizeof(*x->word));
  }
  z->len = x->len;
  return HUGE_FAST_OK;
}

huge_fast_status huge_nat_from_bytes_le(huge_nat *z,
                                        const unsigned char *bytes,
                                        size_t byte_count) {
  if (z == NULL || (bytes == NULL && byte_count != 0)) {
    return HUGE_FAST_EINVAL;
  }
  if (byte_count > SIZE_MAX - 7) {
    return HUGE_FAST_ERANGE;
  }

  size_t word_count = (byte_count + 7) / 8;
  huge_nat result = {0};
  huge_fast_status status = huge_nat_reserve(&result, word_count);
  if (status != HUGE_FAST_OK) {
    return status;
  }
  if (word_count > 0) {
    memset(result.word, 0, word_count * sizeof(*result.word));
  }
  for (size_t i = 0; i < byte_count; i++) {
    unsigned shift = (unsigned)((i % 8) * 8);
    result.word[i / 8] |= (uint64_t)bytes[i] << shift;
  }
  result.len = word_count;
  huge_nat_normalize(&result);

  huge_nat_swap(z, &result);
  huge_nat_clear(&result);
  return HUGE_FAST_OK;
}

huge_fast_status huge_nat_add(huge_nat *z, const huge_nat *x,
                              const huge_nat *y) {
  if (z == NULL || x == NULL || y == NULL) {
    return HUGE_FAST_EINVAL;
  }

  size_t x_len = x->len;
  size_t y_len = y->len;
  size_t longer = x_len > y_len ? x_len : y_len;
  if (longer == SIZE_MAX) {
    return HUGE_FAST_ERANGE;
  }
  huge_fast_status status = huge_nat_reserve(z, longer + 1);
  if (status != HUGE_FAST_OK) {
    return status;
  }

  uint64_t carry = 0;
  for (size_t i = 0; i < longer; i++) {
    uint64_t x_word = i < x_len ? x->word[i] : 0;
    uint64_t y_word = i < y_len ? y->word[i] : 0;
    huge_double_word sum =
        (huge_double_word)x_word + (huge_double_word)y_word + carry;
    z->word[i] = (uint64_t)sum;
    carry = (uint64_t)(sum >> 64);
  }
  if (carry != 0) {
    z->word[longer] = carry;
    z->len = longer + 1;
  } else {
    z->len = longer;
  }
  return HUGE_FAST_OK;
}

huge_fast_status huge_nat_subtract(huge_nat *z, const huge_nat *x,
                                   const huge_nat *y) {
  if (z == NULL || x == NULL || y == NULL) {
    return HUGE_FAST_EINVAL;
  }
  if (huge_nat_compare(x, y) < 0) {
    return HUGE_FAST_ERANGE;
  }

  size_t x_len = x->len;
  size_t y_len = y->len;
  huge_fast_status status = huge_nat_reserve(z, x_len);
  if (status != HUGE_FAST_OK) {
    return status;
  }

  uint64_t borrow = 0;
  for (size_t i = 0; i < x_len; i++) {
    uint64_t x_word = x->word[i];
    uint64_t y_word = i < y_len ? y->word[i] : 0;
    huge_double_word subtrahend = (huge_double_word)y_word + borrow;
    huge_double_word radix = (huge_double_word)1 << 64;
    huge_double_word difference =
        radix + (huge_double_word)x_word - subtrahend;
    z->word[i] = (uint64_t)difference;
    borrow = (huge_double_word)x_word < subtrahend;
  }
  if (borrow != 0) {
    return HUGE_FAST_ERANGE;
  }
  z->len = x_len;
  huge_nat_normalize(z);
  return HUGE_FAST_OK;
}

huge_fast_status huge_nat_multiply(huge_nat *z, const huge_nat *x,
                                   const huge_nat *y) {
  if (z == NULL || x == NULL || y == NULL) {
    return HUGE_FAST_EINVAL;
  }
  if (z == x || z == y) {
    huge_nat temporary = {0};
    huge_fast_status status = huge_nat_multiply(&temporary, x, y);
    if (status == HUGE_FAST_OK) {
      huge_nat_swap(z, &temporary);
    }
    huge_nat_clear(&temporary);
    return status;
  }
  if (x->len == 0 || y->len == 0) {
    z->len = 0;
    return HUGE_FAST_OK;
  }
  if (x->len > SIZE_MAX - y->len) {
    return HUGE_FAST_ERANGE;
  }

  size_t need = x->len + y->len;
  huge_fast_status status = huge_nat_reserve(z, need);
  if (status != HUGE_FAST_OK) {
    return status;
  }
  if (z->word == NULL) {
    return HUGE_FAST_EINVAL;
  }
  memset(z->word, 0, need * sizeof(*z->word));

  for (size_t i = 0; i < x->len; i++) {
    uint64_t carry = 0;
    for (size_t j = 0; j < y->len; j++) {
      size_t position = i + j;
      huge_double_word product =
          (huge_double_word)x->word[i] * y->word[j] + z->word[position] +
          carry;
      z->word[position] = (uint64_t)product;
      carry = (uint64_t)(product >> 64);
    }

    size_t position = i + y->len;
    while (carry != 0 && position < need) {
      huge_double_word sum = (huge_double_word)z->word[position] + carry;
      z->word[position] = (uint64_t)sum;
      carry = (uint64_t)(sum >> 64);
      position++;
    }
    if (carry != 0) {
      return HUGE_FAST_ERANGE;
    }
  }

  z->len = need;
  huge_nat_normalize(z);
  return HUGE_FAST_OK;
}

static huge_fast_status huge_nat_multiply_word(huge_nat *z,
                                                uint64_t factor) {
  if (z->len == 0 || factor == 1) {
    return HUGE_FAST_OK;
  }
  if (factor == 0) {
    z->len = 0;
    return HUGE_FAST_OK;
  }
  if (z->len == SIZE_MAX) {
    return HUGE_FAST_ERANGE;
  }

  huge_fast_status status = huge_nat_reserve(z, z->len + 1);
  if (status != HUGE_FAST_OK) {
    return status;
  }

  uint64_t carry = 0;
  size_t original_len = z->len;
  for (size_t i = 0; i < original_len; i++) {
    huge_double_word product =
        (huge_double_word)z->word[i] * factor + carry;
    z->word[i] = (uint64_t)product;
    carry = (uint64_t)(product >> 64);
  }
  if (carry != 0) {
    z->word[original_len] = carry;
    z->len = original_len + 1;
  }
  return HUGE_FAST_OK;
}

static huge_fast_status huge_nat_add_word(huge_nat *z, uint64_t addend) {
  if (addend == 0) {
    return HUGE_FAST_OK;
  }
  if (z->len == 0) {
    return huge_nat_set_u64(z, addend);
  }
  if (z->len == SIZE_MAX) {
    return HUGE_FAST_ERANGE;
  }

  huge_fast_status status = huge_nat_reserve(z, z->len + 1);
  if (status != HUGE_FAST_OK) {
    return status;
  }

  uint64_t carry = addend;
  size_t i = 0;
  while (carry != 0 && i < z->len) {
    huge_double_word sum = (huge_double_word)z->word[i] + carry;
    z->word[i] = (uint64_t)sum;
    carry = (uint64_t)(sum >> 64);
    i++;
  }
  if (carry != 0) {
    z->word[z->len] = carry;
    z->len++;
  }
  return HUGE_FAST_OK;
}

huge_fast_status huge_nat_from_decimal(huge_nat *z, const char *text) {
  if (z == NULL || text == NULL || text[0] == '\0') {
    return HUGE_FAST_EINVAL;
  }

  size_t digits = strlen(text);
  for (size_t i = 0; i < digits; i++) {
    if (text[i] < '0' || text[i] > '9') {
      return HUGE_FAST_EINVAL;
    }
  }

  huge_nat result = {0};
  size_t first_width = digits % HUGE_DECIMAL_BLOCK_DIGITS;
  if (first_width == 0) {
    first_width = HUGE_DECIMAL_BLOCK_DIGITS;
  }

  size_t offset = 0;
  while (offset < digits) {
    size_t width = offset == 0 ? first_width : HUGE_DECIMAL_BLOCK_DIGITS;
    uint64_t block = 0;
    for (size_t i = 0; i < width; i++) {
      block = block * 10 + (uint64_t)(text[offset + i] - '0');
    }

    huge_fast_status status;
    if (offset == 0) {
      status = huge_nat_set_u64(&result, block);
    } else {
      status = huge_nat_multiply_word(&result, HUGE_DECIMAL_BLOCK);
      if (status == HUGE_FAST_OK) {
        status = huge_nat_add_word(&result, block);
      }
    }
    if (status != HUGE_FAST_OK) {
      huge_nat_clear(&result);
      return status;
    }
    offset += width;
  }

  huge_nat_swap(z, &result);
  huge_nat_clear(&result);
  return HUGE_FAST_OK;
}

static huge_fast_status huge_nat_divide_word(huge_nat *z, uint64_t divisor,
                                              uint64_t *remainder) {
  if (divisor == 0) {
    return HUGE_FAST_EDIVZERO;
  }

  uint64_t rest = 0;
  for (size_t i = z->len; i > 0; i--) {
    huge_double_word current =
        ((huge_double_word)rest << 64) | z->word[i - 1];
    z->word[i - 1] = (uint64_t)(current / divisor);
    rest = (uint64_t)(current % divisor);
  }
  huge_nat_normalize(z);
  if (remainder != NULL) {
    *remainder = rest;
  }
  return HUGE_FAST_OK;
}

huge_fast_status huge_nat_print_decimal(FILE *stream, const huge_nat *x) {
  if (stream == NULL || x == NULL) {
    return HUGE_FAST_EINVAL;
  }
  if (x->len == 0) {
    return fputs("0\n", stream) == EOF ? HUGE_FAST_EINVAL : HUGE_FAST_OK;
  }

  huge_nat remaining = {0};
  huge_fast_status status = huge_nat_copy(&remaining, x);
  if (status != HUGE_FAST_OK) {
    return status;
  }

  if (remaining.len > SIZE_MAX - remaining.len / 32 - 2) {
    huge_nat_clear(&remaining);
    return HUGE_FAST_ERANGE;
  }
  size_t capacity = remaining.len + remaining.len / 32 + 2;
  if (capacity > SIZE_MAX / sizeof(uint64_t)) {
    huge_nat_clear(&remaining);
    return HUGE_FAST_ERANGE;
  }
  uint64_t *blocks = malloc(capacity * sizeof(*blocks));
  if (blocks == NULL) {
    huge_nat_clear(&remaining);
    return HUGE_FAST_ENOMEM;
  }

  size_t count = 0;
  while (remaining.len > 0) {
    if (count == capacity) {
      if (capacity > SIZE_MAX / 2 ||
          capacity * 2 > SIZE_MAX / sizeof(*blocks)) {
        free(blocks);
        huge_nat_clear(&remaining);
        return HUGE_FAST_ERANGE;
      }
      size_t next_capacity = capacity * 2;
      uint64_t *grown =
          realloc(blocks, next_capacity * sizeof(*blocks));
      if (grown == NULL) {
        free(blocks);
        huge_nat_clear(&remaining);
        return HUGE_FAST_ENOMEM;
      }
      blocks = grown;
      capacity = next_capacity;
    }
    status = huge_nat_divide_word(&remaining, HUGE_DECIMAL_BLOCK,
                                  &blocks[count]);
    if (status != HUGE_FAST_OK) {
      free(blocks);
      huge_nat_clear(&remaining);
      return status;
    }
    count++;
  }

  bool output_error = fprintf(stream, "%" PRIu64, blocks[count - 1]) < 0;
  for (size_t i = count - 1; i > 0 && !output_error; i--) {
    output_error =
        fprintf(stream, "%019" PRIu64, blocks[i - 1]) < 0;
  }
  if (!output_error) {
    output_error = fputc('\n', stream) == EOF;
  }

  free(blocks);
  huge_nat_clear(&remaining);
  return output_error ? HUGE_FAST_EINVAL : HUGE_FAST_OK;
}

static bool huge_nat_get_bit(const huge_nat *x, size_t bit) {
  size_t word_index = bit / 64;
  unsigned bit_index = (unsigned)(bit % 64);
  return word_index < x->len &&
         ((x->word[word_index] >> bit_index) & UINT64_C(1)) != 0;
}

static huge_fast_status huge_nat_shift_one_add_bit(huge_nat *x,
                                                    unsigned bit) {
  if (bit > 1) {
    return HUGE_FAST_EINVAL;
  }
  if (x->len == 0) {
    if (bit == 0) {
      return HUGE_FAST_OK;
    }
    return huge_nat_set_u64(x, 1);
  }
  if (x->len == SIZE_MAX) {
    return HUGE_FAST_ERANGE;
  }

  huge_fast_status status = huge_nat_reserve(x, x->len + 1);
  if (status != HUGE_FAST_OK) {
    return status;
  }
  uint64_t carry = bit;
  size_t original_len = x->len;
  for (size_t i = 0; i < original_len; i++) {
    huge_double_word shifted =
        (huge_double_word)x->word[i] * 2 + carry;
    x->word[i] = (uint64_t)shifted;
    carry = (uint64_t)(shifted >> 64);
  }
  if (carry != 0) {
    x->word[original_len] = carry;
    x->len = original_len + 1;
  }
  return HUGE_FAST_OK;
}

static huge_fast_status huge_nat_divide_no_alias(
    huge_nat *q, huge_nat *r, const huge_nat *x, const huge_nat *y) {
  if (y->len == 0) {
    return HUGE_FAST_EDIVZERO;
  }
  if (x->len == 0) {
    q->len = 0;
    r->len = 0;
    return HUGE_FAST_OK;
  }
  if (huge_nat_compare(x, y) < 0) {
    q->len = 0;
    return huge_nat_copy(r, x);
  }

  size_t bits = huge_nat_bit_length(x);
  size_t quotient_words = (bits + 63) / 64;
  if (y->len == SIZE_MAX) {
    return HUGE_FAST_ERANGE;
  }
  huge_fast_status status = huge_nat_reserve(q, quotient_words);
  if (status != HUGE_FAST_OK) {
    return status;
  }
  status = huge_nat_reserve(r, y->len + 1);
  if (status != HUGE_FAST_OK) {
    return status;
  }

  if (q->word == NULL) {
    return HUGE_FAST_EINVAL;
  }
  memset(q->word, 0, quotient_words * sizeof(*q->word));
  q->len = quotient_words;
  r->len = 0;

  for (size_t bit = bits; bit > 0;) {
    bit--;
    status =
        huge_nat_shift_one_add_bit(r, huge_nat_get_bit(x, bit) ? 1U : 0U);
    if (status != HUGE_FAST_OK) {
      return status;
    }
    if (huge_nat_compare(r, y) >= 0) {
      status = huge_nat_subtract(r, r, y);
      if (status != HUGE_FAST_OK) {
        return status;
      }
      q->word[bit / 64] |= UINT64_C(1) << (bit % 64);
    }
  }
  huge_nat_normalize(q);
  return HUGE_FAST_OK;
}

huge_fast_status huge_nat_divide(huge_nat *q, huge_nat *r,
                                 const huge_nat *x, const huge_nat *y) {
  if (q == NULL || r == NULL || x == NULL || y == NULL || q == r) {
    return HUGE_FAST_EINVAL;
  }

  if (q == x || q == y || r == x || r == y) {
    huge_nat quotient = {0};
    huge_nat remainder = {0};
    huge_fast_status status =
        huge_nat_divide_no_alias(&quotient, &remainder, x, y);
    if (status == HUGE_FAST_OK) {
      huge_nat_swap(q, &quotient);
      huge_nat_swap(r, &remainder);
    }
    huge_nat_clear(&quotient);
    huge_nat_clear(&remainder);
    return status;
  }
  return huge_nat_divide_no_alias(q, r, x, y);
}

static huge_fast_status huge_nat_modular_power_division(
    huge_nat *z, const huge_nat *base, const huge_nat *exponent,
    const huge_nat *modulus) {
  huge_nat result = {0};
  huge_nat reduced_base = {0};
  huge_nat product = {0};
  huge_nat quotient = {0};
  huge_nat remainder = {0};

  huge_fast_status status =
      huge_nat_divide(&quotient, &reduced_base, base, modulus);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  status = huge_nat_set_u64(&result, 1);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  status = huge_nat_divide(&quotient, &remainder, &result, modulus);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  huge_nat_swap(&result, &remainder);

  size_t bits = huge_nat_bit_length(exponent);
  for (size_t bit = 0; bit < bits; bit++) {
    if (huge_nat_get_bit(exponent, bit)) {
      status = huge_nat_multiply(&product, &result, &reduced_base);
      if (status != HUGE_FAST_OK) {
        goto cleanup;
      }
      status =
          huge_nat_divide(&quotient, &remainder, &product, modulus);
      if (status != HUGE_FAST_OK) {
        goto cleanup;
      }
      huge_nat_swap(&result, &remainder);
    }

    if (bit + 1 < bits) {
      status =
          huge_nat_multiply(&product, &reduced_base, &reduced_base);
      if (status != HUGE_FAST_OK) {
        goto cleanup;
      }
      status =
          huge_nat_divide(&quotient, &remainder, &product, modulus);
      if (status != HUGE_FAST_OK) {
        goto cleanup;
      }
      huge_nat_swap(&reduced_base, &remainder);
    }
  }

  status = huge_nat_copy(z, &result);

cleanup:
  huge_nat_clear(&result);
  huge_nat_clear(&reduced_base);
  huge_nat_clear(&product);
  huge_nat_clear(&quotient);
  huge_nat_clear(&remainder);
  return status;
}

static uint64_t huge_nat_montgomery_factor(uint64_t modulus_word) {
  uint64_t inverse = 1;
  huge_double_word radix = (huge_double_word)1 << 64;
  for (unsigned i = 0; i < 6; i++) {
    uint64_t product =
        (uint64_t)((huge_double_word)modulus_word * inverse);
    uint64_t correction =
        (uint64_t)(radix + (huge_double_word)2 - product);
    inverse = (uint64_t)((huge_double_word)inverse * correction);
  }
  return (uint64_t)(radix - inverse);
}

static huge_fast_status huge_nat_montgomery_multiply(
    huge_nat *z, const huge_nat *x, const huge_nat *y,
    const huge_nat *modulus, uint64_t factor, huge_nat *scratch) {
  size_t words = modulus->len;
  if (words == 0 || words > (SIZE_MAX - 2) / 2) {
    return words == 0 ? HUGE_FAST_EDIVZERO : HUGE_FAST_ERANGE;
  }

  huge_fast_status status = huge_nat_multiply(scratch, x, y);
  if (status != HUGE_FAST_OK) {
    return status;
  }

  size_t total = words * 2 + 2;
  status = huge_nat_reserve(scratch, total);
  if (status != HUGE_FAST_OK) {
    return status;
  }
  if (scratch->len < total) {
    memset(scratch->word + scratch->len, 0,
           (total - scratch->len) * sizeof(*scratch->word));
  }
  scratch->len = total;

  for (size_t i = 0; i < words; i++) {
    uint64_t multiplier =
        (uint64_t)((huge_double_word)scratch->word[i] * factor);
    uint64_t carry = 0;
    for (size_t j = 0; j < words; j++) {
      size_t position = i + j;
      huge_double_word sum =
          (huge_double_word)multiplier * modulus->word[j] +
          scratch->word[position] + carry;
      scratch->word[position] = (uint64_t)sum;
      carry = (uint64_t)(sum >> 64);
    }

    size_t position = i + words;
    while (carry != 0) {
      if (position >= total) {
        return HUGE_FAST_ERANGE;
      }
      huge_double_word sum =
          (huge_double_word)scratch->word[position] + carry;
      scratch->word[position] = (uint64_t)sum;
      carry = (uint64_t)(sum >> 64);
      position++;
    }
  }

  status = huge_nat_reserve(z, words + 1);
  if (status != HUGE_FAST_OK) {
    return status;
  }
  memcpy(z->word, scratch->word + words,
         (words + 1) * sizeof(*z->word));
  z->len = words + 1;
  huge_nat_normalize(z);
  while (huge_nat_compare(z, modulus) >= 0) {
    status = huge_nat_subtract(z, z, modulus);
    if (status != HUGE_FAST_OK) {
      return status;
    }
  }
  return HUGE_FAST_OK;
}

static huge_fast_status huge_nat_modular_power_montgomery(
    huge_nat *z, const huge_nat *base, const huge_nat *exponent,
    const huge_nat *modulus) {
  huge_fast_status status = HUGE_FAST_OK;
  size_t words = modulus->len;
  if (words == 0 || words > (SIZE_MAX - 1) / 2) {
    return words == 0 ? HUGE_FAST_EDIVZERO : HUGE_FAST_ERANGE;
  }

  huge_nat power_of_radix_squared = {0};
  huge_nat rr = {0};
  huge_nat reduced_base = {0};
  huge_nat quotient = {0};
  huge_nat one = {0};
  huge_nat scratch = {0};
  huge_nat result = {0};
  huge_nat temporary = {0};
  huge_nat powers[16] = {{0}};

  size_t power_words = words * 2 + 1;
  status = huge_nat_reserve(&power_of_radix_squared, power_words);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  if (power_of_radix_squared.word == NULL) {
    status = HUGE_FAST_EINVAL;
    goto cleanup;
  }
  memset(power_of_radix_squared.word, 0,
         power_words * sizeof(*power_of_radix_squared.word));
  power_of_radix_squared.word[power_words - 1] = 1;
  power_of_radix_squared.len = power_words;

  status = huge_nat_divide(&quotient, &rr, &power_of_radix_squared,
                           modulus);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  status = huge_nat_divide(&quotient, &reduced_base, base, modulus);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  status = huge_nat_set_u64(&one, 1);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }

  uint64_t factor = huge_nat_montgomery_factor(modulus->word[0]);
  status = huge_nat_montgomery_multiply(&powers[0], &one, &rr, modulus,
                                        factor, &scratch);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  status = huge_nat_montgomery_multiply(&powers[1], &reduced_base, &rr,
                                        modulus, factor, &scratch);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }
  for (size_t i = 2; i < 16; i++) {
    status = huge_nat_montgomery_multiply(
        &powers[i], &powers[i - 1], &powers[1], modulus, factor, &scratch);
    if (status != HUGE_FAST_OK) {
      goto cleanup;
    }
  }

  status = huge_nat_copy(&result, &powers[0]);
  if (status != HUGE_FAST_OK) {
    goto cleanup;
  }

  size_t bits = huge_nat_bit_length(exponent);
  size_t nibbles = (bits + 3) / 4;
  for (size_t nibble = nibbles; nibble > 0;) {
    nibble--;
    for (unsigned square = 0; square < 4; square++) {
      status = huge_nat_montgomery_multiply(
          &temporary, &result, &result, modulus, factor, &scratch);
      if (status != HUGE_FAST_OK) {
        goto cleanup;
      }
      huge_nat_swap(&result, &temporary);
    }

    unsigned value = 0;
    for (unsigned bit = 0; bit < 4; bit++) {
      size_t exponent_bit = nibble * 4 + bit;
      if (exponent_bit < bits &&
          huge_nat_get_bit(exponent, exponent_bit)) {
        value |= 1U << bit;
      }
    }
    if (value != 0) {
      status = huge_nat_montgomery_multiply(
          &temporary, &result, &powers[value], modulus, factor, &scratch);
      if (status != HUGE_FAST_OK) {
        goto cleanup;
      }
      huge_nat_swap(&result, &temporary);
    }
  }

  status = huge_nat_montgomery_multiply(z, &result, &one, modulus, factor,
                                        &scratch);

cleanup:
  huge_nat_clear(&power_of_radix_squared);
  huge_nat_clear(&rr);
  huge_nat_clear(&reduced_base);
  huge_nat_clear(&quotient);
  huge_nat_clear(&one);
  huge_nat_clear(&scratch);
  huge_nat_clear(&result);
  huge_nat_clear(&temporary);
  for (size_t i = 0; i < 16; i++) {
    huge_nat_clear(&powers[i]);
  }
  return status;
}

huge_fast_status huge_nat_modular_power(huge_nat *z, const huge_nat *base,
                                        const huge_nat *exponent,
                                        const huge_nat *modulus) {
  if (z == NULL || base == NULL || exponent == NULL || modulus == NULL) {
    return HUGE_FAST_EINVAL;
  }
  if (z == base || z == exponent || z == modulus) {
    huge_nat temporary = {0};
    huge_fast_status status =
        huge_nat_modular_power(&temporary, base, exponent, modulus);
    if (status == HUGE_FAST_OK) {
      huge_nat_swap(z, &temporary);
    }
    huge_nat_clear(&temporary);
    return status;
  }
  if (modulus->len == 0) {
    return HUGE_FAST_EDIVZERO;
  }
  if (modulus->len == 1 && modulus->word[0] == 1) {
    z->len = 0;
    return HUGE_FAST_OK;
  }
  if (exponent->len == 0) {
    return huge_nat_set_u64(z, 1);
  }

  if ((modulus->word[0] & UINT64_C(1)) != 0) {
    return huge_nat_modular_power_montgomery(z, base, exponent, modulus);
  }
  return huge_nat_modular_power_division(z, base, exponent, modulus);
}
