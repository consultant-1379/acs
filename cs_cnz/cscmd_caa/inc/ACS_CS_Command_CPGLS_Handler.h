/*
 * ACS_CS_Command_CPGLS_Handler.h
 *
 *  Created on: Aug 21, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_COMMAND_CPGLS_HANDLER_H_
#define ACS_CS_COMMAND_CPGLS_HANDLER_H_

#include <iostream>
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_Command_Common.h"

class ACS_CS_Command_Exitcodes;

class ACS_CS_Command_CPGLS_Handler
{
	public:
		//constructors
      ACS_CS_Command_CPGLS_Handler(int argc,  char* argv[]);

      //destructor
      virtual ~ACS_CS_Command_CPGLS_Handler();

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

      //function for printing the table
      NSCommands::Exit_Code CPGroupTablePrint();

		//members
      ACS_CS_Command_Exitcodes* exitCode;   //object handling the command's exit code
      int argCount;                 //number of arguments given in command
      char** argValue;            //arguments given in command
};


#endif /* ACS_CS_COMMAND_CPGLS_HANDLER_H_ */
