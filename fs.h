#ifndef __FS__H__
#define __FS__H__

#include "fs_def.h"
#include "device_io.h"

/* 在启动整个文件系统大模块前，必须调用此函数 */
void fs_init(void);

/* 将文件系统挂载到某盘符上 */
enum {
    FS_TYPE_FULFS,
    FS_TYPE_TOTAL
};

bool fs_mount(device_handle_t device, char drive_letter, int fs_type);


/* 模拟文件IO的系统调用 */

#define FS_ERROR -1
#define FS_SUCCESS 0

int fs_open(const char* path);
void fs_close(int fd);

int fs_read(int fd, char* buf, int count);
int fs_write(int fd, const char* buf, int count);
bool fs_ftruncate(int fd, fs_off_t size);
fs_off_t fs_lseek(int fd, fs_off_t off, int where);

int fs_mkdir(const char* path);
int fs_rmdir(const char* path);

int fs_link(const char* src_path, const char* new_path);
int fs_unlink(const char* path);

int fs_symlink(const char* src_path, const char* new_path);
int fs_readlink(const char *path, char *buf, size_t size);

int fs_stat(const char *path, struct fs_stat *buf);



int fs_opendir(fs_dir_t* dir, const char *path);
int fs_readdir(fs_dir_t* dir, char* name);
void fs_closedir(fs_dir_t* dir);




#endif /* __FS__H__ */
