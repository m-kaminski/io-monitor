// open/close
static orig_open_f_type orig_open = NULL;
static orig_open64_f_type orig_open64 = NULL;
static orig_fopen_f_type orig_fopen = NULL;
static orig_fopen_f_type orig_fopen64 = NULL;
static orig_creat_f_type orig_creat = NULL;
static orig_creat64_f_type orig_creat64 = NULL;
static orig_close_f_type orig_close = NULL;
static orig_fclose_f_type orig_fclose = NULL;

// writes
static orig_write_f_type orig_write = NULL;
static orig_send_f_type orig_send = NULL;
static orig_pwrite_f_type orig_pwrite = NULL;
static orig_writev_f_type orig_writev = NULL;
static orig_pwritev_f_type orig_pwritev = NULL;
static orig_fprintf_f_type orig_fprintf = NULL;
static orig_vfprintf_f_type orig_vfprintf = NULL;
static orig_fwrite_f_type orig_fwrite = NULL;

// reads
static orig_read_f_type orig_read = NULL;
static orig_recv_f_type orig_recv = NULL;
static orig_pread_f_type orig_pread = NULL;
static orig_readv_f_type orig_readv = NULL;
static orig_preadv_f_type orig_preadv = NULL;
static orig_fread_f_type orig_fread = NULL;
static orig_fscanf_f_type orig_fscanf = NULL;
static orig_vfscanf_f_type orig_vfscanf = NULL;

// sync/flush
static orig_fsync_f_type orig_fsync = NULL;
static orig_fdatasync_f_type orig_fdatasync = NULL;
static orig_sync_f_type orig_sync = NULL;
static orig_syncfs_f_type orig_syncfs = NULL;
static orig_fflush_f_type orig_fflush = NULL;

// xattrs
static orig_setxattr_f_type orig_setxattr = NULL;
static orig_lsetxattr_f_type orig_lsetxattr = NULL;
static orig_fsetxattr_f_type orig_fsetxattr = NULL;
static orig_getxattr_f_type orig_getxattr = NULL;
static orig_lgetxattr_f_type orig_lgetxattr = NULL;
static orig_fgetxattr_f_type orig_fgetxattr = NULL;
static orig_listxattr_f_type orig_listxattr = NULL;
static orig_llistxattr_f_type orig_llistxattr = NULL;
static orig_flistxattr_f_type orig_flistxattr = NULL;
static orig_removexattr_f_type orig_removexattr = NULL;
static orig_lremovexattr_f_type orig_lremovexattr = NULL;
static orig_fremovexattr_f_type orig_fremovexattr = NULL;

// filesystem mount/umount
#ifndef __FreeBSD__
static orig_mount_f_type orig_mount = NULL;
static orig_umount_f_type orig_umount = NULL;
static orig_umount2_f_type orig_umount2 = NULL;
#endif

// directory metadata
static orig_opendir_f_type orig_opendir = NULL;
static orig_fdopendir_f_type orig_fdopendir = NULL;
static orig_closedir_f_type orig_closedir = NULL;
static orig_readdir_f_type orig_readdir = NULL;
static orig_readdir_r_f_type orig_readdir_r = NULL;
static orig_dirfd_f_type orig_dirfd = NULL;
static orig_rewinddir_f_type orig_rewinddir = NULL;
static orig_seekdir_f_type orig_seekdir = NULL;
static orig_telldir_f_type orig_telldir = NULL;

// file metadata
static orig_fstat_f_type orig_fstat = NULL;
static orig_lstat_f_type orig_lstat = NULL;
static orig_stat_f_type orig_stat = NULL;
static orig_access_f_type orig_access = NULL;
static orig_faccessat_f_type orig_faccessat = NULL;
static orig_chmod_f_type orig_chmod = NULL;
static orig_fchmod_f_type orig_fchmod = NULL;
static orig_fchmodat_f_type orig_fchmodat = NULL;
static orig_chown_f_type orig_chown = NULL;
static orig_fchown_f_type orig_fchown = NULL;
static orig_lchown_f_type orig_lchown = NULL;
static orig_fchownat_f_type orig_fchownat = NULL;
static orig_utime_f_type orig_utime = NULL;

// allocate
static orig_posix_fallocate_f_type orig_posix_fallocate = NULL;
static orig_fallocate_f_type orig_fallocate = NULL;

// truncate
static orig_truncate_f_type orig_truncate = NULL;
static orig_ftruncate_f_type orig_ftruncate = NULL;

// network
static orig_connect_f_type orig_connect = NULL;
static orig_accept_f_type orig_accept = NULL;
static orig_bind_f_type orig_bind = NULL;
static orig_listen_f_type orig_listen = NULL;
static orig_socket_f_type orig_socket = NULL;
