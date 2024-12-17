#include <stdio.h>

int myVar = 10;

void myFunc(){
    printf("Hello World\n");
}
void foo();

int main()
{
   myVar++;
   myFunc();
   foo();
   printf("%d\n",myVar);
}
