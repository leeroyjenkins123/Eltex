#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_NAME "posix_shared_memory"
#define PERMISSIONS 0666
#define SHM_SIZE 256

int main(){
	int shm_id = shm_open(SHM_NAME, O_RDWR, PERMISSIONS);

	if(shm_id < 0){
		perror("shm_open");
		exit(EXIT_FAILURE);
	}

	char *shm_memory = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);

	if (shm_memory == MAP_FAILED)
    {
        perror("mmap");
        close(shm_id);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

	printf("Server: %s\n", shm_memory);
    
	strcpy(shm_memory, "Hello!");

    munmap(shm_memory, SHM_SIZE);
    close(shm_id);
    unlink(SHM_NAME);

	return 0;
}