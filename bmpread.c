#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILENAME "an.bmp"

#define hton32(x) \
	((((x) & 0xFF000000) >> 24) | \
	 (((x) & 0x00FF0000) >>  8) | \
	 (((x) & 0x0000FF00) <<  8) | \
	 (((x) & 0x000000FF) << 24))

#define ntoh32 hton32

struct BITMAPFILEHEADER
{
	uint8_t BM[2];
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t pixel_array_offset;
}__attribute__((packed));

struct BITMAPINFOHEADER
{
	uint32_t size;
	int32_t width;
	int32_t height;
	uint16_t nu_color_plane;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t image_size;
}__attribute__((packed));

struct dips_image
{
	uint32_t width;
	uint32_t height;
	uint8_t *intensity;
};


#define rgb2gray(x) (					\
		((float)((x) & 0xFF) * 0.21) +			\
		((float)(((x) >> 8) & 0xFF) * 0.72) +	\
		((float)(((x) >> 16) & 0xFF) * 0.07)	\
		)


int main(int argc, char **argv)
{
	unsigned char *addr;
	int fd;
	struct stat sb;

	if (argc < 2) {
		fprintf(stderr, "Usage: bmpread <file name>\n");
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY);
	assert(fd != -1);

	assert(fstat(fd, &sb) != -1);
	fprintf(stderr, "%d\n", sb.st_size);
	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	assert(addr != MAP_FAILED);

	//printf("%x\n", addr[0]);
	if (addr[0] == 'B' && addr[1] == 'M')
	{
		fprintf(stderr, "BMP format file\n");
		
		struct BITMAPFILEHEADER *bmphdr = (struct BITMAPFILEHEADER*)addr;
		fprintf(stderr, "\tsize: %d\n" \
				"\tpixel array offset: %d\n",
				bmphdr->size,
				bmphdr->pixel_array_offset);

		struct BITMAPINFOHEADER *bmpInfoHdr = 
			(struct BITMAPINFOHEADER*) (bmphdr + 1);

		fprintf(stderr, "Bitmap info header:\n");
		fprintf(stderr, "\theader size %d\n", bmpInfoHdr->size);
		fprintf(stderr, "\twidth %d height %d\n",
				bmpInfoHdr->width, bmpInfoHdr->height);
		fprintf(stderr, "\tbits per pixel %d\n", bmpInfoHdr->bits_per_pixel);
		fprintf(stderr, "\tcompression %d\n", bmpInfoHdr->compression);
		fprintf(stderr, "image size %d\n", bmpInfoHdr->image_size);

		if (bmpInfoHdr->bits_per_pixel == 32)
		{
			uint32_t *pixels = 
				(uint32_t *)(addr + bmphdr->pixel_array_offset);

			struct dips_image img;
			img.width = bmpInfoHdr->width;
			img.height = bmpInfoHdr->height;
			img.intensity = (uint8_t *) malloc(img.width * img.height);
			uint8_t *pi = img.intensity;
			uint32_t *pp = pixels;
			int i;
			printf("%d %d\n", img.width, img.height);
			for (i = 0; i < img.width * img.height ; i++)
			{
		
				printf("%d ", *pp);
				//
				//*pi = rgb2gray(*pp);
				//pi++;
				pp++;

			}
		}

	}

	assert(munmap(addr, sb.st_size) != -1);

	assert(close(fd) != -1);

	return EXIT_SUCCESS;
}






