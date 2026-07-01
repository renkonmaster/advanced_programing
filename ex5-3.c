#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_matrix(int n, int mat[][n]) {
  double len = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (len < log10(mat[i][j])) {
        len = log10(mat[i][j]);
      }
    }
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      printf("%*d ", (int)ceil(len), mat[i][j]);
    }
    printf("\n");
  }

  printf("\n");
}

// a[i][j] = sum sum a[i][k]*a[k][j] 
void square_matrix(int n, int mat[][n]){
  int *result = calloc(n * n, sizeof(int));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      for (int k = 0; k < n; ++k) {
        result[n * i + j] += mat[i][k] * mat[k][j];
      }
    }
  }

  memcpy(mat, result, n * n * sizeof(int));

  free(result);
}

int main() {
  int n;
  scanf("%d", &n);

  int m[n][n];
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      m[i][j] = i * n + j;
    }
  }

  print_matrix(n, m);
  square_matrix(n, m);
  print_matrix(n, m);
  square_matrix(n, m);
  print_matrix(n, m);
}
