/*=================================================================== */
/**
   @file  acs_aca_message_store_message.h 

   @brief Header file for API module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       28/01/2013   XHARBAV   Initial Release
==================================================================== */
//**************************************************************************
// DESCRIPTION 
//   This class represents an MTAP message as seen from the client's
//   perspective. 
//  
//   When instantiated, it is in the 'Empty' state; when filled in 
//   (can only be done by the ACS_ACA_MessageStoreAccessPoint) it moves
//   to either of the states:
//
//    - 'Filled',  indicating that the message now is valid and contains 
//                 data; 
//    - 'Skipped', implying that a message with the actual number was  
//                 wasted by the CP side. It contains no data, only
//                 a message number;
//    - 'Lost',    telling that Message Store (i.e. the AP side) has lost 
//                 a message of that number. No data, only a message number.
//

// CHANGES
//   RELEASE REVISION HISTORY
//   REV NO      DATE            NAME       DESCRIPTION
//   A           2000-05-10      uabcajn    product release
//   B           2007-01-15      qnicmut    Blade Cluster adaptations
//******************************************************************************
#ifndef ACS_ACA_MessageStoreMessage_H
#define ACS_ACA_MessageStoreMessage_H
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
//#include <winsock2.h>
//#include <windows.h>
#include <acs_aca_common.h>

//class ACS_ACA_InternalMessageStoreAccessPoint;

// Class for handling of ACS_ACA_MessageStoreMessage
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      ACS_ACA_MessageStoreMessage 
 */
/*=================================================================== */
class ACS_ACA_MessageStoreMessage {
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          ENUMERATION DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      State
 */
/*=================================================================== */
	enum State
	{ // State of Message Store Message
		Empty,			// Queue empty
		Filled,			// Filled message
		Skipped,		// Message lost by application
		Lost			// Message lost by MTAP-ACA
	};
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief      Constructor for ACS_ACA_MessageStoreMessage class.
 */
/*=================================================================== */
	ACS_ACA_MessageStoreMessage();
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief      Constructor for ACS_ACA_MessageStoreMessage class.

   @param      anotherMSM 
 */
/*=================================================================== */
	ACS_ACA_MessageStoreMessage(const ACS_ACA_MessageStoreMessage & anotherMSM);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_MessageStoreMessage();
/*=================================================================== */
/**
   @brief      getMessageId

   @return     ULONGLONG
 */
/*=================================================================== */    
	ULONGLONG getMessageId() const;
/*=================================================================== */
/**
   @brief      getState

   @return     State
 */
/*=================================================================== */
	State getState() const;
/*=================================================================== */
/**
   @brief      getDataLength

   @return     int
 */
/*=================================================================== */
	int getDataLength() const;
/*=================================================================== */
/**
   @brief      getData

   @return     unsigned char
 */
/*=================================================================== */
	const unsigned char * getData() const;
/*=================================================================== */
/**
   @brief      clear

   @return     int
 */
/*=================================================================== */
	int clear(); // Will go to 'Empty' state.
/*=====================================================================
                          PROTECTED DECLARATION SECTION
==================================================================== */    
protected:
/*=================================================================== */
/**
   @brief      setData
 
   @param      messageId

   @param      data

   @param      dataLength

   @param      newState

   @return     void
 */
/*=================================================================== */
	void setData(
		ULONGLONG messageId,
		const unsigned char * data,
		int dataLength,
		State newState);
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
	ULONGLONG messageIdM;
	State stateM;
	int dataLengthM;
	unsigned char * dataM;
	int allocatedMemorySizeM;
	unsigned char * allocatedMemoryMd;

	friend class ACS_ACA_InternalMessageStoreAccessPoint;
};
#endif
