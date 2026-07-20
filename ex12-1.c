#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define Nmeas 5

float my_random(float lower, float upper);
void make_data(float *data, int n);
void quick_sort(float a[], int left, int right);
int partition(float a[], int left, int right);

long long swap_cnt = 0;
long long comp_cnt = 0;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    return 1;
  }
  char *end;
  long n = strtol(argv[1], &end, 10);

  float *data = malloc((size_t)n * sizeof(*data));

  if (data == NULL) {
    return 1;
  }
  
  long long swap_sum = 0;
  long long comp_sum = 0;

  for (int i = 0; i < Nmeas; ++i) {
    make_data(data, n);

    swap_cnt = 0;
    comp_cnt = 0;

    quick_sort(data, 0, n - 1);

    int print_count = n < 10 ? n : 10;
    printf("First: ");
    for (int j = 0; j < print_count; ++j) {
      printf("%f ", data[j]);
    }

    printf("\nLast: ");
    for (int j = n - print_count; j < n; ++j) {
      printf("%f ", data[j]);
    }

    printf("\nThe number of comparisons: %lld\n", comp_cnt);
    printf("The number of swaps: %lld\n", swap_cnt);

    comp_sum += comp_cnt;
    swap_sum += swap_cnt;
  }

  printf("\nThe average number of comparisons: %lld\n", comp_sum / Nmeas);
  printf("The average number of swaps: %lld\n", swap_sum / Nmeas);

  free(data);

  return 0;
}

void make_data(float *data, int n) {
  srand(1);

  for (int i = 0; i < n; ++i) {
    data[i] = my_random(0, 10000);
  }
}

float my_random(float lower, float upper) {
  float r;
  r = (upper - lower) * rand() / (RAND_MAX + 1.0) + lower;
  return fabs(r);
}

void quick_sort(float a[], int left, int right) {
  int mid;

  if (left < right) {
    mid = partition(a, left, right);

    if (mid != left) {
      quick_sort(a, left, mid);
    }

    if (mid + 1 != right) {
      quick_sort(a, mid + 1, right);
    }
  }
}

int partition(float a[], int left, int right) {
  float pivot, swap;
  int i, j;

  pivot = a[left];
  i = left - 1;
  j = right + 1;

  while (1) {
    do {
      i++;
      comp_cnt++;
    } while (a[i] < pivot);

    do {
      j--;
      comp_cnt++;
    } while (a[j] > pivot);

    if (i >= j) {
      return j;
    }

    swap = a[i];
    a[i] = a[j];
    a[j] = swap;
    swap_cnt++;
  }
}
