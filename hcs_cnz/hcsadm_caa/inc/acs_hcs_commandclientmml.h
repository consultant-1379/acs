//======================================================================
//
// NAME
//      CommandClientMml.h
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
//      Command client to send MML commands for execution.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-05-20 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_CommandClientMml_h
#define AcsHcs_CommandClientMml_h

#include <string>
#include <list>

#include "acs_hcs_commandclient.h"
#include "acs_hcs_configuration.h"

namespace AcsHcs
{
	/**
	* CommandClientMml - Handles sending of commands and reception of respective results for MML.
	* - uses the genuine AP-command mml.exe for the communication with the CP.
	* - stores the printouts received in the printout cache.
	*/
	class CommandClientMml : public CommandClient
	{
	public:
		CommandClientMml();
		~CommandClientMml();

		/**
		* CommandClientMml::ExceptionCreateProcess - Exception while creating a Process.
		* CommandClientMml::ExceptionCreateProcess is thrown on problems while creating a Process.
		*/
		class ExceptionCreateProcess : public Exception
		{
		public:
			ExceptionCreateProcess(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionCreateProcess() throw() {}
		};

		/**
		* Assert if the command passed is allowed to be sent.
		* @param[in] command The command to be checked.
		* @throws CommandClient::ExceptionCommandForbidden
		*/
		void assertCommandIsAllowed(const DocumentXml::Node& command) const;
		
		/**
		* Send the command to the server and store the command result in a file.
		* @param[in] command The command to be executed.
		* @param[in] cpName  The CP name to connect to the CP
		* @return The name of the file where the command result has been stored.
		* @throws CommandClient::ExceptionIo
		*/
		std::string send(const DocumentXml::Node& command, const std::string& cpName);
		std::string send(const DocumentXml::Node& command);

	private:
		CommandClientMml(const CommandClientMml&);
		CommandClientMml& operator=(const CommandClientMml&);

		/**
		* Connects and launches the MML commands and redirects the printout to a file.
		* @param[in] command The command to be executed.
		* @param[in] cpName  The CP name to connect to the CP
		* @return The name of the file where the command result has been stored.
		* @throws CommandClient::ExceptionIo
		*/
		std::string execute(const DocumentXml::Node& command, const std::string& cpName);

#ifdef ACS_HCS_SIMULATION_USE
		std::string executeSimulated(const DocumentXml::Node& command, const std::string& cpName) const;
#endif // ACS_HCS_SIMULATION_USE
	};
}

#endif // AcsHcs_CommandClientMml_h
