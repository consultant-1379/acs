//========================================================================================
// COPYRIGHT (C) Ericsson AB 2011 - All rights reserved
//
// The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written permission from Ericsson
// AB or in accordance with the terms and conditions stipulated in the agreement/contract
// under which the program(s) have been supplied.
//========================================================================================

#ifndef AcsHcs_TransferQueue_h
#define AcsHcs_TransferQueue_h

#include "acs_hcs_exception.h"
#include <string>
#include <ace/ACE.h>


namespace AcsHcs
{
	/**
	* Sends the report and all log files to the remote destination using the transfer queue.
	*/
	class TransferQueue
	{
	public:

		/**
		* TransferQueue::ExceptionTQNotFound - Exception indicatingtransfer queue not found.
		* TransferQueue::ExceptionTQNotFound is thrown in the case of transfer queue not found.
		*/
		class ExceptionTQNotFound : public Exception
		{
		public:
			ExceptionTQNotFound(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionTQNotFound() throw(){}
		};

		/**
		* Sends the report and all log files to the remote destination using the transfer queue.
		* @param[in] jobId JOb ID of the job whose report and log files need to be sent via transfer queue.
		* @param[in] transferQueue The transfer queue to be used for transferring the report and all the log files.
		*/
		void sendFilesToTq(const std::string& jobId, const std::string& transferQueue, const std::string& jobName,const std::string time_stamp, int compressionState = 1 );

		/**
		* Sends the report and log file to the remote destination using the transfer queue.
		* @param[in] fileName The name of the file to be sent via transfer queue. If it is a path, then all files in this
		  folder need to be transfered.
		* @param[in] pDest The transfer queue to be used for transferring the report and all the log files.
		* @param[in] filePath The path where the files to be transfered are located.
		* @throws Directory::ExceptionTQNotFound
		*/
		void sendFileToTq(/*const std::string& fileName */ char * pDest,const std::string& filePath,const std::string& jobId, const std::string& jobName, const std::string time_stamp, int compressionState = 1);

		/**
		* Checks if the transfer queue exists and is valid or not.
		* @param[in] transferQueue The transfer queue to be used for transferring the report and all the log files.
		* @return true if the transfer queue is valid, false otherwise.
		*/
		bool isTQValid(const std::string& transferQueue);
	};
}


#endif // AcsHcs_TransferQueue_h
