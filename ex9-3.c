#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define FILENAME_SIZE 256
#define PRINT_LIMIT 20

int file_dump(char *filename) {
  FILE *fp;
  fp = fopen(filename, "rb");

  if (fp == NULL) {
    printf("No file\n");
    return 1;
  }

  char buffer[16];
  int cnt = 0;

  size_t nread;
  while ((nread = fread(buffer, 1, sizeof buffer, fp))>0) {
    if (cnt >= PRINT_LIMIT) {
      printf("more ? ");
      char yes;
      scanf(" %c", &yes);
      if (yes != 'y') {
        break;
      }

      cnt = 0;
    }
    for (size_t i = 0; i < nread; ++i) {
      printf("%02X ", (unsigned char)buffer[i]);
    }

    for (size_t i = nread; i < sizeof buffer; ++i) {
      printf("   ");
    }
    
    printf("| ");
    for (size_t i = 0; i < nread; ++i) {
      if (isprint((unsigned char)buffer[i])) {
        printf("%c", buffer[i]);
      } else {
        printf(" ");
      }
    }

    for (size_t i = nread; i < sizeof buffer; ++i) {
      printf(" ");
    }
    
    printf(" |\n");
    cnt++;
  }

  fclose(fp);

  return 0;
}

int main() {
  printf("filename ? : ");
  char filename[FILENAME_SIZE];
  if (fgets(filename, FILENAME_SIZE, stdin) == NULL) {
    return 1;
  }

  filename[strcspn(filename, "\n")] = '\0';

  if (file_dump(filename) == 1) {
    return 1;
  }

  return 0;
}
