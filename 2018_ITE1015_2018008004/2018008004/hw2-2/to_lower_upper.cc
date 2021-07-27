#include<stdio.h>

int main()
{
char a[10];
scanf("%s", a);
for (int i = 0 ; a[i]!='\0' ; i++){
if(a[i]<91)
a[i] += 32;
else
a[i] -= 32;
}

printf("%s\n", a);

return 0;
}
