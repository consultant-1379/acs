//*****************************************************************************
//
// .NAME
//     ACS_APGCC_Config - Internal template to simplify handling of
//                  configuration files.
// .LIBRARY 3C++
// .PAGENAME APGCC_MCC
// .HEADER  APGCC
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE filnamn.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1998.
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
// The MCS_ConfigFile class supports the handling of Configuration
// files. The user must define it's own configuration data class.
// The only requirement is that the class must implement these methods:
//
//    bool good()       Return true if the data item is valid.
//								Used to verify that a read item is valid.
//
//    operator==()		Return true if equal. Used by find(),
//                      insert() and replace().
//
//		operator<()			Return true if this item  is less than the
//                      given item. Used by sort().
//
//    operator<<()      Output operator; Write own data type format.
//								Used by close() and flush().
//
//    operator>>()      Input operator; reads own data type format.
//								Used by open().
//
// Each item must be unique in the config file. This is checked
// with the operator== method, when the item is inserted in the
// internal storage.
//
// The method good() is called after the operator>> to check
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
// class MyData {
//   public:
//     MyData()
//       :num(-1)			// Make sure to initiate the item as invalid
//       {str[0]=0;}
//
//     MyData(int num, const char* str)
//       {n=num; strcpy(s,str);}
//
//     MyData(const Nydata& d)                  // copy constructor
//       { n=d.n; strcpy(s,d.s); }
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
//     friend ostream& operator<<(ostream& os, MyData& d);
//     friend istream& operator>>(istream& is, MyData &d);
//
//  private:
//     int n;		   // this is our 'unique' identifier in the data
//     char s[20];   // this is just some test string
// }
//
// // Define our own read and write methods for our data type.
//
// ostream& operator<<(ostream& os, MyData& d)
// {
//    // Save into ostream and separate each item with ';'.
//    os << d.num << ";" << d.str << endl;
//    return os;
// }
//
// istream& operator>>(istream& is, MyData &d)
// {
//    // Retreive data from istream.
//    char tmp[256];
//    is.getline(tmp,';');    // read until first semicolon
//    d.n = atoi(tmp);
//    is.getline(tmp),        // read until end-of-line
//    strcpy(d.s,tmp);
//    return is;
// }
//
// int main() {
//    MCS_ConfigFile<MyData> cfg("mydata.cfg");
//    cfg.open();
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
// typedef MCS_ConfigFile<MyData> MyCfgFile;
// MyCfgFile cfg("mydata.cfg");
// MyCfgFile::iterator it;
//
// if ( !cfg.open() )
//    error handling...
//
// for (it = cfg.begin(); it!=cfg.end(); it++)
//    cout << *it;
//
// cfg.close();
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
//      1998-03-18 by UAB/I/LN  UABSJEN

// .REVISION
//      A       98-03-18

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME            DESCRIPTION
//      A               98-03-18        SJEN            First release
//      B               99-02-05        SJEN            Second release
//                                                      Uses STL only.

// .LINKAGE
//      -lMCC

//*****************************************************************************
#ifndef ACS_APGCC_CONFIG_H
#define ACS_APGCC_CONFIG_H

#ifdef _APGCCINCIOSTREAM_H
#  ifndef INCLUDED_FSTREAM_H
#    include <fstream.h>
#  endif
#else
#  ifndef INCLUDED_FSTREAM
#    include <fstream>
#  endif
#endif //_APGCCINCIOSTREAM_H

//#ifndef INCLUDED_ALGORITHM
#  include <algorithm>
//#  define INCLUDED_ALGORITHM
//#endif

//#ifndef INCLUDED_LIST
#  include <list>
//#  define INCLUDED_LIST
//#endif

//#ifndef APGCC_PATH_H
#  include "ACS_APGCC_Path.H"
//#endif

#include <iostream>
#include <list>
using namespace std;
#define	_APGCCINCIOSTREAM_H 1



/*PR_01
 template <class T>
 */
template <typename T>
class ACS_APGCC_ConfigFile
{
public:

#ifdef _APGCCINCIOSTREAM_H
   // Define some friends that must exist
   friend ostream& operator<<(ostream& os, ACS_APGCC_ConfigFile<T>& c);
   friend istream& operator>>(istream& is, ACS_APGCC_ConfigFile<T>& c);
#else
   // Define some friends that must exist
   friend std::ostream& operator<<(std::ostream& os, ACS_APGCC_ConfigFile<T>& c);
   friend std::istream& operator>>(std::istream& is, ACS_APGCC_ConfigFile<T>& c);
#endif

   // Define some new types
   /*typedef std::list<T> List;
   typedef List::iterator iterator;
   typedef List::const_iterator const_iterator;*/ //apg43 prior compilation changes:errorC2146(qcharoh)

   /*PR_01 rimosso typename
   typedef std::list<typename T> List;//added the keyword "typename" before "T"
   */
   typedef std::list<T> List;
   typedef typename List::iterator iterator;//added the keyword "typename" before "List"
   typedef typename List::const_iterator const_iterator;//added the keyword "typename" before "List"

   // Constructor
   // Sets name of config file if given.
   // Expands the given filename using MCS_Path().
   ACS_APGCC_ConfigFile(const char* fileName = 0)
   :changed_(false)
   {
      if ( fileName != NULL )
         cfgFile_ = ACS_APGCC_Path(fileName);
   }


   // Destructor
   // Stores the data items in the config file, if they've changed.
   ~ACS_APGCC_ConfigFile()
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
   // Note that the given filename will be expanded to
   // it's complete path using MCS_Path().

   bool open(const char* fileName = 0)
   {

      // Clear the internal data list
     this->clear();

      // Check if filename is given
      if ( fileName != NULL )
         cfgFile_ = ACS_APGCC_Path(fileName);

      // Check that we've a valid filename
      if ( cfgFile_.length() == 0 )
         return false;


#ifdef _APGCCINCIOSTREAM_H
      ifstream fin;
#else
      std::ifstream fin;
#endif

      // Open the config file
      fin.open(cfgFile_.c_str(), ios::in);

      if ( !fin ) {

         // The file doesn't exist, so we'll create it
#ifdef _APGCCINCIOSTREAM_H
         ofstream fout (cfgFile_.c_str(), ios::app);
#else
         std::ofstream fout (cfgFile_.c_str(), ios::app);
#endif

         bool created = fout.good();

         fout.close();

         return created;
      }

      // Read the global data first
      std::getline(fin,globalData_,'\n');

      // Next, read the list elements
      // Declare data item
      T d;


      // While more data items, read config file
      while ( !fin.eof() ) {

         fin >> d;

         // If data item was successfully translated, save it
         if ( d.good() )
            list_.push_back( d );

      } // while


      // Close config file
      fin.close();

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


   // Insert an item into the list. As all items must be
   // unique within the list, this method calls replace().
   // Requires operator==() in the data item as it calls replace().

   void insert(const T& d)
   {
      list_.push_back( T(d) );

      changed_ = true;
   }

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
#ifdef _APGCCINCIOSTREAM_H
      ofstream fout;
#else
      std::ofstream fout;
#endif

      // Open the file and truncate it if it exists
      fout.open(cfgFile_.c_str(), ios::out | ios::trunc);

      if ( fout.bad() )
         return false;

      // Write the global data first
      fout << globalData_.c_str() << endl;

      // Write out all data items
      for (iterator it=list_.begin(); it!=list_.end(); it++) {
         fout << *it;
      }

      fout.close();

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


#ifdef _APGCCINCIOSTREAM_H

template <class T>
ostream& operator<<(ostream& os, ACS_APGCC_ConfigFile<T>& c)
{
   // Write the global data first
   os << c.globalData_.c_str() << endl;

   // Write all data items to config file
  typedef typename ACS_APGCC_ConfigFile<T>::iterator it_type;
   for ( it_type it=c.list_.begin(); it!=c.list_.end(); it++) {
      os << *it;
   }

   return os;
}


template <class T>
istream& operator>>(istream& is, ACS_APGCC_ConfigFile<T> &c)
{
   T d;

   // Clear list before reading new data items
   c.clear();

   // Check that the stream is Okey
   if ( is.bad() )
      return is;

   // Read the global data first
   std::getline(is,c.globalData_,'\n');

   // Read input until end-of-file
   while ( !is.eof() ) {

      is >> d;

      if ( is.eof() )
         break;

      // If read data item was valid, store it
      if ( d.good() )
         c.list_.push_back(d);
   }

   // Internal list is equal to the config file
   c.changed_ = false;

   return is;
}

#else // ! _APGCCINCIOSTREAM_H

template <class T>
std::ostream& operator<<(std::ostream& os, ACS_APGCC_ConfigFile<T>& c)
{
   // Write the global data first
   os << c.globalData_.c_str() << endl;

   // Write all data items to config file
   typedef typename ACS_APGCC_ConfigFile<T>::iterator iterator_CF_Type;
   for (iterator_CF_Type it=c.list_.begin(); it!=c.list_.end(); it++) {
      os << *it;
   }

   return os;
}


template <class T>
std::istream& operator>>(std::istream& is, ACS_APGCC_ConfigFile<T> &c)
{
   T d;

   // Clear list before reading new data items
   c.clear();

   // Check that the stream is Okey
   if ( is.bad() )
      return is;

   // Read the global data first
   std::getline(is,c.globalData_,'\n');

   // Read input until end-of-file
   while ( !is.eof() ) {

      is >> d;

      if ( is.eof() )
         break;

      // If read data item was valid, store it
      if ( d.good() )
         c.list_.push_back(d);
   }

   // Internal list is equal to the config file
   c.changed_ = false;

   return is;
}

#endif //_APGCCINCIOSTREAM_H


#endif   // ACS_APGCC_CONFIG_H
