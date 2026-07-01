#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ZIPCHAR_SIZE 8
#define PREF_SIZE 20
#define CITY_SIZE 50
#define ADDR_SIZE 100
#define BUF_SIZE 255

#define ZIPTABLE_MAX 4000

typedef struct _ziptable {
  int zip;
  char pref[PREF_SIZE];
  char city[CITY_SIZE];
  char addr[ADDR_SIZE];
} ziptable_t;

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

int read_from_csv(ziptable_t **ziptable, FILE *infp) {
  int chofs, zip, n = 0;
  char instr[BUF_SIZE];
  char zip_char[ZIPCHAR_SIZE];
  char pref[PREF_SIZE];
  char city[CITY_SIZE];
  char addr[ADDR_SIZE];

  while (fgets(instr, BUF_SIZE, infp) != NULL) {
    ziptable_t *p = malloc(sizeof(ziptable_t));
    if (p == NULL) {
      printf("Failed to Allocate Memory.\n");
      return n;
    }
    ziptable[n] = p;
    chofs = 0;
    chofs = csv_dq_read(instr, zip_char, '"', chofs);
    zip = atoi(zip_char);
    p->zip = zip;

    chofs = skip_delim(instr, ',', chofs);
    chofs = csv_dq_read(instr, pref, '"', chofs);
    strcpy(p->pref, pref);

    chofs = skip_delim(instr, ',', chofs);
    chofs = csv_dq_read(instr, city, '"', chofs);
    strcpy(p->city, city);

    chofs = skip_delim(instr, ',', chofs);
    chofs = csv_dq_read(instr, addr, '"', chofs);
    strcpy(p->addr, addr);

    n++;
  }
  return n;
}

int main() {
  FILE *fp;
  fp = fopen("tokyo_all_dat.txt", "r");

  if (fp == NULL) {
    printf("Failed to Open a file\n");
    return 1;
  }
  ziptable_t *zip_table[ZIPTABLE_MAX];
  int n;
  n = read_from_csv(zip_table, fp);

  fclose(fp);

  printf("Number of data : %d\n", n);

  int zip = 0;

  printf("zipcode? : ");
  scanf("%d", &zip);

  for (int i = 0; i < n; ++i) {
    if (zip == zip_table[i]->zip) {
      printf("address : %s\n", zip_table[i]->addr);
      break;
    }

    if (i == n - 1) {
      printf("no data\n");
    }
  } 

  for (int i = 0; i < n; ++i) {
    free(zip_table[i]);
    zip_table[i] = NULL;
  }

  return 0;
}
