#include <stdio.h>
typedef struct
{
 double real;
 double imag;
}Complex;

void printComplex(Complex a)
{
//if(a.imag <= 0)
//printf("%f - %fi\n", a.real, -a.imag);
//else  				허수부가 음수일 때!
printf("%f + %fi\n", a.real, a.imag);
}

void printAddComp(Complex a,Complex b){
Complex add = {a.real+b.real,a.imag+b.imag};
printComplex(add);
}

void printMulComp(Complex a, Complex b){
Complex mul = {a.real*b.real-a.imag*b.imag , a.real*b.imag + a.imag*b.real};
printComplex(mul);
}

int main()
{
Complex a , b;
scanf("%lf %lf %lf %lf",&a.real,&a.imag,&b.real,&b.imag);

printf("복소수1:\n");
printComplex(a);
printf("복소수2:\n");
printComplex(b);
printf("두 복소수의 합:\n");
printAddComp(a,b);
printf("두 복소수의 곱:\n");
printMulComp(a,b);

return 0;
}
