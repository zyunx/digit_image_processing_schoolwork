#include "intensity_image.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define THETA_SAMPLE	180
#define P_HALF_SAMPLE	200
#define P_SAMPLE		(2*P_HALF_SAMPLE)

#define TRANS_P(x,y,theta) \
	(x)*cosf((theta)) + (y)*sinf((theta))
	
#define EDGE_THRESHHOLD 0.9f


int detect_straightline(struct intensity_image *img,
		struct intensity_image *to)
{
	int *ax;
	float max_p;
	float theta, p;
	int i, j, k;
	int p_cell;

	max_p = sqrtf(img->width * img->width +
			img->height * img->height);
	fprintf(stderr, "MAX_P %f\n", max_p);

	ax = (int *)malloc(THETA_SAMPLE * P_SAMPLE * sizeof(int));
	memset(ax, 0, THETA_SAMPLE * P_SAMPLE * sizeof(int));
	
	for (i = 0; i < img->height; i++)
	{
		for (j = 0; j < img->width; j++)
		{
			if (img->intensity[i*img->width + j] > EDGE_THRESHHOLD)
			{
			//	fprintf(stderr, "DOT [%d, %d]\n", j, i);
				for (k = 0; k < THETA_SAMPLE; k++)
				{
					p = TRANS_P(j, i, k * M_PI / THETA_SAMPLE);
					p_cell =  p * P_HALF_SAMPLE / max_p + P_HALF_SAMPLE;
					p_cell = p_cell > P_SAMPLE -1 ? P_SAMPLE-1 : p_cell;
					ax[k * P_SAMPLE + p_cell]++;

				//	fprintf(stderr, "PRAM %d[%f] %d[%f]\n", k,  k * M_PI / THETA_SAMPLE, p_cell, p);
				}

			}

		}
	}
/*
	struct intensity_image *mm = intensity_image_construct(P_SAMPLE, THETA_SAMPLE);

	for (i = 0; i < THETA_SAMPLE; i++)
	{
		for (j = 0; j < P_SAMPLE; j++)
		{
			mm->intensity[i * P_SAMPLE + j] = ax[i * P_SAMPLE + j] / 400.0f;
		}
	}
	
	intensity_image_write(mm, stdout);
	intensity_image_destroy(mm);


*/
	int x, y;
	// theta = 0
	/*
	for (j = 0; j < P_SAMPLE; j++)
	{
		if (ax[j] > 300)
		{
			x = (j - P_HALF_SAMPLE) * max_p / P_HALF_SAMPLE;
			for (y = 0; y < to->height; y++)
				to->intensity[y + to->width + x] = 1;
		}
	}*/

	for (i = 1; i < THETA_SAMPLE; i++)
	{
		for (j = 0; j < P_SAMPLE; j++)
		{
			if (ax[i * P_SAMPLE + j] > 300)
			{
				theta = i * M_PI / THETA_SAMPLE;
				p = (j - P_HALF_SAMPLE) * max_p / P_HALF_SAMPLE;
				fprintf(stderr, "Parameter pair: %f, %f\n", theta, p);

				for (x = 0; x < to->width; x++)
				{
					y = -(cosf(theta) / sinf(theta)) * x + p / sinf(theta);
					
					if (y < to->height)
						to->intensity[y * to->width + x] = 1;
				}
			}
		}
	}
	//*/
}


int main(int argc, char **argv)
{
	struct intensity_image *img, *to;

	img = intensity_image_read(stdin);
	to = intensity_image_construct(img->width, img->height);

	detect_straightline(img, to);
	
	intensity_image_write(to, stdout);

	intensity_image_destroy(to);
	intensity_image_destroy(img);
	return 0;
}
