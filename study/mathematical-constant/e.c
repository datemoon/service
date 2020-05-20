#include <stdio.h>
#include <stdlib.h>



int main(int argc,char *argv[])
{
	if(argc != 4)
	{
		printf("%s a b n\n",argv[0]);
		return -1;
	}
	double a=atof(argv[1]),
		  b=atof(argv[2]);
	int n = atoi(argv[3]);
	double x = b/n;
	double y = a;
	int i=0;
	for(i=0;i<n;++i)
	{
		y = y - y/(a+x)*x;
	}
	printf("a:y=%f\n",a/y);
	return 0;
}
