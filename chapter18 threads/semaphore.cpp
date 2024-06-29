#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void *read(void *arg);
void *accu(void *arg);

/**
 * 生产者与消费者问题
 */
static sem_t sem_one;   // 数量
static sem_t sem_two;   // 容量
static int num;

#define NUM_SIZE 5

int main(int argc, char const *argv[])
{
    pthread_t t_id1, t_id2;
    int i;
    int sum = 0;
    int args[NUM_SIZE] = {3, 2, 1, 9, 5};
    sem_init(&sem_one, 0, 0);
    sem_init(&sem_two, 0, 1);
   
    pthread_create(&t_id1, NULL, &accu, (void *)args);
    pthread_create(&t_id2, NULL, &read, (void *)&sum);

    pthread_join(t_id1, NULL);
    pthread_join(t_id2, NULL);
    
    printf("The result is %d\n", sum);
    sem_destroy(&sem_one);
    sem_destroy(&sem_two);
    return 0;
}

void *accu(void *arg)
{
    for (int i = 0; i < NUM_SIZE; ++i)
    {
        sem_wait(&sem_two);
        num = ((int *)arg)[i];
        sem_post(&sem_one);
    }
    return NULL;
}

void *read(void *arg)
{
    for (int i = 0; i < NUM_SIZE; ++i)
    {
        sem_wait(&sem_one);
        *(int *)arg += num;
        sem_post(&sem_two);
    }
    return NULL;
}
