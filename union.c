#include "intensity_image.h"


int main()
{
	struct intensity_image *a, *b, *c;
	float *pa, *pb, *pc, *end;
	

	a = intensity_image_read(stdin);
	b = intensity_image_read(stdin);
	c = intensity_image_construct(a->width, a->height);

	pa = a->intensity;
	pb = b->intensity;
	pc = c->intensity;

	end = a->intensity +  a->width * a->height;
	while (pa < end)
	{
		*pc = *pa > *pb ? *pa : *pb;
		pa++;
		pb++;
		pc++;
	}

	intensity_image_write(c, stdout);

	intensity_image_destroy(c);
	intensity_image_destroy(b);
	intensity_image_destroy(a);
	return 0;
}
