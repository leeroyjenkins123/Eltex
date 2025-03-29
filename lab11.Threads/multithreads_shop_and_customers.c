#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define CUSTOMERS_NUM 3
#define SHOPS_NUM 5
#define CUSTOMERS_MIN 10000
#define CUSTOMERS_MAX 12000
#define FILL_AMOUNT 500

struct Shop
{
	pthread_mutex_t mutex;
	int productsAvailable;
};
struct Shop shops[SHOPS_NUM];

struct Customer
{
	bool done;
	int productsNeed;
};
struct Customer customers[CUSTOMERS_NUM];

bool running = true;

void init()
{
	for (int i = 0; i < SHOPS_NUM; i++)
	{
		pthread_mutex_init(&shops[i].mutex, NULL);
		shops[i].productsAvailable = FILL_AMOUNT;
	}

	for(int i=0;i<CUSTOMERS_NUM;i++){
		customers[i].productsNeed = 0;
	}
}

int get_random_need()
{
	return CUSTOMERS_MIN + rand() % (CUSTOMERS_MAX - CUSTOMERS_MIN + 1);
}

void *loader_thread_handler(void *arg)
{
	while (running)
	{
		for (int i = 0; i < SHOPS_NUM; i++)
		{
			if (pthread_mutex_trylock(&shops[i].mutex) == 0)
			{
				shops[i].productsAvailable += FILL_AMOUNT;
				printf("Loader fill shop %d to %d\n", i, shops[i].productsAvailable);
				pthread_mutex_unlock(&shops[i].mutex);
				sleep(1);
			}
		}
		bool all_done = true;
		for (int i = 0; i < CUSTOMERS_NUM; i++)
		{
			if (!customers[i].done)
			{
				all_done = false;
				break;
			}
		}
		if (all_done)
		{
			running = false;
			break;
		}
	}
	return 0;
}

void *customer_thread_handler(void *arg)
{
	int id = *(int *)arg;
	while (customers[id].productsNeed > 0)
	{
		for (int i = 0; i < SHOPS_NUM; i++)
		{
			int shopId = rand() % SHOPS_NUM;
			if (pthread_mutex_trylock(&shops[shopId].mutex) == 0)
			{
				int available = shops[shopId].productsAvailable;
				if (available > 0)
				{
					int productsTaken = (available >= customers[id].productsNeed) ? customers[id].productsNeed : available;
					shops[shopId].productsAvailable -= productsTaken;
					customers[id].productsNeed -= productsTaken;
					printf("\033[0;34mCustomer %d: visit shop %d, products take %d, left to buy %d\033[0;37m\n", id, shopId, productsTaken, customers[id].productsNeed);
					pthread_mutex_unlock(&shops[shopId].mutex);
					if (customers[id].productsNeed == 0)
					{
						customers[id].done = true;
						printf("\033[0;32mCustomer %d: all needed products have been bought\033[0;37m\n", id);
						pthread_exit(NULL);
					}
					sleep(2);
					break;
				}
				else
				{
					pthread_mutex_unlock(&shops[shopId].mutex);
				}
			}
		}
		sleep(2);
	}
	return NULL;
}

int main()
{
	srand(time(NULL));
	init();
	pthread_t loaderThread;
	pthread_t cutomerThreads[CUSTOMERS_NUM];
	int customerIndexes[CUSTOMERS_NUM];

	for (int i = 0; i < CUSTOMERS_NUM; i++)
	{
		customers[i].productsNeed = get_random_need();
		customerIndexes[i] = i;
		customers[i].done = false;
		printf("Customer %d: products need to buy %d\n", i, customers[i].productsNeed);
	}

	for (int i = 0; i < CUSTOMERS_NUM; i++)
	{
		pthread_create(&cutomerThreads[i], NULL, customer_thread_handler, &customerIndexes[i]);
	}

	pthread_create(&loaderThread, NULL, loader_thread_handler, NULL);

	for (int i = 0; i < CUSTOMERS_NUM; i++)
	{
		pthread_join(cutomerThreads[i], NULL);
	}

	pthread_join(loaderThread, NULL);

	for (int i = 0; i < SHOPS_NUM; i++)
	{
		pthread_mutex_destroy(&shops[i].mutex);
	}

	printf("\033[0;32mAll customers have bought all their needed products\033[0;37m\n");
	return 0;
}