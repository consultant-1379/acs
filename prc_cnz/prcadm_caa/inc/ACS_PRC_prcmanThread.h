/*
 * ACS_PRC_prcmanThread.h
 *
 *  Created on: Nov 22, 2010
 *      Author: xpaomaz
 */

#ifndef ACS_PRC_PRCMANTHREAD_H_
#define ACS_PRC_PRCMANTHREAD_H_

#include <stdio.h>
#include <iostream>

#include "ace/Task.h"

#include <ace/UNIX_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Acceptor.h>

using namespace std;

/** @class  ACS_PRC_prcmanThread  ACS_PRC_prcmanThread.h
 *	@brief  ACS_PRC_prcmanThread class This class is intended to be the tread for prc
 *	@brief  manager application that wait for connection from peer prc manager from the
 *	@brief  oher node. this class extends ACE_Task_Base.
  *	@author xpaomaz (Paola Mazzone)
 *	@date 2010-11-25
 *	@version R1A
 */

class ACS_PRC_prcmanThread : public ACE_Task_Base{

public:

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_PRC_prcmanThread Default constructor
	 *
	 *	ACS_PRC_prcmanThread Constructor
	 *
	 *	@remarks Remarks
	 */
	ACS_PRC_prcmanThread();
	/** @brief ACS_PRC_suInfo Destructor
	 *
	 *	ACS_APGCC_ACS_PRC_suInfo Destructor
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_PRC_prcmanThread();

	//===========//
	// Functions //
	//===========//

	/**	@brief svc method.
	 *  @brief svc. This method is performs real workload.
	 *
	 *	@return int.
	 *
	 */
	virtual int svc ( void );

	void stop(){ stopPrcmanThread = true; };//HV92897

	void start(){ stopPrcmanThread = false; };//HV92897

private :
	bool stopPrcmanThread;//HV92897
};

#endif /* ACS_PRC_PRCMANTHREAD_H_ */
