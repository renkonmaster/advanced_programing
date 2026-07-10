#include <stdio.h>

#define BUF_SIZE 255

int encrypt(FILE *infp, FILE *outfp, int n) {
  int ch;
  
  while ((ch = getc(infp))!= EOF) {
    if (ch >= 'A' && ch <= 'Z'){
      ch += n;
      ch = ((ch - 'A')% ('Z' - 'A' + 1)) + 'A';
    }
    fprintf(outfp, "%c", ch);
  }
  return 0;
}

int main() {
  char *plain_file = "ex9-0-plain.txt";
  char *cipher_file = "ex9-0-cipher.txt";
  FILE *fp_plain, *fp_cipher;
  fp_plain = fopen(plain_file, "r");

  if (fp_plain == NULL) {
    printf("Failed to Read '%s'\n", plain_file);
  }
  
  fp_cipher = fopen(cipher_file, "w");
  if (fp_cipher == NULL) {
    printf("Failed to Read '%s'\n", cipher_file);
  }

  int n = 23;
  if (encrypt(fp_plain, fp_cipher, n) == 1) {
    printf("Failed to Encrypt\n");
  }

  fclose(fp_plain);
  fclose(fp_cipher);

  return 0;
}
