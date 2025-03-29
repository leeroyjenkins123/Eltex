#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	int sum = 0;

	clock_t start = clock();
	for (int i = 0; i < 180000000; i++)
	{
		sum++;
	}
	clock_t end = clock();

	printf("Sum = %d\nTime = %f\n", sum, (double)(end - start) / CLOCKS_PER_SEC);
	return 0;
}