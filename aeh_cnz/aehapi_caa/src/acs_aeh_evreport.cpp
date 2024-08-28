//========================================================================================
/** @file  acs_aeh_evreport.cpp
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-6
 *	@version 0.9.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P0.9.1 | 2011-05-06 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2011-05-13 | xfabron      | Released for ITSTEAM2 sprint 11     |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 | 2011-06-07 | xfabron      | Released for ITSTEAM2 sprint 12     |
 *	+========+============+==============+=====================================+
 *
 * SEE ALSO
 *		-
 *
 */
//========================================================================================

#include "acs_aeh_evreport.h"
#include "acs_aeh_evreportimpl.h"
#include "acs_aeh_exception.h"
#include "ace/OS.h"


//========================================================================================
//	Constructor
//========================================================================================

acs_aeh_evreport::acs_aeh_evreport()
{
	evReportImplp_ = new ACS_AEH::acs_aeh_evreportimpl;
}


//========================================================================================
//	Copy constructor
//========================================================================================

acs_aeh_evreport::acs_aeh_evreport(const acs_aeh_evreport& evReport):acs_aeh_error(evReport)
{
	evReportImplp_ = new ACS_AEH::acs_aeh_evreportimpl(*evReport.evReportImplp_);
}


//========================================================================================
//	Destructor
//========================================================================================

acs_aeh_evreport::~acs_aeh_evreport(){

	delete evReportImplp_;
}


//========================================================================================
//	Assignment operator
//========================================================================================

acs_aeh_evreport& acs_aeh_evreport::operator=(const acs_aeh_evreport& evReport)
{
	*evReportImplp_ = *evReport.evReportImplp_;
	return *this;
}


//========================================================================================
//	send Event Message
//========================================================================================

ACS_AEH_ReturnType acs_aeh_evreport::sendEventMessage(
							acs_aeh_processName processNamePtr,
							acs_aeh_specificProblem specificProblem,
							acs_aeh_percSeverity percSeverityPtr,
							acs_aeh_probableCause probableCausePtr,
							acs_aeh_objClassOfReference objClassOfReferencePtr,
							acs_aeh_objectOfReference objectOfReferencePtr,
							acs_aeh_problemData problemDataPtr,
							acs_aeh_problemText problemTextPtr)
{

	try{
		evReportImplp_->sendEventMessage(processNamePtr, specificProblem, percSeverityPtr,
						probableCausePtr, objClassOfReferencePtr, objectOfReferencePtr,
						problemDataPtr, problemTextPtr, false);

		setError(ACS_AEH_noErrorType);
		setErrorText(ACS_AEH_noError);

		return ACS_AEH_ok;

	}catch(acs_aeh_exception &ex){

		switch(ex.getErrorCode()){
		case PARAMERROR:
			setError(ACS_AEH_syntaxError);
			break;
		case SYSTEMERROR:
			setError(ACS_AEH_eventDeliveryFailure);
			break;
		default:
			setError(ACS_AEH_genericError);
			break;
		}//end switch

		std::stringstream s;
		s << ex;
		setErrorText("acs_aeh_evreport", s.str().data());

		return ACS_AEH_error;
	}

}


//========================================================================================
//	send Event Message
//========================================================================================

ACS_AEH_ReturnType acs_aeh_evreport::sendEventMessage(
						acs_aeh_processName processNamePtr,
						acs_aeh_specificProblem specificProblem,
						acs_aeh_percSeverity percSeverityPtr,
						acs_aeh_probableCause probableCausePtr,
						acs_aeh_objClassOfReference objClassOfReferencePtr,
						acs_aeh_objectOfReference objectOfReferencePtr,
						acs_aeh_problemData problemDataPtr,
						acs_aeh_problemText problemTextPtr,
						bool manualCease)
{


	try{
		evReportImplp_->sendEventMessage(processNamePtr, specificProblem, percSeverityPtr,
						probableCausePtr, objClassOfReferencePtr, objectOfReferencePtr,
						problemDataPtr, problemTextPtr, manualCease);

		setError(ACS_AEH_noErrorType);
		setErrorText(ACS_AEH_noError);

		return ACS_AEH_ok;

	}catch(acs_aeh_exception &ex){

		switch(ex.getErrorCode()){
		case PARAMERROR:
			setError(ACS_AEH_syntaxError);
			break;
		case SYSTEMERROR:
			setError(ACS_AEH_eventDeliveryFailure);
			break;
		default:
			setError(ACS_AEH_genericError);
			break;
		}//end switch

		std::stringstream s;
		s << ex;
		setErrorText("acs_aeh_evreport", s.str().data());

		return ACS_AEH_error;
	}

}
