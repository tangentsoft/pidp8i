int main()
{
	int ar[20],i,j,n;

	n=14;
	for (i=1;i<n;i++) {
		ar[i]=1;
		for (j=i-1;j>1;j--)
			ar[j]=ar[j-1]+ar[j];
		for (j=0;j<2*(n-i-1);j++)
			putc(' ');
		for (j=1;j<i+1;j++)
			printf("%4d",ar[j]);
		printf("\r\n");
	}
}
