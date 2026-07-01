#include <stdio.h>

void swap(char* a, char* b) { // 誤り
  char temp;
  temp = *a;
  *a = *b;
  *b = temp;
  printf("In swap: addresses of a, b = %p, %p\n", a, b);
}

int main() {
  char a = 'A', b = 'B';
  printf("In main: addresses of a, b = %p, %p\n", &a, &b);
  printf("Before: a=%c, b=%c\n", a, b);
  swap(&a, &b);
  printf("After : a=%c, b=%c\n", a, b);
  return 0;
}
