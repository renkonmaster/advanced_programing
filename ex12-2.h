#ifndef EX12_2_H
#define EX12_2_H

typedef struct {
  double re; // real part（実部）
  double im; // imaginary part（虚部）
} complex_number;

complex_number cmp_add(complex_number, complex_number);
complex_number cmp_sub(complex_number, complex_number);
complex_number cmp_mul(complex_number, complex_number);

#endif