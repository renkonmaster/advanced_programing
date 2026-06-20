#include <stdio.h>
#include <string.h>

#define N_PERSON 5

typedef struct {
  char address[100]; /* Detailed Address */
  char zip[10]; /* Postal Code */
  char country[20]; /* Country/Region */
} address_st;

typedef struct {
  char name[40]; /* Name */
  int gender; /* Gender(1=Male, 2=Female) */
  address_st addr; /* Address */
} pdata2_st;
pdata2_st pdata2[N_PERSON];

void init_pdata(pdata2_st*);
void print_pdata(pdata2_st*, char*);

int main() {
  char name[40];
  scanf("%s", name);

  init_pdata(pdata2);
  print_pdata(pdata2, name);

  return 0;
}

void init_pdata(pdata2_st *pdata) {
  strcpy(pdata[0].name, "Taro");
  pdata[0].gender = 1;
  strcpy(pdata[0].addr.address, "Ookayama, Meguro-ku, Tokyo");
  strcpy(pdata[0].addr.country, "Japan");

  strcpy(pdata[1].name, "Hanako");
  pdata[1].gender = 2;
  strcpy(pdata[1].addr.address, "Suzukakedai, Midori-ku, Yokohama");
  strcpy(pdata[1].addr.country, "Japan");

  strcpy(pdata[2].name, "Jiro");
  pdata[2].gender = 1;
  strcpy(pdata[2].addr.address, "Tamachi, Minato-ku, Tokyo");
  strcpy(pdata[2].addr.country, "Japan");

  strcpy(pdata[3].name, "Ichiro");
  pdata[3].gender = 1;
  strcpy(pdata[3].addr.address, "Miami, Florida");
  strcpy(pdata[3].addr.country, "USA");

  strcpy(pdata[4].name, "Naomi");
  pdata[4].gender = 2;
  strcpy(pdata[4].addr.address, "PalmBeach, Florida, USA");
  strcpy(pdata[4].addr.country, "USA");
}

void print_pdata(pdata2_st *pdata, char* name) {
  for (int i = 0; i < N_PERSON; i++) {
    if (strcmp(pdata[i].name, name) == 0) {
      if (pdata[i].gender == 1) 
        printf("Gender: male, ");
      else if (pdata[i].gender == 2)
        printf("Gender: female, ");
      else
        printf("Gender: other, ");
      printf("Address: %s, Country: %s\n", pdata[i].addr.address, pdata[i].addr.country);
      return;
    }
  }
  printf("Not found\n");
}
