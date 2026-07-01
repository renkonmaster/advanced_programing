#include <stdio.h>
#include <string.h>
#include <math.h>

#define EPS 1.0e-6f

void vec_mult_s(int n, float *in, float a, float *out);
int vec_div(int n, float *in, float a, float *out);
void vec_sub(int n, float *in1, float *in2, float *out);
int mat_inv(int n, float in[n][n], float out[n][n]);
void make_id(int n, float out[n][n]);
void print_mat(int n, float mat[n][n]);
void mults_mat(int n, float mat1[][n], float mat2[][n], float result[][n]);

int main() {
  FILE *fp;
  fp = fopen("matrix.txt", "r");

  if (fp == NULL) {
    printf("Failed to Read matrix.txt File\n");
    return 1;
  }

  int n;
  fscanf(fp, "%d", &n);

  float in[n][n];
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      fscanf(fp, "%f", &in[i][j]);
    }
  }

  fclose(fp);

  float out[n][n];
  int ret = mat_inv(n, in, out);

  if (ret != 1) {
    print_mat(n, out);
  } else {
    printf("Not invertible\n");
  }
}

void vec_mult_s(int n, float *in, float a, float *out) {
  for (int i = 0; i < n; ++i) {
    out[i] = in[i] * a;
  }
}

int vec_div(int n, float *in, float a, float *out) {
  if (fabs(a) < EPS)
    return 1;
  for (int i = 0; i < n; ++i) {
    out[i] = in[i] / a;
  }

  return 0;
}

void vec_sub(int n, float *in1, float *in2, float *out) {
  for (int i = 0; i < n; ++i) {
    out[i] = in1[i] - in2[i];
  }
}

void make_id(int n, float out[n][n]) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      out[i][j] = 0;
      if (i == j)
        out[i][j] = 1;
    }
  }
}

int mat_inv(int n, float in[n][n], float out[n][n]) {
  float mat1[n][n];
  memcpy(mat1, in, sizeof(mat1));

  float mat2[n][n];
  make_id(n, mat2);

  for (int k = 0; k < n; ++k) {
    int pivot_row = k;

    while (pivot_row < n && fabsf(mat1[pivot_row][k]) < EPS) {
      pivot_row++;
    }

    if (pivot_row == n) {
      return 1;
    }

    if (pivot_row != k) {
      for (int j = 0; j < n; ++j) {
        float tmp;
        tmp = mat1[k][j];
        mat1[k][j] = mat1[pivot_row][j];
        mat1[pivot_row][j] = tmp;

        tmp = mat2[k][j];
        mat2[k][j] = mat2[pivot_row][j];
        mat2[pivot_row][j] = tmp;
      }
    }

    float pivot = mat1[k][k];

    vec_div(n, mat1[k], pivot, mat1[k]);
    vec_div(n, mat2[k], pivot, mat2[k]);

    for (int i = 0; i < n; ++i) {
      if (i == k)
        continue;
      float mul = mat1[i][k];
      float sub1[n];
      float sub2[n];
      vec_mult_s(n, mat1[k], mul, sub1);
      vec_sub(n, mat1[i], sub1, mat1[i]);

      vec_mult_s(n, mat2[k], mul, sub2);
      vec_sub(n, mat2[i], sub2, mat2[i]);
    }
  }

  memcpy(out, mat2, sizeof(mat2));

  return 0;
}

void print_mat(int n, float mat[n][n]) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      printf("%9f ", mat[i][j]);
    }
    printf("\n");
  }

  printf("\n");
}

void mults_mat(int n, float mat1[][n], float mat2[][n], float result[][n]) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      result[i][j] = 0.0f;
      for (int k = 0; k < n; ++k) {
        result[i][j] += mat1[i][k] * mat2[k][j];
      }
    }
  }
}