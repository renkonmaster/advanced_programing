#include <stdio.h>
#include <stdlib.h>

typedef union {
  unsigned long long ull;
  unsigned char uc[8];
} ulonglong_uchar;

int main(void) {
  ulonglong_uchar ull_uc;
  char string[128];
  char *endptr;
  scanf("%s", string);

  for (int i = 0; i < 8; ++i) {
    ull_uc.uc[i] = (strtoull(string, &endptr, 10)>> 8 * i) & 0xFF;
  }

  printf("%llu\n", ull_uc.ull);

  return 0;
}
