#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_GRAY_PAIR 10

float gray_pair[MAX_GRAY_PAIR][2];
int nu_gray_pair;

int main(int argc, char **argv)
{
	uint32_t width;
	uint32_t height;
	float gray;
	int i, j, k, f;

	/* argument check */
	if (argc % 2 == 0)
	{
		printf("Usage: gray2bi [<gray level pair> ...]\n");
	}

	nu_gray_pair = argc / 2;
	for (i = 0; i < nu_gray_pair; i++)
	{
		sscanf(argv[1 + i*2], "%f", &gray_pair[i][0]);
		sscanf(argv[1 + i*2 + 1], "%f", &gray_pair[i][1]);
		/*printf("%f %f\n", gray_pair[i][0],
				gray_pair[i][1]);
		exit(0);*/
	}
	
	scanf("%d", &width);
	scanf("%d", &height);
	printf("%d %d\n", width, height);

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			scanf("%f", &gray);
		//	printf("%f ", gray);
			
			f = 0;
			for (k = 0; k < nu_gray_pair; k++)
			{
				if (gray <= gray_pair[k][1] &&
						gray >= gray_pair[k][0])
				{
					f = 1;
					break;
				}
				
			}

			if (f)
				printf("1 ");
			else
				printf("0 ");
				
		}
	}

	return EXIT_SUCCESS;
}
