//*****************************************************************************
//
// .NAME
//     MCS_Collections - Internal templates to simplify handling of
//                       persistent lists and queues.
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  MCS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_Collections.H

// .COPYRIGHTcount
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

// .DESCRIPTION
//
// Note that neither of these classes are thread safe.
//
// MCS_PersistentList  - Implements a persistent list.
// MCS_PersistentFIFO  - Implements a persistent First In First Out queue.
//                       This class can be used to implement a persistent
//                       pipe.
//
// A user defined class that needs to be persitent must inherit from
// MCS_IPersistent, and implement the methods Load() and Save().
//
// MCS_PersistentFIFO
// ==================
//
// // Define a class to hold our data.
//
// class MyData: public MCS_IPersistent {
//   public:
//     virtual bool Load(MCS_IStream* reader)
//     {
//       reader->Read(&data1,sizeof(int));
//       reader->Read(&data2,sizeof(int));
//       reader->ReadString(str1);
//       reader->ReadString(str2);
//     }
//
//     virtual bool Save(MCS_IStream* writer)
//     {
//       reader->Write(&data1,sizeof(int));
//       reader->Write(&data2,sizeof(int));
//       reader->WriteString(str1);
//       reader->WriteString(str2);
//     }
//
//
//  private:
//     int data1;	    // this is our 'unique' identifier in the data
//     int data2;	    // this is our 'unique' identifier in the data
//     string str1;   // this is just some test string
//     string str2;   // this is just some test string
// }
//
// int main() {
//    MCS_PersistentFIFO<MyData> pipe("mydata.pipe");
//    MyData d;
//    //Write some data
//    for (int i=0; i<10; i++) {
//       d.data1 = i;
//       pipe.push_back(d);
//    }
//    // Read data
//    while ( !pipe.empty() ) {
//       pipe.pop(d);
//       cout << "Got record no. " << d.data1 << endl;
//    }
//    pipe.close();
//    return 0;
// }
//
//
// MCS_PersistentList
// ==================
// The use of MCS_PersistentList requires also that the following
// methods are implemented by the user's class:
//
//    bool good()       Return true if the data item is valid.
//								Used to verify that a read item is valid
//                      after operation Load().
//
//    operator==()		Return true if equal. Used by find(),
//                      insert() and replace().
//
//		operator<()			Return true if this item  is less than the
//                      given item. Used by sort().
//                      Not needed if no sorting is to be done.
//
// Each item must be unique in the config file. This is checked
// with the operator== method, when the item is inserted in the
// internal storage.
//
// The method good() is called after Load() to check
// that the read data was valid before storing it in the internal
// storage.
//
// If the data type contains allocated memory then both the
// assign operator and the copy constructor must be implemented, as
// well as the copy constructor.
//
// Example:
//
// // Define a class to hold our data.
//
// class MyData: public MCS_IPersistent {
//   public:
//     MyData()
//       :num(-1)			// Make sure to initiate the item as invalid
//       {}
//
//     MyData(int num, const char* str)
//       :n(num), s(str)
//       {}
//
//     MyData(const Nydata& d)                  // copy constructor
//       :n(d.n), s(d.s)
//       {}
//
//     bool operator==(const MyData& d) const   // used by find()/replace()
//       {return n==d.n;}
//
//     bool operator<(const MyData& d) const    // used by sort()
//       {return n==d.n;}
//
//     bool good() const                        // used to determine if this
//       {return (n>=0 ? true : false); }       // item is valid or not
//
//     virtual bool Load(MCS_IStream* reader)
//     {
//       reader->Read(&n,sizeof(int));
//       reader->ReadString(s);
//     }
//
//     virtual bool Save(MCS_IStream* writer)
//     {
//       writer->Write(&n,sizeof(int));
//       writer->WriteString(s);
//     }
//
//
//  private:
//     int n;		   // this is our 'unique' identifier in the data
//     string s;   // this is just some test string
// }
//
//
// int main() {
//    MCS_PersistentList<MyData> cfg("mydata.cfg");
//    cfg.open();    // read file
//    cout << "before:" << endl;
//    cout << cfg;			   // operator<< must be defined!
//    cfg.replace( MyData(1234,"Good") );  // replace item 1234
//    cfg.insert( MyData(6543,"Bye") );    // insert item 6543
//    cout << "after:" << endl;
//    cout << cfg;			   // operator<< must be defined!
//    cfg.close();
//    return 0;
// }
//
// > cat mydata.cfg
// 1234 Hello
//
// > a.out
// before:
// 1234 Hello
// after:
// 1234 Good
// 6543 Bye
//
// > cat mydata.cfg
// 1234 Good
// 6543 Bye
//
// Traversing the items in the configuration file is done with
// an iterator. The iterator mimics the STL's way of iterators.
//
// Example:
//
// typedef MCS_PersistentList<MyData> MyCfgFile;
// MyCfgFile cfg("mydata.cfg");
// MyCfgFile::iterator it;
//
// if ( !cfg.open() )
//    error handling...
//
// for (it = cfg.begin(); it!=cfg.end(); it++)
//    cout << *it;   // operator<< must be defined!
//
// cfg.close();
//
//
//
//  It does not make use of any UNIX signals; nor will it throw any
//  exceptions.
//
//  Over- and underflow will not be notified.

// .ERROR HANDLING
//
//  .fill_offRemove fisrt element of the file ->
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

//***                         Porting                      ***
//       REV NO          DATE            NAME    DESCRIPTION
//       PR_01           10-04-15        AI      First version
//       PR_01_01		 10-06-08		 FR		 Second version
//*****************************************************************************


#ifndef ACS_APGCC_COLLECTIONS_H_
#define ACS_APGCC_COLLECTIONS_H_


//ADDs for ACElib porting
#include "ace/OS_NS_string.h"    // For memset()
#include "ace/FILE.h"			  // For remove()
//end ADDs
#  include "ACS_APGCC_Streams.H"   // Sets _MCCUSESTDC if not WIN32
#ifndef INCLUDED_STDLIB_H
//ADDs for ACElib porting
//#  include <stdlib.h>
#  include "ace/OS_NS_stdlib.h"
//end ADDs
#  define INCLUDED_STDLIB_H
#endif

#ifndef INCLUDED_ALGORITHM
#  include <algorithm>
#  define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_LIST
#  include <list>
#  define INCLUDED_LIST
#endif

#ifdef NDEBUG
#  undef _DEBUG_FIFOFILE
#endif

#ifdef _DEBUG_FIFOFILE
#  define _FFPRINT(x)   x
#else
#  define _FFPRINT(x)
#endif


//
// Interface class that must be implemented
// by objects that wants to be persistent.
// See APGCC_Streams.H for details on the APGCC_IStream interface.
//fs_
class ACS_APGCC_IPersistent
{
public:
   virtual bool Load(ACS_APGCC_IStream* reader) = 0;
   virtual bool Save(ACS_APGCC_IStream* writer) = 0;
};



//
// Template for first in-first out data file.
//
template <class T>
class ACS_APGCC_PersistentFIFO
{
public:
   enum {
      MAX_FRAGMENT_PERCENT = 10
   };

   // Internal structures
   typedef struct {
      u_long front;   // Pointer to first used record
      u_long back;    // Pointer to the end mark
      u_long count;   // Number of records
      u_long free;    // Pointer to the first free (deleted) record
      u_long freeLast;// Pointer to last record in free list
      u_long deleted; // Number of deleted records (free holes)Remove fisrt element of the file ->
      u_long rsvd;    // Reserved for future use
   } FileHeader;

   typedef struct {
      u_long prev;
      u_long next;
      u_long size;
   } DataHeader;


   // Constructor
   //-----------------------------------------------------------------
   ACS_APGCC_PersistentFIFO(const char* filename=0, int max_frag=0)
      :/*front_(0),back_(0),free_(0),count_(0),deleted_(0),*/ max_fragment_percent_(MAX_FRAGMENT_PERCENT), /*PR_01_01 BEGIN*/ fs_(filename) /*PR_01_01 END*/
   {
      if (filename)
         fname_ = filename;

      if (max_frag > 0 && max_frag <= 100) {
         max_fragment_percent_ = max_frag;
      }
   }


   // Destructor
   //-----------------------------------------------------------------
   ~ACS_APGCC_PersistentFIFO()
   {
      fs_.Close();
   }


   // Opens and initiate the file.
   // It is possible to give a value for the max fragmentation that
   // is allowed for the stored records. This is then used in method
   // pop_front()/pop() to order a packing of the rest of the record
   // when needed. However, compressing is now ordered manually.
   // IMPORTANT: If the stream is to be accessed by multiple threads
   // and/or processes, the user *must* call Lock() and Unlock() before
   // and after any reading or writing to the stream.
   // Note also that it is possible that open() returns with error
   // if some other thread/process has Locked the file.
   // On Win32 check with GetLastError() and ERROR_LOCK_VIOLATION.
   // On Unix check the errno variable.
   // For example:
   //   bool opened_ok;
   //   do {
   //      opened_ok = pipe.open();
   //      if ( !opened_ok )
   //         Sleep(1000);
   //   } while ( GetLastError() == ERROR_LOCK_VIOLATION );
   //-----------------------------------------------------------------
   bool open(const char* filename=0, int max_frag=0)
   {
      if (filename)
         fname_ = filename;

      if ( fname_.length() == 0 ) {
         return false;
      }

      if (max_frag > 0 && max_frag <= 100) {
         max_fragment_percent_ = max_frag;
      }

      if ( !fs_.Open(fname_.c_str()) ) {
         _FFPRINT(cout << "FIFOFile: open; Open failed" << endl);
         return false;
      }

      FileHeader fh;
      memset(&fh,0,sizeof(FileHeader));

      if ( fs_.Empty() ) {
         _FFPRINT(cout << "FIFOFile: New file; Writing file header..." << endl);

         if ( !fs_.Write(&fh,sizeof(FileHeader)) ) {
            return false;
         }

      }
      else {
         _FFPRINT(cout << "FIFOFile: Existing file; Reading file header..." << endl);

         if ( !fs_.Read(&fh,sizeof(FileHeader)) ) {
            return false;
         }

         /*
         // Save file pointers
         front_   = fh.front;
         back_    = fh.back;
         free_    = fh.free;
         count_   = fh.count;
         deleted_ = fh.deleted;
         */
         _FFPRINT(cout << "FIFOFile: Front="<<fh.front<<", Back="<<fh.back<<", Free="<<fh.free<<", count="<<fh.count<<", deleted="<<fh.deleted<<endl);
      }

      return true;
   } // open


   // Close the file
   //-----------------------------------------------------------------
   void close()
   {
      fs_.Close();
   }


   // Removes the file
   // Returns true on success
   //-----------------------------------------------------------------
   bool erase()
   {
      this->close();

      return ( ::remove(fname_.c_str()) == 0 );
   } // erase


   // Returns true if there are no more records (=file empty)
   //-----------------------------------------------------------------
   bool empty() const
   {
      return ( this->size() == 0 );
   } // empty


   // Returns number of stored records in file
   //-----------------------------------------------------------------
   int size() const
   {
      // Read file header
      FileHeader fh;

      const_cast<ACS_APGCC_PersistentFIFO*>(this)->fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      const_cast<ACS_APGCC_PersistentFIFO*>(this)->fs_.Read(&fh,sizeof(FileHeader));

      return fh.count;
   }


   // Returns the record from the back of the file.
   // The record is left in the file.
   // Returns a valid record on success.
   // Returns an 'empty' record if the file is empty.
   //-----------------------------------------------------------------
   T back()
   {
      T d;

      // Read file header
      FileHeader fh;

      fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Read(&fh,sizeof(FileHeader));


      if ( fh.count > 0 ) {

         // Set read pos
         fs_.SetPos(fh.back,ACS_APGCC_FileStream::FS_FILE_BEGIN);

         // Read end mark
         DataHeader dh;
         fs_.Read(&dh,sizeof(DataHeader));

         u_long dataPos = dh.prev;

         // Set pos to previous record
         fs_.SetPos(dataPos,ACS_APGCC_FileStream::FS_FILE_BEGIN);

         // Read data header
         fs_.Read(&dh,sizeof(DataHeader));

         // Read actual data
         d.Load(&fs_);

         // Compare current pos with size information
         u_long currPos = fs_.GetPos();

         if ( currPos > dataPos + dh.size ) {
            _FFPRINT(cout << "FIFOFile: back; ERROR: DataObject reads over to next record!" << endl;);
         }

      }// if !empty

      return d;
   } // back


   // Returns the record from the front of the file.
   // The record is left in the file.
   // Returns a valid record on success.
   // Returns an 'empty' record if the file is empty.
   //-----------------------------------------------------------------
   T front()
   {
      T d;

      // Read file header
      FileHeader fh;

      fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Read(&fh,sizeof(FileHeader));


      if ( fh.count > 0 ) {

         // Set read pos
         fs_.SetPos(fh.front,ACS_APGCC_FileStream::FS_FILE_BEGIN);

         // Read data header
         DataHeader dh;
         fs_.Read(&dh,sizeof(DataHeader));

         _FFPRINT(cout << "FIFOFile: front; Reading from pos="<<fh.front<<", Got size="<<dh.size<<endl;);

         // Read actual data
         d.Load(&fs_);

         // Compare current pos with size information
         u_long currPos = fs_.GetPos();

         if ( currPos > fh.front + dh.size ) {
            _FFPRINT(cout << "FIFOFile: front; ERROR: DataObject reads over to next record!" << endl);
         }

      }// if !empty

      return d;
   } // front


   // Returns and removes the record from the front of the file.
   // Equals: r=front(); pop_front() without an extra record copy.
   // Returns true on success.
   // See also front() and pop_front().
   //-----------------------------------------------------------------
   bool pop(T& d)
   {
      // Read file header
      FileHeader fh;

      fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Read(&fh,sizeof(FileHeader));


      if ( fh.count > 0 ) {

         // Set read pos
         fs_.SetPos(fh.front,std::ios::beg);

         // Read data header
         DataHeader dh;
         fs_.Read(&dh,sizeof(DataHeader));

         _FFPRINT(cout << "FIFOFile: pop; Reading from pos="<<fh.front<<", Got size="<<dh.size<<endl);

         // Read actual data
         d.Load(&fs_);

         // Compare current pos with size information
         u_long currPos = fs_.GetPos();

         if ( currPos > fh.front + dh.size ) {
            _FFPRINT(cout << "FIFOFile: pop; ERROR: DataObject reads over to next record!" << endl);
         }

         // Remove record
         return this->pop_front();
      }
      return false;
   } // pop


   // Removes a record from the front of the file.
   // Returns true on success.
   //-----------------------------------------------------------------
   bool pop_front()
   {

      FileHeader fh;

      // Read file header
      fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Read(&fh,sizeof(FileHeader));

      if ( fh.count == 0 ) {
         return false;
      }

      // Set temp pointers
      u_long front_   = fh.front;
      u_long back_    = fh.back;
      u_long count_   = fh.count;
      u_long deleted_ = fh.deleted;


      // Set read pos
      fs_.SetPos(fh.front,ACS_APGCC_FileStream::FS_FILE_BEGIN);

      // Read data header
      DataHeader curr;
      fs_.Read(&curr,sizeof(DataHeader));

      _FFPRINT(cout << "pop_front; Current: free="<<fh.free<<", freeLast="<<fh.freeLast<<", deleted="<<fh.deleted<<endl);


      // Save new front pos
      fh.front = curr.next;
      fh.deleted = ++deleted_;

      if ( fh.free == 0 ) {
         fh.free     = front_;
         fh.freeLast = front_;
      }
      else {
         // Update free list
         DataHeader dh;
         fs_.SetPos(fh.freeLast,ACS_APGCC_FileStream::FS_FILE_BEGIN);
         fs_.Read(&dh,sizeof(DataHeader));

         // Link to this record
         dh.next = front_;

         fs_.SetPos(fh.freeLast,ACS_APGCC_FileStream::FS_FILE_BEGIN);
         fs_.Write(&dh,sizeof(DataHeader));

      }

      // Link record into free list
      curr.prev = fh.freeLast;
      curr.next = fh.free;

      _FFPRINT(cout << "pop_front; Current: front="<<front_<<", back="<<fh.back<<endl;);

      fs_.SetPos(front_,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Write(&curr,sizeof(DataHeader));

      // Update pointers
      fh.freeLast = front_;
      front_      = fh.front;

      // Decrease number of stored records
      fh.count = --count_;

      // Check for end of file
      if ( fh.front == fh.back ) {

         // If empty truncate file
         /*
         if ( fh.count == 0 ) {

            //this->erase();
            //return this->open();

            return true;
            //NOTREACHED
         }
         */

         memset(&fh,0,sizeof(FileHeader));

         front_   = fh.front;
         back_    = fh.back;
         //free_    = fh.free;
         count_   = fh.count;
         deleted_ = fh.deleted;
         _FFPRINT(cout << "FIFOFile: pop_front; ***EMPTY** front="<<fh.front<<", Back="<<fh.back<<", Free="<<fh.free<<", count="<<fh.count<<", deleted="<<fh.deleted<<endl;);
      }

      _FFPRINT(
      cout << "pop_front; Updated: front="<<fh.front<<", back="<<fh.back<<endl;
      cout << "pop_front; Updated: free="<<fh.free<<", freeLast="<<fh.freeLast<<endl;
      cout << "pop_front; Updated: count="<<fh.count<<", deleted="<<fh.deleted<<endl;
      );

      // Update file header
      fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Write(&fh,sizeof(FileHeader));

      return true;
   } // pop_front


   // Adds a record to the end of the file.
   // Returns true on success.
   // See also pop().
   //-----------------------------------------------------------------
   bool push_back(T& d)
   {
      FileHeader fh;
      DataHeader prev;
      DataHeader endmark;

      memset(&fh,0,sizeof(FileHeader));
      memset(&prev,0,sizeof(DataHeader));
      memset(&endmark,0,sizeof(DataHeader));

      // Read file header
      fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Read(&fh,sizeof(FileHeader));

      // Set temp pointers
      u_long front_ = fh.front;
      u_long back_  = fh.back;
      u_long count_ = fh.count;

      // Check if empty file
      if ( fh.count == 0 ) {

         // Set file pointers
         front_ = sizeof(FileHeader);
         back_  = sizeof(FileHeader);

         // Set write pos
         fs_.SetPos( back_, ACS_APGCC_FileStream::FS_FILE_BEGIN );

         // Write data header
         DataHeader curr;
         memset(&curr,0,sizeof(DataHeader));

         // Link in the record (circulair list)
         // Note: this is temporary data; see below
         curr.prev = front_;
         curr.next = front_;

         _FFPRINT(cout << "FIFOFile: push_back; Writing data header at pos "<<back_<<endl;);

         fs_.Write(&curr,sizeof(DataHeader));

         // Write data
         d.Save(&fs_);

         // We always write an empty data header to mark
         // the end of the data stream; The size will be zero.
         endmark.prev = front_;  // because previous record was the first one
         endmark.next = front_;  // because it's a circulair list
         endmark.size = 0;       // marks last written record

         // Save current write pos as new write pos
         back_ = fs_.GetPos();

         // Write end mark
         _FFPRINT(cout << "FIFOFile: push_back; Writing end mark at pos "<<back_<<endl;);

         fs_.Write(&endmark,sizeof(DataHeader));


         // We need to update the previous written data
         // header with the size information as well
         // as the next pointer
         curr.size = back_ - front_;
         curr.next = back_;

         // Rewrite data header
         _FFPRINT(cout << "FIFOFile: push_back; Updating data header at pos "<<front_<<", size="<<curr.size<<endl;);

         fs_.SetPos( front_, ACS_APGCC_FileStream::FS_FILE_BEGIN );
         fs_.Write(&curr,sizeof(DataHeader));

      }
      else {

         // Save write pos
         u_long dataPos = back_;
         DataHeader curr;
         memset(&curr,0,sizeof(DataHeader));

         // The back pointer points at the end mark, which
         // we must read to obtain the prev and next pointers
         fs_.SetPos( back_, ACS_APGCC_FileStream::FS_FILE_BEGIN );
         fs_.Read(&curr, sizeof(DataHeader));

         _FFPRINT(cout << "FIFOFile: push_back; Last data header at pos "<<back_<<endl;);

         // Set the file pos to there the data shall be written
         fs_.SetPos( back_+sizeof(DataHeader), ACS_APGCC_FileStream::FS_FILE_BEGIN );

         // Write data
         _FFPRINT(cout<<"FIFOFile: push_back; Writing data at pos "<<back_+sizeof(DataHeader)<<endl;);

         d.Save(&fs_);

         // We always write an empty data header to mark
         // the end of the data stream; The size will be zero.
         endmark.prev = back_;  // link to the previous record
         endmark.next = front_; // because it's a circulair list
         endmark.size = 0;      // marks last written record

         // Save current write pos as new write pos
         back_ = fs_.GetPos();

         // Write end mark
         _FFPRINT(cout<<"FIFOFile: push_back; Writing end mark at pos "<<back_<<endl;);

         fs_.Write(&endmark,sizeof(DataHeader));


         // We need to update the previous data
         // header with the size information as well
         // as the next pointer
         curr.size = back_ - endmark.prev;
         curr.next = back_;

         // Rewrite data header
         _FFPRINT(cout<<"FIFOFile: push_back; Updating data header at pos "<<front_<<", size="<<curr.size<<endl;);

         fs_.SetPos( dataPos, ACS_APGCC_FileStream::FS_FILE_BEGIN );
         fs_.Write(&curr,sizeof(DataHeader));

      }

      // Finally we also update the file header
      fh.front = front_;
      fh.back  = back_;
      fh.count = ++count_;
      _FFPRINT(cout<<"FIFOFile: push_back; Updating file header; front="<<front_<<", back="<<back_<<", count="<<count_<<endl;);

      fs_.SetPos( 0, ACS_APGCC_FileStream::FS_FILE_BEGIN );
      fs_.Write(&fh,sizeof(FileHeader));

      return true;
   } // push_back


   // Lock the file making all access exclusive.
   //-----------------------------------------------------------------
   bool Lock()
   {
      return fs_.Lock();
   }


   // Unlock the file removing exclusive access.
   //-----------------------------------------------------------------
   bool Unlock()
   {
      return fs_.Unlock();
   }


   // Do not use this method unless absolutely required, as
   // the file is removed and re-created when the last record has been read.
   // As new records are added to the end of this file, it
   // is possible that the file increases to a very large size
   // unless the records are read.
   // This method can be used to pack the records in the file so
   // that there is no unused space in it.
   // @@Does not decrease the file size; Should use truncate()
   //   after the packing.
   //-----------------------------------------------------------------
   void compress(bool force=false)
   {
      FileHeader fh;

      // Read file header
      fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Read(&fh,sizeof(FileHeader));

      // Set temp pointers
      u_long front_   = fh.front;
      u_long back_    = fh.back;
      u_long count_   = fh.count;
      u_long deleted_ = fh.deleted;

      // Check if a compress is needed
      if ( count_ == 0 || deleted_ == 0 ) {
         _FFPRINT(cout<<"compress; Not needed! deleted="<<deleted_ << endl;);
         return;
      }

      u_long fragment_percent = ( 100 * deleted_ / count_ );
      if ( !force && fragment_percent < max_fragment_percent_ ) {
         _FFPRINT(cout<<"compress; Not needed! fragmented="<<fragment_percent << "%" << endl;);
         return;
      }


      //FileHeader fh;

      fs_.SetPos( 0, ACS_APGCC_FileStream::FS_FILE_BEGIN );
      fs_.Read(&fh, sizeof(FileHeader));

      DataHeader first;

      fs_.SetPos(front_,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Read(&first,sizeof(DataHeader));

      // Check if possible to move first record
      if ( first.size + sizeof(DataHeader) > front_ - sizeof(FileHeader) ) {
         _FFPRINT(cout<<"compress; Not enough of free space for compression" << endl;);
         return;
      }

      _FFPRINT(cout << "compress; compressing started (fragmented="<<fragment_percent<<"%)..." << endl;);
      _FFPRINT(cout << "compress; Current; front="<<front_<<", back="<<back_<<", free="<<fh.free<<endl;);

      u_long wpos = sizeof(FileHeader);
      u_long rpos = fh.front;
      u_long prev = sizeof(FileHeader);
      DataHeader dh;
      char* buf = 0;
      int bufSize = 0;

      do {

         _FFPRINT(cout << "compress; Moving; from pos="<<rpos<<", to pos="<<wpos<<endl;);

         // Read data header from old position
         fs_.SetPos(rpos,ACS_APGCC_FileStream::FS_FILE_BEGIN);
         if ( !fs_.Read(&dh,sizeof(DataHeader)) )
            break;

         // Check for end mark
         if ( dh.size > 0 ) {

            // Allocate new data buffer if needed
            if ( dh.size > bufSize ) {
               bufSize = dh.size;
               delete [] buf;
               buf = new char[bufSize];
            }

            // Read data from old position
            fs_.Read(buf,dh.size);

            // Update read pos
            rpos = dh.next;

            // Update data header with new positions
            dh.prev = prev;
            dh.next = wpos + sizeof(DataHeader) + dh.size;
         }
         else {
            // End mark
            // Update data header with new positions
            dh.prev = prev;
            dh.next = sizeof(FileHeader);
         }


         _FFPRINT(
            cout << "compress; Pointer: rpos="<<rpos<<", wpos="<<wpos<<", prev="<<prev<<endl;
            cout << "compress; Writing: dh.prev="<<dh.prev<<", next="<<dh.next<<", size="<<dh.size<<endl;
         );

         // Write data header to new position
         fs_.SetPos(wpos,ACS_APGCC_FileStream::FS_FILE_BEGIN);
         if ( !fs_.Write(&dh,sizeof(DataHeader)) )
            break;

         // Skip data for the end mark
         if ( dh.size > 0 ) {
            // Write data to new position
            if ( !fs_.Write(buf,dh.size) )
               break;
         }

         // Update prev pointer
         prev = wpos;

         // Update write pos
         wpos = fs_.GetPos();

      } while ( dh.size != 0 );  // until end mark

      delete [] buf;

      // Update pointers
      front_   = sizeof(FileHeader);
      back_    = prev;
      deleted_ = 0;

      // Update file header
      fh.front    = front_;
      fh.back     = back_;
      fh.free     = 0;
      fh.freeLast = 0;
      fh.count    = count_;
      fh.deleted  = 0;

      fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      fs_.Write(&fh,sizeof(FileHeader));

      _FFPRINT(cout << "compress; Update: front="<<front_<<", back="<<back_<<endl;);
      _FFPRINT(cout << "compress; compressing ending..." << endl;);
   } // compress



   //============================================================
   //====== The following methods are for INTERNAL USE ONLY =====
   //============================================================

   //Returns the internal file handle.
   // INTERNAL USE ONLY
   //-----------------------------------------------------------------

   int handle() const
   {
      return fs_.handle();
   }

   // Returns number of deleted records
   // INTERNAL USE ONLY
   //-----------------------------------------------------------------
   int deleted() const
   {
      FileHeader fh;

      // Read file header
      const_cast<ACS_APGCC_PersistentFIFO*>(this)->fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      const_cast<ACS_APGCC_PersistentFIFO*>(this)->fs_.Read(&fh,sizeof(FileHeader));

      return fh.deleted;
   }


   // Dump pipe structure
   //-----------------------------------------------------------------
   void dump()
   {

      FileHeader fh;
      fs_.SetPos(0,ACS_APGCC_FileStream::FS_FILE_BEGIN);

      if ( !fs_.Read(&fh,sizeof(FileHeader)) ) {
         cout << "FIFOFile: dump; Failed to read file header!"<<endl;
         return;
      }

      cout << "Dump of data file: " << fname_.c_str() << endl;
      cout << "front  = " << fh.front << endl;
      cout << "back   = " << fh.back << endl;
      cout << "free   = " << fh.free << endl;
      cout << "count  = " << fh.count << endl;
      cout << "deleted= " << fh.deleted << endl << endl;

      // Start start pos
      fs_.SetPos(fh.front,ACS_APGCC_FileStream::FS_FILE_BEGIN);

      // Read data records until end mark is found
      DataHeader dh;
      int nrec = 0;

      while ( fs_.Read(&dh,sizeof(DataHeader)) ) {

         cout << "--- Record " << ++nrec << "--- " << endl;
         cout << "prev = " << dh.prev << endl;
         cout << "next = " << dh.next << endl;
         cout << "size = " << dh.size << endl;

         if ( dh.size == 0 ) {
            _FFPRINT(cout << "--- end mark ---" << endl;);
            return;
         }

         // Skip over data
         fs_.SetPos(dh.next,ACS_APGCC_FileStream::FS_FILE_BEGIN);
      } // while
   }


private:

   int max_fragment_percent_;
   std::string fname_;
   ACS_APGCC_FileStream fs_;
};



template <class T>
class ACS_APGCC_PersistentList
{
public:

   // Define some new types
   /*typedef std::list<T> List;
   typedef List::iterator iterator;
   typedef List::const_iterator const_iterator;*/ // apg43 prior compilation changes:error C2146(qcharoh)


   typedef std::list< T> List;//added the keyword "typename" before "T"
   typedef typename List::iterator iterator;//added the keyword "typename" before "List"
   typedef typename List::const_iterator const_iterator;//added the keyword "typename" before "List"

   // Constructor
   // Sets name of config file if given.
   ACS_APGCC_PersistentList(const char* fileName = 0)
   :changed_(false)
   {

      if ( fileName != NULL )
         cfgFile_ = fileName;
   }


   // Destructor
   // Stores the data items in the config file, if they've changed.
   ~ACS_APGCC_PersistentList()
   {
      if (changed_)
         close();
   }


   // Returns an iterator to the first data item.
   iterator begin()
   {
      return list_.begin();
   }


   // Returns an const iterator to the first data item.
   const_iterator begin() const
   {
      return list_.begin();
   }


   // Returns an iterator to one past the last item.
   iterator end()
   {
      return list_.end();
   }


   // Returns an const iterator to one past the last item.
   const_iterator end() const
   {
      return list_.end();
   }


   // Opens the given config file, and loads the
   // data items found in the file. operator>>() is
   // used to read from the file, and method good() to
   // validate the data before it is stored in the internal
   // list. Invalid data items are discarded.

   bool open(const char* fileName = 0)
   {

      // Clear the internal data list
     this->clear();

      // Check if filename is given
      if ( fileName != NULL )
         cfgFile_ = fileName;

      // Check that we've a valid filename
      if ( cfgFile_.length() == 0 )
         return false;


      ACS_APGCC_FileStream fs;

      // Open the config file
      fs.Open(cfgFile_.c_str());

      if ( fs.Empty() ) {
         fs.Close();
         return true;
      }

      // Declare data item
      T d;

      // Read the global data first
      fs.ReadString(globalData_);

      // Next, read the list elements
      // While more data items, read config file
      while ( d.Load(&fs) ) {
         // If data item was successfully translated, save it
         if ( d.good() )
            list_.push_back( d );

      } // while


      // Close config file
      fs.Close();

      // No changes so far...
      changed_ = false;

      return true;
   }


   // Returns the complete (expanded) path where the config file is located.
   const char* configFilePath()
   {
      return cfgFile_.c_str();
   }


   // Returns the global data associated with this file.
   // Not valid until after open() is called.
   const char* globalData() const
   {
      return globalData_.c_str();
   }


   // Sets the global data
   // NOTE: Must not include any new line characters
   void globalData(const std::string& str)
   {
      globalData_ = str;
      changed_ = true;
   }


   // Returns the number of data items in the internal list
   int size()
   {
      return list_.size();
   }


   // Clear the internal data list

   void clear()
   {

      // Set this flag to be able to flush out the empty list
      // to the config file at closure
      // Note that we do NOT clear the global data!
      changed_ = (this->size() > 0 ? true : false);

      // Erase all items
      list_.erase(list_.begin(), list_.end());
   }


   // Insert an item into the list.
   void insert(const T& d)
   {
      list_.push_back( T(d) );

      changed_ = true;
   }


   // Inserts an item into the list
   void insert(T& d)
   {
      list_.push_back( T(d) );

      changed_ = true;
   }


   // Removes an item from the list
   void remove(const T& d)
   {
      int old_size = this->size();

      list_.remove(d);

      // Set changed flag if the item was removed
      changed_ = (old_size == this->size() ? false : true);
   }

   // Removes an item from the list
   void remove(T& d)
   {
      int old_size = this->size();

      list_.remove(d);

      // Set changed flag if the item was removed
      changed_ = (old_size == this->size() ? false : true);
   }


   // Replace an item in the list
   // Requires operator==() in the data item.
   void replace(const T& d)
   {
      iterator it;

      it = std::find(list_.begin(), list_.end(), d);
      if ( it != list_.end() ) {
         (*it) = d;
         changed_ = true;
      }
   }

   // Replace an item in the list
   // Requires operator==() in the data item.
   void replace(T& d)
   {
      iterator it;

      it = std::find(list_.begin(), list_.end(), d);
      if ( it != list_.end() ) {
         (*it) = d;
         changed_ = true;
      }
   }


   // Find an item.
   // Requires operator==() on the data item.
   bool find(T& d)
   {
      bool found = false;

      iterator it;

      it = std::find(list_.begin(), list_.end(), d);
      if ( it != list_.end() ) {
         found = true;
         d = (*it);
      }

      return found;
   }


   // Find an item.
   // Requires operator==() on the data item.
   bool find(const T& d) const
   {
      bool found = false;

      const_iterator it;

      it = std::find(list_.begin(), list_.end(), d);
      if ( it != list_.end() ) {
         found = true;
         //d = (*it);
      }

      return found;
   }


   // Sorts the data list
   void sort()
   {
      list_.sort();
      changed_=true; // Not really true... Special requirement from THHO.
   }


   // Flush changed items to the configuration file.
   // This method does NOT clear the internal data list.
   bool flush()
   {
      // Erase old file
      ::remove(cfgFile_.c_str());

      ACS_APGCC_FileStream fs;

      if ( fs.Open(cfgFile_.c_str()) ) {

         // Write the global data first
         fs.WriteString(globalData_);

         // Write out all data items
         for (iterator it=list_.begin(); it!=list_.end(); it++) {
            //fout << *it;
            it->Save(&fs);
         }

         // Close stream
         fs.Close();

      }
      else {
         return false;
      }


      // The config file and the internal data list is equal now
      changed_ = false;

      return true;
   }


   // Close down and flush all changed items.
   // This also clears the internal data list.

   void close()
   {
      // If the internal list has changed, write it to the config file
      if ( changed_ )
         this->flush();

      // Clear internal list
      this->clear();

      // Not changed...
      changed_ = false;
   }


   // Get access to the internal list.
   // Do not call this unless absolutely required.
   List& list()
   {
      return list_;
   }


protected:

   bool        changed_;
   List        list_;
   std::string cfgFile_;
   std::string globalData_;
};

#endif /* ACS_APGCC_COLLECTIONS_H_ */
