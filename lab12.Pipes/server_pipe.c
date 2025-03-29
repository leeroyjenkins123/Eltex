#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_NAME "/tmp/myfifo"

int main(){
	int fd;

	if (mkfifo(FIFO_NAME, 0666) < 0) {
        perror("mkfifo failed");
    }

    fd = open(FIFO_NAME, O_WRONLY);


    if (write(fd, "Hi!", 4) < 0) {
        perror("write failed");
    }

    close(fd);

	return 0;
}