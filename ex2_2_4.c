#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define N 60000
#define Nmeas 5

float data[N];

float my_random(float lower, float upper);
void make_data(int salt);
void quick_sort(float a[], int left, int right);
int partition(float a[], int left, int right);

long long swap_cnt = 0;
long long comp_cnt = 0;

int main(void) {

    long long swap_sum = 0;
    long long comp_sum = 0;

    for (int i = 0; i < Nmeas; ++i) {
        make_data(i * 1000);

        swap_cnt = 0;
        comp_cnt = 0;

        quick_sort(data, 0, N - 1);

        printf("First: ");
        for (int j = 0; j < 10; ++j) {
            printf("%f ", data[j]);
        }

        printf("\nLast: ");
        for (int j = N - 11; j < N; ++j) {
            printf("%f ", data[j]);
        }

        printf("\nThe number of comparisons: %lld\n", comp_cnt);
        printf("The number of swaps: %lld\n", swap_cnt);

        comp_sum += comp_cnt;
        swap_sum += swap_cnt;
    }

    printf("\nThe average number of comparisons: %lld\n", comp_sum/Nmeas);
    printf("The average number of swaps: %lld\n", swap_sum/Nmeas);

    return 0;
}

void make_data(int salt) {
    srand((unsigned int)time(NULL) + salt);

    for (int i = 0; i < N; ++i) {
        data[i] = my_random(0, 10000);
    }
}

float my_random(float lower, float upper)
{
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
