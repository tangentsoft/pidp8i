/*  File io.c: 2.1 (83/03/20,16:02:07) */
/*% cc -O -c %
 *
 */

#include <stdio.h>
#include "defs.h"
#include "data.h"

/*
 *  open input file
 */
openin (p) char *p;
{
    strcpy(fname, p);
    fixname (fname);
    if (!checkname (fname))
        return (NO);
    if ((input = fopen (fname, "r")) == NULL) {
        pl ("Open failure\n");
        return (NO);
    }
    kill_line ();
    return (YES);
}

/*
 *  open output file
 */
openout ()
{
    outfname (fname);
    if ((output = fopen (fname, "w")) == NULL) {
        pl ("Open failure");
        return (NO);
    }
    kill_line ();
    return (YES);
}

/*
 *  Change input filename to output filename.  We already checked that
 *  the input file name matches *.c, so we replace the 'c' with an 's'
 *  and then, space permitting, append a 'b'.
 *
 */
outfname (s)
char    *s;
{
    char *os = s;
    while (*s)
        s++;
    *--s = 's';

    if ((s - os) < (NAMESIZE - 2)) {
        *++s = 'b';
        *++s = '\0';
    }

    return 0;

}

/*
 *  remove NL from filenames
 *
 */
fixname (s)
char    *s;
{
    while (*s && *s++ != EOL);
    if (!*s) return 0;
    *(--s) = 0;
    return 0;
}

/*
 *  check that filename is "*.c"
 */
checkname (s)
char    *s;
{
    while (*s)
        s++;
    if (*--s != 'c')
        return (NO);
    if (*--s != '.')
        return (NO);
    return (YES);
}

kill_line ()
{
    lptr = 0;
    line[lptr] = 0;
    return 0;
}

inln ()
{
    int k;
    FILE    *unit;

    FOREVER {
        if (feof (input))
            return 0;
        if ((unit = input2) == NULL)
            unit = input;
        kill_line ();
        while ((k = fgetc (unit)) != EOF) {
            if ((k == EOL) | (lptr >= LINEMAX))
                break;
            if (k != 13) line[lptr++] = k;
        }
        line[lptr] = 0;
        if (output && cmode) {
            outstr("/\t");
            ol(line);
        }
        if (k <= 0)
            if (input2 != NULL) {
                input2 = inclstk[--inclsp];
                fclose (unit);
            }
        if (lptr) {
            if ((ctext) & (cmode)) {
                comment ();
                outstr (line);
                nl ();
            }
            lptr = 0;
            return 0;
        }
    }
}

inbyte ()
{
    while (ch () == 0) {
        if (feof (input))
            return (0);
        preprocess ();
    }
    return (gch ());
}

inchar ()
{
    if (ch () == 0)
        inln ();
    if (feof (input))
        return (0);
    return (gch ());
}

gch ()
{
    if (ch () == 0)
        return (0);
    else
        return (line[lptr++] & 127);
}

nch ()
{
    if (ch () == 0)
        return (0);
    else
        return (line[lptr + 1] & 127);
}

ch ()
{
    return (line[lptr] & 127);
}

/*
 *  print a carriage return and a string only to console
 *
 */
pl (str)
char    *str;
{
    int k;

    k = 0;
    putchar (EOL);
    while (str[k])
        putchar (str[k++]);
    return 0;
}
