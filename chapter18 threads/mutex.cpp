#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD 100

void *thread_inc(void *args);
void *thread_dec(void *args);

long long sum = 0;
pthread_mutex_t mutex;

int main(int argc, char const *argv[])
{
    pthread_t thread_id[NUM_THREAD];
    int i;

    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < NUM_THREAD; ++i)
    {
        if (i % 2)
            pthread_create(&(thread_id[i]), NULL, &thread_inc, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, &thread_dec, NULL);
    }

    for (i = 0; i < NUM_THREAD; ++i)
        pthread_join(thread_id[i], NULL);
    
    printf("result: %lld\n", sum);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *thread_inc(void *args)
{
    int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < 5000; ++i)
        sum += 1;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *thread_dec(void *args)
{
    int i;
    for (i = 0; i < 5000; ++i)
    {
        pthread_mutex_lock(&mutex);
        sum -= 1;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
