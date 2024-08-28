//======================================================================
//
// NAME
//      CommandClient.h
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      Command client to send commands for execution.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-05-16 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_CommandClient_h
#define AcsHcs_CommandClient_h

#include <string>

#include "acs_hcs_exception.h"
#include "acs_hcs_documentxml.h"

namespace AcsHcs
{
	/**
	* CommandClient - Handles sending of commands and reception of respective results.
	*/
	class CommandClient
	{
	public:
		/**
		* CommandClient::ExceptionIo - IO Exception.
		* CommandClient::ExceptionIo is thrown in the case of IO problems.
		*/
		class ExceptionIo : public Exception
		{
		public:
			ExceptionIo(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionIo() throw(){}
		};

		/**
		* CommandClient::ExceptionCommandForbidden - Command is forbidden.
		* CommandClient::ExceptionCommandForbidden is thrown in the case that a forbidden command should be sent.
		*/
		class ExceptionCommandForbidden : public Exception
		{
		public:
			ExceptionCommandForbidden(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionCommandForbidden() throw(){}
		};

		CommandClient();
		virtual ~CommandClient();

		/**
		* Assert if the command passed is allowed to be sent.
		* @param[in] command The command to be checked.
		* @throws CommandClient::ExceptionCommandForbidden
		*/
		virtual void assertCommandIsAllowed(const DocumentXml::Node& command) const = 0;

		/**
		* Send the command to the server and store the command result in a file.
		* @param[in] command The command to be sent.
		* @param[in] cpName The CP name to connect to the CP

		* @return The name of the file where the command result has been stored.
		* @throws CommandClient::ExceptionIo
		* @throws CommandClient::ExceptionCommandForbidden
		*/
		virtual std::string send(const DocumentXml::Node& command, const std::string& cpName) = 0;

	private:
		CommandClient(const CommandClient&);
		CommandClient& operator=(const CommandClient&);
	};
}

#endif // AcsHcs_CommandClient_h
