/*
 * ACS_APSESH_JtpMgrInterface.h
 *
 *  Created on: May 10, 2011
 *      Author: bjax
 */

#ifndef ACS_APSESH_JTPMGRINTERFACE_H_
#define ACS_APSESH_JTPMGRINTERFACE_H_

#include "ACS_APSESH_JtpReturnType.h"

class ACS_APSESH_JtpMgrInterface
{

public:

   // Performs initialization needed for JTP connections
   //
   // This function registers a JTP service for APSESH, which will allow the Jtp
   // Mgr to accept connections destined for that service.
   virtual ACS_APSESH_JtpReturnType init() = 0;

   // Gets quorum data sent from SESH.
   //
   // This function tries to retrieve quorum data from SESH via JTP.  This is a
   // blocking function; it will wait until it receives the data, or until an
   // error is encountered.
   //
   // @param buffer
   //   The buffer into which the quorum data will be placed.
   virtual ACS_APSESH_JtpReturnType getData(char*& buffer, int& bufLen) = 0;



   // Notifies SESH of APSESH's success or failure in sending quorum data to CS.
   // This function is not blocking.
   //
   // @param buffer
   //   Zero if APSESH failed in sending the quorum data to CS, and nonzero if it
   // succeded.
   virtual ACS_APSESH_JtpReturnType sendStatusCs(short status) = 0;

   virtual void setJtpWaitTimeout(const unsigned int& timeout) = 0;

   virtual void resetJtpWaitTimeout() = 0;

   virtual void unregister() = 0;
};



#endif /* ACS_APSESH_JTPMGRINTERFACE_H_ */
