#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int main(void) {
  char filename[] = "big2.bin";
  unsigned int a = 0, b = 0;

  FILE *fp;
  fp = fopen(filename, "r+b");
  if (fp == NULL) {
    printf("Failed to Open\n");
    exit(EXIT_FAILURE);
  }

  unsigned char bytes[6];
  if (fread(bytes, sizeof bytes[0], sizeof bytes, fp) != sizeof bytes) {
    printf("Failed to read\n");
    fclose(fp);
    return 1;
  }

  a = ((unsigned int)bytes[0] << 16) |
      ((unsigned int)bytes[1] << 8) |
      ((unsigned int)bytes[2]);
  b = ((unsigned int)bytes[3] << 16) |
      ((unsigned int)bytes[4] << 8) |
      ((unsigned int)bytes[5]);

  unsigned long long product = (unsigned long long)a * b;
  unsigned char result[6];
  result[0] = (product >> 40) & 0xff;
  result[1] = (product >> 32) & 0xff;
  result[2] = (product >> 24) & 0xff;
  result[3] = (product >> 16) & 0xff;
  result[4] = (product >> 8) & 0xff;
  result[5] = (product) & 0xff;

  if (fseek(fp, 6, SEEK_SET) != 0) {
    printf("Failed to seek\n");
    fclose(fp);
    return 1;
  }

  if (fwrite(result, sizeof result[0], sizeof result, fp) != sizeof result) {
    printf("Failed to write\n");
    fclose(fp);
    return 1;
  }

  fclose(fp);

  file_dump(filename);

  return 0;
}