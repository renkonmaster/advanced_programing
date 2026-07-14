// fflushを実行する場合  : 0.266412
// fflushを実行しない場合: 0.001490

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
  char filename[] = "test.bin";
  FILE *fp;
  fp = fopen(filename, "wb");
  if (fp == NULL) {
    printf("Failed to Open test.bin\n");
    exit(EXIT_FAILURE);
  }

  clock_t start, end;

  srand((unsigned int)time(NULL));

  start = clock();
  for (int i = 0; i < 100000; ++i) {
    unsigned int x = rand() % 256;
    fputc(x, fp);
    fflush(fp);
  }
  end = clock();
  fclose(fp);

  printf("fflush each time: %f sec\n", (double)(end - start) / CLOCKS_PER_SEC);

  fp = fopen("test.bin", "wb");
  if (fp == NULL) {
    printf("Failed to Open test.bin\n");
    exit(EXIT_FAILURE);
  }
  
  start = clock();
  for (int i = 0; i < 100000; ++i) {
    unsigned int x = rand() % 256;
    fputc(x, fp);
  }
  end = clock();
  fclose(fp);

  printf("no fflush       : %f sec\n", (double)(end-start)/CLOCKS_PER_SEC);

  return 0;
}
