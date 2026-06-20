#include <stdio.h>

int data[100] = {
    0,   1,   2,   4,   5,   6,   7,   9,   10,  12,  13,  15,  16,  17,  18,
    19,  20,  21,  23,  26,  27,  28,  29,  30,  31,  33,  34,  35,  37,  38,
    39,  40,  41,  42,  44,  46,  47,  48,  49,  51,  52,  53,  54,  55,  56,
    57,  59,  60,  61,  63,  64,  65,  67,  68,  71,  72,  73,  74,  75,  76,
    77,  78,  79,  80,  83,  84,  85,  86,  87,  89,  90,  91,  92,  93,  94,
    95,  96,  97,  98,  99,  101, 102, 104, 105, 107, 108, 110, 111, 112, 113,
    114, 115, 117, 118, 119, 120, 123, 124, 125, 127};

int ex_data[500] = {};
int bin_search(int i);
int linear_search(int i);

int main() {
  for (int i = 0; i < 100; i++) {
    ex_data[i] = data[i];
  }
  for (int i = 100; i < 200; i++) {
    ex_data[i] = data[i - 100] + 128;
    ex_data[i + 100] = data[i - 100] + 256;
    ex_data[i + 200] = data[i - 100] + 384;
    ex_data[i + 300] = data[i - 100] + 512;
  }

  int sum = 0;
  for (int i = 0; i < 640; i++) {
    int cnt = bin_search(i);
    sum += cnt;
  }
  printf("%d\n", sum / 640);

  sum = 0;
  for (int i = 0; i < 640; i++) {
    int cnt = linear_search(i);
    sum += cnt;
  }
  printf("%d\n", sum / 640);
}

// 二分探索
int bin_search(int i) {
  int lw = 0;
  int up = 499;
  int cnt = 0;
  int md = (lw + up) / 2;
  while (lw <= up) {
    md = (lw + up) / 2;

    if (ex_data[md] == i) {
      break;
    } else if (ex_data[md] < i) {
      cnt++;
      lw = md + 1;
    } else {
      cnt++;
      up = md - 1;
    }
  }

  return cnt;
}

// Linear
int linear_search(int i) {
  int j = 0;
  while (j < 500) {
    if (ex_data[j] == i) {
      break;
    } else if (ex_data[j] > i) {
      j++;
      break;
    }
    j++;
  }
  return j;
}
