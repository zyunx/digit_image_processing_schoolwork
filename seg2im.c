#include "intensity_image.h"
#include <stdio.h>

#define BUF_LEN	50
char buf[BUF_LEN];

int main()
{
	struct intensity_image *img;
	int w, h;
	int xs, ys, xe, ye, tx, ty;
	float k;
	int j;

	scanf("%d %d", &w, &h);

	img = intensity_image_construct(w, h);

	while (1)
	{
		scanf("%d%d%d%d", &xs, &ys, &xe, &ye);
		fprintf(stderr, "%d %d %d %d\n", xs, ys, xe, ye);
		if (xs == xe && ys == ye) break;

		if (xs > xe)
		{
			tx = xs;
			xs = xe;
			xe = tx;

			ty = ys;
			ys = ye;
			ye = ty;
		}

		k = ((float)(ye - ys)) / ((float)(xe - xs));

		for (j = xs; j<= xe; j++)
		{
			ty = (int)(ys + k * (j - xs));
			if (ty >= 0 && ty < h)
				img->intensity[ty * w + j] = 1;
		}
	}

	intensity_image_write(img, stdout);
	intensity_image_destroy(img);
	return 0;
}
