ire0 (n, d)
{
    while (n > 0) n = n - d;
    return n == 0;
}

main ()
{
    int i, st;
    st = 0;

    for (i = 3; i < 1000; i++) {
             if (ire0 (i, 3)) st = st + i;
        else if (ire0 (i, 5)) st = st + i;

        if (st > 1000) {
            printf("%d + ", st);
            st = 0;
        }
    }

    printf("%d\n", st);
}
