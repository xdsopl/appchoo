
CFLAGS = -std=c99 -D_GNU_SOURCE=1 -Wall -W -O3 -ffast-math $(shell sdl-config --cflags)
LDFLAGS = -lm $(shell sdl-config --libs) -lSDL_image

appchoo: appchoo.c

test: appchoo
	exec `echo -e 'duke.jpg echo duke\nbörek.jpg echo börek\ncat.jpg echo cat\nmirror.png echo mirror\ntachikoma.jpg echo tachikoma' | ./appchoo`

clean:
	rm -f appchoo

