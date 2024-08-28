//*****************************************************************************
//
// .NAME
//     ACS_APGCC_Streams - Implements different types of streams.
// .LIBRARY 3C++
// .PAGENAME ACS_APGCC
// .HEADER  ACS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_Streams.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2000.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson Utvecklings AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
// .DESCRIPTION
//
//  ACS_APGCC_IStream      - Interface that a new Stream must implement.
//  ACS_APGCC_FileStream   - Implements a stream using native the file system
//                     calls.
//  ACS_APGCC_StdStream    - Implements a stream that uses ostream ostream for
//                     writing resp. reading of data.
//
//
//  It does not make use of any UNIX signals; nor will it throw any
//  exceptions.
//
//  Over- and underflow will not be notified.

// .ERROR HANDLING
//
//  .fill_off
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.
//  .fill_on

// DOCUMENT NO
//      190 89-CAA 109 0171

// AUTHOR
//      2000-08-15 by UAB/I/V  QABSJEN

// .REVISION
//      A       00-08-15

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME            DESCRIPTION
//      A               00-08-15        SJEN            First release

// .LINKAGE
//      -

//*****************************************************************************

#ifndef ACS_APGCC_STREAMS_H_
#define ACS_APGCC_STREAMS_H_


#if !defined (ACS_APGCC_MCCUSESTDC)
#define ACS_APGCC_MCCUSESTDC
#endif
#  include <string>

#if defined (ACS_APGCC_MCCUSESTDC)
//ADDs for ACElib porting
//#  include <stdio.h>
#  include "ace/OS_NS_stdio.h"
//end ADDs
#else // !ACS_APGCC_MCCUSESTDC
#  if defined (ACS_APGCC_MCCINCIOSTREAM_H)
#    ifndef INCLUDED_FSTREAM_H
#      include <fstream.h>
#      define INCLUDED_FSTREAM_H
#    endif
#  else
#    ifndef INCLUDED_FSTREAM
#      include <fstream>
#      define INCLUDED_FSTREAM
#    endif
#  endif //ACS_APGCC_MCCINCIOSTREAM_H
#endif // ACS_APGCC_MCCUSESTDC

#ifndef INCLUDED_IO_H
//ADDs for ACElib porting
//#  include <io.h>             // for access()
#include "ace/OS_NS_unistd.h"  // for access()
#include "ace/OS_NS_fcntl.h"   // for fcntl()
#include "ace/Malloc_T.h" 		// for bad()
//end ADDs
#  define INCLUDED_IO_H
#endif

#ifndef INCLUDED_SYS_STAT_H
//ADDS for ACElib porting
//#  include <sys/stat.h>   // for stat()
#  include "ace/OS_NS_sys_stat.h"
//end ADDs
#  define INCLUDED_SYS_STAT_H
#endif

#include "ace/streams.h" // for std::ios::beg ...
//
// Interface class for streams.
//=============================
//
class ACS_APGCC_IStream
{
public:
   virtual bool Read(void* buf, size_t len) = 0;
   virtual bool Write(const void* buf, size_t len) = 0;

   // High level support functions
   //-------------------------------

   // Reads a std string from the stream.
   // Note that the string must have been written
   // using WriteString().
   //---------------------------------------------
   virtual bool ReadString(std::string& out)
   {
      int len;

      // Clear out variable
      out = "";
      // Read length of string
      if ( !this->ReadInt(len) || len < 0 )
         return false;
      char* buf = new char[len+1];
      // Read string data including null sign
      if ( !this->Read(buf,len) ) {
         delete [] buf;
         return false;
      }

      out = buf;
      delete [] buf;

      return true;
   }


   // Writes a std string to the stream.
   // Note that the string must then be retrieved
   // using ReadString().
   //---------------------------------------------
   virtual bool WriteString(const std::string& in)
   {
      int len = in.length() + 1; // including null sign
      // Write length of string
      if ( !this->WriteInt(len) )
         return false;

      // Write string data including null sign
      if ( !this->Write(in.c_str(),len) )
         return false;

      return true;
   }


   // Read an integer to the stream.
   //---------------------------------------------
   virtual bool ReadInt(int& out)
   {
      // Read int as string
      if ( !this->Read(&out,sizeof(out)) )
         return false;

      return true;
   }


   // Writes an integer to the stream.
   //---------------------------------------------
   virtual bool WriteInt(const int in)
   {
      return this->Write(&in,sizeof(in));
   }


protected:

   virtual long GetPos() = 0;
   // Get the current position in the stream

   virtual bool SetPos(long distance, size_t moveMethod) = 0;
   // Set a position within the stream

   virtual bool Empty() const = 0;
   // Returns true if stream is empty

   virtual bool Lock() = 0;
   // Lock the access to the stream

   virtual bool Unlock() = 0;
   // Unlock the access to the stream
};



//
// Implementation class for FIFO File stream.
//============================================
//
class ACS_APGCC_FileStream: public ACS_APGCC_IStream
{
public:

   // Constructor
   //---------------------------------------------
   ACS_APGCC_FileStream(const char* filename=0)
      :fh_(NULL)
   {
      if (filename)
         fname_ = filename;

      if (fname_.length() > 0)
         this->Open();
   }


   // Termination
   //---------------------------------------------
   ~ACS_APGCC_FileStream()
   {
      this->Close();
   }


   // Called to open the stream
   //---------------------------------------------
   bool Open(const char* filename=0)
   {
      if (filename)
         fname_ = filename;

      if (fname_.length() == 0)
         return false;


      // Open the file
      fh_ = ::fopen(fname_.c_str(),"r+b");

      if ( !fh_ ) {
         fh_ = ::fopen(fname_.c_str(),"w+b");
      }

      return ( fh_ != NULL );

   } // Open


   // Called to close the stream
   //---------------------------------------------
   void Close()
   {

      if ( fh_ )
         ::fclose(fh_);

      fh_ = NULL;

   } // Close


   // Called to read data from the stream
   //---------------------------------------------
   virtual bool Read(void* buf, size_t len)
   {
      if ( !fh_ )
         return false;
      size_t read = ::fread(buf,sizeof(char),len,fh_);
      return ( read == len );

   } // Read


   // Called to write data to the stream
   //---------------------------------------------
   virtual bool Write(const void* buf, size_t len)
   {

      if ( !fh_ )
         return false;

      size_t written = ::fwrite(buf,sizeof(char),len,fh_);

      return ( written == len );

   }


   // Returns true if stream is empty.
   // May not be possible for all types of streams
   //---------------------------------------------
   virtual bool Empty() const
   {

      size_t sz = 0;
      struct stat finfo;

      if ( ::fstat(::fileno(fh_),&finfo) == 0 )
         sz = finfo.st_size;

      return ( sz == 0 );

   } // Empty


   // Returns current position in the stream
   // May not be possible for all types of streams
   //---------------------------------------------
   virtual long GetPos()
   {

      if ( fh_ )
         return ::ftell(fh_);
      else
         return 0;

   } // GetPos


   enum { // Constants for SetPos()

      FS_FILE_BEGIN = SEEK_SET,
      FS_FILE_CURR  = SEEK_CUR,
      FS_FILE_END   = SEEK_END

   };


   // Sets the current position in the stream
   // May not be possible for all types of streamsfileno
   //---------------------------------------------
   virtual bool SetPos(long distance, size_t moveMethod)
   {

      if ( !fh_ || ::fseek(fh_,distance,moveMethod) == 0 )
         return false;

      return true;
   } // SetPos


   // Lock the access to the stream
   // May not be possible for all types of streams
   //---------------------------------------------
   virtual bool Lock()
   {
      struct flock lock;

      lock.l_type   = F_WRLCK;
      lock.l_start  = 0;
      lock.l_whence = SEEK_SET;
      lock.l_len    = 1;

      // wait for lock
      int res = ::fcntl(this->handle(), F_SETLKW, &lock);

      return ( res < 0 ? false : true );
   }


   // Unlock the access to the stream
   // May not be possible for all types of streams
   //---------------------------------------------
   virtual bool Unlock()
   {
      struct flock lock;

      lock.l_type   = F_UNLCK;
      lock.l_start  = 0;
      lock.l_whence = SEEK_SET;
      lock.l_len    = 1;

      int res = ::fcntl(this->handle(), F_SETLK, &lock);

      return ( res < 0 ? false : true );
   }


   // Internal; Do not use!
   // Get access to internal handle
   //---------------------------------------------
   int handle() const
   {
      return ::fileno(fh_);
   }


private:

   std::string fname_;
   FILE* fh_;
};





//
// Implementation class for Standard streams.
//============================================
//
class ACS_APGCC_StdStream: public ACS_APGCC_IStream
{
public:

   enum {
      FS_FILE_BEGIN = std::ios::beg,
      FS_FILE_CURR  = std::ios::cur,
      FS_FILE_END   = std::ios::end
   };


public:


#ifdef ACS_APGCC_MCCINCIOSTREAM_H
   ACS_APGCC_StdStream(ostream* os)
      :os_(os), is_(0)
   {
   }

   ACS_APGCC_StdStream(istream* is)
     :is_(is), os_(0)
   {
   }
#else //!ACS_APGCC_MCCINCIOSTREAM_H
   ACS_APGCC_StdStream(std::ostream* os)
      :os_(os), is_(0)
   {
   }

   ACS_APGCC_StdStream(std::istream* is)
   {
	   is_=is;
	   os_=0;
   }
#endif //ACS_APGCC_MCCINCIOSTREAM_H

   virtual ~ACS_APGCC_StdStream()
   {
   }

   // Read data from stream
   //---------------------------------------------
   virtual bool Read(void* buf, size_t len)
   {
      if ( !is_ )
         return false;

      is_->read((char*)buf,len);

      if ( is_->bad() )
         return false;
      else
         return true;
   }

   // Write data to stream
   //---------------------------------------------
   virtual bool Write(const void* buf, size_t len)
   {
      if ( !os_ )
         return false;

      os_->write((const char*)buf,len);

      if ( os_->bad() )
         return false;
      else
         return true;
   }


protected:

   // Get current position in stream
   //---------------------------------------------
   virtual long GetPos()
   {
      if ( is_ ) {   // if input stream
         return is_->tellg();
      }
      else if ( os_ ) {  // if output stream
         // No support for seek/tell
         //return os_->tellg();
      }
      return 0;
   }


   // Set current position in stream
   //---------------------------------------------
   virtual bool SetPos(long distance, size_t moveMethod)
   {
      int newPos;

      if ( is_ ) {   // if input stream

//#ifdef ACS_APGCC_MCCINCIOSTREAM_H
//         is_->seekg(distance,(enum ios::seek_dir)moveMethod);
//#else
//         is_->seekg(distance,(enum std::ios_base::seekdir)moveMethod);
//#endif //apg43 prior compilation changes:C1001(qcharoh)

#if _MSC_VER >= 1300 //visual C++ 7.0 and above
is_->seekg(distance,moveMethod);  //removed the type casting for second argument "moveMethod"
#else                //Visual C++ 6.0 and below
		 #ifdef ACS_APGCC_MCCINCIOSTREAM_H
			          is_->seekg(distance,(enum ios::seek_dir)moveMethod);
		 #else
		              is_->seekg(distance,( ios_base::seekdir)moveMethod);
		 #endif
#endif


		 newPos = is_->tellg();

         if (newPos < 0)
            return false;

      }
      else if ( os_ ) {  // if output stream
         /*
         // No support for seek/tell
         os_->seekg(distance,(enum std::ios_base::seekdir)moveMethod);
         newPos = os_->tellg();

         if (newPos < 0)
            return false;
         */
         return false;
      }

      return true;
   }

   // Returns true if empty
   // Not possible for this stream
   //---------------------------------------------
   virtual bool Empty() const
   {
      return false;
   }

   // Lock the access to the stream
   // Not possible for this stream
   //---------------------------------------------
   virtual bool Lock()
   {
      return false;
   }


   // Unlock the access to the stream
   // Not possible for this stream
   //---------------------------------------------
   virtual bool Unlock()
   {
      return false;
   }


private:

#ifdef ACS_APGCC_MCCINCIOSTREAM_H
   ostream* os_;
   istream* is_;
#else
   std::ostream* os_ ;
   std::istream* is_ ;
#endif
};


#if defined (_SET_ACS_APGCC_MCCUSESTDC)
#  define ACS_APGCC_MCCUSESTDC
#  undef _SET_ACS_APGCC_MCCUSESTDC
#endif

#endif /* ACS_APGCC_STREAMS_H_ */
