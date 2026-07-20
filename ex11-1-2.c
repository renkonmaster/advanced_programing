#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned char b;
  unsigned char g;
  unsigned char r;
} rgb_t;

int check_bmp(FILE *fp) {
  int unsatisfied = 0;

  unsigned char fileHeader[14];
  if (fseek(fp, 0, SEEK_SET) != 0) {
    fclose(fp);
    exit(EXIT_FAILURE);
  }
  if (fread(fileHeader, 1, 14, fp) != 14) {
    fclose(fp);
    exit(EXIT_FAILURE);
  }

  if (fileHeader[0] != 'B' || fileHeader[1] != 'M') {
    unsatisfied++;
  }

  unsigned char bit_count_byte[2];
  if (fseek(fp, 0x001C, SEEK_SET) != 0) {
    fclose(fp);
    exit(EXIT_FAILURE);
  }

  if (fread(bit_count_byte, 1, 2, fp) != 2) {
    fclose(fp);
    exit(EXIT_FAILURE);
  }

  unsigned int bit_count = ((unsigned int)bit_count_byte[0]) |
                           ((unsigned int)bit_count_byte[1] << 8);

  if (bit_count != 24) {
    unsatisfied++;
  }

  unsigned char data_offset[4];
  if (fseek(fp, 0x000A, SEEK_SET) != 0) {
    fclose(fp);
    exit(EXIT_FAILURE);
  }

  if (fread(data_offset, 1, 4, fp) != 4) {
    fclose(fp);
    exit(EXIT_FAILURE);
  }
  unsigned int data_offset_num = ((unsigned int)data_offset[0]) |
                                 ((unsigned int)data_offset[1] << 8) |
                                 ((unsigned int)data_offset[2] << 16) |
                                 ((unsigned int)data_offset[3] << 24);

  if (data_offset_num != 54) {
    unsatisfied++;
  }

  return unsatisfied;
}

int main(void) {
  char filename[256];
  scanf("%255s", filename);

  FILE *fp;
  fp = fopen(filename, "rb");

  if (fp == NULL) {
    printf("Can not open.\n");
    return 1;
  }

  int n = check_bmp(fp);
  if (n != 0) {
    printf("Different file type.\n");
    fclose(fp);
    return 0;
  }

  int x_size, y_size;
  fseek(fp, 0x0012, SEEK_SET);

  fread(&x_size, 4, 1, fp);
  fread(&y_size, 4, 1, fp);

  printf("Horizontal size = %d\n", x_size);
  printf("Vertical size = %d\n", y_size);

  int data_offset;
  if (fseek(fp, 0x0a, SEEK_SET) != 0) {
    fclose(fp);
    return 1;
  }
  if (fread(&data_offset, 4, 1, fp) != 1) {
    fclose(fp);
    return 1;
  }

  rgb_t color[x_size][y_size];
  fseek(fp, data_offset, SEEK_SET);

  int row_size = x_size * 3;
  int padding = (4 - (row_size % 4)) % 4;

  for (int y = 0; y < y_size; y++) {

    for (int x = 0; x < x_size; x++) {

      fread(&color[x][y].b, 1, 1, fp);
      fread(&color[x][y].g, 1, 1, fp);
      fread(&color[x][y].r, 1, 1, fp);
    }
    fseek(fp, padding, SEEK_CUR);
  }

  printf("Upper-left (R=%d, G=%d, B=%d)\n", color[0][y_size - 1].r,
         color[0][y_size - 1].g, color[0][y_size - 1].b);
  printf("Upper-right (R=%d, G=%d, B=%d)\n", color[x_size - 1][y_size - 1].r,
         color[x_size - 1][y_size - 1].g, color[x_size - 1][y_size - 1].b);
  printf("Lower-left (R=%d, G=%d, B=%d)\n", color[0][0].r, color[0][0].g,
         color[0][0].b);
  printf("Lower-right (R=%d, G=%d, B=%d)\n", color[x_size - 1][0].r,
         color[x_size - 1][0].g, color[x_size - 1][0].b);

  fclose(fp);
  return 0;
}
