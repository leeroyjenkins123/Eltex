#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define FIFO_NAME "/tmp/myfifo"

int main()
{
	int fd;
	char buffer[20];

	fd = open(FIFO_NAME, O_RDONLY);
	read(fd, &buffer, sizeof(buffer));
	printf("Client: %s\n", buffer);
	close(fd);

	unlink(FIFO_NAME);

	return 0;
}