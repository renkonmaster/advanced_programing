#include "hugeint.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

huge_int *decimal_to_huge(const char *str);
void print_huge_decimal(const huge_int *x);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return 1;
  }

  huge_int *x = decimal_to_huge(argv[1]);

  if (x == NULL) {
    return 1;
  }

  huge_int *y = decimal_to_huge(argv[2]);

  if (y == NULL) {
    free(x);
    return 1;
  }

  huge_int *result = huge_add(x, y);

  print_huge_decimal(result);
  
  free(x);
  free(y);
  free(result);

  return 0;
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
  
  while(!is_zero(current)) {
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
