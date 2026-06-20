#include <iso646.h>
#include <stdio.h>

typedef struct {
  double re; // real part（実部）
  double im; // imaginary part（虚部）
} complex_number;

complex_number cmp_add(complex_number, complex_number);
complex_number cmp_sub(complex_number, complex_number);
complex_number cmp_mul(complex_number, complex_number);

int main() {
  complex_number x = {3.0, 4.0};
  complex_number y = {3.0, -4.0};
  complex_number z = {2.0, 1.0};

  complex_number ans = cmp_mul(x, y);
  ans = cmp_sub(ans, z);
  printf("%f + j * %f\n", ans.re, ans.im);
  return 0;
}

complex_number cmp_add(complex_number a, complex_number b) {
  complex_number ret;
  ret.re = a.re + b.re;
  ret.im = a.im + b.im;
  return ret;
}

complex_number cmp_sub(complex_number a, complex_number b) {
  complex_number ret;
  ret.re = a.re - b.re;
  ret.im = a.im - b.im;
  return ret;
}

complex_number cmp_mul(complex_number a, complex_number b) {
  complex_number ret;
  ret.re = (a.re * b.re) - (a.im * b.im);
  ret.im = (a.im * b.re) + (a.re * b.im);
  return ret;
}
