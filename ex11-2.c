#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BMP_HEADER_SIZE 54
#define OUTPUT_FILENAME "25B10610-ex11-2.bmp"

typedef struct {
  unsigned char b;
  unsigned char g;
  unsigned char r;
} rgb_t;

typedef enum { ROTATE_RIGHT_90, ROTATE_LEFT_90, ROTATE_180 } rotation_t;

unsigned int read_le16(const unsigned char *bytes) {
  return ((unsigned int)bytes[0]) | ((unsigned int)bytes[1] << 8);
}

uint32_t read_le32(const unsigned char *bytes) {
  return ((uint32_t)bytes[0]) | ((uint32_t)bytes[1] << 8) |
         ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

void write_le32(unsigned char *bytes, uint32_t value) {
  bytes[0] = (unsigned char)(value & 0xFF);
  bytes[1] = (unsigned char)((value >> 8) & 0xFF);
  bytes[2] = (unsigned char)((value >> 16) & 0xFF);
  bytes[3] = (unsigned char)((value >> 24) & 0xFF);
}

int check_bmp(FILE *fp) {
  unsigned char header[BMP_HEADER_SIZE];

  if (fseek(fp, 0, SEEK_SET) != 0) {
    return -1;
  }

  if (fread(header, 1, sizeof(header), fp) != sizeof(header)) {
    return -1;
  }

  int unsatisfied = 0;

  if (header[0] != 'B' || header[1] != 'M') {
    unsatisfied++;
  }

  if (read_le16(&header[0x001C]) != 24) {
    unsatisfied++;
  }

  if (read_le32(&header[0x000A]) != BMP_HEADER_SIZE) {
    unsatisfied++;
  }

  return unsatisfied;
}

int parse_rotation(int argc, char *argv[], rotation_t *rotation) {
  if (rotation == NULL) {
    return 1;
  }

  if (argc == 1) {
    *rotation = ROTATE_180;
    return 0;
  }

  if (argc != 2) {
    return 1;
  }

  if (strcmp(argv[1], "-left") == 0) {
    *rotation = ROTATE_LEFT_90;
    return 0;
  }

  if (strcmp(argv[1], "-right")==0) {
    *rotation = ROTATE_RIGHT_90;
    return 0;
  }

  return 1;
}

rgb_t *read_pixels(FILE *fp, int width, int height, uint32_t data_offset) {
  if (width <= 0 || height <= 0) {
    return NULL;
  }

  if ((size_t)width > SIZE_MAX / (size_t)height) {
    return NULL;
  }

  size_t pixel_count = (size_t)width * (size_t)height;

  if (pixel_count > SIZE_MAX / sizeof(rgb_t)) {
    return NULL;
  }

  rgb_t *pixels = malloc(pixel_count * sizeof(rgb_t));

  if (pixels == NULL) {
    return NULL;
  }

  if (fseek(fp, (long)data_offset, SEEK_SET) != 0) {
    free(pixels);
    return NULL;
  }

  if ((size_t)width > SIZE_MAX / 3) {
    free(pixels);
    return NULL;
  }

  size_t row_data_size = (size_t)width * 3;
  size_t padding = (4 - (row_data_size % 4)) % 4;

  for (int file_y = 0; file_y < height; file_y++) {
    int y = height - 1 - file_y;

    for (int x = 0; x < width; x++) {
      unsigned char bgr[3];

      if (fread(bgr, 1, sizeof(bgr), fp) != sizeof(bgr)) {
        free(pixels);
        return NULL;
      }

      size_t index = (size_t)y * (size_t)width + (size_t)x;

      pixels[index].b = bgr[0];
      pixels[index].g = bgr[1];
      pixels[index].r = bgr[2];
    }

    if (padding != 0) {
      unsigned char padding_bytes[3];

      if (fread(padding_bytes, 1, padding, fp) != padding) {
        free(pixels);
        return NULL;
      }
    }
  }

  return pixels;
}

rgb_t *rotate_image(const rgb_t *src, int src_width, int src_height,
                    rotation_t rotation, int *dst_width, int *dst_height) {
  if (src == NULL || dst_width == NULL || dst_height == NULL) {
    return NULL;
  }

  switch (rotation) {
  case ROTATE_RIGHT_90:
  case ROTATE_LEFT_90:
    *dst_width = src_height;
    *dst_height = src_width;
    break;

  case ROTATE_180:
    *dst_width = src_width;
    *dst_height = src_height;
    break;

  default:
    return NULL;
  }

  if ((size_t)(*dst_width) > SIZE_MAX / (size_t)(*dst_height)) {
    return NULL;
  }

  size_t pixel_count = (size_t)(*dst_width) * (size_t)(*dst_height);

  if (pixel_count > SIZE_MAX / sizeof(rgb_t)) {
    return NULL;
  }

  rgb_t *dst = malloc(pixel_count * sizeof(rgb_t));

  if (dst == NULL) {
    return NULL;
  }

  for (int y = 0; y < src_height; y++) {
    for (int x = 0; x < src_width; x++) {
      int dst_x;
      int dst_y;

      switch (rotation) {
      case ROTATE_RIGHT_90:
        dst_x = src_height - 1 - y;
        dst_y = x;
        break;

      case ROTATE_LEFT_90:
        dst_x = y;
        dst_y = src_width - 1 - x;
        break;

      case ROTATE_180:
        dst_x = src_width - 1 - x;
        dst_y = src_height - 1 - y;
        break;

      default:
        free(dst);
        return NULL;
      }

      size_t src_index = (size_t)y * (size_t)src_width + (size_t)x;

      size_t dst_index = (size_t)dst_y * (size_t)(*dst_width) + (size_t)dst_x;

      dst[dst_index] = src[src_index];
    }
  }

  return dst;
}

int write_bmp(const char *filename,
              const unsigned char original_header[BMP_HEADER_SIZE],
              const rgb_t *pixels, int width, int height) {
  if (filename == NULL || pixels == NULL || width <= 0 || height <= 0) {
    return 1;
  }

  size_t row_data_size = (size_t)width * 3;
  size_t padding = (4 - (row_data_size % 4)) % 4;
  size_t row_size = row_data_size + padding;

  if ((size_t)height > SIZE_MAX / row_size) {
    return 1;
  }

  size_t image_size = row_size * (size_t)height;

  if (image_size > UINT32_MAX - BMP_HEADER_SIZE) {
    return 1;
  }

  uint32_t file_size = (uint32_t)(BMP_HEADER_SIZE + image_size);

  unsigned char header[BMP_HEADER_SIZE];
  memcpy(header, original_header, sizeof(header));

  header[0] = 'B';
  header[1] = 'M';

  write_le32(&header[0x0002], file_size);
  write_le32(&header[0x000A], BMP_HEADER_SIZE);
  write_le32(&header[0x0012], (uint32_t)width);
  write_le32(&header[0x0016], (uint32_t)height);
  write_le32(&header[0x001E], 0);
  write_le32(&header[0x0022], (uint32_t)image_size);

  header[0x001A] = 1;
  header[0x001B] = 0;
  header[0x001C] = 24;
  header[0x001D] = 0;

  FILE *out_fp = fopen(filename, "wb");

  if (out_fp == NULL) {
    printf("Can not open output file.\n");
    return 1;
  }

  if (fwrite(header, 1, sizeof(header), out_fp) != sizeof(header)) {
    fclose(out_fp);
    return 1;
  }

  unsigned char zero_padding[3] = {0, 0, 0};

  for (int file_y = 0; file_y < height; file_y++) {
    int y = height - 1 - file_y;

    for (int x = 0; x < width; x++) {
      size_t index = (size_t)y * (size_t)width + (size_t)x;

      unsigned char bgr[3] = {pixels[index].b, pixels[index].g,
                              pixels[index].r};

      if (fwrite(bgr, 1, sizeof(bgr), out_fp) != sizeof(bgr)) {
        fclose(out_fp);
        return 1;
      }
    }

    if (padding != 0 && fwrite(zero_padding, 1, padding, out_fp) != padding) {
      fclose(out_fp);
      return 1;
    }
  }

  if (fclose(out_fp) != 0) {
    return 1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  rotation_t rotation;

  if (parse_rotation(argc, argv, &rotation) != 0) {
    return 1;
  }

  char filename[256];
  scanf("%255s", filename);

  FILE *fp = fopen(filename, "rb");

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

  unsigned char header[BMP_HEADER_SIZE];

  if (fseek(fp, 0, SEEK_SET) != 0 ||
      fread(header, 1, sizeof(header), fp) != sizeof(header)) {
    fclose(fp);
    return 1;
  }

  uint32_t width_value = read_le32(&header[0x0012]);
  uint32_t height_value = read_le32(&header[0x0016]);
  uint32_t data_offset = read_le32(&header[0x000A]);

  if (width_value == 0 || height_value == 0 || width_value > INT_MAX ||
      height_value > INT_MAX) {
    fclose(fp);
    return 1;
  }

  int x_size = (int)width_value;
  int y_size = (int)height_value;

  rgb_t *pixels = read_pixels(fp, x_size, y_size, data_offset);

  fclose(fp);

  if (pixels == NULL) {
    return 1;
  }

  int rotated_x_size;
  int rotated_y_size;

  rgb_t *rotated = rotate_image(pixels, x_size, y_size, rotation,
                                &rotated_x_size, &rotated_y_size);

  free(pixels);

  if (rotated == NULL) {
    printf("Failed to rotate image.\n");
    return 1;
  }

  if (write_bmp(OUTPUT_FILENAME, header, rotated, rotated_x_size,
                rotated_y_size) != 0) {
    free(rotated);
    printf("Failed to write BMP file.\n");
    return 1;
  }

  free(rotated);
  return 0;
}
