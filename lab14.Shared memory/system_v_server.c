#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#define PERMISSIONS 0666
#define SHM_SIZE 256
#define KEY_ID 50

void create_file()
{
	int fd = open("sysv_shared_memory", O_CREAT | O_RDWR, PERMISSIONS);
	if (fd == -1)
	{
		perror("open");
		exit(1);
	}
	close(fd);
}

int main()
{
	create_file();

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

    strcpy(shm_memory, "Hi!");

	while(strcmp(shm_memory, "Hello!") != 0){
		sleep(1);
	}

	printf("Client: %s\n", shm_memory);
	
	if(shmdt(shm_memory) < 0){
		perror("shmdt");
		exit(EXIT_FAILURE);
	}

	if(shmctl(shm_id, IPC_RMID, NULL) < 0){
		perror("shmctl");
		exit(EXIT_FAILURE);
	}

	return 0;
}