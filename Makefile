
CFLAGS = -std=c99 -D_GNU_SOURCE=1 -Wall -W -O3 $(shell sdl-config --cflags)
LDFLAGS = $(shell sdl-config --libs) -lSDL_image

appchoo: appchoo.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

test: appchoo
	exec `echo -e 'duke.jpg echo duke\nbörek.jpg echo börek\ncat.jpg echo cat\nmirror.png echo mirror\ntachikoma.jpg echo tachikoma' | ./appchoo -t 10 -d 'echo timeout'`

clean:
	rm -f appchoo

