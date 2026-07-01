#include <stdio.h>

typedef struct {
  char str1[3];
  unsigned char num1;
  char str2[7];
  unsigned int num2;
  char str3[10];
} test_t;

int main() { 
  test_t a[3];

  printf("%p\n", &a[0]);
  printf("%p\n", &a[1]);
  printf("%p\n", &a[2]);

  printf("%p, %p, %p, %p, %p\n", &a[0].str1, &a[0].num1, &a[0].str2, &a[0].num2, &a[0].str3);
}
