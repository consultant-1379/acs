/*
 * ACS_PRC_prcispThread.h
 *
 *  Created on: Nov 22, 2010
 *      Author: xpaomaz
 */

#ifndef ACS_PRC_prcispTHREAD_H_
#define ACS_PRC_prcispTHREAD_H_

#include <stdio.h>
#include <iostream>

#include "ace/Task.h"

#include <ace/UNIX_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Acceptor.h>

using namespace std;

/** @class  ACS_PRC_prcispThread  ACS_PRC_prcispThread.h
 *	@brief  ACS_PRC_prcispThread class This class is intended to be the tread for prc
 *	@brief  manager application that wait for connection from peer prc manager from the
 *	@brief  oher node. this class extends ACE_Task_Base.
  *	@author xpaomaz (Paola Mazzone)
 *	@date 2010-11-25
 *	@version R1A
 */

class ACS_PRC_prcispThread : public ACE_Task_Base{

public:

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_PRC_prcispThread Default constructor
	 *
	 *	ACS_PRC_prcispThread Constructor
	 *
	 *	@remarks Remarks
	 */
	ACS_PRC_prcispThread();
	/** @brief ACS_PRC_suInfo Destructor
	 *
	 *	ACS_APGCC_ACS_PRC_suInfo Destructor
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_PRC_prcispThread();

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
};

#endif /* ACS_PRC_prcispTHREAD_H_ */
