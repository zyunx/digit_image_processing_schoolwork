#include "intensity_image.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#define THETA_SAMPLE	90
int THETA_SAMPLE;
//#define P_HALF_SAMPLE	200
int P_HALF_SAMPLE;
#define P_SAMPLE		(2*P_HALF_SAMPLE)
int STRAIGHT_LINE_THRESHHOLD;

int SEGMENT_THRESHHOLD;

#define TRANS_P(x,y,theta) \
	(x)*cosf((theta)) + (y)*sinf((theta))
	
#define EDGE_THRESHHOLD 0.9f


#define INTENSITY(img, x, y)	((img)->intensity[(y) * (img)->width + (x)])

int morph_hit(int x, int y, float theta, struct intensity_image *img)
{
	int f;
	int tx, ty;
	float tp, cp;
	int tp_cell, cp_cell;
	int max_p;
	int w, h;
	int s;

	w = img->width;
	h = img->height;

	max_p = sqrtf(w*w + h*h);

	cp = TRANS_P(x, y, theta);
	cp_cell =  cp * P_HALF_SAMPLE / max_p + P_HALF_SAMPLE;

	s = max_p / P_HALF_SAMPLE;

	f = 0;
	for (tx = x ; f == 0 && tx > 0 && tx < w &&  tx <= x + s; tx++)
	{
		for (ty = y - s > 0 ? y-s : 0; f == 0 && ty < h && ty <= y + s; ty++)
		{
			tp = TRANS_P(tx, ty, theta);
			tp_cell =  tp * P_HALF_SAMPLE / max_p + P_HALF_SAMPLE;
			//	fprintf(stderr, "TX TY %d %d\n", tx, ty);
			if (INTENSITY(img, tx, ty) > EDGE_THRESHHOLD &&
					tp_cell == cp_cell)
			{
				f = 1;
							//fprintf(stderr, "Start[%d] %d,%d\n", tp_cell, x, y);
			}
		}
	}

	return f;
}

int segment(float theta, float p, int min, struct intensity_image *img)
{
	int32_t xs, ys, xe, ye; // x start, y start, x end, y end
	int32_t status = 0; // 0: not started, 1 started
	int x, y;
	int tx, ty; // tmp x, tmp y
	int f, count; // quantilized predicate flag
	
	uint32_t w, h;
	float *it;

	float cp, tp;
	int cp_cell, tp_cell;

	int ypre;

	w = img->width;
	h = img->height;
	it = img->intensity;

	int max_p = sqrtf(img->width * img->width +
			img->height * img->height);

	cp_cell =  p * P_HALF_SAMPLE / max_p + P_HALF_SAMPLE;
	//cp_cell =  cp_cell > P_SAMPLE -1 ? P_SAMPLE-1 : cp_cell;
	//fprintf(stderr, "[%d]\n", cp_cell);

	status = 0;
	if (theta < M_PI / THETA_SAMPLE)
	{
		x =	p;
		for (y = 0; y < h; y++)
		{
		}	
	}
	else
	{
		for (x = 0; x < w; x++)
		{	
			y = -(cosf(theta) / sinf(theta)) * x + p / sinf(theta);	
	//	fprintf(stderr, "Y: %d\n", y);	
			if (status == 0)
			{
				if (morph_hit(x, y, p, img))
				{
					status = 1;
					count = 0;
					xs = x;
					ys = y;
				} 

			} 
			else if (status == 1)
			{
				if (morph_hit(x, y, p, img))
				{
					count++;
				} 
				else
				{
					status = 0;
					xe = x;
					ye = y;
					if (count > min)
					{
						printf("%d %d %d %d\n", xs, ys, xe, ye);
					}

				}
			}
		}// end for
	} // end if (theta < ...)
}

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
	//fprintf(stderr, "MAX_P %f\n", max_p);

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
						ax[k * P_SAMPLE + p_cell]++;

				//	fprintf(stderr, "PRAM %d[%f] %d[%f]\n", k,  k * M_PI / THETA_SAMPLE, p_cell, p);
				}

			}

		}
	}

	int x, y;
	// theta = 0
	for (j = 0; j < P_SAMPLE; j++)
	{
		if (ax[j] > STRAIGHT_LINE_THRESHHOLD)
		{
			//fprintf(stderr, "theta == 0\n");
			p = (j - P_HALF_SAMPLE) * max_p / P_HALF_SAMPLE;
			if (ax[j] > STRAIGHT_LINE_THRESHHOLD)
			{
				fprintf(stderr, "Parameter pair: %f, %f\n", 0.0f, p);
				segment(theta, p , SEGMENT_THRESHHOLD, img);
			}
		}
	}

	// for theta > 0
	for (i = 1; i < THETA_SAMPLE; i++)
	{
		for (j = 0; j < P_SAMPLE; j++)
		{
			if (ax[i * P_SAMPLE + j] > STRAIGHT_LINE_THRESHHOLD)
			{
				theta = i * M_PI / THETA_SAMPLE;
				p = (j - P_HALF_SAMPLE) * max_p / P_HALF_SAMPLE;
				fprintf(stderr, "Parameter pair: %f, %f\n", theta, p);
/*				printf("%d %d %d %d\n",
						0, (int)(p / sinf(theta)),
						to->width,
						(int)
						(-(cosf(theta) / sinf(theta)) * to->width + p / sinf(theta)));
*/
				segment(theta, p, SEGMENT_THRESHHOLD, img);
				
				for (x = 0; x < to->width; x++)
				{
					y = -(cosf(theta) / sinf(theta)) * x + p / sinf(theta);
					
					if (y >= 0 && y < to->height)
						to->intensity[y * to->width + x] = 1;
				}
			}
		}
	}
	//*/
}


void usage()
{
	printf("segment <theta sample> <r sample> " \
			"<straight line threshhold>" \
			"<segment threshhold\n");
}

int main(int argc, char **argv)
{
	struct intensity_image *img, *to;

	if (argc < 5)
	{
		usage();
		exit(EXIT_FAILURE);
	}

	sscanf(argv[1], "%d", &THETA_SAMPLE);
	sscanf(argv[2], "%d", &P_HALF_SAMPLE);
	sscanf(argv[3], "%d", &STRAIGHT_LINE_THRESHHOLD);
	sscanf(argv[4], "%d", &SEGMENT_THRESHHOLD);

	img = intensity_image_read(stdin);
	to = intensity_image_construct(img->width, img->height);

	printf("%d %d\n", img->width, img->height);
	detect_straightline(img, to);
	printf("%d %d %d %d\n", 0, 0, 0, 0);
	intensity_image_destroy(to);
	intensity_image_destroy(img);
	return 0;
}
