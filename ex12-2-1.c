#include <stdio.h>
#include "ex12-2.h"

int main(void) {
  complex_number x = {3.0, 4.0};
  complex_number y = {3.0, -4.0};
  complex_number z = {2.0, 1.0};

  complex_number ans = cmp_mul(x, y);
  ans = cmp_sub(ans, z);
  printf("%f + j * %f\n", ans.re, ans.im);
  return 0;
}