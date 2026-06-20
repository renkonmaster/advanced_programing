#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define N 20000
#define Nmeas 2

float data[N];

float my_random(float lower, float upper);
void make_data(int salt);

int main(void) {

    for (int i = 0; i < Nmeas; ++i) {
        make_data(i * 1000);

        for (int j = 0; j < 10; ++j) {
            printf("%f\n", data[j]);
        }

        for (int j = N - 11; j < N; ++j) {
            printf("%f\n", data[j]);
        }

        if(i != Nmeas - 1) {
            printf("\n");
        }
    }

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