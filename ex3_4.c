#include <stdio.h>

unsigned int strtoui(char * string);

int main() {
  char string[128];
  scanf("%s", string);

  unsigned int n;
  n = strtoui(string);

  printf("\n%x\n", n);
  return 0;
}

unsigned int strtoui(char * string) {
  unsigned int ret = 0;
  int i = 0;
  while(string[i] != '\0') {
    unsigned int tmp = 0;
    if (string[i] >= '0' && string[i] <= '9')
      tmp = string[i] - '0';
    else if (string[i] >= 'A' && string[i] <= 'F')
      tmp = string[i] - 'A' + 10;
    else if (string[i] >= 'a' && string[i] <= 'f')
      tmp = string[i] - 'a' + 10;

    ret = (ret * 16) + tmp;
    ++i;
  }
  
  return ret;
}