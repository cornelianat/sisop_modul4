#define FUSE_USE_VERSION 28
#include<fuse.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<errno.h>
#include<sys/time.h>

static const char *dirpath = "/home/tasha/Documents";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
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
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
 	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  	int fd = 0 ;
	char ext[100];
	int len=strlen(fpath);
	int i,j,k;
	for(i=0;i<len;i++){
		if(fpath[i]=='.'){
			k=0;
			for(j=i;j<len;j++){
				ext[k]=fpath[j];
				k++;
			}
		}
	}
	if(strcmp(ext,".doc")==0||strcmp(ext,".pdf")==0||strcmp(ext,".txt")==0){		
		char old[1000],newName[1000];
		char err[1000];
		sprintf(old,"%s",fpath);
		sprintf(newName,"%s.ditandai",fpath);
		rename(old,newName);
		sprintf(err,"chmod 000 %s.ditandai",fpath);
		system(err);
		system("zenity --error --text=\"Terjadi Kesalahan! File berisi konten berbahaya.\n\"");	
		
		if ((chdir("/home/tasha/modulmpat/rahasia/")) < 0)
		    {
			mkdir("/home/tasha/modulmpat/rahasia",0777);
		    }
		char msg[1000];
		sprintf(msg,"mv /home/tasha/Documents/*.ditandai /home/tasha/modulmpat/rahasia");
		system(msg);
			
		return -errno;
	}
	else{
		(void) fi;
		fd = open(fpath, O_RDONLY);
		if (fd == -1) return -errno;
		res = pread(fd, buf, size, offset);
		if (res == -1) res = -errno;
		close(fd);
		return res;
	}
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
