#ifndef ACS_DSD_SERVICEACCEPTORTASK_H_
#define ACS_DSD_SERVICEACCEPTORTASK_H_

/** @file ACS_DSD_ServiceAcceptorTask.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-18
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2010-11-18 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_Task_Base.h"
#include "ACS_DSD_ServiceAcceptor.h"
#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_AppProt_SvcHandler.h"
#include "ACS_DSD_CpProt_SvcHandler.h"
#include "ACS_DSD_SrvProt_SvcHandler.h"

/** @class ACS_DSD_ServiceAcceptorTask ACS_DSD_ServiceAcceptorTask.h
 *	@brief ACS_DSD_ServiceAcceptorTask  class
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-18
 *	@version 0.1
 *
 *	ACS_DSD_ServiceAcceptorTask Class detailed description
 */
class ACS_DSD_ServiceAcceptorTask: public ACS_DSD_Task_Base
{
public:
	/// Composite State of Acceptors
	enum Service_Acceptors_State
	{
		ALL_ACCEPTORS_DOWN = 0x0,
		SVRAPP_ACCEPTOR_UP = 0x1,
		CPAP_ACCEPTOR_UP = 0x2,
		SVRSVR_ACCEPTOR_UP = 0x4,
		ALL_ACCEPTORS_UP = SVRAPP_ACCEPTOR_UP | CPAP_ACCEPTOR_UP | SVRSVR_ACCEPTOR_UP
	};

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_DSD_ServiceAcceptorTask Default constructor
	 *
	 *	ACS_DSD_ServiceAcceptorTask Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	ACS_DSD_ServiceAcceptorTask(uint n_threads = 1, ACE_Reactor * reactor = 0);


	//==============//
	// Destructor //
	//==============//

	/** @brief ~ACS_DSD_ServiceAcceptorTask Destructor
	 *
	 *	ACS_DSD_ServiceAcceptorTask Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_DSD_ServiceAcceptorTask();


	//==============//
	// Functions 	//
	//==============//

	/** @brief svc method
	 *
	 *	svc method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	//virtual int svc (void);


	/** @brief start_activity method
	 *
	 *	start_activity method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int start_activity();


	/** @brief start_activity method
	 *
	 *	stop_activity method detailed description
	 *
	 *  @param[in] wait_termination Indicates if the method must wait for thread termination
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int stop_activity(bool wait_termination);


	/** @brief set_service_handler_reactor method
	 *
	 *	set_service_handler_reactor method detailed description
	 *
	 *	@param[in] service_handler_reactor Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_service_handler_reactor(ACE_Reactor * service_handler_reactor)
	{
		_svrapp_acceptor.set_service_handler_reactor(service_handler_reactor);
		_cpap_acceptor.set_service_handler_reactor(service_handler_reactor);
		_srvsrv_acceptor.set_service_handler_reactor(service_handler_reactor);
	};

	/** @brief get_acceptors_state method
	 *
	 *	Return the state (UP or DOWN) of the three acceptors (SVRAPP, CPAP, SRVSRV) associated to the task
	 *
	 *	@return A mask indicating the state of the three acceptors
	 *	@remarks Remarks
	 */
	Service_Acceptors_State get_acceptors_state();


	/** @brief check_and_restore_acceptors method
	 *
	 *	Check the state of the three acceptors (SVRAPP, CPAP, SRVSRV) associated to the task
	 *	and eventually try to restore them if DOWN.
	 *
	 *	@return
	 *		0  all 3 acceptors are UP;
	 *		1  no action has been performed because the check is not applicable in the current task state;
	 *	   -1  at least one acceptor has been found DOWN and the attempt to restore it has failed.
	 *	@remarks Remarks
	 */
	int check_and_restore_acceptors();


	/// Indicates if the task is active
	inline bool is_active() { return this->thr_count_ > 0; }

private:
	/** @brief init_AppProt_Acceptor method
	 *
	 *	init_AppProt_Acceptor method detailed description
	 *
	 *	@param[in] sap_id sap_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int init_AppProt_Acceptor(const char *sap_id = 0);


	/** @brief init_CpProt_Acceptor method
	 *
	 *	 init_CpProt_Acceptor method detailed description
	 *
	 *	@param[in] sap_id sap_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int init_CpProt_Acceptor(const char *sap_id = 0);


	/** @brief init_SVRSVR_Acceptor method
	 *
	 *	init_SVRSVR_Acceptor method detailed description
	 *
	 *	@param[in] sap_id sap_id Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int init_SrvSrvProt_Acceptor(const char *sap_id = 0);


private:
	ACS_DSD_ServiceAcceptor<ACS_DSD_AppProt_SvcHandler, ACS_DSD_UNIX_SAP>  _svrapp_acceptor;   					 /// acceptor for SVR-APP protocol
	ACS_DSD_ServiceAcceptor<ACS_DSD_CpProt_SvcHandler,  ACS_DSD_INET_SAP>  _cpap_acceptor;		 				 /// acceptor for CP-AP protocol
	ACS_DSD_ServiceAcceptor<ACS_DSD_SrvProt_SvcHandler, ACS_DSD_INET_SAP>  _srvsrv_acceptor;		 			 /// acceptor for SVR-SVR protocol
};

#endif /* ACS_DSD_SERVICEACCEPTORTASK_H_ */
