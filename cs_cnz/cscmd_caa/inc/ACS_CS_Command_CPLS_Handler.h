/*
 * ACS_CS_Command_CPLS_Handler.h
 *
 *  Created on: Aug 21, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_COMMAND_CPLS_HANDLER_H_
#define ACS_CS_COMMAND_CPLS_HANDLER_H_

#include <iostream>
#include <map>

#include "ACS_CS_API.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_Command_Common.h"

typedef std::map<std::string, BoardID> stringBoardIDMap;

class ACS_CS_Command_Exitcodes;

class ACS_CS_Command_CPLS_Handler
{
	public:
		//constructors
      ACS_CS_Command_CPLS_Handler(int argc, char* argv[]);

		//destructor
		virtual ~ACS_CS_Command_CPLS_Handler();

      //function for performing the command
      bool handleCommand();

      //functions for getting exit codes and descriptions for them
      NSCommands::Exit_Code getExitCode();
      std::string getExitCodeDescription();

	private:
      //functions for checking options given in command
      NSCommands::Exit_Code checkOptions(int argc, char* argv[]);

      //functions for setting exit codes and return values
      bool setExitCodeAndReturnTrue(NSCommands::Exit_Code exitCode);
      bool setExitCodeAndReturnFalse(NSCommands::Exit_Code exitCode);

      //function for telling which options have been given
      bool isLongList() const;

      //function for printing the table
      NSCommands::Exit_Code CPTablePrint();

		//members
      ACS_CS_Command_Exitcodes* exitCode;   //object handling the command's exit code
      int argCount;                 //number of arguments given in command
      char** argValue;            //arguments given in command

      //variable for holding option and option argument data
      bool boolLongList;
};

#endif /* ACS_CS_COMMAND_CPLS_HANDLER_H_ */
