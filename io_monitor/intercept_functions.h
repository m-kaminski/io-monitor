



ssize_t write(int fd, const void* buf, size_t count)
{
   CHECK_LOADED_FNS()
   PUTS("write")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_written = orig_write(fd, buf, count);
   GET_END_TIME()

   if (bytes_written < 0) {
      error_code = errno;
   }

   record(FILE_WRITE, WRITE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_written);
   check_for_http(FILE_WRITE, fd, buf, count, TIME_BEFORE(), TIME_AFTER());
   return bytes_written;
}

//*****************************************************************************

ssize_t send(int fd, const void* buf, size_t count, int flags)
{
   CHECK_LOADED_FNS()
   PUTS("send")
   DECL_VARS()
   GET_START_TIME()
     const ssize_t bytes_written = orig_send(fd, buf, count, flags);
   GET_END_TIME()

   if (bytes_written < 0) {
      error_code = errno;
   }

   record(FILE_WRITE, WRITE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_written);
   check_for_http(FILE_WRITE, fd, buf, count, TIME_BEFORE(), TIME_AFTER());
   return bytes_written;
}

//*****************************************************************************


ssize_t pwrite(int fd, const void* buf, size_t count, off_t offset)
{
   CHECK_LOADED_FNS()
   PUTS("pwrite")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_written = orig_pwrite(fd, buf, count, offset);
   GET_END_TIME()

   if (bytes_written < 0) {
      error_code = errno;
   }

   record(FILE_WRITE, WRITE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_written);

   return bytes_written;
}

//*****************************************************************************

ssize_t writev(int fd, const struct iovec* iov, int iovcnt)
{
   CHECK_LOADED_FNS()
   PUTS("writev")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_written = orig_writev(fd, iov, iovcnt);
   GET_END_TIME()

   if (bytes_written < 0) {
      error_code = errno;
   }

   record(FILE_WRITE, WRITE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_written);

   return bytes_written;
}

//*****************************************************************************

ssize_t pwritev(int fd, const struct iovec* iov, int iovcnt, off_t offset)
{
   CHECK_LOADED_FNS()
   PUTS("pwritev")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_written = orig_pwritev(fd, iov, iovcnt, offset);
   GET_END_TIME()

   if (bytes_written < 0) {
      error_code = errno;
   }

   record(FILE_WRITE, WRITE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_written);

   return bytes_written;
}

//*****************************************************************************

int fprintf(FILE* stream, const char* format, ...)
{
   CHECK_LOADED_FNS()
   PUTS("fprintf")
   DECL_VARS()
   GET_START_TIME()
   va_list args;
   va_start(args, format);
   const ssize_t bytes_written = orig_vfprintf(stream, format, args);
   va_end(args);
   GET_END_TIME()

   ssize_t record_bytes_written;

   if (bytes_written >= 0) {
      error_code = 0;
      record_bytes_written = bytes_written;
   } else {
      error_code = -1;
      record_bytes_written = 0;
   }

   record(FILE_WRITE, WRITE, fileno(stream), NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, record_bytes_written);

   return bytes_written;
}

//*****************************************************************************

int vfprintf(FILE* stream, const char* format, va_list ap)
{
   CHECK_LOADED_FNS()
   PUTS("vfprintf")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_written = orig_vfprintf(stream, format, ap);
   GET_END_TIME()

   ssize_t record_bytes_written;

   if (bytes_written >= 0) {
      error_code = 0;
      record_bytes_written = bytes_written;
   } else {
      error_code = -1;
      record_bytes_written = 0;
   }

   record(FILE_WRITE, WRITE, fileno(stream), NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, record_bytes_written);

   return bytes_written;
}

//*****************************************************************************

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
   CHECK_LOADED_FNS()
   PUTS("fwrite")
   DECL_VARS()
   GET_START_TIME()
   const size_t rc = orig_fwrite(ptr, size, nmemb, stream);
   GET_END_TIME()

   if (rc < nmemb) {
      error_code = 1;
   }

   // our recording of 0 bytes here is not accurate, however we don't
   // have an easy way of knowing how many bytes were converted.
   record(FILE_WRITE, WRITE, fileno(stream), NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, rc * nmemb);

   return rc;
}

//*****************************************************************************

ssize_t read(int fd, void* buf, size_t count)
{
   CHECK_LOADED_FNS()
   PUTS("read")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_read = orig_read(fd, buf, count);
   GET_END_TIME()

   if (bytes_read < 0) {
      error_code = errno;
   }

   record(FILE_READ, READ, fd, NULL, NULL,
         TIME_BEFORE(), TIME_AFTER(), error_code, bytes_read);
   check_for_http(FILE_READ, fd, buf, count, TIME_BEFORE(), TIME_AFTER());
   
   return bytes_read;
}

//*****************************************************************************

ssize_t recv(int fd, void* buf, size_t count, int flags)
{
   CHECK_LOADED_FNS()
   PUTS("recv")
   DECL_VARS()
   GET_START_TIME()
     const ssize_t bytes_recv = orig_recv(fd, buf, count, flags);
   GET_END_TIME()

   if (bytes_recv < 0) {
      error_code = errno;
   }

   record(FILE_READ, READ, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_recv);
   check_for_http(FILE_READ, fd, buf, count, TIME_BEFORE(), TIME_AFTER());
   return bytes_recv;
}

//*****************************************************************************

ssize_t pread(int fd, void* buf, size_t count, off_t offset)
{
   CHECK_LOADED_FNS()
   PUTS("pread")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_read = orig_pread(fd, buf, count, offset);
   GET_END_TIME()

   if (bytes_read < 0) {
      error_code = errno;
   }

   record(FILE_READ, READ, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_read);

   return bytes_read;
}

//*****************************************************************************

ssize_t readv(int fd, const struct iovec* iov, int iovcnt)
{
   CHECK_LOADED_FNS()
   PUTS("readv")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_read = orig_readv(fd, iov, iovcnt);
   GET_END_TIME()

   if (bytes_read < 0) {
      error_code = errno;
   }

   record(FILE_READ, READ, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_read);

   return bytes_read;
}

//*****************************************************************************

ssize_t preadv(int fd, const struct iovec* iov, int iovcnt, off_t offset)
{
   CHECK_LOADED_FNS()
   PUTS("preadv")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_read = orig_preadv(fd, iov, iovcnt, offset);
   GET_END_TIME()

   if (bytes_read < 0) {
      error_code = errno;
   }

   record(FILE_READ, READ, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_read);

   return bytes_read;
}

//*****************************************************************************

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
   CHECK_LOADED_FNS()
   PUTS("fread")
   DECL_VARS()
   GET_START_TIME()
   const size_t items_read = orig_fread(ptr, size, nmemb, stream);
   GET_END_TIME()

   if (items_read < nmemb) {
      if (ferror(stream)) {
         error_code = 1;
      }
   }

   record(FILE_READ, READ, fileno(stream), NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, items_read * size);

   return items_read;
}

//*****************************************************************************

int fscanf(FILE* stream, const char* format, ...)
{
   CHECK_LOADED_FNS()
   PUTS("fscanf")
   DECL_VARS()
   GET_START_TIME()
   va_list args;
   va_start(args, format);
   const int rc = orig_vfscanf(stream, format, args);
   va_end(args);
   GET_END_TIME()

   if (rc == EOF) {
      error_code = errno;
   }

   // our recording of 0 bytes here is not accurate, however we don't
   // have an easy way of knowing how many bytes were converted.
   record(FILE_READ, READ, fileno(stream), NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int vfscanf(FILE* stream, const char* format, va_list ap)
{
   CHECK_LOADED_FNS()
   PUTS("vfscanf")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_vfscanf(stream, format, ap);
   GET_END_TIME()

   if (rc == EOF) {
      error_code = errno;
   }

   // our recording of 0 bytes here is not accurate, however we don't
   // have an easy way of knowing how many bytes were converted.
   record(FILE_READ, READ, fileno(stream), NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int fsync(int fd)
{
   CHECK_LOADED_FNS()
   PUTS("fsync")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fsync(fd);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(SYNCS, SYNC, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int fdatasync(int fd)
{
   CHECK_LOADED_FNS()
   PUTS("fdatasync")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fdatasync(fd);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(SYNCS, SYNC, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

void sync()
{
   CHECK_LOADED_FNS()
   PUTS("sync")
   DECL_VARS()
   GET_START_TIME()
   sync();
   GET_END_TIME()
   record(SYNCS, SYNC, FD_NONE, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), 0, ZERO_BYTES);
}

//*****************************************************************************

int syncfs(int fd)
{
   CHECK_LOADED_FNS()
   PUTS("syncfs")
   DECL_VARS()
   GET_START_TIME()
   const int rc = syncfs(fd);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(SYNCS, SYNC, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int setxattr(const char* path,
             const char* name,
             const void* value,
             size_t size,
             int flags)
{
   CHECK_LOADED_FNS()
   PUTS("setxattr")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_setxattr(path, name, value, size, flags);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(XATTRS, SETXATTR, FD_NONE, path, name,
          TIME_BEFORE(), TIME_AFTER(), error_code, size);

   return rc;
}

//*****************************************************************************

int lsetxattr(const char* path,
              const char* name,
              const void* value,
              size_t size,
              int flags)
{
   CHECK_LOADED_FNS()
   PUTS("lsetxattr")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_lsetxattr(path, name, value, size, flags);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(XATTRS, SETXATTR, FD_NONE, path, name,
          TIME_BEFORE(), TIME_AFTER(), error_code, size);

   return rc;
}

//*****************************************************************************

int fsetxattr(int fd,
              const char* name,
              const void* value,
              size_t size,
              int flags)
{
   CHECK_LOADED_FNS()
   PUTS("fsetxattr")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fsetxattr(fd, name, value, size, flags);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(XATTRS, SETXATTR, fd, name, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, size);

   return rc;
}

//*****************************************************************************

ssize_t getxattr(const char* path, const char* name, void* value, size_t size)
{
   CHECK_LOADED_FNS()
   PUTS("getxattr")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_read = orig_getxattr(path, name, value, size);
   GET_END_TIME()
   ssize_t recorded_bytes_read = bytes_read;

   if (bytes_read == -1) {
      error_code = errno;
      recorded_bytes_read = 0;
   }

   record(XATTRS, GETXATTR, FD_NONE, path, name,
          TIME_BEFORE(), TIME_AFTER(), error_code, recorded_bytes_read);

   return bytes_read;
}

//*****************************************************************************

ssize_t lgetxattr(const char* path, const char* name, void* value, size_t size)
{
   CHECK_LOADED_FNS()
   PUTS("lgetxattr")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_read = orig_lgetxattr(path, name, value, size);
   GET_END_TIME()

   ssize_t recorded_bytes_read = bytes_read;

   if (bytes_read == -1) {
      error_code = errno;
      recorded_bytes_read = 0;
   }

   record(XATTRS, GETXATTR, FD_NONE, path, name,
          TIME_BEFORE(), TIME_AFTER(), error_code, recorded_bytes_read);

   return bytes_read;
}

//*****************************************************************************

ssize_t fgetxattr(int fd, const char* name, void* value, size_t size)
{
   CHECK_LOADED_FNS()
   PUTS("fgetxattr")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t bytes_read = orig_fgetxattr(fd, name, value, size);
   GET_END_TIME()

   ssize_t recorded_bytes_read = bytes_read;

   if (bytes_read == -1) {
      error_code = errno;
      recorded_bytes_read = 0;
   }

   record(XATTRS, GETXATTR, fd, name, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, recorded_bytes_read);

   return bytes_read;
}

//*****************************************************************************

ssize_t listxattr(const char* path, char* list, size_t size)
{
   CHECK_LOADED_FNS()
   PUTS("listxattr")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t list_size = orig_listxattr(path, list, size);
   GET_END_TIME()

   if (list_size < 0) {
      error_code = errno;
   }

   record(XATTRS, LISTXATTR, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return list_size;
}

//*****************************************************************************

ssize_t llistxattr(const char* path, char* list, size_t size)
{
   CHECK_LOADED_FNS()
   PUTS("llistxattr")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t list_size = orig_llistxattr(path, list, size);
   GET_END_TIME()

   if (list_size < 0) {
      error_code = errno;
   }

   record(XATTRS, LISTXATTR, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return list_size;
}

//*****************************************************************************

ssize_t flistxattr(int fd, char* list, size_t size)
{
   CHECK_LOADED_FNS()
   PUTS("flistxattr")
   DECL_VARS()
   GET_START_TIME()
   const ssize_t list_size = orig_flistxattr(fd, list, size);
   GET_END_TIME()

   if (list_size < 0) {
      error_code = errno;
   }

   record(XATTRS, LISTXATTR, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return list_size;
}

//*****************************************************************************

int removexattr(const char* path, const char* name)
{
   CHECK_LOADED_FNS()
   PUTS("removexattr")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_removexattr(path, name);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(XATTRS, REMOVEXATTR, FD_NONE, path, name,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int lremovexattr(const char* path, const char* name)
{
   CHECK_LOADED_FNS()
   PUTS("lremovexattr")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_lremovexattr(path, name);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(XATTRS, REMOVEXATTR, FD_NONE, path, name,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int fremovexattr(int fd, const char* name)
{
   CHECK_LOADED_FNS()
   PUTS("fremovexattr")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fremovexattr(fd, name);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(XATTRS, REMOVEXATTR, fd, name, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

#ifndef __FreeBSD__
int mount(const char* source, const char* target,
          const char* filesystemtype, unsigned long mountflags,
          const void* data)
{
   CHECK_LOADED_FNS()
   PUTS("mount")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_mount(source, target, filesystemtype, mountflags, data);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_SYSTEMS, MOUNT, FD_NONE, source, target,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}
#endif
//*****************************************************************************

int umount(const char* target)
{
   CHECK_LOADED_FNS()
   PUTS("umount")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_umount(target);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_SYSTEMS, UMOUNT, FD_NONE, target, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int umount2(const char* target, int flags)
{
   CHECK_LOADED_FNS()
   PUTS("umount2")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_umount2(target, flags);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_SYSTEMS, UMOUNT, FD_NONE, target, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

FILE* fopen(const char* path, const char* mode)
{
   CHECK_LOADED_FNS()
   PUTS("fopen")
   DECL_VARS()
   GET_START_TIME()
   FILE* rc = orig_fopen(path, mode);
   GET_END_TIME()
   int fd;

   if (rc == NULL) {
      error_code = errno;
      fd = FD_NONE;
   } else {
      fd = fileno(rc);
   }

   char* real_path = realpath(path, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, mode,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return rc;
}

//*****************************************************************************

FILE* fopen64(const char* path, const char* mode)
{
   CHECK_LOADED_FNS()
   PUTS("fopen64")
   DECL_VARS()
   GET_START_TIME()
   FILE* rc = orig_fopen64(path, mode);
   GET_END_TIME()

   if (rc == NULL) {
      error_code = errno;
   }

   char* real_path = realpath(path, NULL);
   const char* record_path;
   if (real_path != NULL) {
      record_path = real_path;
   } else {
      record_path = path;
   }

   int fd = FD_NONE;
   if (rc != NULL) {
      fd = fileno(rc);
   }

   record(FILE_OPEN_CLOSE, OPEN, fd, record_path, mode,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   if (real_path != NULL) {
      free(real_path);
   }

   return rc;
}

//*****************************************************************************

FILE* _IO_new_fopen(const char* path, const char* mode)
{
   CHECK_LOADED_FNS()
   PUTS("_IO_new_fopen")
   DECL_VARS()
   GET_START_TIME()
   FILE* rc = orig_fopen(path, mode);
   GET_END_TIME()
   int fd;

   if (rc == NULL) {
      error_code = errno;
      fd = FD_NONE;
   } else {
      fd = fileno(rc);
   }

   char* real_path = realpath(path, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, mode,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return rc;
}

//*****************************************************************************

int fflush(FILE* fp)
{
   CHECK_LOADED_FNS()
   PUTS("fflush")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fflush(fp);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(SYNCS, FLUSH, fileno(fp), NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

DIR* opendir(const char *name)
{
   CHECK_LOADED_FNS()
   PUTS("opendir")
   DECL_VARS()
   GET_START_TIME()
   DIR* rc = orig_opendir(name);
   GET_END_TIME()

   if (rc == NULL) {
      error_code = errno;
   }

   record(DIR_METADATA, OPENDIR, FD_NONE, name, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

DIR* fdopendir(int fd)
{
   CHECK_LOADED_FNS()
   PUTS("fdopendir")
   DECL_VARS()
   GET_START_TIME()
   DIR* rc = orig_fdopendir(fd);
   GET_END_TIME()

   if (rc == NULL) {
      error_code = errno;
   }

   record(DIR_METADATA, OPENDIR, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int closedir(DIR* dirp)
{
   CHECK_LOADED_FNS()
   PUTS("closedir")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_closedir(dirp);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(DIR_METADATA, CLOSEDIR, FD_NONE, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

struct dirent* readdir(DIR* dirp)
{
   CHECK_LOADED_FNS()
   PUTS("readdir")
   DECL_VARS()
   GET_START_TIME()
   struct dirent* rc = orig_readdir(dirp);
   GET_END_TIME()

   if (rc == NULL) {
      error_code = errno;
   }

   record(DIR_METADATA, READDIR, FD_NONE, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int readdir_r(DIR* dirp, struct dirent* entry, struct dirent** result)
{
   CHECK_LOADED_FNS()
   PUTS("readdir_r")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_readdir_r(dirp, entry, result);
   GET_END_TIME()

   record(DIR_METADATA, READDIR, FD_NONE, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), rc, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int dirfd(DIR* dirp)
{
   CHECK_LOADED_FNS()
   PUTS("dirfd")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_dirfd(dirp);
   GET_END_TIME()

   if (rc < 0) {
      error_code = errno;
   }

   record(DIR_METADATA, DIRFD, FD_NONE, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

void rewinddir(DIR* dirp)
{
   CHECK_LOADED_FNS()
   PUTS("rewinddir")
   DECL_VARS()
   GET_START_TIME()
   orig_rewinddir(dirp);
   GET_END_TIME()
   record(DIR_METADATA, REWINDDIR, FD_NONE, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), 0, ZERO_BYTES);
}

//*****************************************************************************

void seekdir(DIR* dirp, long loc)
{
   CHECK_LOADED_FNS()
   PUTS("seekdir")
   DECL_VARS()
   GET_START_TIME()
   orig_seekdir(dirp, loc);
   GET_END_TIME()
   record(DIR_METADATA, SEEKDIR, FD_NONE, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), 0, ZERO_BYTES);
}

//*****************************************************************************

long telldir(DIR* dirp)
{
   CHECK_LOADED_FNS()
   PUTS("telldir")
   DECL_VARS()
   GET_START_TIME()
   const long loc = orig_telldir(dirp);
   GET_END_TIME()

   if (loc < 0L) {
      error_code = errno;
   }

   record(DIR_METADATA, TELLDIR, FD_NONE, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return loc;
}

//*****************************************************************************

int fstat(int fildes, struct stat* buf)
{
   CHECK_LOADED_FNS()
   PUTS("fstat")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fstat(fildes, buf);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, STAT, fildes, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int lstat(const char* path, struct stat* buf)
{
   CHECK_LOADED_FNS()
   PUTS("lstat")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_lstat(path, buf);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, STAT, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int stat(const char* path, struct stat* buf)
{
   CHECK_LOADED_FNS()
   PUTS("stat")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_stat(path, buf);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, STAT, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int access(const char* path, int amode)
{
   CHECK_LOADED_FNS()
   PUTS("access")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_access(path, amode);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, ACCESS, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int faccessat(int fd, const char* path, int mode, int flag)
{
   CHECK_LOADED_FNS()
   PUTS("faccessat")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_faccessat(fd, path, mode, flag);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, ACCESS, fd, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int chmod(const char* path, mode_t mode)
{
   CHECK_LOADED_FNS()
   PUTS("chmod")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_chmod(path, mode);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, CHMOD, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int fchmod(int fildes, mode_t mode)
{
   CHECK_LOADED_FNS()
   PUTS("fchmod")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fchmod(fildes, mode);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, CHMOD, fildes, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int fchmodat(int fd, const char* path, mode_t mode, int flag)
{
   CHECK_LOADED_FNS()
   PUTS("fchmodat")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fchmodat(fd, path, mode, flag);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, CHMOD, fd, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int chown(const char* path, uid_t owner, gid_t group)
{
   CHECK_LOADED_FNS()
   PUTS("chown")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_chown(path, owner, group);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, CHOWN, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int fchown(int fildes, uid_t owner, gid_t group)
{  
   CHECK_LOADED_FNS()
   PUTS("fchown")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fchown(fildes, owner, group);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, CHOWN, fildes, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int lchown(const char* path, uid_t owner, gid_t group)
{  
   CHECK_LOADED_FNS()
   PUTS("lchown")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_lchown(path, owner, group);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, CHOWN, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int fchownat(int fd, const char* path, uid_t owner, gid_t group, int flag)
{  
   CHECK_LOADED_FNS()
   PUTS("fchownat")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fchownat(fd, path, owner, group, flag);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, CHOWN, fd, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int utime(const char* path, const struct utimbuf* times)
{
   CHECK_LOADED_FNS()
   PUTS("utime")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_utime(path, times);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_METADATA, UTIME, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int posix_fallocate(int fd, off_t offset, off_t len)
{
   CHECK_LOADED_FNS()
   PUTS("posix_fallocate")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_posix_fallocate(fd, offset, len);
   GET_END_TIME()
   ssize_t bytes_written;

   // according to man page, errno is NOT set on error!
   if (rc == 0) {
      bytes_written = len;
   } else {
      bytes_written = ZERO_BYTES;
   }

   record(FILE_SPACE, ALLOCATE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), rc, bytes_written);

   return rc;
}

//*****************************************************************************

int fallocate(int fd, int mode, off_t offset, off_t len)
{
   CHECK_LOADED_FNS()
   PUTS("ftruncate")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_fallocate(fd, mode, offset, len);
   GET_END_TIME()
   ssize_t bytes_written;

   if (rc == 0) {
      error_code = 0;
      bytes_written = len;
   } else {
      error_code = errno;
      bytes_written = ZERO_BYTES;
   }

   record(FILE_SPACE, ALLOCATE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_written);

   return rc;
}

//*****************************************************************************

int truncate(const char* path, off_t length)
{
   CHECK_LOADED_FNS()
   PUTS("truncate")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_truncate(path, length);
   GET_END_TIME()
   ssize_t bytes_written;

   if (rc == 0) {
      error_code = 0;
      bytes_written = length;
   } else {
      error_code = errno;
      bytes_written = ZERO_BYTES;
   }

   record(FILE_SPACE, TRUNCATE, FD_NONE, path, NULL,
          TIME_BEFORE(), TIME_AFTER(), rc, bytes_written);

   return rc;
}

//*****************************************************************************

int ftruncate(int fd, off_t length)
{
   CHECK_LOADED_FNS()
   PUTS("ftruncate")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_ftruncate(fd, length);
   GET_END_TIME() 
   ssize_t bytes_written;

   if (rc == 0) {
      error_code = 0;
      bytes_written = length;
   } else {
      error_code = errno;
      bytes_written = ZERO_BYTES;
   }

   record(FILE_SPACE, TRUNCATE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, bytes_written);

   return rc;
}

//*****************************************************************************
int connect(int socket, const struct sockaddr *addr, socklen_t addrlen)
{
   CHECK_LOADED_FNS()
   PUTS("connect")
   DECL_VARS()
   GET_START_TIME()
     const int ret = orig_connect(socket, addr, addrlen);
   GET_END_TIME();

   const int fd = socket;

   if (ret == -1) {
      error_code = errno;
   }

   char *real_path = real_ip(addr, NULL);
   if (!real_path) {
     return ret;
   }
   record(SOCKETS, CONNECT, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return ret;
}

//*****************************************************************************

//int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
//int listen(int sockfd, int backlog)
int socket(int domain, int type, int protocol);

int bind(int sockfd, const struct sockaddr *addr,
	 socklen_t addrlen)
{
   CHECK_LOADED_FNS()
   PUTS("bind")
   DECL_VARS()
   GET_START_TIME()
     const int ret = orig_bind(sockfd, addr, addrlen);
   GET_END_TIME();

   const int fd = sockfd;

   if (ret == -1) {
      error_code = errno;
   }

   char *real_path = real_ip(addr, NULL);
   if (!real_path) {
     return ret;
   }
   record(SOCKETS, BIND, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return ret;
  // handle bind (command after socket and before accept

}

//*****************************************************************************

int open(const char* pathname, int flags, ...)
{
   CHECK_LOADED_FNS()
   PUTS("open")
   DECL_VARS()
   GET_START_TIME()
   const int fd = orig_open(pathname, flags);
   GET_END_TIME()

   if (fd == -1) {
      error_code = errno;
   }

   char* real_path = realpath(pathname, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return fd;
}

//*****************************************************************************

int open64(const char* pathname, int flags, ...)
{
   CHECK_LOADED_FNS()
   PUTS("open64")
   DECL_VARS()
   GET_START_TIME()
   const int fd = orig_open64(pathname, flags);
   GET_END_TIME()

   if (fd == -1) {
      error_code = errno;
   }

   char* real_path = realpath(pathname, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return fd;
}

//*****************************************************************************

int creat(const char* pathname, mode_t mode)
{
   CHECK_LOADED_FNS()
   PUTS("creat")
   DECL_VARS()
   GET_START_TIME()
   const int fd = orig_creat(pathname, mode);
   GET_END_TIME()

   if (fd == -1) {
      error_code = errno;
   }

   char* real_path = realpath(pathname, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return fd;
}

//*****************************************************************************

int creat64(const char* pathname, mode_t mode)
{
   CHECK_LOADED_FNS()
   PUTS("creat64")
   DECL_VARS()
   GET_START_TIME()
   const int fd = orig_creat64(pathname, mode);
   GET_END_TIME()

   if (fd == -1) {
      error_code = errno;
   }

   char* real_path = realpath(pathname, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return fd;
}

//*****************************************************************************

int close(int fd)
{
   CHECK_LOADED_FNS()
   PUTS("close")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_close(fd);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_OPEN_CLOSE, CLOSE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int fclose(FILE* fp)
{
   CHECK_LOADED_FNS()
   PUTS("fclose")
   DECL_VARS()
   GET_START_TIME()
   const int fd = fileno(fp);
   const int rc = orig_fclose(fp);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_OPEN_CLOSE, CLOSE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}



