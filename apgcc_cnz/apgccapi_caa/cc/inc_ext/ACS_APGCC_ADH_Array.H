//*****************************************************************************
//
// .NAME
//     Array - Simple Array class
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  MCS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_Array.H

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
//      A               10-10-04        xantior         Porting APGCC classes
//
// .LINKAGE
//      -
//
//*****************************************************************************
#ifndef ACS_APGCC_ADH_Array_H
#define ACS_APGCC_ADH_Array_H

#define USE_OWN_ARRAY

#ifdef USE_OWN_ARRAY

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
		ACE_OS::memcpy(data_,a.data_,size_*sizeof(T));
	}

	Array& operator=(const Array& a)	// assign operator
	{
		if ( &a != this ) {
			size_ = size_;
			if (data_)
				delete [] data_;
			if (a.data_) {
				data_ = new T[size_];
				::memcpy(data_,a.data_,size_*sizeof(T));
			}
		}
		return *this;
	}

	const T& operator[](int n) const
	{
		if (n < 0 || n >= size_ || data_ == 0)
			throw "Array; Array subscript out of range";
		return data_[n];
	}

	T& operator[](int n)
	{
		if (n < 0)
			throw "Array; Array subscript out of range";
		if (n >= size_ || data_ == 0)
			resize(n+1);
			//throw "Array; Array subscript out of range";
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

#ifdef WIN32
#define USE_STL
#endif

#define USE_STD
// use standard (STL/RW) components in Array,either STL or Rouge Wave

#ifdef USE_STD
// if true, use standard components (STL or RW)
// if false, use home made component

#ifdef USE_STL
// if true, use STL
#include <vector>
//using namespace std;

#else
// otherwise, use Rouge Wave
#include <rw/tpvector.h>
#undef vector
#define vector RWTPtrVector

#endif // USE_STL


#endif  //USE_STD


#undef USE_EXCEPT	   // if defined, use exceptions instead of exit(-1)

void getNumber( unsigned long& upperFourBytes,
		unsigned long& lowerFourBytes );


template<class T>
class Array
{
public:
  // Constructorer
  // Destructorer
  // operator=
  Array():data_(0), size_(0), allocsize_(0) {}

  ~Array()
    {
#ifdef USE_STD
      clear();
#else
      delete [] data_;
#endif
    }


  // operator ??

  // Conversion operators


  const T& operator[](int idx) const
    {
#ifdef USE_STD
      if (idx < 0 || idx >= allocsize_)
#else
	if (idx < 0 || idx >= allocsize_ || data_ == 0)
#endif
#ifdef USE_EXCEPT
	  throw "MCS_ADH_Array: Array subscription out of range";
#else
      exit(-1);
#endif

#ifdef USE_STL
      return *data_[idx];
#else
      return *data_[idx];
#endif
    }

  T& operator[](int idx)
    {
      if (idx < 0)
#ifdef USE_EXCEPT
        throw "MCS_ADH_Array: Array subscription negative";
#else
      exit(-1);
#endif

#ifdef USE_STD
      if (idx >= allocsize_)
        resize(idx+1);
#else
      if (idx >= allocsize_ || data_ == 0)
        resize(idx+1);
#endif

      if (idx >= size_)
	size_ = idx+1;

#ifdef USE_STD
      return *data_[idx];
#else
      return *data_[idx];
#endif
    }

  void resize(int newsize)
// Description:
//    Resize the array
// Parameters:
//    newsize                  Integer, new size of the array
// Return value:
//    -
// Additional information:
//    -
    {
      if (newsize == allocsize_ || newsize < 0) return;

#ifdef USE_STD
      for(int i=size(); i<newsize; i++) {
#ifdef USE_STL
        data_.push_back(new T);
#else
        data_.reshape(i+1);
        data_[i] = new T;
#endif
      }
#else
      if (newsize > allocsize_)
        newsize = 10*( (newsize/10) + 1 );

      T* olddata = data_;
      data_ = new T[newsize];

      int count = (allocsize_ > newsize ? newsize:allocsize_);

      for (int i=0; i<count; i++)
        data_[i] = olddata[i];

      delete [] olddata;
#endif

      allocsize_ = newsize;
    }

  void clear()
// Description:
//    Short description
// Parameters:
//    -
// Return value:
//    -
// Additional information:
//    -
    {
#ifdef USE_STD
      T*  t;
#ifdef USE_STL
      while (!data_.empty()) {
        t = data_.back();
        data_.pop_back();
        delete t;
      }
#else
      int i=0;
      while (i < data_.length()) {
        t = data_[i++];
        delete t;
      }
      data_.resize(0);
#endif
#else
      delete [] data_;
      data_      = 0;
#endif
      size_      = 0;
      allocsize_ = 0;
    };

  int size()
// Description:
//    Short description
// Parameters:
//    -
// Return value:
//    -
// Additional information:
//    -
    {
#ifdef USE_STD
#ifdef USE_STL
      return data_.size();
#else
      return data_.length();
#endif
#else
      return size_;
#endif
    }

  int allocsize() {return allocsize_;}
// Description:
//    Short description
// Parameters:
//    -
// Return value:
//    -
// Additional information:
//    -

private:
#ifdef USE_STD
#ifdef USE_STL
	std::vector<T*> data_;
#else
	vector<T> data_;
#endif
#else
  T* data_;
#endif
  int size_;
  int allocsize_;

  Array(const Array& a);		// copy constructor
  Array& operator=(const Array&); 	// assign operator
};

#endif // USE_OWN_ARRAY

#endif // ACS_APGCC_ADH_Array_H

