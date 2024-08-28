/*
 * ACS_CS_Command_Util.h
 *
 *  Created on: Aug 21, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_COMMAND_UTIL_H_
#define ACS_CS_COMMAND_UTIL_H_

#include <iostream>
#include "ACS_CS_Command_Common.h"
#include "ACS_CS_API.h"

class ACS_CS_PDU;

class ACS_CS_Command_Util
{
	public:
      //function checking if command is allowed in current system
      static NSCommands::Exit_Code isLegalCommand();

      //functions checking if given name is reserved or already in use
      static NSCommands::Exit_Code isOccupiedName(const std::string name);
      static NSCommands::Exit_Code isOccupiedCPName(const std::string cpName);
      static NSCommands::Exit_Code isOccupiedCPGroupName(const std::string cpGroupName);

      static bool isApBoardDefined(uint16_t apId);
      static bool isIpAddress(const std::string ipAddress);

      //function retrieving a list of the identities of the APs in the system
      static ACS_CS_API_NS::CS_API_Result getAPList(ACS_CS_API_IdList& apList);

      //function returning every character in a given string in upper case
      static std::string stringToUpper(const std::string& stringToConvert);

      //function to get current O&M profile
      static NSCommands::Exit_Code getCurrentOmProfile(unsigned int& currProfile);

      //function to check if a command is allowed in current O&M profile
      static NSCommands::Exit_Code isCmdAllowedInCurrentProfile(
         const NSCommands::Command cmd,
         bool& allowed);

	private:
      //constructors
      ACS_CS_Command_Util();
};


#endif /* ACS_CS_COMMAND_UTIL_H_ */
