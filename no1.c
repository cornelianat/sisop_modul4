#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/tasha/Documents";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	char newFile[1000];
	if (strcmp(path, "/") != 0) {
		memcpy(newFile, path, strlen(path) - 9);
		newFile[strlen(path) - 9] = '\0';
	} else {
		memcpy(newFile, path, strlen(path));
	}

	sprintf(fpath,"%s%s",dirpath,newFile);
	printf("full path: %s\n", fpath);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	printf("xmp_readdir: %s\n", path);
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		char* newName;
		newName = strcat(de->d_name, ".ditandai");
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		printf("de->d_name: %s\n", de->d_name);
		res = (filler(buf, newName, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	printf("xmp_read: %s", path);
	char fpath[1000];
	char newFile[1000];
	if(strcmp(path,"/") == 0)
	{
		memcpy(newFile, path, strlen(path));
		//path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else {
		memcpy(newFile, path, strlen(path) - 9);
		newFile[strlen(path)-9] = '\0';
		sprintf(fpath, "%s%s",dirpath,newFile);
	}
	int res = 0;
	int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
