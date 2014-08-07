#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define rgb2gray(x) ((					\
		((float)((x) & 0xFF) * 0.21) +			\
		((float)(((x) >> 8) & 0xFF) * 0.72) +	\
		((float)(((x) >> 16) & 0xFF) * 0.07)	\
		) / 255.0f)


int main()
{
	uint32_t width;
	uint32_t height;
	uint32_t rgb;
	
	int i, j;
	
	scanf("%d", &width);
	scanf("%d", &height);
	printf("%d %d\n", width, height);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			scanf("%d", &rgb);
			printf("%f ", rgb2gray(rgb));
		}
		printf("\n");
	}

	return EXIT_SUCCESS;
}
