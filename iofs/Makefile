build: clean
	gcc -Wall iofs.c -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse  -pthread -L/lib -lfuse -lrt -ldl -o iofs.out

test: build
	./iofs mnt

clean:
	-fusermount -u mnt
	-rm iofs
