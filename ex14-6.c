#include "hugeint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

huge_int *decimal_to_huge(const char *str);
huge_int *file_to_huge(const char *filename);
int huge_compare(const huge_int *a, const huge_int *b); // a > b => true

void print_huge_decimal(const huge_int *x);

huge_int *modular_power(const huge_int *base, const huge_int *exponent,
                        const huge_int *modulus);
huge_int *copy_huge(const huge_int *x);
huge_int *huge_mod(const huge_int *x, const huge_int *mod);

int main(int argc, char *argv[]) {
  if (argc != 4) {
    return 1;
  }

  // c = m^e mod N
  // argv[1]:m, argv[2]:N, argv[3]:e

  huge_int *m = file_to_huge(argv[1]);
  huge_int *N = decimal_to_huge(argv[2]);
  huge_int *e = decimal_to_huge(argv[3]);

  if (m == NULL || N == NULL || e == NULL) {
    free(m);
    free(N);
    free(e);
    return 1;
  }

  if (is_zero(N)) {
    free(m);
    free(N);
    free(e);
    return 1;
  }

  print_huge_decimal(m); // 諸悪の根源
  int comparison = huge_compare(m, N);

  if (comparison >= 0) {
    printf("Too big.\n");
    free(m);
    free(N);
    free(e);
    return 1;
  }

  huge_int *c = modular_power(m, e, N);
  free(m);
  free(N);
  free(e);

  if (c == NULL) {
    return 1;
  }

  print_huge_decimal(c);
  free(c);

  return 0;
}

huge_int *modular_power(const huge_int *base, const huge_int *exponent,
                        const huge_int *modulus) {
  huge_int *current_exp = copy_huge(exponent);
  if (current_exp == NULL) {
    return NULL;
  }

  huge_int *current_bas = huge_mod(base, modulus);
  if (current_bas == NULL) {
    free(current_exp);
    return NULL;
  }

  huge_int *one = from_uchar(1);
  if (one == NULL) {
    free(current_exp);
    free(current_bas);
    return NULL;
  }

  huge_int *current_result = huge_mod(one, modulus);
  free(one);

  if (current_result == NULL) {
    free(current_exp);
    free(current_bas);
    return NULL;
  }

  huge_int *two = from_uchar(2);
  if (two == NULL) {
    free(current_exp);
    free(current_bas);
    free(current_result);
    return NULL;
  }

  while (!is_zero(current_exp)) {
    if ((current_exp->num[0] & (unsigned char)1) != 0) {
      huge_int *multiplied = huge_multiply(current_result, current_bas);
      if (multiplied == NULL) {
        free(two);
        free(current_exp);
        free(current_bas);
        free(current_result);
        return NULL;
      }

      huge_int *next_result = huge_mod(multiplied, modulus);
      if (next_result == NULL) {
        free(two);
        free(multiplied);
        free(current_exp);
        free(current_bas);
        free(current_result);
        return NULL;
      }

      free(current_result);
      free(multiplied);

      current_result = next_result;
    }

    huge_int *multiplied = huge_multiply(current_bas, current_bas);
    if (multiplied == NULL) {
      free(two);
      free(current_exp);
      free(current_bas);
      free(current_result);
      return NULL;
    }

    huge_int *next_bas = huge_mod(multiplied, modulus);
    free(multiplied);
    free(current_bas);
    current_bas = next_bas;

    huge_pair divided = huge_divide(current_exp, two);
    free(divided.r);
    free(current_exp);
    current_exp = divided.q;
  }

  free(two);
  free(current_bas);
  free(current_exp);

  return current_result;
}

huge_int *copy_huge(const huge_int *x) {
  size_t bytes = sizeof(huge_int) + (size_t)x->size;

  huge_int *copy = malloc(bytes);
  if (copy == NULL) {
    return NULL;
  }

  memcpy(copy, x, bytes);
  return copy;
}

huge_int *huge_mod(const huge_int *x, const huge_int *mod) {
  huge_pair divided = huge_divide(x, mod);

  free(divided.q);

  return divided.r;
}

huge_int *decimal_to_huge(const char *str) {
  if (str == NULL || str[0] == '\0') {
    return NULL;
  }

  huge_int *ret = from_uchar(0);
  huge_int *ten = from_uchar(10);

  if (ret == NULL || ten == NULL) {
    free(ret);
    free(ten);
    return NULL;
  }

  for (size_t i = 0; str[i] != '\0'; i++) {
    if (str[i] < '0' || str[i] > '9') {
      free(ret);
      free(ten);
      return NULL;
    }

    huge_int *multiplied = huge_multiply(ret, ten);

    free(ret);

    if (multiplied == NULL) {
      free(ten);
      return NULL;
    }

    huge_int *next = from_uchar((unsigned char)(str[i] - '0'));

    ret = huge_add(multiplied, next);
    free(next);
    free(multiplied);

    if (ret == NULL) {
      free(ten);
      return NULL;
    }
  }

  free(ten);

  return ret;
}

void print_huge_decimal(const huge_int *x) {
  if (x == NULL) {
    return;
  }

  if (is_zero(x)) {
    printf("0\n");
    return;
  }

  size_t x_bytes = sizeof(huge_int) + x->size;

  huge_int *current = malloc(sizeof(huge_int) + x->size);
  if (current == NULL) {
    return;
  }

  memcpy(current, x, x_bytes);

  huge_int *ten = from_uchar(10);

  if (ten == NULL) {
    free(current);
    return;
  }

  size_t digit_cnt = 0;
  char *digits = malloc(x->size * 3 + 1);
  if (digits == NULL) {
    free(current);
    free(ten);
    return;
  }

  while (!is_zero(current)) {
    huge_pair divided = huge_divide(current, ten);
    free(current);
    current = divided.q;
    digits[digit_cnt] = ((char)(to_ulonglong(divided.r) + '0'));

    digit_cnt++;
    free(divided.r);
  }

  for (size_t i = digit_cnt; i > 0; i--) {
    printf("%c", digits[i - 1]);
  }

  printf("\n");

  free(digits);
  free(ten);
  free(current);
}

huge_int *file_to_huge(const char *filename) {
  FILE *fp = fopen(filename, "rb");

  if (fp == NULL) {
    return NULL;
  }

  huge_int *result = malloc(sizeof(huge_int) + BUFFER_SIZE);

  if (result == NULL) {
    fclose(fp);
    return NULL;
  }

  size_t current = 0;

  // prefix
  result->num[current] = 0x01;
  current++;

  int ch;
  fseek(fp, 0, SEEK_SET);
  while ((ch = fgetc(fp)) != EOF) {
    if (current >= BUFFER_SIZE - 1) {
      free(result);
      fclose(fp);
      return NULL;
    }

    result->num[current] = (unsigned char)ch;
    current++;
  }

  // suffix
  result->num[current] = 0x02;
  current++;

  result->size = (int)current;

  fclose(fp);
  return result;
}

int huge_compare(const huge_int *a, const huge_int *b) {
  int a_size = a->size;
  while (a_size > 0 && a->num[a_size - 1] == 0) {
    a_size--;
  }

  int b_size = b->size;
  while (b_size > 0 && b->num[b_size - 1] == 0) {
    b_size--;
  }

  if (a_size < b_size) {
    return -1;
  }

  if (a_size > b_size) {
    return 1;
  }

  for (int i = a_size; i > 0; --i) {
    if (a->num[i - 1] < b->num[i - 1]) {
      return -1;
    }

    if (a->num[i - 1] > b->num[i - 1]) {
      return 1;
    }
  }

  return 0;
}
