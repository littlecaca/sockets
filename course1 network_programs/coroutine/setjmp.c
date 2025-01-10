#include <setjmp.h>
#include <stdio.h>


jmp_buf env1, env2, env3;

void func1(void)
{
    int cur = 0;
    int ret = setjmp(env1);
    if (ret == 0)
        longjmp(env3, 1);
    
    printf("func1: %d [%d]\n", ret, cur++);
    if (ret < 20)
    {
	    longjmp(env2, ++ret);    
    }
}

void func2(void)
{
    int cur = 0;
    int ret = setjmp(env2);
    printf("func2: %d [%d]\n", ret, cur++);
    if (ret < 20)
    {
	    longjmp(env1, ++ret);    
    }
}

int main()
{
    int ret = setjmp(env3);
    if (ret == 0)
    	func1();
    else
        func2();
    
    return 0;
}