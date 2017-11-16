int main()
{
    int i, j, st;
    st = 0;

    for (i = 3; i < 1000; i++) {
        for (j = i; j >= 3; j = j - 3) ;
        if (j == 0) {
            st = st + i;
        }
        else {
            for (j = i; j >= 5; j = j - 5) ;
            if (j == 0) {
                st = st + i;
            }
        }

        if (st > 2000) {
            printf("%d + ", st);
            st = 0;
        }
    }

    puts("0");

    return 0;
}
