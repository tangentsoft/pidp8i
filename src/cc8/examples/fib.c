#include <init.h>
#include <libc.h>

fib(n)
int n;
{
    if (n < 2)
        return n;
    else
        return fib(n-1)+fib(n-2);
}


main()
{
        int i;
        for (i=0;i<10;i++)
                printf("Fib#:%d = %d\r\n", i, fib(i));
}
