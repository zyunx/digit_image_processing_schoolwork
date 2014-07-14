#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PNG_0	0x89
#define PNG_1	0x50
#define PNG_2	0x4E
#define PNG_3	0x47
#define PNG_4	0x0D
#define PNG_5	0x0A
#define PNG_6	0x1A
#define PNG_7	0x0A

#define FILENAME "a.png"

#define hton32(x) \
	((((x) & 0xFF000000) >> 24) | \
	 (((x) & 0x00FF0000) >>  8) | \
	 (((x) & 0x0000FF00) <<  8) | \
	 (((x) & 0x000000FF) << 24))

struct chunk_header
{
	uint32_t len;
	char type[4];
};

int main()
{
	unsigned char *addr;
	unsigned char *chunks;
	struct chunk_header *chdr;
	int fd;
	struct stat sb;

	fd = open(FILENAME, O_RDONLY);
	assert(fd != -1);

	assert(fstat(fd, &sb) != -1);

	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	assert(addr != MAP_FAILED);

	//printf("%x\n", addr[0]);
	if (addr[0] == PNG_0 &&
			addr[1] == PNG_1 &&
			addr[2] == PNG_2 &&
			addr[3] == PNG_3 &&
			addr[4] == PNG_4 &&
			addr[5] == PNG_5 &&
			addr[6] == PNG_6 &&
			addr[7] == PNG_7)
	{
		printf("PNG file\n");
		chunks = addr + 8;

		do
		{
			chdr = (struct chunk_hear*) chunks;
			chunks += 8 + hton32(chdr->len) + 4;
			printf("%c%c%c%c %d\n", chdr->type[0],
					chdr->type[1],
					chdr->type[2],
					chdr->type[3],
					hton32(chdr->len));

		} while(!(chdr->type[0] == 'I' &&
					chdr->type[1] == 'E' &&
					chdr->type[2] == 'N' &&
					chdr->type[3] == 'D'));
	}

	assert(munmap(addr, sb.st_size) != -1);

	assert(close(fd) != -1);

	return EXIT_SUCCESS;
}
