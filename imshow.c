#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <xcb/xcb.h>

struct dips_image
{
	uint32_t width;
	uint32_t height;
	uint8_t *intensity;
};


int display_image(struct dips_image *);

int main()
{
	struct dips_image img;
	uint8_t *p;
	float v;
	int i;

	scanf("%d", &img.width);
	scanf("%d", &img.height);
	p = img.intensity = (uint8_t *) malloc(img.width * img.height);
	
	for (i = 0; i < img.width * img.height; i++)
	{
		scanf("%f", &v);
		*p++ = 255 * v;
	}

	display_image(&img);

	free(img.intensity);
	return EXIT_SUCCESS;
}

int display_image(struct dips_image *img)
{
	int i, j;
	xcb_connection_t		*c;
	xcb_screen_t			*screen;
	xcb_drawable_t			win;
	xcb_gcontext_t			foreground;
	xcb_generic_event_t		*e;
	uint32_t				mask = 0;
	uint32_t				values[2];

	xcb_point_t				point[] = {
		{0, 0}
	};
	/* Open the connectioin to the X server.
	 * Use the DISPLAY environment variable as
	 * the default display name */
	c = xcb_connect(NULL, NULL);

	/* Get the screen #screen_nbr */
	screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
	win = screen->root;

	foreground = xcb_generate_id(c);
	mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	values[0] = screen->black_pixel;
	values[1] = 0;
	xcb_create_gc(c, foreground, win, mask, values);
	/*
	printf("\n");
	printf("Informations of screen %ld:\n", screen->root);
	printf("  width............: %d\n", screen->width_in_pixels);
	printf("  height...........: %d\n", screen->height_in_pixels);
	printf("  white pixel......: %d\n", screen->white_pixel);
	printf("  black pixel......: %d\n", screen->black_pixel);
	printf("\n");
*/

	/* Ask for our window's id */
	win = xcb_generate_id(c);

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE;
	/* Create the window */
	xcb_create_window(c,
			XCB_COPY_FROM_PARENT,
			win,
			screen->root,
			0, 0,
			img->width, img->height,
			10,
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			screen->root_visual,
			mask, values);
	/* Map the window on the screen */
	xcb_map_window(c, win);

	/* Make sure commands are sent before we pause,
	 * so window is shown */
	xcb_flush(c);

	while ((e = xcb_wait_for_event(c)))
	{
		switch (e->response_type & ~0x80)
		{
			case XCB_EXPOSE:
				for (i = 0; i < img->height; i++)
				{
					for (j = 0; j < img->width; j++)
					{
						point[0].x = j;
						point[0].y = img->height - i;

						mask = XCB_GC_FOREGROUND;
						//printf("%02x\n", img->intensity[i*img->width + j]);
						values[0] = 
							((img->intensity[i*img->width + j]) << 16) +
							((img->intensity[i*img->width + j]) << 8) +
							img->intensity[i*img->width + j] +
							0x0;
						
						//printf("%x ", values[0]);
						xcb_change_gc(c, foreground,
								mask, values);
						
						xcb_poly_point(c, 
								XCB_COORD_MODE_ORIGIN,
								win, foreground,
								1,
								point);
					}
					
				}
				xcb_flush(c);
				break;
			default:
				break;
		}

		free(e);
	}
//	sleep(5);

	xcb_disconnect(c);
	return 0;
}
