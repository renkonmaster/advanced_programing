#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---------------------------------- */

int skip_delim(char instr[], char delim, int choffset) {
  // printf("skip_delim : offset %d, str %s\n", choffset, &instr[choffset]);
  if (instr[choffset] == delim) {
    choffset++;
  } else {
    fprintf(stderr, "Error at csv_read. Delim Char not found.\n");
    exit(-1);
  }
  return (choffset);
}

int csv_dq_read(char instr[], char outstr[], int quote_ch, int choffset) {
  int instr_pos, outstr_pos;

  instr_pos = choffset;
  outstr_pos = 0;
  // printf("csv_dq_read : offset %d, str %s\n", choffset, &instr[choffset]);
  if (instr[instr_pos] == quote_ch) {
    instr_pos++;
  } else {
    fprintf(stderr, "Error at csv_read. Quote Char not found.\n");
    exit(-1);
  }
  while (instr[instr_pos] != quote_ch) {
    if (!isprint(instr[instr_pos])) {
      fprintf(stderr, "Error at csv_read. Wrong char.\n");
      exit(-1);
    } else {
      outstr[outstr_pos] = instr[instr_pos];
      outstr_pos++;
    }
    instr_pos++;
  }
  outstr[outstr_pos] = 0;           // outstr_pos++;
  if (instr[instr_pos] == quote_ch) /* skip quote char */
    instr_pos++;
  return (instr_pos);
}

/* ---------------------------------- */

#define BUF_SIZE 255
#define ZIPCHAR_SIZE 8
#define PREF_SIZE 20
#define CITY_SIZE 50
#define ADDR_SIZE 100
#define ADDR_ALL_SIZE 255

#define ZIPTABLE_MAX 1000000

typedef struct _ziptable {
  int zip;
  char pref[PREF_SIZE];
  char city[CITY_SIZE];
  char addr[ADDR_SIZE];
} ziptable_t;

int read_from_csv(ziptable_t *ziptable,
                  FILE *infp) { // return the number of data
  int chofs, zip, n = 0;
  char instr[BUF_SIZE];
  char zip_char[ZIPCHAR_SIZE];
  char pref[PREF_SIZE];
  char city[CITY_SIZE];
  char addr[ADDR_SIZE];

  while (fgets(instr, BUF_SIZE, infp) != NULL) {
    chofs = 0;
    chofs = csv_dq_read(instr, zip_char, '"', chofs);
    zip = atoi(zip_char);
    ziptable[n].zip = zip;
    chofs = skip_delim(instr, ',', chofs);
    chofs = csv_dq_read(instr, pref, '"', chofs);
    strcpy(ziptable[n].pref, pref);
    chofs = skip_delim(instr, ',', chofs);
    chofs = csv_dq_read(instr, city, '"', chofs);
    strcpy(ziptable[n].city, city);
    chofs = skip_delim(instr, ',', chofs);
    chofs = csv_dq_read(instr, addr, '"', chofs);
    strcpy(ziptable[n].addr, addr);
    n++;
  }
  return n;
}

int compare(const void *a, const void *b) {
  const ziptable_t *data1 = *(ziptable_t **) a;
  const ziptable_t *data2 = *(ziptable_t **) b;
  if (data1->zip < data2->zip) {
    return -1;
  } else if (data1->zip > data2->zip) {
    return 1;
  } else
    return 0;
}

ziptable_t ziptable[ZIPTABLE_MAX];

int main(void) {
  FILE *infp;
  clock_t start, end;
  int n;

  // 提出は test_dat.txt を用いる
  if ((infp = fopen("test_dat.txt", "r")) == NULL) {
    fprintf(stderr, "File not found\n");
    exit(EXIT_FAILURE);
  }
  /* ---------------------------------- */
  /*    read csv file                   */
  /* ---------------------------------- */
  n = read_from_csv(ziptable, infp);
  fclose(infp);
  /* この後を適切に作成して下さい */

  ziptable_t *p_index[n];
  for (int i = 0; i < n; ++i) {
    p_index[i] = &ziptable[i];
  }

  start = clock();
  qsort(&p_index[0], n, sizeof p_index[0], compare);
  end = clock();

  for (int i = 0; i < n; i += n / 10) {
    // 等間隔に抽出した10件のデータを表示して確認する
    ziptable_t *ptr = p_index[i];
    printf("zip: %.7d, pref: %s, city: %s, addr: %s\n", ptr->zip,
           ptr->pref, ptr->city, ptr->addr);
  }

  printf("\n%lf\n", (double)(end - start) / CLOCKS_PER_SEC);
}
