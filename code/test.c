#include <stdio.h>
#include <string.h>

char* hander(char *str)
{
    // aaabbbbcc
    // abc
    int len = strlen(str);
    if (len < 1)
    {
        return NULL;
    }

    int tail = 1;

    for (int i = 1; i < len; i++)
    {
        int j;
        for (j = 0; j < tail; j++)
            if (str[i] == str[j])
                break;
        if (j == tail)
        {
            str[tail] = str[i];
            tail++;
        }
    }
    str[tail] = '\0';

    return str;
}

int main()
{
    char str[100] = "aaabbbbcccabcd";
    //char str1[100];
    char *str1 = hander(str);

    printf("%s", str1);
    return 0;
}