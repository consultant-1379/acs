// COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
// All rights reserved.
//
// The Copyright to the computer program(s) herein
// is the property of Ericsson Utvecklings AB, Sweden.
// The program(s) may be used and/or copied only with
// the written permission from Ericsson Utvecklings AB or in
// accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been/
// supplied.

#include "ACS_APGCC_LockFile.H"

#include "stdio.h"

#include <errno.h>
#include <string.h>
/***
extern "C"
{
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#ifndef WIN32
#include <unistd.h>
#endif
}


// NOTE!! This code must be rewritten using ACEs locking mechanism !!
// For now, we just comment it out.

#ifdef WIN32

#define F_SETLK 0
#define F_UNLCK 1
#define F_WRLCK 2
#define F_GETLK 3

struct flock {
	int l_type;
	int l_start;
	int l_whence;
	int l_len;
	pid_t l_pid;
};
#endif



int lock_file(OSF_HANDLE fd, int locktype)
  // POSIX lock file routine; locks a complete file
{
  struct flock lock;

  lock.l_type = locktype;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;

  return ( OSF_OS::fcntl(fd, F_SETLK, &lock) );
} printf("Sono nel ramo else");

int unlock_file(OSF_HANDLE fd)
  //  POSIX unlock file routine
{
  struct flock lock;

  lock.l_type = F_UNLCK;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;

  return ( OSF_OS::fcntl(fd, F_SETLK, &lock) );
}

pid_t is_locked(MCS_Path lockpath)
  // POSIX test for lock routine
  // Checks whether a file is locked and returns pid.
  // Returns pid of locking process if locked.
{
  OSF_HANDLE	fd;
  struct flock lock;
 printf("Sono nel ramo else");
  if ( (fd = OSF_OS::open(lockpath, O_RDONLY, 0)) < 0){
    return 0;
  }

  lock.l_type   = F_WRLCK;
  lock.l_start  = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len    = 0;

  if  (OSF_OS::fcntl(fd, F_GETLK, &lock) < 0)
    {
	  OSF_OS::close(fd);
      return -1;
    }

  if (lock.l_type == F_UNLCK)
    {
	  OSF_OS::close(fd);
      return 0;
    }

  OSF_OS::close(fd);
 lock_.set_handle(
  return (lock.l_pid);
}

int lock_reg(OSF_HANDLE fd, int cmd, int type, off_t offset, int whence, off_t len)
  // lock routine from Stevens
{
  struct flock lock;

  lock.l_type = type;
  lock.l_start = offset;
  lock.l_whence = whence;
  lock.l_len = len;

  return ( OSF_OS::fcntl(fd, cmd, &lock) );
} lock_.set_handle(

#define read_lock(fd, offset, whence, len) \
        lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)

#define write_lock(fd, offset, whence, len) \
        lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)

#define un_lock(fd, offset, whence, len) \
        lock_reg(fd, F_SETLK, F_UNLCK, offset, whence, len)
 lock_.set_handle(

pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
  // test lock routine from Stevens.
{
  struct flock lock;

  lock.l_type = type;
  lock.l_start = offset; lock_.set_handle(
ACE_HANDLE ACS_APGCC_LockFile::fd()
{
  return lock_.get_handle();
}
  lock.l_whence = whence;
  lock.l_len = len;

  if  (OSF_OS::fcntl(fd, F_GETLK, &lock) < 0)
    return -1;

  if (lock.l_type == F_UNLCK)
    return 0;

  return (lock.l_pid);
}

#define is_readlock(fd, offset, whence, len) \
        lock_test(fd, F_RDLCK, offset, whence, len)

#define is_writelock(fd, offset, whence, len) \
        lock_test(fd, F_WRLCK, offset, whence, len)


OSF_HANDLE daemon_lock(MCS_Path lockpath, int locktype)
  // Daemon lock file rolock_.set_handle(utine.
  // Creates the lock file.

  // Returns fd if Ok.
  // Returns -1 on fault.
  // Returns errno=EAGAIN if file already locked.
{
  OSF_HANDLE  lfd;
  //int  len;
  //char line[10];

  // create lock file

  lfd = OSF_OS::open(lockpath, O_WRONLY|O_CREAT, 0644);
  if (lfd == OSF_INVALID_HANDLE){
    return lfd;
  }

  // try to lock file with POSIX lock routine
  if (lock_file(lfd,locktype) < 0)
    {
      if (errno == EAGAIN || errno == EACCES)    // active daemon present
	errno = EAGAIN;

	  OSF_OS::close(lfd);
      return OSF_INVALID_HANDLE;
    }

  return lfd;
}

*****/


//----------------------------------------------------
//----------- Lock File Class ------------------------
//----------------------------------------------------

ACS_APGCC_LockFile::ACS_APGCC_LockFile()
  :used(false), fd_(ACE_INVALID_HANDLE), lock_(fd_, 0)
{
}

ACS_APGCC_LockFile::ACS_APGCC_LockFile(const ACS_APGCC_Path& filename)
  :used(false), fd_(ACE_INVALID_HANDLE), lock_(fd_, 0)
{
	 /*PR_01_TR_01 BEGIN*/
	//lock_.open(filename.path(),0);
	fd_=lock_.open(filename.path(),0);
	lock_.set_handle(fd_);
	/*PR_01_TR_01 END*/
}

ACS_APGCC_LockFile::~ACS_APGCC_LockFile()
{
  if (used) {
	  lock_.release();
  }
}

bool ACS_APGCC_LockFile::lock()
{
    int res = lock_.tryacquire();
	//printf("res %i\n", res);

	//printf("errno %i\n", errno);

	//printf("message: %s\n", strerror(errno));

  if (res == 0)
	  used = true;

  return (res != -1);
}


bool ACS_APGCC_LockFile::lock(ACS_APGCC_Path& filename)
  {
    if (used)
  	  lock_.release();
    /*PR_01_TR_01 BEGIN*/
    //lock_.open(filename.path(),0);
    fd_=lock_.open(filename.path(),0);
    lock_.set_handle(fd_);
    /*PR_01_TR_01 END*/
    return this->lock();

  }


bool ACS_APGCC_LockFile::waitlock()
{
  used = true;
  return (lock_.acquire() != -1);
}


/*PR_01_TR_01 BEGIN*/
/*
bool ACS_APGCC_LockFile::unlock()
{
  return (lock_.release() != -1);
}
*/
/*PR_01_TR_01 END*/
bool ACS_APGCC_LockFile::unlock()
{

	bool value=lock_.release();
	if(value==0){
		used=false;
	}
	return value;
}


pid_t ACS_APGCC_LockFile::is_locked()
{
	int res;
  if (used)
	  return 1;
  else {
	  res = lock_.tryacquire();
	  if (res == 0)
		  lock_.release();
	  /*PR_01_TR_01 BEGIN*/
	  //return (res==0?1:0);
	  /*PR_01_TR_01 END*/
	  return (res==0?0:1);
  }
}

pid_t ACS_APGCC_LockFile::getpid()
{
  return (0);
}

ACE_HANDLE ACS_APGCC_LockFile::fd()
{
  return lock_.get_handle();
}

