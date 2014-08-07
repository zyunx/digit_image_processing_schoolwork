#include "intensity_image.h"
#include <stdio.h>
#include <stdlib.h>

int intensity_image_write(struct intensity_image *img, FILE *fp)
{
	int i, cx;
	float *p;

	fprintf(fp, "%d %d\n", img->width,
			img->height);

	p = img->intensity;
	cx = img->width * img->height;

	for (i = 0; i < cx; i++)
	{
		fprintf(fp, "%f ", *p);
		p++;
	}

	return 0;
}

struct intensity_image * intensity_image_read(FILE *fp)
{
	struct intensity_image *img;
	int i, cx;
	float *p;

	img = (struct intensity_image*) malloc(sizeof(struct intensity_image));

	fscanf(fp, "%d", &(img->width));
	fscanf(fp, "%d", &(img->height));

	p = img->intensity = (float *) malloc(
		img->width * img->height * sizeof(float));

	cx =  img->width * img->height;
	for (i = 0; i < cx; i++)
	{
		fscanf(fp, "%f", p);
		p++;
	}

	return img;
}
struct intensity_image * intensity_image_construct(uint32_t w, uint32_t h)
{
	struct intensity_image *img;
	
	img = (struct intensity_image*) malloc(sizeof(struct intensity_image));

	img->width = w;
	img->height = h;

	img->intensity = (float *) malloc(
		img->width * img->height * sizeof(float));

	return img;

}

int intensity_image_destroy(struct intensity_image *img)
{
	free(img->intensity);
	free(img);

	return 0;
}


