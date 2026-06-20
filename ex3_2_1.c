#include <stdio.h>
#include <string.h>

#define N_PERSON 5

typedef struct pdata_st {
  char name[40];
  int gender;
  char address[100];
} pdata_st;

void init_pdata(pdata_st *);

int main() {
  pdata_st pdata[N_PERSON];

  init_pdata(pdata);

  char inpname[40];
  scanf("%s", inpname);

  for (int i = 0; i < N_PERSON; ++i) {
    if (strcmp(pdata[i].name, inpname) == 0) {

      if (pdata[i].gender == 1) {
        printf("Gender: male, ");
      } else if (pdata[i].gender == 2) {
        printf("Gender: female, ");
      } else {
        printf("Gender: other, ");
      }

      printf("Adress: %s\n", pdata[i].address);
      return 0;
    }
  }

  printf("Not Found\n");
}

void init_pdata(pdata_st *pdata) {
  strcpy(pdata[0].name, "Taro");
  pdata[0].gender = 1;
  strcpy(pdata[0].address, "Ookayama, Meguro-ku, Tokyo, JAPAN");

  strcpy(pdata[1].name, "Hanako");
  pdata[1].gender = 2;
  strcpy(pdata[1].address, "Suzukakedai, Midori-ku, Yokohama, JAPAN");

  strcpy(pdata[2].name, "Jiro");
  pdata[2].gender = 1;
  strcpy(pdata[2].address, "Tamachi, Minato-ku,Tokyo, JAPAN");

  strcpy(pdata[3].name, "Ichiro");
  pdata[3].gender = 1;
  strcpy(pdata[3].address, "Miami, Florida, USA");

  strcpy(pdata[4].name, "Naomi");
  pdata[4].gender = 2;
  strcpy(pdata[4].address, "PalmBeach, Florida, USA");
}
