#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5

void* print_thread_index(void *arg){
	int index = *(int*)arg;
	printf("Thread with index: %d\n", index);
	pthread_exit(NULL);
}

int main(){

	pthread_t threads[NUM_THREADS];
	int thread_indexes[NUM_THREADS];

	for(int i=0;i<NUM_THREADS;i++){
		thread_indexes[i] = i;
		pthread_create(&threads[i], NULL, print_thread_index, (void*)&thread_indexes[i]);
		// if( != 0){
		// 	perror("pthread_create error");
		// 	exit(EXIT_FAILURE);
		// }
	}

	for(int i=0;i<NUM_THREADS;i++){
		pthread_join(threads[i], NULL);
	}

	return 0;
}