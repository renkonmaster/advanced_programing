#include "hugeint.h"
#include <stdio.h>
#include <stdlib.h>

int huge_to_file(const huge_int *x, const char *filename);
huge_int *decimal_to_huge(const char *str);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return 1;
  }

  huge_int *input = decimal_to_huge(argv[2]);

  if (huge_to_file(input, argv[1]) != 0) {
    printf("Error\n");
    free(input);
    return 1;
  }

  free(input);
  
  return 0;
}

int huge_to_file(const huge_int *x, const char *filename){
  FILE *fp = fopen(filename, "wb");
  if (fp == NULL) {
    return 1;
  }

  // prefix check
  if (x->num[0] != 0x01) {
    fclose(fp);
    return 1;
  }

  for (int i = 1; i < x->size - 1; i++) {
    if (fputc(x->num[i], fp) == EOF) {
      fclose(fp);
      return 1;
    }
  }

  // suffix check
  if (x->num[x->size - 1] != 0x02) {
    fclose(fp);
    return 1;
  }

  fclose(fp);
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
