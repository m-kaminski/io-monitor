
//***********  file io  ************
// open
typedef int (*orig_open_f_type)(const char* pathname, int flags, ...);
typedef int (*orig_open64_f_type)(const char* pathname, int flags, ...);
typedef FILE* (*orig_fopen_f_type)(const char* path, const char* mode);
typedef int (*orig_creat_f_type)(const char* path, mode_t mode);
typedef int (*orig_creat64_f_type)(const char* path, mode_t mode);

// close
typedef int (*orig_close_f_type)(int fd);
typedef int (*orig_fclose_f_type)(FILE* fp);

// write
typedef ssize_t (*orig_write_f_type)(int fd, const void* buf, size_t count);
typedef ssize_t (*orig_send_f_type)(int fd, const void* buf, size_t count, int flags);
typedef ssize_t (*orig_pwrite_f_type)(int fd, const void* buf, size_t count, off_t offset);
typedef ssize_t (*orig_writev_f_type)(int fd, const struct iovec* iov, int iovcnt);
typedef ssize_t (*orig_pwritev_f_type)(int fd, const struct iovec* iov, int iovcnt,
                off_t offset);
typedef int (*orig_fprintf_f_type)(FILE* stream, const char* format, ...);
typedef int (*orig_vfprintf_f_type)(FILE* stream, const char* format, va_list ap);
typedef size_t (*orig_fwrite_f_type)(const void* ptr, size_t size, size_t nmemb, FILE* stream);

// read
typedef ssize_t (*orig_read_f_type)(int fd, void* buf, size_t count);
typedef ssize_t (*orig_recv_f_type)(int fd, void* buf, size_t count, int flags);
typedef ssize_t (*orig_pread_f_type)(int fd, void* buf, size_t count, off_t offset);
typedef ssize_t (*orig_readv_f_type)(int fd, const struct iovec* iov, int iovcnt);
typedef ssize_t (*orig_preadv_f_type)(int fd, const struct iovec* iov, int iovcnt,
               off_t offset);
typedef size_t (*orig_fread_f_type)(void* ptr, size_t size, size_t nmemb, FILE* stream);
typedef int (*orig_fscanf_f_type)(FILE* stream, const char* format, ...);
typedef int (*orig_vfscanf_f_type)(FILE* stream, const char* format, va_list ap);

// sync
typedef int (*orig_fsync_f_type)(int fd);
typedef int (*orig_fdatasync_f_type)(int fd);
typedef void (*orig_sync_f_type)(void);
typedef int (*orig_syncfs_f_type)(int fd);
typedef int (*orig_fflush_f_type)(FILE* fp);


//***********  xattrs  *************
// setxattr
typedef int (*orig_setxattr_f_type)(const char* path,
                                    const char* name,
                                    const void* value,
                                    size_t size,
                                    int flags);
typedef int (*orig_lsetxattr_f_type)(const char* path,
                                     const char* name,
                                     const void* value,
                                     size_t size,
                                     int flags);
typedef int (*orig_fsetxattr_f_type)(int fd,
                                     const char* name,
                                     const void* value,
                                     size_t size,
                                     int flags);

// getxattr
typedef ssize_t (*orig_getxattr_f_type)(const char* path,
                                        const char* name,
                                        void* value,
                                        size_t size);
typedef ssize_t (*orig_lgetxattr_f_type)(const char* path,
                                         const char* name,
                                         void* value,
                                         size_t size);
typedef ssize_t (*orig_fgetxattr_f_type)(int fd,
                                         const char* name,
                                         void* value,
                                         size_t size);

// listxattr
typedef ssize_t (*orig_listxattr_f_type)(const char* path,
                                         char* list,
                                         size_t size);
typedef ssize_t (*orig_llistxattr_f_type)(const char* path,
                                          char* list,
                                          size_t size);
typedef ssize_t (*orig_flistxattr_f_type)(int fd,
                                          char* list,
                                          size_t size);

// removexattr
typedef int (*orig_removexattr_f_type)(const char* path, const char* name);
typedef int (*orig_lremovexattr_f_type)(const char* path, const char* name);
typedef int (*orig_fremovexattr_f_type)(int fd, const char* name);

// mount
#ifndef __FreeBSD__
typedef int (*orig_mount_f_type)(const char* source, const char* target,
                 const char* filesystemtype, unsigned long mountflags,
                 const void* data);
typedef int (*orig_umount_f_type)(const char* target);
typedef int (*orig_umount2_f_type)(const char* target, int flags);
#endif

// directory metadata
typedef DIR* (*orig_opendir_f_type)(const char* name);
typedef DIR* (*orig_fdopendir_f_type)(int fd);
typedef int (*orig_closedir_f_type)(DIR* dirp);
typedef struct dirent* (*orig_readdir_f_type)(DIR* dirp);
typedef int (*orig_readdir_r_f_type)(DIR* dirp,
                                     struct dirent* entry,
                                     struct dirent** result);
typedef int (*orig_dirfd_f_type)(DIR* dirp);
typedef void (*orig_rewinddir_f_type)(DIR* dirp);
typedef void (*orig_seekdir_f_type)(DIR* dirp, long loc);
typedef long (*orig_telldir_f_type)(DIR* dirp);
// leaving out scandir for now due to high complexity

// file metadata
typedef int (*orig_fstat_f_type)(int fd, struct stat* buf);
typedef int (*orig_lstat_f_type)(const char* path, struct stat* buf);
typedef int (*orig_stat_f_type)(const char* path, struct stat* buf);

typedef int (*orig_access_f_type)(const char* path, int amode);
typedef int (*orig_faccessat_f_type)(int fd, const char* path, int mode, int flag);

typedef int (*orig_chmod_f_type)(const char* path, mode_t mode);
typedef int (*orig_fchmod_f_type)(int fd, mode_t mode);
typedef int (*orig_fchmodat_f_type)(int fd, const char* path, mode_t mode, int flag);

typedef int (*orig_chown_f_type)(const char* path, uid_t owner, gid_t group);
typedef int (*orig_fchown_f_type)(int fildes, uid_t owner, gid_t group);
typedef int (*orig_lchown_f_type)(const char* path, uid_t owner, gid_t group);
typedef int (*orig_fchownat_f_type)(int fd, const char* path, uid_t owner,
                                    gid_t group, int flag);

typedef int (*orig_utime_f_type)(const char* path, const struct utimbuf* times);

// allocate
typedef int (*orig_posix_fallocate_f_type)(int fd, off_t offset, off_t len);
typedef int (*orig_fallocate_f_type)(int fd, int mode, off_t offset, off_t len);

// truncate
typedef int (*orig_truncate_f_type)(const char* path, off_t length);
typedef int (*orig_ftruncate_f_type)(int fd, off_t length);

// network
typedef int (*orig_connect_f_type)(int socket, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*orig_accept_f_type)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*orig_bind_f_type)(int socket, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*orig_listen_f_type)(int sockfd, int backlog);
typedef int (*orig_socket_f_type)(int domain, int type, int protocol);
