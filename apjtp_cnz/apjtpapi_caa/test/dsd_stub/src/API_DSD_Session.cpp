/*=================================================================== */
   /**
   @file API_DSD_Session.cpp

   Class method implementation for DSD module.

   This module contains the implementation of class declared in
   the API_DSD_Session.h module

   @version N.N.N

   @documentno CAA 109 0870

   @copyright Ericsson AB, Sweden 2010. All rights reserved.

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       10/08/2010     TA       Initial Release
   **/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "API_DSD_Session.h"
#include "global.h"

using namespace std;

/*===================================================================
   ROUTINE: ACS_DSD_Session
=================================================================== */
ACS_DSD_Session::ACS_DSD_Session() : ACS_DSD_API_Base()
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Session::ACS_DSD_Session()\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Session object created successfully.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Session::ACS_DSD_Session()\n")));
}

/*===================================================================
   ROUTINE: ~ACS_DSD_Session
=================================================================== */
ACS_DSD_Session::~ACS_DSD_Session()
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Session::~ACS_DSD_Session()\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Session object destroyed successfully.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Session::~ACS_DSD_Session()\n")));
}

/*===================================================================
   ROUTINE: send
=================================================================== */
bool ACS_DSD_Session::send(const void* msg, ACE_UINT32 msgSize, ACE_UINT32 timeout)
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Session::send\n")));
	if( msgSize > 0 )
	{
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Sssion: Message is sent successfully.\n")));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Session::send\n")));
		return true;
	}
	else
	{
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Sssion: Message not sent successfully.\n")));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Session::send\n")));
		return false;
	}

}

/*===================================================================
   ROUTINE: recv
=================================================================== */
bool ACS_DSD_Session::recv(void* msg, ACE_UINT32& msgSize, ACE_UINT32 timeout )
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Session::recv\n")));

	if( mMsgType == DSD_API::JINITREQ) //REGISTER
	{
		ACE_TCHAR* mMsg = (ACE_TCHAR*)msg;

		ACE_OS::memset(mMsg,0,sizeof(mMsg));

		//Copy the buffer at 28th locaton.
		ACE_UINT16 ABufLen = short2AXE(sizeof("hello job"));
		ACE_OS::memcpy(&mMsg[28],"hello job",ABufLen);

		ACE_UINT32 len = (28 + ABufLen); //new version (18 bytes for zero)
		//ACE_TCHAR ProtocolVersion = 10;
		ACE_TCHAR ProtocolVersion = 1;
		ACE_TCHAR M = 6;
		//ACE_TCHAR M = 10;	//Invalid Message Type
		ACE_UINT16 Au1 = short2AXE(10);		// Swap Unix short to AXE format
		ACE_UINT16 Au2 = short2AXE(11);

		ACE_UINT16 AMaxBuf = short2AXE(600);
		//copy in Buffer
		ACE_OS::memcpy(&mMsg[0], &M, 1);
		ACE_OS::memcpy(&mMsg[1], &ProtocolVersion, 1);
		ACE_OS::memcpy(&mMsg[2], &Au1, 2);
		ACE_OS::memcpy(&mMsg[4], &Au2, 2);
		ACE_OS::memcpy(&mMsg[6], &ABufLen, 2);
		ACE_OS::memcpy(&mMsg[8], &AMaxBuf, 2);
		msgSize = len;
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nSending jinitreq with U1:10, U2:11\n")));
		//Set the msgType to 1 : JINITREQ
		//mMsgType = DSD_API::JRESULTREQ; //JINITREQ
	}
	else if( mMsgType == DSD_API::JEXINITREQ) //REGISTER
	{
		ACE_TCHAR* mMsg = (ACE_TCHAR*)msg;

		ACE_OS::memset(mMsg,0,sizeof(mMsg));

		//	Conversation object.
		ACE_UINT32 len = 16; //new version (18 bytes for zero)
		//ACE_TCHAR ProtocolVersion = 10;
		ACE_TCHAR ProtocolVersion = 1;
		ACE_TCHAR M = 1;
		//ACE_TCHAR M = 10;	//Invalid Message Type
		ACE_UINT16 Au1 = short2AXE(10);		// Swap Unix short to AXE format
		ACE_UINT16 Au2 = short2AXE(11);

		ACE_UINT16 AMaxBuf = short2AXE(600);
		int temp = 0;
		//copy in Buffer
		ACE_OS::memcpy(&mMsg[0], &M, 1);
		ACE_OS::memcpy(&mMsg[1], &ProtocolVersion, 1);
		ACE_OS::memcpy(&mMsg[2], &Au1, 2);
		ACE_OS::memcpy(&mMsg[4], &Au2, 2);
		ACE_OS::memcpy(&mMsg[6], &AMaxBuf, 2);
		ACE_OS::memcpy(&mMsg[8], &temp, 1);
		ACE_OS::memcpy(&mMsg[9], &temp, 1);
		ACE_OS::memcpy(&mMsg[10], &temp, 1);
		ACE_OS::memcpy(&mMsg[11], &temp, 1);
		ACE_OS::memcpy(&mMsg[12], &temp, 2);
		ACE_OS::memcpy(&mMsg[14], &temp, 2);
		msgSize = len;
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nSending jexinitreq with U1:10, U2:11\n")));

	}
	else if( mMsgType == DSD_API::JINITRSP )
	{
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nFetched jinitind\n")));
		ACE_TCHAR* mMsg = (ACE_TCHAR*)msg;

		ACE_OS::memset(mMsg,0,sizeof(mMsg));

		ACE_UINT32 len = 8; //new version (18 bytes for zero)
		//ACE_TCHAR ProtocolVersion = 10;
		ACE_TCHAR ProtocolVersion = 1;
		ACE_TCHAR M = 7;
		//ACE_TCHAR M = 10;	//Invalid Message Type
		ACE_UINT16 Au1 = short2AXE(10);		// Swap Unix short to AXE format
		ACE_UINT16 Au2 = short2AXE(11);
		ACE_UINT16 Ar = short2AXE(0);

		//copy in Buffer
		ACE_OS::memcpy(&mMsg[0], &M, 1);
		ACE_OS::memcpy(&mMsg[1], &ProtocolVersion, 1);
		ACE_OS::memcpy(&mMsg[2], &Au1, 2);
		ACE_OS::memcpy(&mMsg[4], &Au2, 2);
		ACE_OS::memcpy(&mMsg[6], &Ar, 2);
		msgSize = len;
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nSending jinitrsp with U1:10, U2:11, R:0\n\n")));

	}
	else if( mMsgType == DSD_API::JEXINITRSP)
	{
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nFetched jexinitind\n")));
		ACE_TCHAR* mMsg = (ACE_TCHAR*)msg;

		ACE_OS::memset(mMsg,0,sizeof(mMsg));

		ACE_UINT32 len = 8; //new version (18 bytes for zero)

		ACE_TCHAR ProtocolVersion = 1;
		ACE_TCHAR M = 2;

		ACE_UINT16 Au1 = short2AXE(10);		// Swap Unix short to AXE format
		ACE_UINT16 Au2 = short2AXE(11);
		ACE_UINT16 Ar = short2AXE(0);
		ACE_UINT16 TypeOfDataTrans = 0;

		//copy in Buffer
		ACE_OS::memcpy(&mMsg[0], &M, 1);
		ACE_OS::memcpy(&mMsg[1], &ProtocolVersion, 1);
		ACE_OS::memcpy(&mMsg[2], &Au1, 2);
		ACE_OS::memcpy(&mMsg[4], &Au2, 2);
		ACE_OS::memcpy(&mMsg[6], &Ar, 2);
		ACE_OS::memcpy(&mMsg[8], &TypeOfDataTrans, 1);

		msgSize = len;
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nSending jexinitrsp with U1:10, U2:11, R:0\n\n")));
	}
	else if ( mMsgType == DSD_API::JRESULTREQ )
	{
		ACE_TCHAR* mMsg = (ACE_TCHAR*)msg;

		ACE_OS::memset(mMsg,0,sizeof(mMsg));

		//Copy the buffer at 28th locaton.
		ACE_UINT16 ABufLen = short2AXE(sizeof("Job Result Request"));
		ACE_OS::memcpy(&mMsg[28],"Job Result Request",ABufLen);

		ACE_UINT32 len = (28 + ABufLen); //new version (18 bytes for zero)

		ACE_TCHAR M = 8;
		ACE_UINT8 zero = 0;

		ACE_UINT16 Au1 = short2AXE(10);		// Swap Unix short to AXE format
		ACE_UINT16 Au2 = short2AXE(11);
		ACE_UINT16 Ar = short2AXE(0);

		//copy in Buffer
		ACE_OS::memcpy(&mMsg[0], &M, 1);
		ACE_OS::memcpy(&mMsg[1], &zero , 1);
		ACE_OS::memcpy(&mMsg[2], &Au1, 2);
		ACE_OS::memcpy(&mMsg[4], &Au2, 2);
		ACE_OS::memcpy(&mMsg[6], &Ar, 2);
		ACE_OS::memcpy(&mMsg[8], &ABufLen, 2);
		msgSize = len;
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nRecieved jresultreq with U1:10, U2:11, R:0\n")));

	}
	else if( mMsgType == DSD_API::JEXDATAREQ )
	{
		ACE_TCHAR* mMsg = (ACE_TCHAR*)msg;

		ACE_OS::memset(mMsg,0,sizeof(mMsg));

		//Copy the buffer at 28th locaton.
		ACE_UINT16 ABufLen = short2AXE(sizeof("Conversation Data Request"));
		ACE_OS::memcpy(&mMsg[28],"Conversation Data Request",ABufLen);

		ACE_UINT32 len = (28 + ABufLen); //new version (18 bytes for zero)

		ACE_TCHAR M = 3;

		ACE_UINT16 Au1 = short2AXE(10);		// Swap Unix short to AXE format
		ACE_UINT16 Au2 = short2AXE(11);

		ACE_OS::memcpy(&mMsg[0], &M, 1);
		ACE_OS::memcpy(&mMsg[2], &Au1, 2);
		ACE_OS::memcpy(&mMsg[4], &Au2, 2);
		ACE_OS::memcpy(&mMsg[6], &ABufLen, 2);
		msgSize = len;
//		ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nRecieved jexdatareq with U1:10, U2:11, R:0\n")));
	}
	else if(mMsgType == DSD_API::JEXDISCREQ)
	{
		ACE_TCHAR* mMsg = (ACE_TCHAR*)msg;

		ACE_OS::memset(mMsg,0,sizeof(mMsg));
		ACE_TCHAR M = 5;
		ACE_UINT16 Au1 = short2AXE(10);		// Swap Unix short to AXE format
		ACE_UINT16 Au2 = short2AXE(11);
		ACE_UINT16 Ar = short2AXE(0);		// Swap Unix short to AXE format

		ACE_OS::memcpy(&mMsg[0], &M, 1);
		ACE_OS::memcpy(&mMsg[2], &Au1, 2);
		ACE_OS::memcpy(&mMsg[4], &Au2, 2);
		ACE_OS::memcpy(&mMsg[6], &Ar, 2);
		msgSize = 8;

	}

	//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Message size :%d\n"),msgSize));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Message:%s\n"), (ACE_TCHAR*)msg));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Session: Message received successfully.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Session::recv\n")));
	return true;
}

/*===================================================================
   ROUTINE: close
=================================================================== */
void ACS_DSD_Session::close(void)
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Session::close\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Session::close\n")));
}

