//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have been supplied.
//
//	*********************************************************


#ifndef ACS_CS_GLKHandler_h
#define ACS_CS_GLKHandler_h 1

#include "ACS_CS_TableHandler.h"
#include "ace/RW_Mutex.h"

class ACS_CS_Table;
class ACS_CS_TableEntry;

using std::string;

struct ACS_CS_GLC_DATA;
typedef std::vector<ACS_CS_GLC_DATA> glcVector;


//	This is a class that handles the GLK table. All
//	requests about this table are sent to an instance of
//	this class. The class manages the table when entries are
//	added and removed and saves the table to disk when it
//	has changed.



class ACS_CS_GLKHandler : public ACS_CS_TableHandler
{

  public:

      ACS_CS_GLKHandler();

      virtual ~ACS_CS_GLKHandler();

      virtual int handleRequest (ACS_CS_PDU *pdu);
	  
	  virtual int newTableOperationRequest(ACS_CS_ImModelSubset *subset);

      virtual bool loadTable ();

  private:

      ACS_CS_GLKHandler(const ACS_CS_GLKHandler &right);

      ACS_CS_GLKHandler & operator=(const ACS_CS_GLKHandler &right);
              
      int createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result);
      	  
	  bool getGLCData(glcVector&  glcInfo);

      int handleGetGlobalLogicalClock(ACS_CS_PDU *pdu);

      short unsigned logicalClock;

      ACE_RW_Mutex lock_;     

};


	


#endif
