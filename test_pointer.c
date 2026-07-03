#include <stdio.h>

// ダブルポインタを受け取る関数
void HOGE(int **p)
{
    printf("最終的な値: %d\n", **p);
}

int main() {
    int num = 10;
    int *ptr = &num;  // num のアドレスを ptr に入れる
    
    // ptr（ポインタ変数）自体の「アドレス(&ptr)」を渡すため、
    // 受け取り側は int** 型になる
    HOGE(&ptr); 
    
    return 0;
}