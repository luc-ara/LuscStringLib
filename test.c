#include <stdio.h>
#include "strings.h"

int main(void)
{
    //write tests here

    printf("%zu\n", count_utf8("a\u20AC\U0001F600\U0001F600\U0001F600\U0001F600"));

    return 0;
}