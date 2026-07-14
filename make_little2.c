#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const char *filename = "little2.bin";

  /*
   * A, B は 3バイトで表せる範囲の符号なし整数
   * 0x000000 〜 0xFFFFFF
   */
  unsigned int A = 0x000123;
  unsigned int B = 0x000045;

  unsigned char data[12] = {0};

  /*
   * A をリトルエンディアンで第1〜3バイトに書く
   */
  data[0] = A & 0xFF;
  data[1] = (A >> 8) & 0xFF;
  data[2] = (A >> 16) & 0xFF;

  /*
   * B をリトルエンディアンで第4〜6バイトに書く
   */
  data[3] = B & 0xFF;
  data[4] = (B >> 8) & 0xFF;
  data[5] = (B >> 16) & 0xFF;

  /*
   * 第7〜12バイトは積を書き込む場所なので、初期値は0にしておく
   */
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = 0;
  data[10] = 0;
  data[11] = 0;

  FILE *fp = fopen(filename, "wb");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open %s\n", filename);
    return 1;
  }

  if (fwrite(data, sizeof(data[0]), sizeof(data), fp) != sizeof(data)) {
    fprintf(stderr, "Failed to write %s\n", filename);
    fclose(fp);
    return 1;
  }

  fclose(fp);

  printf("Created %s\n", filename);
  printf("A = %u\n", A);
  printf("B = %u\n", B);
  printf("A * B = %u\n", A * B);

  return 0;
}
