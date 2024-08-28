//	Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_TableHandler_h
#define ACS_CS_TableHandler_h 1

#include "ace/RW_Mutex.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_ImModelSubset.h"

class ACS_CS_PDU;
class ACS_CS_TableRequest;


using std::map;

typedef  std::map<ACS_CS_Protocol::CS_Scope_Identifier,uint16_t> clockMap;

class ACS_CS_TableHandler 
{
  public:
      ACS_CS_TableHandler();

      virtual ~ACS_CS_TableHandler();

      static clockMap globalClockMap;

      virtual int handleRequest (ACS_CS_PDU *pdu) = 0;

      virtual int newTableOperationRequest (ACS_CS_ImModelSubset *subset);

      virtual bool loadTable () = 0;

      static void setGlobalLogicalClock(ACS_CS_Protocol::CS_Scope_Identifier scope, uint16_t clockValue);

      static  clockMap getGlobalLogicalClockMap();

      //static  uint16_t  getGlobalLogicalClock(ACS_CS_Protocol::CS_Scope_Identifier);


  protected:

      uint16_t logicalClock;

  private:
  

      ACS_CS_TableHandler(const ACS_CS_TableHandler &right);

      ACS_CS_TableHandler & operator=(const ACS_CS_TableHandler &right);
	  
	  static ACE_RW_Mutex glcMutex;
};

#endif
