#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hugeint.h"

#define BUFFER_SIZE 4096

huge_int *file_to_huge(const char *filename);
void print_huge_decimal(const huge_int *x);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    return 1;
  }

  huge_int *result = file_to_huge(argv[1]);
  if (result == NULL) {
    return 1;
  }

  print_huge_decimal(result);

  free(result);
  return 0;
}

huge_int *file_to_huge(const char *filename) {
  FILE *fp = fopen(filename, "rb");
  
  if (fp == NULL) {
    return NULL;
  }

  huge_int *result = malloc(sizeof(huge_int) + BUFFER_SIZE);

  if(result == NULL) {
    fclose(fp);
    return NULL;
  }

  size_t current = 0;

  // prefix
  result->num[current] = 0x01;
  current++;

  int ch;
  fseek(fp, 0, SEEK_SET);
  while((ch = fgetc(fp)) != EOF) {
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
    free(ten);
    return;
  }

  size_t digit_cnt = 0;
  char *digits = malloc(x->size * 3 + 1);
  if (digits == NULL) {
    free(current);
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
