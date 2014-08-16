seg2im: seg2im.o intensity_image.o

union: union.c intensity_image.o

spacial_filter: spacial_filter.o intensity_image.o

morph: morph.o intensity_image.o

straight: straight.o intensity_image.o
	$(CC) -o $@ $^ -lm

segment: segment.o intensity_image.o
	$(CC) -o $@ $^ -lm

imshow: imshow.o
	$(CC) -o $@ $^ -lxcb
