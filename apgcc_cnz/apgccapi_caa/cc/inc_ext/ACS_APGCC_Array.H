/*
 * ACS_APGCC_Array.H
 *
 *  Created on: Apr 2, 2010
 *      Author: root
 */

#ifndef ACS_APGCC_ARRAY_H_
#define ACS_APGCC_ARRAY_H_

//*****************************************************************************
//
// .ACS_APGCC_Array
//     Array - Simple Array class
// .LIBRARY 3C++
// .PAGENAME ACS_APGCC
// .HEADER  ACS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE ACS_APGCC_Array.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1998.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only Streamswith
//  the written permission from Ericsson Utvecklings AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.

// .DESCRIPTION
//  Simple template that hides the differences between STL and Rouge Wave
//  vector types
//  It does not make use of any UNIX signals.
//  It will it throw exceptions if the USE_EXCEPT is defined.
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
//		A               10-10-04        xantior         Porting APGG common classes
//
// .LINKAGE
//      -
//
//*****************************************************************************


#ifndef INCLUDED_STRING
#  include <string>
#  define INCLUDED_STRING
#endif

// ADDs for porting ACE lib ( forcing branch compile )
#define USE_OWN_ARRAY

#ifdef USE_OWN_ARRAY

// ADDs for porting ACE lib
#include "ace/OS_NS_string.h"

template<class T>
class Array
{
public:

	Array()
		:data_(0), size_(0)
	{}

	Array(int size)
		:size_(size), data_(new T[size])
	{}

	Array(int size, const T& value)
		:size_(size), data_(new T[size])
	{
		for (int i=0; i<size_; i++)
			data_[i] = value;
	}

	~Array()
	{
		if (data_)
			delete [] data_;
	}

	Array(const Array& a)			// copy constructor
		:size_(a.size_)
	{
		data_ = new T[size_];
		/*
		 * void * ACE_OS::memcpy	(	void * 	t,
		 *								const void * 	s,
		 *								size_t 	len
		 *							)
		 */

		// Ex: OSF_OS::memcpy(data_,a.data_,size_*sizeof(T));

		// ADDs for porting ACE lib
		ACE_OS::memcpy(
					   (void *)data_,
					   (const void *)a.data_,
					    size_*sizeof(T)
					  );

	}

	Array& operator=(const Array& a)	// assign operator
	{
		if ( &a != this ) {
			size_ = size_;
			if (data_)
				delete [] data_;
			if (a.data_) {
				data_ = new T[size_];

				// ADDs for porting ACE lib before ::memcpy only

				ACE_OS::memcpy(data_,a.data_,size_*sizeof(T));
			}
		}
		return *this;
	}

	const T& operator[](int n) const
	{
		if (n < 0 || n >= size_ || data_ == 0)
         throw std::string("Array; Array subscript out of range");
		return data_[n];
	}

	T& operator[](int n)
	{
		if (n < 0)
         throw std::string("Array; Array subscript out of range");
		if (n >= size_ || data_ == 0)
			resize(n+1);
			//throw std::string("Array; Array subscript out of range");
		return data_[n];
	}

	operator const T*() const
	{
		return data_;
	}

	operator T*()
	{
		return data_;
	}

	int size() const
	{
		return size_;
	}

	void clear()
	{
		if ( data_ ) {
			delete [] data_;
			data_ = 0;
			size_ = 0;
		}
	}

	bool resize(int size)
	{
		if (size > size_) {
			T* tmp = new T[size];
			if (!tmp)
				return false;
			// Copy elements
			if (data_) {
				for (int i=0; i<size_; i++)
					tmp[i] = data_[i];
				delete [] data_;
			}
			data_ = tmp;
			size_ = size;
		}
		return true;
	}

private:

	T* data_;
	int size_;
};


#else // not USE_OWN_ARRAY

#ifndef INCLUDED_VECTOR
#  include <vector>
#  define INCLUDED_VECTOR
#endif

#ifndef APGCC_OS_H
#  include "APGCC_OS.H"
#endif

//ADDs for ACElib porting
#include <string>
#include <vector>
#include <iterator>
//ends

using namespace std;

template<typename T>
class  ACS_APGCC_Array
{
public:

// ex typedef T Item
// ADDs for porting ACE lib

//typedef T Item;
//typedef vector<Item> ItemList;
//typedef ItemList::iterator  iterator;
//typedef ItemList::const_iterator const_iterator;

   typedef T Item;

   typedef vector<T> ItemList;
   typename ItemList::iterator iterator;
   typename ItemList::const_iterator const_iterator;

   //vector<T>::iterator it;

   ACS_APGCC_Array()
   :allocSize_(0)
   {}


   ~APGCC_Array()
   {
      //this->clear();
   }



   const T& operator[](int idx) const
   {
      if ( idx >= allocSize_ )
         this->resize( idx+1 );

      return list_[idx]; //*list_[idx];
   }


   T& operator[](int idx)
   {
      if ( idx >= allocSize_ )
         this->resize( idx+1 );

      return list_[idx]; //*list_[idx];
   }


   void resize(int newsize)
   {
      if ( newsize > allocSize_ ) {

         for (int i=this->size(); i<newsize; i++) {
            //list_.push_back(new T);
            list_.push_back( T() );
         }

         allocSize_ = newsize;
      }
   }


   void clear()
   {
      //T*  t;
      /*
      Item t;

      while ( !list_.empty() ) {
         t = list_.back();
         list_.pop_back();
         //delete t;
      }
      */

      list_.erase(list_.begin(),list_.end());

      allocSize_ = 0;
   };


   int size()
   {
      return list_.size();
   }


   ACS_APGCC_Array& operator=(const ACS_APGCC_Array& arr)
   {
      if ( this == &arr )
         return *this;

      // Release allocated resources
      this->clear();

      // Create our new array by iterating the given one
      //ItemList::const_iterator it;

      // ItemList::const_iterator it ;
      // Adds for ACElib porting substitute to previews row because not compile
      int it;

      //T* t;
      Item t;

      //for (it=arr.list_.begin(); it!=arr.list_.end(); it++) {
      // Adds for ACElib porting substitute to previews row because not compile
      for (it=0; it!=this->arr.list_.size(); it++) {

         //t = *it;
    	 // Adds for ACElib porting substitute to previews row because not compile
    	 t = arr.list_[it];

         //list_.push_back( new T( *t ) );
         list_.push_back( t );
      }

      // Set allocated size
      allocSize_ = this->size();

      return *this;
   }

private:

   ItemList list_;
   int allocSize_;

   ACS_APGCC_Array(const ACS_APGCC_Array& a);           // copy constructor

};

#endif // USE_OWN_ARRAY


#endif /* ACS_APGCC_ARRAY_H_ */
