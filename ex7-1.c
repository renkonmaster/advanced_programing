#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node_st {
  char fullname[30];
  struct node_st *next;
} node_t;

int add_node(node_t **head_p, char *str) {
  node_t *new_node = malloc(sizeof(node_t));

  if (new_node == NULL) {
    printf("Failed to Allocate Memory\n");
    return 1;
  }

  strncpy(new_node->fullname, str, sizeof(new_node->fullname) - 1);
  new_node->fullname[sizeof(new_node->fullname) - 1] = '\0';

  new_node->next = (*head_p)->next;
  (*head_p)->next = new_node;

  return 0;
}

void free_list(node_t *head) {
  while (head != NULL) {
    node_t *next = head->next;
    free(head);
    head = next;
  }
}

int main() {
  node_t *head = malloc(sizeof(node_t));

  if (head == NULL) {
    printf("Failed to Allocate Memory\n");
    return 1;
  }

  head->next = NULL;

  for (int i = 0; i < 3; ++i) {
    char fullname[30];
    printf("Input > ");

    if (fgets(fullname, sizeof(fullname), stdin) == NULL) {
      free_list(head);
      return 1;
    }
    fullname[strcspn(fullname, "\n")] = '\0';

    if (add_node(&head, fullname) == 1) {
      free_list(head);
      return 1;
    }
  }

  node_t *node;
  for (node = head->next; node != NULL; node = node->next)
    printf("%s \n", node->fullname);

  free_list(head);
  return 0;
}
