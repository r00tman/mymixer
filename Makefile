all: mymixer

mymixer: main.c
	gcc -o mymixer main.c `pkgconf --cflags --libs libusb-1.0`

clean:
	rm mymixer
