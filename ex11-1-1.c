#include <stdio.h>
#include <string.h>

void print_result(int n) {
  if (n == 0) {
    printf("OK.\n");
  } else {
    printf("Number of unsatisfied items: %d.\n", n);
    printf("Different file type.\n");
  }
}

int main(void) {
  int unsatisfied = 0;
  char filename[256];
  scanf("%255s", filename);

  FILE *fp;
  fp = fopen(filename, "rb");

  if (fp == NULL) {
    printf("Can not open.\n");
    return 1;
  }

  unsigned char fileHeader[14];
  if (fseek(fp, 0, SEEK_SET) != 0) {
    fclose(fp);
    return 1;
  }
  if (fread(fileHeader, 1, 14, fp) != 14) {
    fclose(fp);
    return 1;
  }

  if (fileHeader[0] != 'B' || fileHeader[1] != 'M') {
    unsatisfied++;
  }

  unsigned char bit_count_byte[2];
  if (fseek(fp, 0x001C, SEEK_SET) != 0) {
    fclose(fp);
    return 1;
  }

  if (fread(bit_count_byte, 1, 2, fp) != 2) {
    fclose(fp);
    return 1;
  }

  unsigned int bit_count = ((unsigned int)bit_count_byte[0]) |
                           ((unsigned int)bit_count_byte[1] << 8);

  if (bit_count != 24) {
    unsatisfied++;
  }

  unsigned char data_offset[4];
  if (fseek(fp, 0x000A, SEEK_SET) != 0) {
    fclose(fp);
    return 1;
  }

  if (fread(data_offset, 1, 4, fp) != 4) {
    fclose(fp);
    return 1;
  }
  unsigned int data_offset_num = ((unsigned int)data_offset[0]) |
                                 ((unsigned int)data_offset[1] << 8) |
                                 ((unsigned int)data_offset[2] << 16) |
                                 ((unsigned int)data_offset[3] << 24);

  if (data_offset_num != 54) {
    unsatisfied++;
  }

  fclose(fp);
  print_result(unsatisfied);
  return 0;
}
