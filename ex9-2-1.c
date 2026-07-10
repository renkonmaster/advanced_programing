#include <stdio.h>
#include <stdlib.h>

int main() {
  char *output = "ex9-2.dat";

  FILE *fp;
  fp = fopen(output, "wb");
  if (fp == NULL) {
    printf("Failed to Open %s\n", output);
  }
  
  srand(1);

  for (int i = 0; i < 1000; ++i ){
    short int x = rand() % 10000;
    fwrite(&x, sizeof x, 1, fp);
  }

  fclose(fp);
}
