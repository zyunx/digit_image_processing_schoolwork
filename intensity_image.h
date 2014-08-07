#ifndef _CLOUD2HAN9_DIPS_INTENSITY_IMAGE_H
#define _CLOUD2HAN9_DIPS_INTENSITY_IMGAE_H

#include <stdio.h>
#include <stdint.h>

struct intensity_image
{
	uint32_t width;
	uint32_t height;
	float *intensity;
};

struct intensity_image * intensity_image_construct(uint32_t w, uint32_t h);
int intensity_image_destroy(struct intensity_image*);
int intensity_image_write(struct intensity_image *, FILE *fp);
struct intensity_image* intensity_image_read(FILE *fp);

#endif
