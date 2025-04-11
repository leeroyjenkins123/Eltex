#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#define SHM_SIZE 256
#define PERMISSIONS 0666
#define KEY_ID 50

int main()
{
	key_t key;
	int shm_id;

	key = ftok("sysv_shared_memory", KEY_ID);

	if (key < 0)
	{
		perror("ftok");
		exit(EXIT_FAILURE);
	}

	shm_id = shmget(key, SHM_SIZE, IPC_CREAT | PERMISSIONS);
	if (shm_id < 0)
	{
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	char *shm_memory = (char *)shmat(shm_id, NULL, 0);

	if (shm_memory == (char *)-1)
	{
		perror("shmat");
		exit(1);
	}
	printf("Server: %s\n", shm_memory);

	strcpy(shm_memory, "Hello!");

	if (shmdt(shm_memory) < 0)
	{
		perror("shmdt");
		exit(EXIT_FAILURE);
	}

	return 0;
}