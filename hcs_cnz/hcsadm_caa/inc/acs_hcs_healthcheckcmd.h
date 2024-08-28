//======================================================================
//
// NAME
//      HealthCheckCmd.h
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
//      Implementation of Base class for handling HC commands.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_HealthCheckCmd_h
#define AcsHcs_HealthCheckCmd_h

//#include <windows.h>
#include <map>
#include <string>
#include <sstream>
#include <ace/ACE.h>
#include "ace/Task.h"

#include "acs_hcs_exception.h"

namespace AcsHcs
{
	/**
	* Health Check Command on HC Engine - Base class to handle all kinds of HC commands.
	*/
	class HealthCheckCmd : public ACE_Task_Base
	{
	public:

		/**
		* stopEventFlag: This boolean flag is used to come out of threads gracefully.
		*/

		static bool stopEventFlag;

		/** CmdArgs stores all arguments of a health check command
		*/
		class CmdArgs
		{
		public:
			static const char* OPT_ADD_RULE;
			static const char* OPT_BACKGROUND;
			static const char* OPT_CATS;
			static const char* OPT_CP_NAME;
			static const char* OPT_DETAILS;
			static const char* OPT_EXPERT_USER;
			static const char* OPT_JOB_NAME;
			static const char* OPT_REPORT;
			static const char* OPT_RULES;
			static const char* OPT_RULE_SET;
			static const char* OPT_RULE_SETS;
			static const char* OPT_SCHEDULE;
			static const char* OPT_ITERATIONS;
			static const char* OPT_USER_NAME;
			static const char* OPT_TRANS_QUEUE;

			typedef std::map<std::string, std::string> Args;

			/** check whether a key was sent by the operator
			 *@param in key to be checked
			 *@returns true if the key exites
			 */
			bool exists(const std::string& key) const;
			/** get the value for a key defined by the operator
			*@param key the key whose value is looked for
			*@returns the value for the key
			*/
			std::string get(const std::string& key) const;
			/**
			* set a key and value defined by the command
			*@param key the key from the command
			*@param value the value belonging to key
			*/
			void set(const std::string& key, const std::string& value);

		private:
			Args args;
		};

		/**
		* Environment::ExceptionAuth - Exception indicating authorization error.
		* Environment::ExceptionAuth is thrown in the case of authorization errors.
		*/
		class ExceptionAuth : public Exception
		{
		public:
			ExceptionAuth(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionAuth() throw() {}
		};

		/**
		* HealthCheckCmd::ExceptionJobNotFound - Exception indicating that a Job has not been found.
		* HealthCheckCmd::ExceptionJobNotFound is thrown in the case of a Job has not been found.
		*/
		class ExceptionJobNotFound : public Exception
		{
		public:
			ExceptionJobNotFound( const std::string& where, const std::string& what, const std::string& cause ) : Exception( where, what, cause ) {}
			~ExceptionJobNotFound() throw() {}
		};

		/**
		* Starts a new thread for the execution of the command passed.
		* @param[in] cmd The command to be executed in a thread of its own.
		* @return 0 on success, -1 otherwise.
		*/
		static int Start(HealthCheckCmd* cmd);

		/**
		* Constructor.
		* The constructor also parses the command message and stores the
		* result in this object.
		* @param[in] cmdMsg The command message as received from the user.
		* @param[in] pipe   The communication channel towards the user.
		*/
		HealthCheckCmd(char* cmdMsg, ACE_HANDLE pipe);
		//HealthCheckCmd(char* cmdMsg, ACE_HANDLE* pipe);

		/**
		* Destructor.
		* The destructor also closes the communication channel.
		*/
		virtual ~HealthCheckCmd();

		/**
		* Execute this command.
		* @return 0 on success, -1 otherwise.
		*/
		virtual int execute() = 0;

		/**
		* Send a status message to the command client.
		* @param[in] statusMsg The status message.
		* @return 0 on success, -1 otherwise.
		*/
		virtual int sendStatus(const std::string& statusMsg);

	protected:
		CmdArgs cmdArgs;

	private:
		//static void Execute(void* cmd);
		int svc(void);

		void disconnect();

		ACE_HANDLE pipe;
	};
}

#endif // AcsHcs_HealthCheckCmd_h
