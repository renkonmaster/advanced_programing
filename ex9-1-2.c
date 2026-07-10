#include <stdio.h>
#include <stdlib.h>

int main() {
  char *input = "ex9-1.txt";
  
  FILE *infp;
  infp = fopen(input, "r");
  if (infp == NULL) {
    printf("Failed to Open %s\n", input);
  }

  int sum = 0;
  for (int i = 0; i < 1000; ++i){
    int x;
    fscanf(infp, "%d", &x);
    sum += x;
  }
  fclose(infp);

  printf("sum = %d\n", sum);
}
