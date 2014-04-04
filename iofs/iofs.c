
#define FUSE_USE_VERSION 26
#define OUTPUT 1
#define INPUT 0

#define HIGH 1
#define LOW 0

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int iofs_mode[14] = {OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT,OUTPUT};
int iofs_value[14] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};

static int iofs_getattr(const char *path, struct stat *stbuf) {
	int i;
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	}
	if(strcmp(path, "/mode") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	}
	if(strcmp(path, "/value") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	}
	for(i = 0;i < 14;i++) {
		char filename[20];
		snprintf(filename,20,"/mode/pin_%i",i);
		if(strcmp(path,filename) == 0) {
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = 2;
			return 0;
		}
		snprintf(filename,20,"/value/pin_%i",i);
		if(strcmp(path,filename) == 0) {
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = 2;
			return 0;
		}
	}
	return -ENOENT;
}

static int iofs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	(void) offset;
	(void) fi;
	int i;
	if (strcmp(path, "/") == 0) {
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		filler(buf, "mode", NULL, 0);
		filler(buf, "value", NULL, 0);
	} else if (strcmp(path, "/mode") == 0 || strcmp(path, "/value") == 0) {
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		for(i = 0;i < 14;i++) {
			char filename[10];
			snprintf(filename,10,"pin_%i",i);
			filler(buf,filename,NULL,0);
		}
	} else return -ENOENT;
	return 0;
}

static int iofs_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len,s;
	int i;
	for(i = 0;i < 14;i++) {
		char filename[20];
		char value[5];
		snprintf(filename,20,"/mode/pin_%i",i);
		if(strcmp(path,filename) == 0) {
			snprintf(value,5,"%i\n",iofs_mode[i]);
			len = strlen(value);
			if(offset >= len) return 0;
			s = len-offset;
			memcpy(buf,value+offset,s);
			return s;
		}
		snprintf(filename,20,"/value/pin_%i",i);
		if(strcmp(path,filename) == 0) {
			switch(iofs_mode[i]) {
				case OUTPUT:
					snprintf(value,5,"%i\n",iofs_value[i]);
					len = strlen(value);
					if(offset >= len) return 0;
					s = len-offset;
					if(size > s) memcpy(buf,value+offset,s);
					else memcpy(buf,value+offset,s=size);
					return s;
					break;
				case INPUT:
					// Fetch input over serial
					break;
			}
			return 0;
		}
	}
	return -ENOENT;
}

int iofs_unlink(const char *path) {
	int i;
	for(i = 0;i < 14;i++) {
		char filename[20];
		snprintf(filename,20,"/mode/pin_%i",i);
		if(strcmp(path,filename) == 0) return -EACCES;
		snprintf(filename,20,"/value/pin_%i",i);
		if(strcmp(path,filename) == 0) return -EACCES;
	}
	return -ENOENT;
}

int iofs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
	return -EACCES;
}

int iofs_open(const char *path,struct fuse_file_info *fi) {
	int i;
	for(i = 0;i < 14;i++) {
		char filename[20];
		snprintf(filename,20,"/mode/pin_%i",i);
		if(strcmp(path,filename) == 0) return 0;
		snprintf(filename,20,"/value/pin_%i",i);
		if(strcmp(path,filename) == 0) return 0;
	}
	return -ENOENT;
}

int iofs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	int i,mode;
	if(offset > 0) return size;
	if(size <= 0) return 0;
	if(buf[0] != '0' && buf[0] != '1') return -EINVAL;
	
	for(i = 0;i < 14;i++) {
		char filename[20];
		snprintf(filename,20,"/mode/pin_%i",i);
		mode = 0;
		if(strcmp(path,filename) == 0) {
			
			return size;
		}
		snprintf(filename,20,"/value/pin_%i",i);
		mode = 1;
		if(strcmp(path,filename) == 0) {
			
			return size;
		}
	}
	return -ENOENT;
}

static struct fuse_operations iofs_oper = {
	.getattr	= iofs_getattr,
	.readdir	= iofs_readdir,
	.open		= iofs_open,
	.read		= iofs_read,
	.unlink		= iofs_unlink,
	.create		= iofs_create,
	.write		= iofs_write
};

int main(int argc, char *argv[]) {
	return fuse_main(argc, argv, &iofs_oper, NULL);
}

