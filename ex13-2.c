#include <stdio.h>
#include <stdlib.h>

unsigned long long from_str(char *str){
  char *end;
  unsigned long long ret = strtoull(str, &end, 10);
  return ret;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return 1;
  }

  unsigned long long result = 1;
  unsigned long long base = from_str(argv[1]);
  unsigned int exponent = from_str(argv[2]);

  while (exponent != 0) {
    if ((exponent & 1U) != 0) {
      result *= base;
    }

    exponent >>= 1;

    if (exponent != 0) {
      base *= base;
    }
  }

  printf("%llu\n", result);

  return 0;
}
