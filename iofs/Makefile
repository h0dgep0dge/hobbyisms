build: clean
	gcc -Wall iofs.c `pkg-config fuse --cflags --libs` -o iofs.out

test: build
	./iofs.out mnt

clean:
	-fusermount -u mnt
	-rm iofs.out
