#include <stdio.h>

int main() {
  unsigned int n;
  scanf("%x", &n);
  char string[128];

  int len = 0;
  unsigned int tmp = n;
  do {
    len++;
    tmp /= 16;
  } while (tmp != 0);


  tmp = n;
  for (int i = len - 1; i >= 0; --i) {
    if (tmp % 16 < 10) {
      string[i] = (tmp % 16) + '0';
    } else {
      string[i] = (tmp % 16) - 10 + 'A';
    }
    tmp /= 16;
  }
  
  string[len] = '\0';

  printf("\n%s\n", string);
  return 0;
}
