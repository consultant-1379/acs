/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2014
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file acs_bur_BrmAsyncActionProgress.h
 *
 * @brief
 * Maintains a loacal AsyncActionProgress structure
 *
 * @author
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE            USER         DESCRIPTION/TR
 * ----------------------------------------------
 * 01/12/2011     EGIMARR       Initial Release
 * 10/02/2014     XVENJAM       Fix for TR HS28643
 ****************************************************************************/

#ifndef ACS_BUR_BRMASYNCACTIONPROGRESS_H_
#define ACS_BUR_BRMASYNCACTIONPROGRESS_H_

#include <acs_apgcc_omhandler.h>
#include "acs_bur_Define.h"
#include <ACS_TRA_trace.h>
#include <string>

using namespace std;

class acs_bur_BrmAsyncActionProgress
{
public:
  acs_bur_BrmAsyncActionProgress(char* p);
  ~acs_bur_BrmAsyncActionProgress();
  string 	getActionName() const;
  string 	getAdditionalInfo() const;
  string	getProgressInfo() const;
  int 		getProgressPercentage() const;
  int 		getResult() const;
  string 	getResultInfo() const;
  int 		getActionId() const;
  string 	getTimeActionStarted() const;
  string 	getTimeActionCompleted() const;
  string	getTimeOfLastStatusUpdate() const;
  int		getState() const;
  void 		setOpCode(int code);
  void		setOpCode(int code, string functionName);
  int 		getOpCode() const;
  string 	getOpMessage() const;

private:

  string 	id;
  string 	actionName;
  string	additionalInfo;
  string 	progressInfo;
  int		progressPercentage;
  int		result;
  string 	resultInfo;
  int 		state;
  int		actionId;
  string 	timeActionStarted;
  string	timeActionCompleted;
  string	timeOfLastStatusUpdate;
  string 	m_functionName;
  int		opCode;
  ACS_TRA_trace* aapTrace;
  ACS_TRA_Logging* burLog;
};

#endif /* ACS_BUR_BRMASYNCACTIONPROGRESS_H_ */
