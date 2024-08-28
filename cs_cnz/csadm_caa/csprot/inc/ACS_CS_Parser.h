

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_Parser_h
#define ACS_CS_Parser_h 1


#include "ACS_CS_Protocol.h"
#include <queue>

// ACS_CS_PDU
#include "ACS_CS_PDU.h"


class ACS_CS_Header;
class ACS_CS_PDU;

using std::queue;








class ACS_CS_Parser 
{

		struct DataBuffer
	  {
		  char * buffer;
		  int size;
	  };


  public:
      ACS_CS_Parser(ACS_CS_Protocol::CS_Protocol_Type type);

      virtual ~ACS_CS_Parser();


      int newData (const char *buffer, int size);

      void flush ();

      bool getPDU (ACS_CS_PDU &pdu);

      int parse ();

      int getBufferSize () const;

      int getPDUQueue () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_Parser(const ACS_CS_Parser &right);

      ACS_CS_Parser & operator=(const ACS_CS_Parser &right);


      int getData (char *buffer, int size);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

      queue<DataBuffer * > bufferQueue;

      queue<ACS_CS_PDU * > pduQueue;

       int bufferSize;

       ACS_CS_HeaderBase *currentHeader;

       ACS_CS_Protocol::CS_Protocol_Type protocolType;

    // Additional Implementation Declarations

};


// Class ACS_CS_Parser 



#endif
