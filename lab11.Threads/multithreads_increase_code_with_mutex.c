#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define KERNELS 4
#define TOTAL 180000000

int sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *increasing_sum_using_thread(void *arg)
{
	for (int i = 0; i < TOTAL / KERNELS; i++)
	{
		pthread_mutex_lock(&mutex);
		sum++;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main()
{

	pthread_t threads[KERNELS];
	clock_t start = clock();
	for (int i = 0; i < KERNELS; i++)
	{
		pthread_create(&threads[i], NULL, increasing_sum_using_thread, NULL);
	}
	for (int i = 0; i < KERNELS; i++)
	{
		pthread_join(threads[i], NULL);
	}
	clock_t end = clock();
	printf("Sum: %d\nTime: %f\n", sum, (double)(end - start) / CLOCKS_PER_SEC);

	return 0;
}