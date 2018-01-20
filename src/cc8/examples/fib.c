#include <init.h>
#include <libc.h>

int fib(n)
{
    if (n < 2)
        return n;
    else
        return fib(n-1)+fib(n-2);
}


int main()
{
        int i;
        for (i=0;i<10;i++)
                printf("Fib#:%d = %d\r\n", i, fib(i));
}
