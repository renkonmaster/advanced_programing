#include <stdio.h>

int main(void) {
  int n;
  char string[128];

  scanf("%d", &n);

  int len = 0;
  int tmp = n;

  do {
    len++;
    tmp /= 10;
  } while (tmp != 0);

  tmp = n;
  string[len] = '\0';
  for (int i = len - 1; i >= 0; --i) {
    string[i] = (tmp % 10) + '0';
    tmp /= 10;
  }

  printf("\n%s\n", string);
}