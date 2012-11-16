
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
	int bytes = image->format->BytesPerPixel;
	if (bytes != 3 && bytes != 4)
		return 0;
	if (w >= image->w && h >= image->h)
		return 1;
	int fx = (image->w + (w-1)) / w;
	int fy = (image->h + (h-1)) / h;
	int f = fx > fy ? fx : fy;
	int pitch = image->pitch;
	image->clip_rect.w = image->w /= f;
	image->clip_rect.h = image->h /= f;
	image->pitch = (image->w * bytes + 3) & (~3);
	uint8_t *pixels = image->pixels;
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			for (int c = 0; c < bytes; c++) {
				uint32_t sum = 0;
				for (int j = 0; j < f; j++)
					for (int i = 0; i < f; i++)
						 sum += pixels[(y*f+j) * pitch + (x*f+i) * bytes + c];
				pixels[y * image->pitch + x * bytes + c] = sum / (f*f);
			}
		}
	}
	return 1;
}

void center_image(SDL_Rect *dest, SDL_Rect *src)
{
	if (dest->w > src->w) {
		dest->x += dest->w / 2 - src->w / 2;
		dest->w = src->w;
	} else {
		src->x += src->w / 2 - dest->w / 2;
		src->w = dest->w;
	}
	if (dest->h > src->h) {
		dest->y += dest->h / 2 - src->h / 2;
		dest->h = src->h;
	} else {
		src->y += src->h / 2 - dest->h / 2;
		src->h = dest->h;
	}
}

void handle_events(SDL_Rect *rects, char **apps, int num)
{
	static int mouse_x = 0;
	static int mouse_y = 0;
	int button = 0;
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
			case SDL_MOUSEMOTION:
				mouse_x = event.motion.x;
				mouse_y = event.motion.y;
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button) {
					case SDL_BUTTON_LEFT:
						button = 1;
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
	if (!button)
		return;
	for (int i = 0; i < num; i++) {
		if (rects[i].x <= mouse_x && mouse_x < (rects[i].x + rects[i].w) && rects[i].y <= mouse_y && mouse_y < (rects[i].y + rects[i].h)) {
			fputs(apps[i], stdout);
			exit(0);
		}
	}

}

int main(int argc, char **argv)
{
	(void)argc; (void)argv;

	int max = 32;
	int num = 0;
	char imgs[max][256];
	char *apps[max];

	while (num < max && fgets(imgs[num], 256, stdin)) {
		imgs[num][strnlen(imgs[num], 256) - 1] = 0;
		char *delim = strchr(imgs[num], ' ');
		apps[num] = delim + 1;
		*delim = 0;
		num++;
	}

	atexit(SDL_Quit);
	SDL_Init(SDL_INIT_VIDEO);

	uint32_t flags = SDL_SWSURFACE|SDL_FULLSCREEN;

	SDL_Rect** modes = SDL_ListModes(0, flags);

	int w = modes[0]->w;
	int h = modes[0]->h;

	SDL_Surface *screen = SDL_SetVideoMode(w, h, 32, flags);

	if (!screen)
		exit(1);
	if (screen->format->BytesPerPixel != 4)
		exit(1);
	if (screen->w != w || screen->h != h)
		exit(1);

	SDL_WM_SetCaption("Application Chooser", "appchoo");

	int num_x = 1;
	int num_y = 1;

	while (num > (num_x * num_y)) {
		if (num_y < num_x)
			num_y++;
		else
			num_x++;
	}

	SDL_Rect rects[max];
	for (int i = 0; i < num; i++) {
		SDL_Surface *image = IMG_Load(imgs[i]);

		SDL_Rect dest = screen->clip_rect;

		dest.w /= num_x;
		dest.h /= num_y;

		dest.x += (i % num_x) * dest.w;
		dest.y += ((i / num_x) % num_y) * dest.h;

		if (!fit_image(image, dest.w, dest.h))
			exit(1);

		SDL_Rect src = image->clip_rect;

		center_image(&dest, &src);

		SDL_BlitSurface(image, &src, screen, &dest);
		SDL_FreeSurface(image);
		rects[i] = dest;
	}

	SDL_Flip(screen);

	for (;;) {
		if (SDL_GetTicks() > (10 * 60 * 1000))
			exit(0);
		SDL_Delay(100);
		handle_events(rects, apps, num);
	}
	return 0;
}

