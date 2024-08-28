/*
 * acs_alog_communicationhandler.h
 *
 *  Created on: Jun 14, 2011
 *      Author: xanttro
 */

#ifndef ACS_ALOG_COMMUNICATIONHANDLER_H_
#define ACS_ALOG_COMMUNICATIONHANDLER_H_

#include "ace/ACE.h"
#include "string"
#include "acs_alog_types.h"
#include <ace/Condition_T.h>
#include <ace/Task.h>
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>
#include "ace/SOCK_Connector.h"
#define PIPE_NAME		"/var/run/ap/ALOG_API_pipe"

class acs_alog_communicationhandler
{
public:
	acs_alog_communicationhandler();
	virtual ~acs_alog_communicationhandler();
	acs_alog_returnType getErrorCode() const;
    std::string getErrorText() const;
    ACE_HANDLE getHandle() const;
    void setErrorCode(acs_alog_returnType errorCode);
    void setErrorText(std::string errorText);
    void setHandle(ACE_HANDLE handle);
    int sendMsg(const char *eventMsg);

    static ACE_Recursive_Thread_Mutex  _acs_alog_Mutex_pipe;

private:
	ACE_HANDLE handle;
	acs_alog_returnType errorCode;
	std::string errorText;
};

#endif /* ACS_ALOG_COMMUNICATIONHANDLER_H_ */
