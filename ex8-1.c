#include <stdio.h>
#include <string.h>

#define SIZE 128
#define NAMESIZE 100

int hash(char *str) {
  int hashval = 0;
  while (*str != '\0') {
    hashval = hashval + *str;
    str++;
  }
  return (hashval % SIZE);
}

typedef struct person_st {
  char name[NAMESIZE];
  int gender;
  int age;
} person_t;

person_t table[SIZE];
const person_t init = {"", 0, 0};

int main() {
  for (int i = 0; i < SIZE; ++i) {
    table[i] = init;
  }

  FILE *fp;
  fp = fopen("personal_data.txt", "r");

  if (fp == NULL) {
    printf("Failed to Open a file\n");
    return 1;
  }

  char buff[128];
  while (fgets(buff, sizeof(buff), fp) != NULL) {
    person_t data;
    int n = sscanf(buff, "%s %d %d", data.name, &data.gender, &data.age);
    if (n != 3) {
      printf("Input error\n");
      return 1;
    }
    int key = hash(data.name);

    if (strcmp(table[key].name, init.name) == 0) {

      table[key] = data;
    } else {
      printf("collision");
      return 1;
    }
  }

  fclose(fp);

  while (1) {
    printf("Name? > ");
    char name[NAMESIZE];
    if (fgets(name, NAMESIZE, stdin) == NULL) {
      return 1;
    }
    name[strcspn(name, "\n")] = '\0';

    if (strcmp(name, "exit") == 0) {
      break;
    }

    int key = hash(name);

    if (strcmp(table[key].name, init.name) == 0) {
      printf("Nodata\n");
    } else {
      printf("(%s ", table[key].name);
      switch (table[key].gender) {
      case 1:
        printf("male ");
        break;
      case 2:
        printf("female ");
        break;
      default:
        printf("other ");
        break;
      }
      printf("%d)\n", table[key].age);
    }
  }

  return 0;
}
