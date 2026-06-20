#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define N 60000
#define Nmeas 5

float data[N];

float my_random(float lower, float upper);
void make_data(int salt);
void bubble_sort();

long long swap_cnt = 0;
long long comp_cnt = 0;

int main(void) {

    long long swap_sum = 0;
    long long comp_sum = 0;

    for (int i = 0; i < Nmeas; ++i) {
        make_data(i * 1000);

        swap_cnt = 0;
        comp_cnt = 0;

        bubble_sort();

        swap_sum += swap_cnt;
        comp_sum += comp_cnt;

        for (int j = 0; j < 10; ++j) {
            printf("%f\n", data[j]);
        }

        for (int j = N - 11; j < N; ++j) {
            printf("%f\n", data[j]);
        }

        printf("\nThe number of comparisons: %lld\n", comp_cnt);
        printf("The number of swaps: %lld\n", swap_cnt);
    }

    printf("\nThe average number of comparisons:%lld\n", comp_sum/Nmeas);
    printf("The average number of swaps:%lld\n", swap_sum/Nmeas);

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

void bubble_sort() {
    for (int i = N - 1; i > 0; --i){
        for (int j = 0; j < i; ++j){
            comp_cnt++;
            if (data[j] > data[j + 1]){
                float swap = data[j + 1];
                data[j + 1] = data[j];
                data[j] = swap;
                swap_cnt++;
            }
        }
    }
}