//******************************************************************************
//
// NAME
// acs_rtr_msread.h
//
// COPYRIGHT Ericsson AB, Sweden 2012.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
// AUTHOR 
// 2012-12-10 by XLANSRI
//******************************************************************************

#ifndef ACS_RTR_MSread_h
#define ACS_RTR_MSread_h

#include "acs_aca_message_store_client.h"//<ACS_ACA_MessageStoreAccessPoint.H>



class MSread :public ACS_ACA_MessageStoreClient//ACS_ACA_MessageStoreAccessPoint
{
public:
	MSread(const char* MSname, unsigned cpSystemId=~0U);
	MSread(const char* MSname, const char* CPsite);
    ~MSread();
	bool getLastCommittedRTRfileNumber(unsigned int& lcf);
	bool getLastBlockNumberFromMS(unsigned int& bno);
	bool getMSbuf(ACS_ACA_MessageStoreMessage::State& msgState,
		          unsigned int& bufln,
				  unsigned char*& buf,
				  bool extraDataReq,
				  int& additionalData);

    bool alignMessageStore(unsigned int blockNo,
		                   unsigned int noOfMess,
						   unsigned int& alignedSize);
	bool commitBlockTransfer(unsigned int bno);
	bool MSconnect(void);
	void MSdisconnect(void);
	bool connectedToMSD(void);
	bool disconnectOnShutdown();
private:
	bool MSconnection;
	ACE_Recursive_Thread_Mutex _msSync;
};

#endif
