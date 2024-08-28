/*
 * ACS_APGCC_CmdParams_R1.cpp
 *
 *  Created on: Apr 14, 2010
 *      Author: root
 */

//#include "mcc_warning_level4.h"
#include "ACS_APGCC_CmdParams_R1.H"


#include <string>
#ifdef _DEBUG
#  undef DEBUG_CMDPAR
#  include <iostream>
#endif

using namespace std;




//Defined trace points
//--------------------
//ACS_TRA_trace traCmdPar = ACS_TRA_DEF ("ACS_APGCC_CmdParams_R1", "C200");



//------------
// ACS_APGCC_CmdParams_R1
//------------

#define HEADER_SIZE  sizeof(DWORD)


ACS_APGCC_CmdParams_R1::ACS_APGCC_CmdParams_R1()
:cmdCode(0), result(0), encodedSize_(0), encodedBuf_(0)
{
}


ACS_APGCC_CmdParams_R1::ACS_APGCC_CmdParams_R1(const ACS_APGCC_CmdParams_R1& cmd)
  :cmdCode(cmd.cmdCode), result(cmd.result), encodedSize_(cmd.encodedSize_), encodedBuf_(0)
{
	if ( cmd.encodedBuf_ ) {
		encodedBuf_ = new unsigned char[encodedSize_];
		::memcpy(encodedBuf_,cmd.encodedBuf_,encodedSize_);
	}
	data = cmd.data;
}


ACS_APGCC_CmdParams_R1& ACS_APGCC_CmdParams_R1::operator=(const ACS_APGCC_CmdParams_R1& cmd)
{
	if ( &cmd != this ) {
		cmdCode		= cmd.cmdCode;
		result		= cmd.result;
		encodedSize_ = cmd.encodedSize_;

		if ( encodedBuf_ ) {
			delete [] encodedBuf_;
			encodedBuf_ = 0;
		}

		if ( cmd.encodedBuf_ ) {
			encodedBuf_ = new unsigned char[encodedSize_];
			::memcpy(encodedBuf_,cmd.encodedBuf_,encodedSize_);
		}

		data = cmd.data;
	}
	return *this;
}


ACS_APGCC_CmdParams_R1::~ACS_APGCC_CmdParams_R1()
{
   if (encodedBuf_) {
      delete [] encodedBuf_;
   }
}


void ACS_APGCC_CmdParams_R1::clear()
{
   result=0;
   data.clear();

   if (encodedBuf_)
      delete [] encodedBuf_;

   encodedBuf_ = 0;
}


// Returns the number of command data
//------------------------------------
int ACS_APGCC_CmdParams_R1::numOfData()
{
   return this->data.size();
}


// Encodes the command data into a buffer
//-----------------------------------------
void ACS_APGCC_CmdParams_R1::encode()
{

   //----------------------------------------------------------------------------
   // Encoded format:
   //
   // Size:      1         4       4        4        2     1-?     2    1-?
   //      [headerSize][cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:       0        1-4     5-8      9-12    13-14   15-?
   //----------------------------------------------------------------------------

   BYTE HeaderSize = 1 + 4 + 4 + 4;
	int i;

   // Calculate size of command data
   // Each string can be upto 65535 characters

   this->encodedSize_ = 0;

	for (i=0; i<this->numOfData(); i++) {
		this->encodedSize_ += 2;
      this->encodedSize_ += strlen(data[i]);
   }

   // Add the size of the header
   this->encodedSize_ += HeaderSize;


#ifdef DEBUG_CMDPAR
   cout << "encode(); Calculated buffer size=" << this->encodedSize_ << endl;
#endif


   // Allocate large enough buffer
	if ( encodedBuf_ ) {
		delete [] encodedBuf_;
	}
	encodedBuf_ = new unsigned char [ this->encodedSize_ + 1 ];

   // Fill the buffer
   encodedBuf_[0] = HeaderSize;
	encodedBuf_[1] = static_cast<unsigned char>( (cmdCode >> 24) % 256 );
	encodedBuf_[2] = static_cast<unsigned char>( (cmdCode >> 16) % 256 );
	encodedBuf_[3] = static_cast<unsigned char>( (cmdCode >>  8) % 256 );
	encodedBuf_[4] = static_cast<unsigned char>( cmdCode % 256 );
	encodedBuf_[5] = static_cast<unsigned char>( (result >> 24) % 256 );
	encodedBuf_[6] = static_cast<unsigned char>( (result >> 16) % 256 );
	encodedBuf_[7] = static_cast<unsigned char>( (result >>  8) % 256 );
	encodedBuf_[8] = static_cast<unsigned char>( result % 256 );
	encodedBuf_[9] = static_cast<unsigned char>( (numOfData() >> 24) % 256 );
	encodedBuf_[10]= static_cast<unsigned char>( (numOfData() >> 16) % 256 );
	encodedBuf_[11]= static_cast<unsigned char>( (numOfData() >>  8) % 256 );
	encodedBuf_[12]= static_cast<unsigned char>( numOfData() % 256 );

   // Fill in the command data string
   // Start index for first string
	int idx = 13;
   int len;

	for (i=0; i<this->numOfData(); i++) {
		len = strlen(data[i]);

      encodedBuf_[idx++] = static_cast<unsigned char>( (len / 256) % 256 );
      encodedBuf_[idx++] = static_cast<unsigned char>( len % 256 );

      // Copy the string
      ::memcpy( &encodedBuf_[idx], (const char*)data[i], len );

      idx += len;
	}

#ifdef DEBUG_CMDPAR
   int displaySize = (this->getEncodedSize() > 512 ? 512 : this->getEncodedSize());

   ACS_APGCC::dump_hex(encodedBuf_,displaySize,"ACS_APGCC_CmdParams_R1::encode(); encoded buffer:");
#endif
   /** PR_01
   if (ACS_TRA_ON(traCmd)) {
      char traStr[100];
      ::sprintf(traStr,"encode(); cmd=%i,result=%i,numOfdata=%i,encodedSize_=%u",cmdCode,result,this->numOfData(),this->getEncodedSize());
      ACS_TRA_event(&traCmd,traStr);
   }
   **/

}


void ACS_APGCC_CmdParams_R1::decode()
{
   // Clear internal structures
   result = -1;

   if ( this->encodedBuf_ == NULL ) {
      return;
   }

   //----------------------------------------------------------------------------
   // Encoded format:
   //
   // Size:      1         4       4        4        2     1-?     2    1-?
   //      [headerSize][cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:       0        1-4     5-8      9-12    13-14   15-?
   //----------------------------------------------------------------------------

   int dataCnt, len, idx, cnt;
   const unsigned char* buf = this->encodedBuf_;   // Lazy, lazy


#ifdef DEBUG_CMDPAR
   cout << "--> Decoding..." << endl;
   int displaySize = ( encodedSize_ > 512 ? 512 : encodedSize_ );
   ACS_APGCC::dump_hex(buf,displaySize,"ACS_APGCC_CmdParams_R1::decode(); received buffer:");
#endif

   BYTE HeaderSize = buf[0];

   // Extract values
   cmdCode = (buf[1] << 24) + (buf[2] << 16) + (buf[3] << 8) + buf[4];
   result  = (buf[5] << 24) + (buf[6] << 16) + (buf[7] << 8) + buf[8];
   dataCnt = (buf[9] << 24) + (buf[10] << 16) + (buf[11] << 8) + buf[12];


#ifdef DEBUG_CMDPAR
   cout << "cmdCode     = " << cmdCode << endl;
   cout << "result      = " << result << endl;
   cout << "dataCnt     = " << dataCnt << endl;
#endif

   // Start index for first string
   idx = HeaderSize;
   cnt = 0;

   int i,j;
   string tmpstr;

   for (i=0; i<dataCnt; i++) {

      tmpstr = "";

      len = 256*buf[idx] + buf[idx+1];
		idx += 2;

      for (j=0; j<len; j++) {
		   tmpstr += (char)buf[idx+j];
		}

      data[cnt++] = tmpstr;

      idx += len;
   }


#ifdef DEBUG_CMDPAR
   cout << "decode: numOfData=" << this->numOfData() << endl;
   for (i=0; i<this->numOfData(); i++) {
      cout << "decode: data["<<i<<"] = '" << (const char*) data[i] << "'" << endl;
   }
   cout << "<-- After decode..." << endl;
#endif

   /* PR_01
   if (ACS_TRA_ON(traCmd)) {
      char traStr[100];
      ::sprintf(traStr,"decode(); cmd=%i,result=%i,numOfData=%i",cmdCode,result,this->numOfData());
      ACS_TRA_event(&traCmd,traStr);
   }
   */
}


// Returns the encoded buffer
//-----------------------------------------
const char* ACS_APGCC_CmdParams_R1::getEncodedBuf() const
{
   return reinterpret_cast<const char*>(this->encodedBuf_);
}


// Returns the size of the encoded data
//-----------------------------------------
int ACS_APGCC_CmdParams_R1::getEncodedSize() const
{
   return this->encodedSize_;
}


// Saves the command data into the stream
//-----------------------------------------
bool ACS_APGCC_CmdParams_R1::Save(ACS_APGCC_IStream* wr)
{
	if (!wr)
		return false;

   // Encode the data
   this->encode();


   DWORD msgSize = this->encodedSize_;

   if ( msgSize > 0 && this->encodedBuf_ ) {

      // Write message size
      if ( !wr->Write( &msgSize, sizeof(msgSize) ) )
         return false;

      // Write command data
      if ( !wr->Write( this->encodedBuf_, msgSize ) )
         return false;

      return true;
   }

	return false;
}


// Loads the command data from the stream
//-----------------------------------------
bool ACS_APGCC_CmdParams_R1::Load(ACS_APGCC_IStream* rd)
{
	if (!rd)
		return false;

   // Clear data fields
	data.clear();

	DWORD msgSize = 0;

   // Read total length of message
   if ( !rd->Read( &msgSize, sizeof(msgSize) ) ) {
      return false;
   }

   // Allocate buffer
   if ( this->encodedBuf_ ) {
      delete [] this->encodedBuf_;
   }
   this->encodedBuf_ = new unsigned char[msgSize];
   this->encodedSize_ = msgSize;


   // Read command data
   if ( !this->encodedBuf_ || !rd->Read(this->encodedBuf_,msgSize) ) {
      return false;
   }


   // Decode data
   this->decode();

	return true;
}



void ACS_APGCC_CmdParams_R1::dump(const char* s, bool dump_encodedbuf)
{

	ACE_UNUSED_ARG(s);
	ACE_UNUSED_ARG(dump_encodedbuf);
	#ifdef _DEBUG
	int len, cnt, i;
	int idx;

   cout << "- - - - - d u m p - - - - -" << endl;
   if ( s )
      cout << s << endl;
   else
      cout << "ACS_APGCC_CmdParams_R1::dump()" << endl;

   cout << "cmdCode   = " << cmdCode << endl;
   cout << "result    = " << result << endl;
   cout << "numOfData = " << this->numOfData() << endl;
   cout << "data.size = " << this->data.size() << endl;
   for (i=0; i<this->numOfData(); i++) {
      cout << "data["<<i<<"] = " << (const char*)this->data[i] << endl;
   }

   if ( !dump_encodedbuf )
      goto End;

   cout << endl << endl;

   //----------------------------------------------------------------------------
   // Encoded format:
   //
   // Size:      1         4       4        4        2     1-?     2    1-?
   //      [headerSize][cmdCode][result][numOfData][len1][data1][len2][data2]...
   // Pos:       0        1-4     5-8      9-12    13-14   15-?
   //----------------------------------------------------------------------------

   this->encode();

   cout << "Encoded format:" << endl;
   cout << "encodedSize_   = " << encodedSize_ << endl;
   cout << "encodedBuf_[0] = " << (int)encodedBuf_[0] << ", header size" << endl;
   cout << "encodedBuf_[1] = " << (int)encodedBuf_[1] << ", cmdCode.byte3" << endl;
   cout << "encodedBuf_[2] = " << (int)encodedBuf_[2] << ", cmdCode.byte2" << endl;
   cout << "encodedBuf_[3] = " << (int)encodedBuf_[3] << ", cmdCode.byte1" << endl;
   cout << "encodedBuf_[4] = " << (int)encodedBuf_[4] << ", cmdCode.byte0, cmdCode=";
   cnt = (encodedBuf_[1] << 24) + (encodedBuf_[2] << 16) + (encodedBuf_[3] << 8) + encodedBuf_[4];
   cout << cnt << endl;

   cout << "encodedBuf_[5] = " << (int)encodedBuf_[5] << ", result.byte3" << endl;
   cout << "encodedBuf_[6] = " << (int)encodedBuf_[6] << ", result.byte2" << endl;
   cout << "encodedBuf_[7] = " << (int)encodedBuf_[7] << ", result.byte1" << endl;
   cout << "encodedBuf_[8] = " << (int)encodedBuf_[8] << ", result.byte0, result=";
   cnt = (encodedBuf_[5] << 24) + (encodedBuf_[6] << 16) + (encodedBuf_[7] << 8) + encodedBuf_[8];
   cout << cnt << endl;

   cout << "encodedBuf_[9] = " << (int)encodedBuf_[9] << ", numOfData.byte3" << endl;
   cout << "encodedBuf_[10]= " << (int)encodedBuf_[10]<< ", numOfData.byte2" << endl;
   cout << "encodedBuf_[11]= " << (int)encodedBuf_[11]<< ", numOfData.byte1" << endl;
   cout << "encodedBuf_[12]= " << (int)encodedBuf_[12]<< ", numOfData.byte0, numOfData=";

   // Calculate number of data items
   cnt = (encodedBuf_[9] << 24) + (encodedBuf_[10] << 16) + (encodedBuf_[11] << 8) + encodedBuf_[12];
   cout << cnt << endl;

   // Start index for first string
   idx = (int)encodedBuf_[0];

   for (i=0; i<cnt; i++) {

      len = 256*encodedBuf_[idx] + encodedBuf_[idx+1];
	   idx += 2;

      cout << "encodedBuf_[" << idx-2 << "] = " << (int)encodedBuf_[idx-2]
	        << ", strlen.high" << endl;
      cout << "encodedBuf_[" << idx-1 << "] = " << (int)encodedBuf_[idx-1]
	        << ", strlen.low" << endl;

      for (int j=0; j<len; j++) {
          cout << "encodedBuf_[" << idx+j << "] = " << encodedBuf_[idx+j] << endl;
	   }
      idx += len;
   }

End:
   cout << "- - -  e n d  d u m p - - -" << endl;
#endif //_DEBUG
}

