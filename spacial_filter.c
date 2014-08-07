#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <xcb/xcb.h>

#include "intensity_image.h"

struct mask
{
	uint32_t width;
	uint32_t height;
	uint32_t cx;
	uint32_t cy;
	float *coeffient;
};

struct mask filter_mask;

void usage()
{
	fprintf(stderr, "spacial_filter <mask_width> <mask_height> <mask_width * mask_height coeffients>\n");
}

void compute_filter_mask(int argc, char **argv)
{
	int i, j;
	float *p;
	char **ps;

	if (argc >= 5)
	{
		sscanf(argv[1], "%d", &filter_mask.width);
		sscanf(argv[2], "%d", &filter_mask.height);
		sscanf(argv[3], "%d", &filter_mask.cx);
		sscanf(argv[4], "%d", &filter_mask.cy);

		fprintf(stderr, "%d %d\n", filter_mask.width, filter_mask.height);
		fprintf(stderr, "%d %d\n", filter_mask.cx, filter_mask.cy);

		if (argc == 5 + filter_mask.width * filter_mask.height)
		{
			p = filter_mask.coeffient = (float *)
				malloc(filter_mask.width * filter_mask.height * sizeof(float));
			ps = &argv[5];

			for (i = 0; i < filter_mask.width * filter_mask.height; i++)
			{
				sscanf(*ps, "%f", p);
				ps++;
				p++;
				fprintf(stderr, "%f ", filter_mask.coeffient[i]);
			}
			return;
		}
		else
		{
			goto display_usage;
		}
	}

display_usage:
	usage();
	exit(EXIT_FAILURE);
}


void filter_pixel(int x, int y, struct intensity_image *from,
		struct intensity_image *to)
{
	int i, j;
	float s = 0;
	uint32_t tx, ty;

	for (i = 0; i < filter_mask.height; i++)
	{
		for (j = 0; j < filter_mask.width; j++)
		{
			tx = x - filter_mask.cx + j;
			ty = y - filter_mask.cy + i;

			if (tx >= 0 && tx < to->width
					&& ty >= 0 && ty < to->height)
			{
				s += filter_mask.coeffient[i * filter_mask.width +j] * 
					from->intensity[ty * from->width + tx];
			}
		}
	}
	to->intensity[y * to->width + x] = s;
}

void filter_all(struct intensity_image *to, struct intensity_image *from)
{
	int i,j;
	for (i = 0; i < to->height; i++)
	{
		for (j = 0; j < to->width; j++)
		{
			filter_pixel(j, i, from, to);
		}
	}
}

int main(int argc, char **argv)
{
	struct intensity_image *img, *filtered_img;
	float *p;
	float v;
	int i;

	compute_filter_mask(argc, argv);

	img = intensity_image_read(stdin);

	filtered_img = intensity_image_construct(img->width, img->height);

	filter_all(filtered_img, img);
	
	//intensity_image_write(img, stdout);
	intensity_image_write(filtered_img, stdout);

	intensity_image_destroy(filtered_img);
	intensity_image_destroy(img);

	return EXIT_SUCCESS;
}

