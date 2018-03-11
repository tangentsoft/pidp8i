#include <init.h>
#include <libc.h>

int fib(n)
int n;
{
    if (n < 2)
        return n;
    else
        return fib(n - 1) + fib(n - 2);
}


int main()
{
    int i, rsl;
    i = 1;
    while (1) {
        rsl = fib(i);
        if (rsl < 0) {
            printf("Overflow:%d\r\n", i);
            break;
        }
        printf("Fib #%d = %d\r\n", i, rsl);
        i++;
    }
}
