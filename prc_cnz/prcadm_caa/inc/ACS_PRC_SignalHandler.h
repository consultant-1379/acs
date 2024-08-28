/*
 * ACS_PRC_prcmanSignalHandler.h
 *
 *  Created on: Nov 25, 2010
 *      Author: xpaomaz
 */

#ifndef ACS_PRC_PRCMANSIGNALHANDLER_H_
#define ACS_PRC_PRCMANSIGNALHANDLER_H_

#include "ace/Event_Handler.h"
#include <string.h>
#include <iostream>

using namespace std;

/** @class  ACS_PRC_prcmanSignalHandler  ACS_PRC_prcmanSignalHandler.h
 *	@brief  ACS_PRC_prcmanSignalHandler : this class is intended to handle signal
 *	@brief  managed by the prcman
 *	@author xpaomaz (Paola Mazzone)
 *	@date 2010-11-25
 *	@version R1A
 */
class ACS_PRC_SignalHandler: public ACE_Event_Handler {

public:

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_PRC_prcmanSignalHandler constructor
	 *
	 *	ACS_PRC_suInfo Constructor
	 *  @param running: a boolean that will be used to set if the manager has to be running or not
	 *
	 *	@remarks Remarks
	 */
	ACS_PRC_SignalHandler(bool &running);

	/** @brief ACS_PRC_prcmanSignalHandler Destructor
	 *
	 *	ACS_PRC_prcmanSignalHandler Destructor
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_PRC_SignalHandler();

	//===========//
	// Functions //
	//===========//

	/**	@brief handle_signal method.
	 *  @brief handle_signal method. This method handle the signals
	 *
	 *	@return int.
	 *
	 */
	virtual int handle_signal (int signum, siginfo_t * = 0,ucontext_t * = 0);

	bool getValue () { return running; }
	int getSignal () { return signal; }
	string getStrSignal () { return signal_string; }
	private:

	bool running;
	string signal_string;
	int signal;
};

#endif /* ACS_PRC_PRCMANSIGNALHANDLER_H_ */
