#include <stdio.h>

int main(void) {
    char string[128];

    scanf("%s", string);

    int n = 0;

    int i = 0;
    while (string[i] != '\0') {
        if (string[i] >= '0' && string[i] <= '9') {
            n = n * 10 + (string[i] - '0');
        } else {
            break;
        }
        i++;
    }

    printf("\n%d\n", n);
    return 0;
}