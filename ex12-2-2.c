#include "ex12-2.h"

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
