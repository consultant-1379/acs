/*
 * APGCC_Command.cpp
 *
 *  Created on: Apr 8, 2010
 *      Author: root
 */


#include "ACS_APGCC_Command.H"
#include "ACS_APGCC_DSD.H"
#include "ACS_APGCC_Util.H"

#include <string>
#include <iostream>
extern "C" {
#  include <stdlib.h>
#  include <stdio.h>
}
#include "ACS_TRA_trace.h"

using namespace std;

namespace {
	ACS_TRA_trace trace(const_cast<char*>("ACS_APGCC_Command"), const_cast<char *>("C300"));
}

//Defined trace points
//--------------------
//Deleted for porting
//ACS_TRA_trace traCmd = ACS_TRA_DEF ("ACS_APGCC_Command", "C200");


//------------
// ACS_APGCC_Array
//------------

ACS_APGCC_Array::Data::Data()
:s_(0)
{
   this->init(0);
}

ACS_APGCC_Array::Data::Data(const Data& d)
:s_(0)
{
   this->init(d.s_);
}

ACS_APGCC_Array::Data::~Data()
{
   ::free(s_);
}

ACS_APGCC_Array::Data& ACS_APGCC_Array::Data::operator=(int n)
{
   char buf[20];
   ::sprintf(buf,"%i",n);
   this->init(buf);
   return *this;
}

ACS_APGCC_Array::Data& ACS_APGCC_Array::Data::operator=(const string& s)
{
   this->init(s.c_str());
   return *this;
}

ACS_APGCC_Array::Data& ACS_APGCC_Array::Data::operator=(const Data& d)
{
   if ( this != &d ) {
      this->init(d.s_);
   }
   return *this;
}

const char* ACS_APGCC_Array::Data::c_str()
{
   return s_;
}

ACS_APGCC_Array::Data::operator int()
{
   return ::atoi(s_);
}

//ACS_APGCC_Array::Data::operator long()
//{
//   return ::atol(s_);
//}

ACS_APGCC_Array::Data::operator const char*()
{
   return s_;
}

//* Comment this out if using std strings
void ACS_APGCC_Array::Data::init(const char* s)
{
   if (s_)
      ::free(s_);
   if (s)
      s_ = ::strdup(s);
   else {
      s_ = ::strdup(""); //new char[1];
      s_[0] = 0;
   }
}
//*/

ACS_APGCC_Array::ACS_APGCC_Array()
:list_(0)
{
   list_ = new DataList;
}

ACS_APGCC_Array::ACS_APGCC_Array(const ACS_APGCC_Array& arr)
:list_(0)
{
   list_ = new DataList;

   // Call assign operator
   this->operator=(arr);
}

ACS_APGCC_Array::~ACS_APGCC_Array()
{
   this->clear();
   delete list_;
}

ACS_APGCC_Array& ACS_APGCC_Array::operator=(const ACS_APGCC_Array& array)
{
   if ( this != &array ) {
      this->clear();

      DataList::iterator it;
      for (it=array.list_->begin(); it!=array.list_->end(); it++) {
         Data* dp = *it;
         list_->push_back( new Data(*dp) );
      }
   }
   return *this;
}

int ACS_APGCC_Array::size()
{
   return list_->size();
}

void ACS_APGCC_Array::clear()
{
   DataList::iterator it;
   for (it=list_->begin(); it!=list_->end(); it++) {
      delete *it;
   }
   list_->erase(list_->begin(), list_->end());
}

const ACS_APGCC_Array::Data& ACS_APGCC_Array::operator[](int i) const
{
   // We cannot increase the number of elements
   // when we're using const index, so throw
   // an exception if index is out of range.
   if ( i >= (int)list_->size() )
      throw std::string("ACS_APGCC_Array: index out of range");

   return *(*list_)[i];
}

ACS_APGCC_Array::Data& ACS_APGCC_Array::operator[](int i)
{
   // As this list will increase "automagically"
   // we must insert elements if the given
   // index is larger than the number of elements
   // we already have.
   while ((int)list_->size() < i+1) {
      list_->push_back( new Data );
   }

   return *(*list_)[i];
}




//------------
// ACS_APGCC_Command
//------------

#define HEADER_SIZE  4


ACS_APGCC_Command::ACS_APGCC_Command()
:cmdCode(0), result(0), encodedSize(0), encodedBuf(0)
{}

ACS_APGCC_Command::ACS_APGCC_Command(const ACS_APGCC_Command& cmd)
  :cmdCode(cmd.cmdCode), result(cmd.result), encodedSize(cmd.encodedSize), encodedBuf(0)
{
	if ( cmd.encodedBuf ) {
		encodedBuf = new unsigned char[encodedSize];
		::memcpy(encodedBuf,cmd.encodedBuf,encodedSize);
	}
	data = cmd.data;
}

ACS_APGCC_Command& ACS_APGCC_Command::operator=(const ACS_APGCC_Command& cmd)
{
	if ( &cmd != this ) {
		cmdCode		= cmd.cmdCode;
		result		= cmd.result;
		encodedSize = cmd.encodedSize;

		if ( encodedBuf ) {
			delete [] encodedBuf;
			encodedBuf = 0;
		}

		if ( cmd.encodedBuf ) {
			encodedBuf = new unsigned char[encodedSize];
			::memcpy(encodedBuf,cmd.encodedBuf,encodedSize);
		}

		data = cmd.data;
	}
	return *this;
}

ACS_APGCC_Command::~ACS_APGCC_Command()
{
   if (encodedBuf) {
      delete [] encodedBuf;
   }
}

void ACS_APGCC_Command::clear()
{
   result=0;
   data.clear();

   if (encodedBuf)
      delete [] encodedBuf;

   encodedBuf = 0;
}

int ACS_APGCC_Command::numOfData()
{
   return data.size();
}

void ACS_APGCC_Command::encode()
{
   // New format:
   // Size:  4      2        2        4       2     1-?     2    1-?
   //      [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:  0-3    4-5      6-7      8-11    12-13    14-?

	int len, i, idx;
	int size = 12;    // start index after header


	for (i=0; i<this->numOfData(); i++) {
		size += 2;
      size += strlen(data[i]);
   }

#ifdef DEBUG_CMD
   cout << "encode(); Calculated buffer size=" << size << endl;
#endif


	encodedSize = size;

	if (encodedBuf != NULL) {
		delete [] encodedBuf;
	}
	encodedBuf = new unsigned char [encodedSize+5]; // Add some extra for DSD headers

   // Decrease for header size
	size -= HEADER_SIZE;

	encodedBuf[0] = (size >> 24) % 256;
	encodedBuf[1] = (size >> 16) % 256;
	encodedBuf[2] = (size >>  8) % 256;
	encodedBuf[3] = size % 256;
	encodedBuf[4] = (cmdCode / 256) % 256;
	encodedBuf[5] = cmdCode % 256;
	encodedBuf[6] = (result < 0 ? ((-result)/256)|0x80 : result/256);
	encodedBuf[7] = (result < 0 ? (-result) % 256 : result % 256);
	encodedBuf[8] = (numOfData() >> 24) % 256;
	encodedBuf[9] = (numOfData() >> 16) % 256;
	encodedBuf[10]= (numOfData() >>  8) % 256;
	encodedBuf[11]= numOfData() % 256;

   // Start index for first string
	idx = 12;

	for (i=0; i<this->numOfData(); i++) {
		len = strlen(data[i]);

      encodedBuf[idx++] = (len / 256) % 256;
      encodedBuf[idx++] = len % 256;

      string tmpstr;

      tmpstr = data[i].c_str();

      for (int j=0; j<len; j++) {
			encodedBuf[idx++] = tmpstr[j];
		}
	}

#ifdef DEBUG_CMD
   int displaySize = (this->getEncodedSize() > 512 ? 512 : this->getEncodedSize());

   ACS_APGCC::dump_hex(encodedBuf,displaySize,"ACS_APGCC_Command::encode(); encoded buffer:");
#endif
//
//	 Deleted for porting
//   if (ACS_TRA_ON(traCmd)) {
//      char traStr[100];
//      ::sprintf(traStr,"encode(); cmd=%i,result=%i,numOfdata=%i,encodedSize=%u",cmdCode,result,this->numOfData(),this->getEncodedSize());
//      ACS_TRA_event(&traCmd,traStr);
//   }

}

void ACS_APGCC_Command::decode(const unsigned char dataBuf[], bool headerIncluded)
{

   // New format:
   // Size:   2        2         4       2     1-?     2    1-?
   //      [cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:   0-1      2-3      4-7      8-9    10-?


   this->clear();
   result = -1;

   if (dataBuf == NULL) {

//      if (ACS_TRA_ON(traCmd)) {
//         ACS_TRA_event(&traCmd,"decode(); ERROR: buffer is NULL");
//      }

      return;
   }


   const unsigned char* buf;

   if ( headerIncluded ) {
      buf = &dataBuf[4];
   }
   else {
      buf = dataBuf;
   }


   int dataCnt, len, idx, cnt;
   int minus = buf[2] & 0x80;


#ifdef DEBUG_CMD
   cout << "--> Decoding..." << endl;
   int displaySize = ( encodedSize > 512 ? 512 : encodedSize );
   MCS::dump_hex(buf,displaySize,"ACS_APGCC_Command::decode(); received buffer:");
#endif

   cmdCode = 256*buf[0] + buf[1];
   result  = 256*(buf[2] & 0x7f) + buf[3];
   dataCnt = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + buf[7];

   if ( minus ) {
      result = -result;
   }


#ifdef DEBUG_CMD
   cout << "cmdCode     = " << cmdCode << endl;
   cout << "result      = " << result << endl;
   cout << "dataCnt     = " << dataCnt << endl;
#endif

   // Start index for first string
   idx = 8;
   cnt = 0;

   int i,j;
   string tmpstr;

   for (i=0; i<dataCnt; i++) {

      tmpstr = "";

      len = 256*buf[idx] + buf[idx+1];
		idx += 2;

      for (j=0; j<len; j++) {
		   tmpstr += char(buf[idx+j]);
		}

      data[cnt++] = tmpstr;

      idx += len;

   }


#ifdef DEBUG_CMD
   cout << "decode: numOfData=" << numOfData() << endl;
   for (i=0; i<numOfData(); i++) {
      cout << "decode: data["<<i<<"] = '" << (const char*) data[i] << "'" << endl;
   }
   cout << "<-- After decode..." << endl;
#endif

//   if (ACS_TRA_ON(traCmd)) {
//      char traStr[100];
//      ::sprintf(traStr,"decode(); cmd=%i,result=%i,numOfData=%i",cmdCode,result,this->numOfData());
//      ACS_TRA_event(&traCmd,traStr);
//   }

}

void ACS_APGCC_Command::getEncodedBuf(unsigned char buf[])
{
   for (int i=0; i<this->getEncodedSize(); i++) {
      buf[i] = encodedBuf[i];
   }
}


int ACS_APGCC_Command::getEncodedSize()
{
   return encodedSize;
}


#ifndef ACS_APGCC_USESERIALIZE

bool ACS_APGCC_Command::Save(ACS_APGCC_IStream* wr)
{
	if (!wr)
		return false;

   // Old format:
	//   2      1        2        2       2     1-?     2    1-?
	// [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
	//  0-1     2       3-4      5-6     7-8    9-?

   // New format:
   // Size:  4      2        2        2       2     1-?     2    1-?
   //      [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:  0-3    4-5      6-7      8-9    10-11    12-?

	// called to get the total length of the data,
	// to be backwards compatible for ACS_APGCC_DSD_Streams.
	int i;
	int size = 10;

	for (i=0; i<this->numOfData(); i++) {
      size += 2;
      size += ::strlen(data[i]);
    }
	//this->encode();

   if ( !wr->Write( &this->encodedSize, sizeof(int) ) )
      return false;

   wr->Write( &this->cmdCode, sizeof(int) );

   int tmp = (this->result < 0 ? (this->result | 0x8000): this->result);
   wr->Write( &tmp, sizeof(int) );

   tmp = this->numOfData();
	wr->Write( &tmp, sizeof(int) );

	// send each data string

	for (i=0; i<this->numOfData(); i++) {
      wr->WriteString( string(data[i]) );
	}

	return true;
}


bool ACS_APGCC_Command::Load(ACS_APGCC_IStream* rd)
{
	if (!rd)
		return false;

   // Old format:
	//   2      1        2        2       2     1-?     2    1-?
	// [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
	//  0-1     2       3-4      5-6     7-8    9-?

   // New format:
   // Size:  4      2        2        2       2     1-?     2    1-?
   //      [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:  0-3    4-5      6-7      8-9    10-11    12-?

	int nData;

   if ( !rd->Read( &nData, sizeof(int) ) )          // total length of data
      return false;

   rd->Read( &this->cmdCode, sizeof(int) );
   rd->Read( &this->result, sizeof(int) );
   rd->Read( &nData, sizeof(int) );


   /*bool minus = (this->result & 0x8000 == 0 ? false:true);*/ //apg43 prior compilation changes:warning(qcharoh)
   bool minus = ((this->result & 0x8000) == 0 ? false:true);//enclosed the expression in the terinary operator in paranthesis

   this->result &= 0x7f00;

   if ( minus )
      this->result = -this->result;

	data.clear();

   string buf;

	for (int i=0; i<nData; i++) {
      rd->ReadString(buf);
		data[i] = buf;
	}

	return true;
}


#else //ACS_APGCC_USESERIALIZE

int ACS_APGCC_Command::writeTo(ACS_APGCC_StreamWriter* wr)
{
	if (!wr)
		return -1;

   // Old format:
	//   2      1        2        2       2     1-?     2    1-?
	// [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
	//  0-1     2       3-4      5-6     7-8    9-?

   // New format:
   // Size:  4      2        2        2       2     1-?     2    1-?
   //      [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:  0-3    4-5      6-7      8-9    10-11    12-?

	// called to get the total length of the data,
	// to be backwards compatible for ACS_APGCC_DSD_Streams.
	int i;
	int size = 10;

	for (i=0; i<this->numOfData(); i++) {
      size += 2;
      size += ::strlen(data[i]);
    }
	//this->encode();

	wr->writeInt ( this->encodedSize );
	wr->writeWord( this->cmdCode );
   wr->writeWord( (this->result < 0 ? (this->result | 0x8000): this->result) );
	wr->writeWord( this->numOfData() );

	// send each data prefixed with its length

	int len;

	for (i=0; i<this->numOfData(); i++) {
		len = ::strlen( data[i] );
		wr->writeWord( len );
		wr->writeString( data[i] );
	}

	return 0;
}


int ACS_APGCC_Command::readFrom(ACS_APGCC_StreamReader* rd)
{
	if (!rd)
		return -1;

   // Old format:
	//   2      1        2        2       2     1-?     2    1-?
	// [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
	//  0-1     2       3-4      5-6     7-8    9-?

   // New format:
   // Size:  4      2        2        2       2     1-?     2    1-?
   //      [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:  0-3    4-5      6-7      8-9    10-11    12-?

	int nData;

	rd->readInt( nData );			// total length of data
	rd->readWord( this->cmdCode );
	rd->readWord( this->result );
	rd->readWord( nData );

   bool minus = (this->result & 0x8000 == 0 ? false:true);

   this->result &= 0x7f00;

   if ( minus )
      this->result = -this->result;

	data.clear();

	char* buf = 0;
	int currLen = 0;
	int len;

	for (int i=0; i<nData; i++) {
		rd->readWord( len );

		if ( len > currLen ) {
			if (buf)
				delete [] buf;
		}
		if (!buf) {
			buf = new char[len+10];
			currLen = len;
		}

		rd->readString( buf, len );
		buf[len] = 0;

		data[i] = buf;
	}

	return 0;
}

#endif //ACS_APGCC_USESERIALIZE


int ACS_APGCC_Command::send(ACS_APGCC_DSD_Stream& stream)
{
   if (getEncodedSize() == 0 || encodedBuf == NULL) {
	   encode();
   }
   if (stream.send_n(encodedBuf, getEncodedSize()) <= 0) {
      return -1;
   }
	return 0;
}


int ACS_APGCC_Command::recv(ACS_APGCC_DSD_Stream& stream)
{
   unsigned char* msgBuf = 0;

   clear();
   result = -1;


#ifdef DSD_R2  // @@

   int size;
   unsigned char head[HEADER_SIZE];

   if (stream.recv_n(head, HEADER_SIZE) <= 0) {
      return -1;
   }

   // Header of 4 bytes
   // Note that DSD sets the highest bit to one, so
   // best to clear it.

   size = ((head[0] & 0x7f) << 24) + (head[1] << 16) + (head[2] << 8) + head[3];

   if (size <= 0) {
      return -1;
   }

   msgBuf = new unsigned char [size+10];

   if (stream.recv_n(msgBuf, size) <= 0) {
      delete [] msgBuf;
      return -1;
   }

   encodedSize = size;

#else // !DSD_R2

   // Receive message buffer
   if ( (encodedSize=stream.recvBuffer(msgBuf)) < 0 ) {
		   //cout<<"recv(); ERROR: recvBuffer() failed"<<endl;
		   if (trace.ACS_TRA_ON()) {

		        				trace.ACS_TRA_event(1, "recv(); ERROR: recvBuffer() failed");
		      		   }

      return -1;
   }

      //OSF_HEX_DUMP((LM_DEBUG,(const char*)msgBuf,(len>256?256:len),"(%t) ACS_APGCC_Command::recv(); DSD.recvBuffer() returns:"));

#endif // DSD_R2

   // Decode received buffer
   this->decode(msgBuf,true);


   if ( msgBuf ) {
      delete [] msgBuf;
   }

   return 0;
}



void ACS_APGCC_Command::dump(const char* s)
{
#ifndef NDEBUG
   int len, cnt, i;
   int idx;

   cout << "- - - - - d u m p - - - - -" << endl;
   if ( s )
      cout << s << endl;
   else
      cout << "ACS_APGCC_Command::dump()" << endl;

   cout << "cmdCode   = " << cmdCode << endl;
   cout << "result    = " << result << endl;
   cout << "numOfData = " << this->numOfData() << endl;
   cout << "data.size = " << this->data.size() << endl;
   for (i=0; i<this->numOfData(); i++) {
      cout << "data["<<i<<"] = " << (const char*)this->data[i] << endl;
   }
   cout << endl << endl;

   // Old format:
	//   2      1        2        2       2     1-?     2    1-?
	// [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
	//  0-1     2       3-4      5-6     7-8    9-?

   // New format:
   // Size:  4      2        2        2       2     1-?     2    1-?
   //      [len][cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:  0-3    4-5      6-7      8-9    10-11    12-?

   if ( encodedBuf == NULL ) {
     this->encode();
   }

   cout << "Encoded format:" << endl;
   cout << "encodedSize   = " << encodedSize << endl;
   cout << "encodedBuf[0] = " << (int)encodedBuf[0] << ", size.byte3" << endl;
   cout << "encodedBuf[1] = " << (int)encodedBuf[1] << ", size.byte2" << endl;
   cout << "encodedBuf[2] = " << (int)encodedBuf[2] << ", size.byte1" << endl;
   cout << "encodedBuf[3] = " << (int)encodedBuf[3] << ", size.byte0, size=";
   len = 256*256*256*encodedBuf[0] + 256*256*encodedBuf[1] + 256*encodedBuf[2] + encodedBuf[3];
   cout << len << endl;
   cout << "encodedBuf[4] = " << (int)encodedBuf[4] << ", cmdCode.high" << endl;
   cout << "encodedBuf[5] = " << (int)encodedBuf[5] << ", cmdCode.low, cmdCode=";
   len = 256*encodedBuf[4] + encodedBuf[5];
   cout << len << endl;
   cout << "encodedBuf[6] = " << (int)encodedBuf[6] << ", result.high" << endl;
   cout << "encodedBuf[7] = " << (int)encodedBuf[7] << ", result.low, result=";
   len = 256*encodedBuf[6] + encodedBuf[7];
   cout << len << endl;
   cout << "encodedBuf[8] = " << (int)encodedBuf[8] << ", numOfData.byte3" << endl;
   cout << "encodedBuf[9] = " << (int)encodedBuf[9] << ", numOfData.byte2" << endl;
   cout << "encodedBuf[10] = " <<(int)encodedBuf[10]<< ", numOfData.byte1" << endl;
   cout << "encodedBuf[11] = " <<(int)encodedBuf[11]<< ", numOfData.byte0, numOfData=";

   // Calculate number of data items
   cnt = (encodedBuf[8] << 24) + (encodedBuf[9] << 16) + (encodedBuf[10] << 8) + encodedBuf[11];
   cout << cnt << endl;

   // Start index for first string
   idx = 12;

   for (i=0; i<cnt; i++) {

      len = 256*encodedBuf[idx] + encodedBuf[idx+1];
	   idx += 2;

      cout << "encodedBuf[" << idx-2 << "] = " << (int)encodedBuf[idx-2]
	        << ", strlen.high" << endl;
      cout << "encodedBuf[" << idx-1 << "] = " << (int)encodedBuf[idx-1]
	        << ", strlen.low" << endl;

      for (int j=0; j<len; j++) {
          cout << "encodedBuf[" << idx+j << "] = " << encodedBuf[idx+j] << endl;
	   }
      idx += len;
   }

   cout << "- - -  e n d  d u m p - - -" << endl;
#endif //NDEBUG
}








