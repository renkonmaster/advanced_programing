#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
  char name[30];
  int age;
  int gender;
} person_t;

void swap_univ(void * a, void * b, size_t size) {
  void *tmp = malloc(size);
  memcpy(tmp, a, size);
  memcpy(a, b, size);
  memcpy(b, tmp, size);
  free(tmp);
};

int main() {
  person_t p1 = {"Alice", 20, 1};
  person_t p2 = {"Bob", 30, 2};
  int one = 1;
  int zero = 0;

  printf("P1:name %s, age %d, gender %d\nP2:name %s, age %d, gender %d\n%d, %d\n", p1.name, p1.age, p1.gender, p2.name, p2.age, p2.gender, zero, one);    // p1, p2, zero, one を表示
  swap_univ(&p1, &p2, sizeof p1); // p1とp2のデータを入れ替える
  swap_univ(&zero, &one, sizeof zero); // oneとzeroの値を入れ替える
  printf("P1:name %s, age %d, gender %d\nP2:name %s, age %d, gender %d\n%d, %d\n", p1.name, p1.age, p1.gender, p2.name, p2.age, p2.gender, zero, one);    // p1, p2, zero, one を表示
}
