#include <stdio.h>

void square (double*);

int main ()
{
	double dvar;
	scanf("%lf", &dvar);
	square(&dvar);
	printf("%f\n", dvar);

return 0;
}

void square (double* a)
{
	*a *= *a;
}
