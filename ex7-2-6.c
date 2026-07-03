#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZIPCHAR_SIZE 8
#define PREF_SIZE 20
#define CITY_SIZE 50
#define ADDR_SIZE 100
#define BUF_SIZE 255
#define FULL_ADDR_SIZE 170

typedef struct _zipnode {
  int zip;
  char fulladdr[FULL_ADDR_SIZE];
  struct _zipnode *next; // 次ノードのポインタ
} zipnode_t;

int add_node(zipnode_t **head_p, int zip, char *fulladdr) {
  zipnode_t *new_node = malloc(sizeof(zipnode_t));

  if (new_node == NULL) {
    return 1;
  }

  new_node->zip = zip;
  strncpy(new_node->fulladdr, fulladdr, FULL_ADDR_SIZE - 1);
  new_node->fulladdr[FULL_ADDR_SIZE - 1] = '\0';
  new_node->next = (*head_p)->next;
  (*head_p)->next = new_node;

  return 0;
}

int print_n_node(zipnode_t *head, int n) {
  zipnode_t *node = head->next;

  for (int i = 0; i < n; ++i) {
    if (node == NULL) {
      return i + 1;
    }
    printf("%d : ", node->zip);
    printf("%s\n", node->fulladdr);
    node = node->next;
  }
  return n;
}

zipnode_t *search_node(zipnode_t *head, char *fulladdr) {
  for (zipnode_t *node = head->next; node != NULL; node = node->next) {
    if (strcmp(node->fulladdr, fulladdr) == 0) {
      return node;
    }
  }

  return NULL;
}

zipnode_t **search_pointer_to_node(zipnode_t **head_p, char *fulladdr){
  for (zipnode_t **p = head_p; *p != NULL; p = &(*p)->next){
    if (strcmp((*p)->fulladdr, fulladdr)== 0) {
      return p;
    }
  }

  return NULL;
}

int skip_delim(char instr[], char delim, int choffset) {
  // printf("skip_delim : offset %d, str %s\n", choffset, &instr[choffset]);
  if (instr[choffset] == delim) {
    choffset++;
  } else {
    fprintf(stderr, "Error at csv_read. Delim Char not found.\n");
    exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
  }
  while (instr[instr_pos] != quote_ch) {
    if (!isprint(instr[instr_pos])) {
      fprintf(stderr, "Error at csv_read. Wrong char.\n");
      exit(EXIT_FAILURE);
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

void read_from_csv(zipnode_t **head, FILE *infp) {
  int chofs, zip = 0;
  char instr[BUF_SIZE];
  char zip_char[ZIPCHAR_SIZE];
  char pref[PREF_SIZE];
  char city[CITY_SIZE];
  char addr[ADDR_SIZE];
  char fulladdr[FULL_ADDR_SIZE];

  while (fgets(instr, BUF_SIZE, infp) != NULL) {
    chofs = 0;
    chofs = csv_dq_read(instr, zip_char, '"', chofs);
    zip = atoi(zip_char);

    chofs = skip_delim(instr, ',', chofs);
    chofs = csv_dq_read(instr, pref, '"', chofs);

    chofs = skip_delim(instr, ',', chofs);
    chofs = csv_dq_read(instr, city, '"', chofs);

    chofs = skip_delim(instr, ',', chofs);
    chofs = csv_dq_read(instr, addr, '"', chofs);

    strcpy(fulladdr, pref);
    strcat(fulladdr, " ");
    strcat(fulladdr, city);
    strcat(fulladdr, " ");
    strcat(fulladdr, addr);

    add_node(head, zip, fulladdr);
  }
}

void free_list(zipnode_t *head) {
  zipnode_t *node = head->next;

  while (node != NULL) {
    zipnode_t *next = node->next;
    free(node);
    node = next;
  }

  head->next = NULL;
}

int main() {
  zipnode_t head_node = {0};
  zipnode_t *head = &head_node;
  head->next = NULL;

  FILE *infp;
  if ((infp = fopen("test_dat.txt", "r")) == NULL) {
    fprintf(stderr, "File not found\n");
    exit(EXIT_FAILURE);
  }
  read_from_csv(&head, infp);
  fclose(infp);

  print_n_node(head, 3);

  while (1) {
    printf("full address ? > ");
    char fulladdr[FULL_ADDR_SIZE];
    if (fgets(fulladdr, FULL_ADDR_SIZE, stdin) == NULL) {
      free_list(head);
      return 1;
    }
    fulladdr[strcspn(fulladdr, "\n")] = '\0';

    if (strcmp(fulladdr, "exit") == 0) {
      printf("Bye!\n");
      break;
    }

    zipnode_t **result = NULL;
    result = search_pointer_to_node(&(head->next), fulladdr);
    if (result == NULL) {
      printf("no data\n");
    } else {
      printf("zipcode = %d\n", (*result)->zip);

      zipnode_t *delete_node = *result;
      *result = delete_node->next;
      free(delete_node);
      print_n_node(head, 3);
    }
  }

  free_list(head);

  return 0;
}
