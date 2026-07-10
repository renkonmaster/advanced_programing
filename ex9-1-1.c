#include <stdio.h>
#include <stdlib.h>

int main() {
  char *output = "ex9-1.txt";

  FILE *outfp;
  outfp = fopen(output, "w");
  if (outfp == NULL) {
    printf("Failed to Open %s\n", output);
  }
  
  srand(1);

  for (int i = 0; i < 1000; ++i ){
    int x = rand() % 10000;
    fprintf(outfp, "%d ", x);
  }

  fclose(outfp);
}
