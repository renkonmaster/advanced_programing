#include <stdio.h>
#include <stdlib.h>

int main() {
  char *input = "ex9-2.dat";
  
  FILE *fp;
  fp = fopen(input, "rb");
  if (fp == NULL) {
    printf("Failed to Open %s\n", input);
  }

  int sum = 0;
  for (int i = 0; i < 1000; ++i){
    short int x;
    fread(&x, sizeof x, 1, fp);
    sum += x;
  }
  fclose(fp);

  printf("sum = %d\n", sum);
}
