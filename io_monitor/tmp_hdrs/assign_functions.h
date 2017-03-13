void assign_lib_functions()
{
     // open/close
   orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
   orig_open64 = (orig_open64_f_type)dlsym(RTLD_NEXT,"open64");
   orig_fopen = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
   orig_fopen64 = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen64");
   orig_creat = (orig_creat_f_type)dlsym(RTLD_NEXT,"creat");
   orig_creat64 = (orig_creat64_f_type)dlsym(RTLD_NEXT,"creat64");
   orig_close = (orig_close_f_type)dlsym(RTLD_NEXT,"close");
   orig_fclose = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");

   // write
   orig_write = (orig_write_f_type)dlsym(RTLD_NEXT,"write");
   orig_send = (orig_send_f_type)dlsym(RTLD_NEXT,"send");
   orig_pwrite = (orig_pwrite_f_type)dlsym(RTLD_NEXT,"pwrite");
   orig_writev = (orig_writev_f_type)dlsym(RTLD_NEXT,"writev");
   orig_pwritev = (orig_pwritev_f_type)dlsym(RTLD_NEXT,"pwritev");
   orig_fprintf = (orig_fprintf_f_type)dlsym(RTLD_NEXT,"fprintf");
   orig_vfprintf = (orig_vfprintf_f_type)dlsym(RTLD_NEXT,"vfprintf");
   orig_fwrite = (orig_fwrite_f_type)dlsym(RTLD_NEXT,"fwrite");

   // read
   orig_read = (orig_read_f_type)dlsym(RTLD_NEXT,"read");
   orig_recv = (orig_recv_f_type)dlsym(RTLD_NEXT,"recv");
   orig_pread = (orig_pread_f_type)dlsym(RTLD_NEXT,"pread");
   orig_readv = (orig_readv_f_type)dlsym(RTLD_NEXT,"readv");
   orig_preadv = (orig_preadv_f_type)dlsym(RTLD_NEXT,"preadv");
   orig_fread = (orig_fread_f_type)dlsym(RTLD_NEXT,"fread");
   orig_fscanf = (orig_fscanf_f_type)dlsym(RTLD_NEXT,"fscanf");
   orig_vfscanf = (orig_vfscanf_f_type)dlsym(RTLD_NEXT,"vfscanf");

   // sync
   orig_fsync = (orig_fsync_f_type)dlsym(RTLD_NEXT,"fsync");
   orig_fdatasync = (orig_fdatasync_f_type)dlsym(RTLD_NEXT,"fdatasync");
   orig_fflush = (orig_fflush_f_type)dlsym(RTLD_NEXT,"fflush");
   orig_sync = (orig_sync_f_type)dlsym(RTLD_NEXT,"sync");
   orig_syncfs = (orig_syncfs_f_type)dlsym(RTLD_NEXT,"syncfs");

   // xattrs
   orig_setxattr = (orig_setxattr_f_type)dlsym(RTLD_NEXT,"setxattr");
   orig_lsetxattr = (orig_lsetxattr_f_type)dlsym(RTLD_NEXT,"lsetxattr");
   orig_fsetxattr = (orig_fsetxattr_f_type)dlsym(RTLD_NEXT,"fsetxattr");
   orig_getxattr = (orig_getxattr_f_type)dlsym(RTLD_NEXT,"getxattr");
   orig_lgetxattr = (orig_lgetxattr_f_type)dlsym(RTLD_NEXT,"lgetxattr");
   orig_fgetxattr = (orig_fgetxattr_f_type)dlsym(RTLD_NEXT,"fgetxattr");
   orig_listxattr = (orig_listxattr_f_type)dlsym(RTLD_NEXT,"listxattr");
   orig_llistxattr = (orig_llistxattr_f_type)dlsym(RTLD_NEXT,"llistxattr");
   orig_flistxattr = (orig_flistxattr_f_type)dlsym(RTLD_NEXT,"flistxattr");
   orig_removexattr = (orig_removexattr_f_type)dlsym(RTLD_NEXT,"removexattr");
   orig_lremovexattr = (orig_lremovexattr_f_type)dlsym(RTLD_NEXT,"lremovexattr");
   orig_fremovexattr = (orig_fremovexattr_f_type)dlsym(RTLD_NEXT,"fremovexattr");

   // mount/umount
#ifndef __FreeBSD__
   orig_mount = (orig_mount_f_type)dlsym(RTLD_NEXT,"mount");
   orig_umount = (orig_umount_f_type)dlsym(RTLD_NEXT,"umount");
   orig_umount2 = (orig_umount2_f_type)dlsym(RTLD_NEXT,"umount2");
#endif

   // directory metadata
   orig_opendir = (orig_opendir_f_type)dlsym(RTLD_NEXT,"opendir");
   orig_fdopendir = (orig_fdopendir_f_type)dlsym(RTLD_NEXT,"fdopendir");
   orig_closedir = (orig_closedir_f_type)dlsym(RTLD_NEXT,"closedir");
   orig_readdir = (orig_readdir_f_type)dlsym(RTLD_NEXT,"readdir");
   orig_readdir_r = (orig_readdir_r_f_type)dlsym(RTLD_NEXT,"readdir_r");
   orig_dirfd = (orig_dirfd_f_type)dlsym(RTLD_NEXT,"dirfd");
   orig_rewinddir = (orig_rewinddir_f_type)dlsym(RTLD_NEXT,"rewinddir");
   orig_seekdir = (orig_seekdir_f_type)dlsym(RTLD_NEXT,"seekdir");
   orig_telldir = (orig_telldir_f_type)dlsym(RTLD_NEXT,"telldir");

   // file metadata
   orig_fstat = (orig_fstat_f_type)dlsym(RTLD_NEXT,"fstat");
   orig_lstat = (orig_lstat_f_type)dlsym(RTLD_NEXT,"lstat");
   orig_stat = (orig_stat_f_type)dlsym(RTLD_NEXT,"stat");
   orig_access = (orig_access_f_type)dlsym(RTLD_NEXT,"access");
   orig_faccessat = (orig_faccessat_f_type)dlsym(RTLD_NEXT,"faccessat");
   orig_chmod = (orig_chmod_f_type)dlsym(RTLD_NEXT,"chmod");
   orig_fchmod = (orig_fchmod_f_type)dlsym(RTLD_NEXT,"fchmod");
   orig_fchmodat = (orig_fchmodat_f_type)dlsym(RTLD_NEXT,"fchmodat");
   orig_chown = (orig_chown_f_type)dlsym(RTLD_NEXT,"chown");
   orig_fchown = (orig_fchown_f_type)dlsym(RTLD_NEXT,"fchown");
   orig_lchown = (orig_lchown_f_type)dlsym(RTLD_NEXT,"lchown");
   orig_fchownat = (orig_fchownat_f_type)dlsym(RTLD_NEXT,"fchownat");
   orig_utime = (orig_utime_f_type)dlsym(RTLD_NEXT,"utime");

   // allocate
   orig_posix_fallocate = (orig_posix_fallocate_f_type)dlsym(RTLD_NEXT,"posix_fallocate");
   orig_fallocate = (orig_fallocate_f_type)dlsym(RTLD_NEXT,"fallocate");

   // truncate
   orig_truncate = (orig_truncate_f_type)dlsym(RTLD_NEXT,"truncate");
   orig_ftruncate = (orig_ftruncate_f_type)dlsym(RTLD_NEXT,"ftruncate");

   // network
   orig_connect = (orig_connect_f_type)dlsym(RTLD_NEXT,"connect");
   orig_accept = (orig_accept_f_type)dlsym(RTLD_NEXT,"accept");
   orig_bind = (orig_bind_f_type)dlsym(RTLD_NEXT,"bind");
   orig_listen = (orig_listen_f_type)dlsym(RTLD_NEXT,"listen");
   orig_socket = (orig_socket_f_type)dlsym(RTLD_NEXT,"socket");
}
