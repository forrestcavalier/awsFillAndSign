#if !defined(librock_fdio_H)
#define librock_fdio_H 1
/* This is a compatibility and portability header to help
   write sources that are platform independent.
   
   More info at http://www.mibsoftware.com/
 */
#include <fcntl.h> //O_RDONLY, etc.

#if defined(LIBROCK_WANT_fileSha256Contents)
    const char *librock_fileSha256Contents(const char *fname, unsigned char *mdBuffer32, unsigned long *contentLength);
#endif
#if defined(LIBROCK_WANT_fileGetContents)
    void *librock_fileGetContents(const char *fileName, off_t *returnLength);
#endif

#if defined __MINGW32__  && defined __MSVCRT__
#	include <io.h>		// _sopen_s
# if !defined LIBROCK_FDIO_DEFINED
#	define LIBROCK_FDIO_DEFINED 1
#	define librock_fdOpenReadOnly(fname) open(fname, O_RDONLY|O_BINARY)
#	define librock_fdSeek _lseek
#	define librock_fdRead _read
#	define librock_fdClose _close
# endif
#elif defined _MSC_VER
# if !defined LIBROCK_FDIO_DEFINED
#	define LIBROCK_FDIO_DEFINED 1
#	include <io.h>		// _sopen_s
#	include <share.h>		// SH_DENYNO
	int librock_fdOpenReadOnly(const char *fname)
	{
			int fd;
			_sopen_s(&fd, fname, _O_RDONLY|_O_BINARY, _SH_DENYNO, 0/*ignored when not O_CREAT */);
			return fd;	
	}
#	define librock_fdSeek _lseek
#	define librock_fdRead _read
#	define librock_fdClose _close
# endif
#else
# if !defined LIBROCK_FDIO_DEFINED
#	include <unistd.h> //open, etc.
#	define LIBROCK_FDIO_DEFINED 1
#	define librock_fdOpenReadOnly(fname) open(fname, O_RDONLY)
#	define librock_fdRead read
#	define librock_fdClose close
#	define librock_fdSeek lseek
# endif
#endif
#endif
