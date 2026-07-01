#include <stdio.h>
#include <stdlib.h>

int main() {
  int *a[10] = {0};

  for (int i = 0; i < 5; ++i) {
    int *tmp;
    tmp = malloc(sizeof(int));

    if (tmp == NULL) {
      printf("Failed to Allocate Memory\n");
      return 1;
    }
    a[i] = tmp;
  }

  printf("C)\n");
  for (int i = 0; i < 10; ++i) {
    printf("a[%d] = %p\n", i, a[i]);
  }
  printf("\n");


  free(a[1]);
  free(a[2]);
  free(a[4]);
  a[1] = NULL;
  a[2] = NULL;
  a[4] = NULL;

  printf("E)\n");
  for (int i = 0; i < 10; ++i) {
    printf("a[%d] = %p\n", i, a[i]);
  }
  printf("\n");

  for (int i = 5; i < 10; ++i) {
    int *tmp;
    tmp = malloc(sizeof(int));

    if (tmp == NULL) {
      printf("Failed to Allocate Memory\n");
      return 1;
    }

    a[i] = tmp;
  }

  printf("G)\n");
  for (int i = 0; i < 10; ++i) {
    printf("a[%d] = %p\n", i, a[i]);
  }

  for (int i = 0; i < 10; ++i) {
    free(a[i]);
  }
}
