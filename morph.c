#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "intensity_image.h"


#define ONE_THRESHHOLD 0.9f

struct structure_element
{
	uint32_t width;
	uint32_t height;
	uint32_t cx;
	uint32_t cy;
	float *values;
};

struct structure_element struct_elem;

uint8_t f_result = 0;
uint8_t h_result = 0;
uint8_t m_result = 0;

void usage()
{
	fprintf(stderr, "morph [[f][h][m]] <mask_width> <mask_height> <mask_width * mask_height coeffients>\n");
}

void parse_args(int argc, char **argv)
{
	int i, j;
	float *p;
	char **ps;

	int argi = 1;

	if (argc - argi >= 1)
	{
		if (!isdigit(argv[argi][0]))
		{
			// f h m operation is specified
			for (i = 0; i < strlen(argv[argi]); i++)
			{
				switch (argv[argi][i])
				{
					case 'f':
						f_result = 1;
						break;
					case 'h':
						h_result = 1;
						break;
					case 'm':
						m_result = 1;
						break;
					default:
						goto display_usage;
				}
			}
			argi++;
		}
	}

	if (argc - argi >= 4)
	{
		sscanf(argv[argi], "%d", &struct_elem.width);
		argi++;
		sscanf(argv[argi], "%d", &struct_elem.height);
		argi++;
		sscanf(argv[argi], "%d", &struct_elem.cx);
		argi++;
		sscanf(argv[argi], "%d", &struct_elem.cy);
		argi++;

		//fprintf(stderr, "%d %d\n", struct_elem.width, struct_elem.height);
		//fprintf(stderr, "%d %d\n", struct_elem.cx, struct_elem.cy);

		if (argc - argi == struct_elem.width * struct_elem.height)
		{
			p = struct_elem.values = (float *)
				malloc(struct_elem.width * struct_elem.height * sizeof(float));
			ps = &argv[argi];

			for (i = 0; i < struct_elem.width * struct_elem.height; i++)
			{
				sscanf(*ps, "%f", p);
				ps++;
				p++;
				//fprintf(stderr, "%f ", struct_elem.values[i]);
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


void probe_pixel(int x, int y, struct intensity_image *from,
		struct intensity_image *to)
{
	int i, j;
	float s = 0;
	uint32_t tx, ty;
	int f, h, m;

	f = 1;
	h = 0;
	m = 1;
	for (i = 0; i < struct_elem.height; i++)
	{
		for (j = 0; j < struct_elem.width; j++)
		{
			tx = x - struct_elem.cx + j;
			ty = y - struct_elem.cy + i;

			if (tx >= 0 && tx < to->width
					&& ty >= 0 && ty < to->height)
			{
				if (struct_elem.values[i * struct_elem.width +j])
				{
					if(from->intensity[ty * from->width + tx] > ONE_THRESHHOLD)
					{
						h = 1;
						m = 0;
					}
					else
					{
						f = 0;
					}

				}
			}
		}
	}

	if (f)
		to->intensity[y * to->width + x] = f_result;
	else if (h)
		to->intensity[y * to->width + x] = h_result;
	else if (m)
		to->intensity[y * to->width + x] = m_result;

}

void probe_all(struct intensity_image *to, struct intensity_image *from)
{
	int i,j;
	for (i = 0; i < to->height; i++)
	{
		for (j = 0; j < to->width; j++)
		{
			probe_pixel(j, i, from, to);
		}
	}
}

int main(int argc, char **argv)
{
	struct intensity_image *img, *result_img;
	float *p;
	float v;
	int i;

	parse_args(argc, argv);

	img = intensity_image_read(stdin);

	result_img = intensity_image_construct(img->width, img->height);

	probe_all(result_img, img);
	
	//intensity_image_write(img, stdout);
	intensity_image_write(result_img, stdout);

	intensity_image_destroy(result_img);
	intensity_image_destroy(img);

	return EXIT_SUCCESS;
}

