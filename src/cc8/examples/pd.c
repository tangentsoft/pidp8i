/* A test of doble precision addition */


/* Add single precision (low order 12 bit signed integer) to 
*  24 bit signed integer and print */

int dadd(rsl,val)
int *rsl,val;
{
int i;

	i=val;
	rsl;
#asm
	DCA JLC
	CLL
	TADI JLC
	TADI STKP
	DCAI JLC
	ISZ JLC
	SZL
	ISZI JLC
	TADI STKP
	SPA CLA
	CMA
	TADI JLC
	DCAI JLC
#endasm

}


int dbadd(rsl,val)
int *rsl,val;
{
int tm,*px;

        tm=*rsl;
        px=rsl+1;
        *rsl=*rsl+val;
        if (*rsl>0 & tm<0 & val>0)
                *px=*px+1;
        if (*rsl<0 & tm>-1 & val<0)
                *px=*px-1;
}

int dprint(vl)
int *vl;
{
        int *p,ct;

        p=vl+1;
        ct=0;
        while (*p>-1) {
                dadd(vl,-1000);
                ct++;
        }
        ct--;
        dadd(vl,1000);
        printf("%d%03d",ct,*vl);
}

int main(void)
{
    int total[2],i;

    memset(total,0,2);

    for (i = 3; i < 1000; i++)
        if (i%5==0 | i%3==0)
                dadd(total,i);

        puts("Total:");
        dprint(total);
        puts("\r\n");
        memset(total,0,2);
        puts("Test:");
        dadd(total,2010);
        dprint(total);
        puts("\r\n");
}








