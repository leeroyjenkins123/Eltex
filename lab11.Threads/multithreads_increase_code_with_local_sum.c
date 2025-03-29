#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define KERNELS 4
#define TOTAL 180000000

int sum = 0;
int local_sums[KERNELS];

void *increasing_sum_using_thread(void *arg)
{
	int index = *(int *)arg;
	local_sums[index] = 0;
	for (int i = 0; i < TOTAL / KERNELS; i++)
	{
		local_sums[index]++;
	}
	return NULL;
}

int main()
{

	pthread_t threads[KERNELS];
	int indexes[KERNELS];
	clock_t start = clock();
	for (int i = 0; i < KERNELS; i++)
	{
		indexes[i] = i;
		pthread_create(&threads[i], NULL, increasing_sum_using_thread, (void*)&indexes[i]);
	}
	for (int i = 0; i < KERNELS; i++)
	{
		pthread_join(threads[i], NULL);
		sum += local_sums[i];
	}
	clock_t end = clock();
	printf("Sum: %d\nTime: %f\n", sum, (double)(end - start) / CLOCKS_PER_SEC);

	return 0;
}