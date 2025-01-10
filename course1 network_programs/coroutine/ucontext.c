#include <ucontext.h>
#include <stdio.h>

ucontext_t ctx[2];
ucontext_t main_ctx;

int count = 0;

void func1(void)
{
    int cur = 0;
    while (count++ < 20)
    {
        printf("func1: %d [%d]\n", count, cur++);
        swapcontext(&ctx[0], &ctx[1]);
	}
}

void func2(void)
{
    int cur = 0;
    while (count++ < 20)
    {
        printf("func2: %d [%d]\n", count, cur++);
        swapcontext(&ctx[1], &ctx[0]);
	}
}

int main()
{
    char stack1[2048] = {0};
    char stack2[2048] = {0};
    
    getcontext(&ctx[0]);
    ctx[0].uc_stack.ss_sp = stack1;
    ctx[0].uc_stack.ss_size = sizeof(stack1);
    ctx[0].uc_link = &main_ctx;
    makecontext(&ctx[0], func1, 0);
    
    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp = stack2;
    ctx[1].uc_stack.ss_size = sizeof(stack2);
    ctx[1].uc_link = &main_ctx;
    makecontext(&ctx[1], func2, 0);
    
    printf("start\n");
    swapcontext(&main_ctx, &ctx[0]);
    
    return 0;
}