#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char array[10];

int main(void) {
  printf("Input an integer: ");
  fflush(stdout);
  int n;
  scanf("%d", &n);

  size_t abs_n;
  if (n == INT_MIN) {
    abs_n = (size_t)INT_MAX + 1;
  } else if (n < 0) {
    abs_n = (size_t)(-n);
  } else {
    abs_n = (size_t)n;
  }

  if (n >= 0) {
    printf("%d + %zu == %zu\n", n, sizeof array, n + sizeof array);
  } else if (abs_n <= sizeof array) {
    printf("%d + %zu == %zu\n", n, sizeof array, sizeof array - abs_n);
  } else {
    printf("%d + %zu == -%zu\n", n, sizeof array, abs_n - sizeof array);
  }
  
  return 0;
}
