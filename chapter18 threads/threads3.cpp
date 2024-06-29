#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


void error_handling(const char *message);

void *sum_thread(void *arg)
{
    int *nums = (int *)arg;
    int *ans = (int *)malloc(sizeof(int));
    *ans = 0;
    for (int i = nums[0]; i <= nums[1]; ++i)
        *ans += i;
    return ans;
}

int main(int argc, char const *argv[])
{
    pthread_t t_id1, t_id2;
    void *thr_ret1, *thr_ret2;
    int args1[2] = {1, 5};
    int args2[2] = {6, 10};
    if (pthread_create(&t_id1, NULL, &sum_thread, (void *)args1) != 0)
        error_handling("pthread_create() error");
    if (pthread_create(&t_id2, NULL, &sum_thread, (void *)args2) != 0)
        error_handling("pthread_create() error");

    if (pthread_join(t_id1, &thr_ret1) != 0)
        error_handling("pthread_join() error");
    if (pthread_join(t_id2, &thr_ret2) != 0)
        error_handling("pthread_join() error");
    
    printf("The answer of sum(1, 10) is %d\n", *(int *)thr_ret1 + *(int *)thr_ret2);
    free(thr_ret1);
    free(thr_ret2);
    return 0;
}

void error_handling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(-1);
}
