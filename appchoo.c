
/* appchoo - Application Chooser with Timeout
 * (c) 2012 Ahmet Inan <ainan@mathematik.uni-freiburg.de>
 * written from scratch, public domain
 */

#include <math.h>
#include <SDL.h>
#include <SDL_image.h>

int fit_image(SDL_Surface *image, int w, int h)
{
	if (!image)
		return 0;
	if (image->format->BytesPerPixel != 3)
		return 0;
	if (w >= image->w && h >= image->h)
		return 1;
	int f = 0;
	do f++; while (image->w / f > w || image->h / f > h);
	w = image->w;
	h = image->h;
	image->clip_rect.w = image->w /= f;
	image->clip_rect.h = image->h /= f;
	image->pitch = image->w * 3;
	uint8_t *pixels = image->pixels;
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			uint32_t s[3] = {0, 0, 0};
			for (int j = 0; j < f; j++)
				for (int i = 0; i < f; i++)
					for (int c = 0; c < 3; c++)
						 s[c] += pixels[((y*f + j) * w + x*f + i) * 3 + c];
			for (int c = 0; c < 3; c++)
				pixels[(y * image->w + x) * 3 + c] = s[c] / (f*f);
		}
	}
	return 1;
}

void handle_events()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_q:
						exit(0);
						break;
					case SDLK_ESCAPE:
						exit(0);
						break;
					default:
						break;
				}
				break;
			case SDL_QUIT:
				exit(0);
				break;
			default:
				break;
		}
	}

}

int main(int argc, char **argv)
{
	// TODO: init values from commandline
	(void)argc; (void)argv;
#if 1
	int w = 640;
	int h = 480;
#else
	int w = 1920;
	int h = 1200;
#endif
	char *image_name;
	image_name = "duke.jpg";

	atexit(SDL_Quit);
	SDL_Init(SDL_INIT_VIDEO);

#if 1
	SDL_Surface *screen = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE);
#else
	SDL_Surface *screen = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE|SDL_FULLSCREEN);
#endif

	if (!screen)
		exit(1);
	if (screen->format->BytesPerPixel != 4)
		exit(1);
	if (screen->w != w || screen->h != h)
		exit(1);

	SDL_WM_SetCaption("Application Chooser", "appchoo");

	SDL_Surface *image = IMG_Load(image_name);
	if (!fit_image(image, screen->w, screen->h))
		exit(1);

	SDL_Rect src, dest;
	if (screen->w > image->w) {
		src.x = 0;
		src.w = image->w;
		dest.x = screen->w / 2 - image->w / 2;
		dest.w = image->w;
	} else {
		src.x = image->w / 2 - screen->w / 2;
		src.w = screen->w;
		dest.x = 0;
		dest.w = screen->w;
	}
	if (screen->h > image->h) {
		src.y = 0;
		src.h = image->h;
		dest.y = screen->h / 2 - image->h / 2;
		dest.h = image->h;
	} else {
		src.y = image->h / 2 - screen->h / 2;
		src.h = screen->h;
		dest.y = 0;
		dest.h = screen->h;
	}

	SDL_BlitSurface(image, &src, screen, &dest);
	SDL_FreeSurface(image);
	SDL_Flip(screen);

	for (;;) {
		SDL_Delay(100);
		handle_events();
	}
	return 0;
}

