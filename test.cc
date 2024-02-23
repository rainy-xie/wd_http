#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MOD(x, y) x % y
#define N 3
#define Y(n) (N + 1) * n
struct
{
    short a;
    char b;
    float c;
} str;

void foo(int[][3]);

int main()
{
    // int a = strcmp("2.56", "4.728");
    // printf("%d\n", a);
    // printf("%ld\n", sizeof(str));
    // printf("%d\n", 2 * (N + Y(5 + 1)));

    // int x = 'f';
    // printf("%c\n", 'a' + (x - 'a' + 1));

    // int a = 1, b = 2, x = 0;
    // if (!(--a))
    //     x--;
    // if (!b)
    //     x = 7;
    // else
    //     ++x;

    // printf("%d %d %d", a, b, x);

    int a[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    foo(a);
    printf("%d", a[2][1]);
}
void foo(int b[][3])
{
    ++b;
    b[1][1];
}