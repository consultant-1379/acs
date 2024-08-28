/*
 * ACS_CS_Command_Util.cpp
 *
 *  Created on: Aug 21, 2013
 *      Author: eanform
 */

//  Copyright Ericsson AB 2007. All rights reserved.
#include "ACS_CS_Command_Util.h"

#include <sstream>
#include "ACS_CS_API.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_Protocol.h"
#include <arpa/inet.h>

using std::set;
using std::string;

NSCommands::Exit_Code ACS_CS_Command_Util::isLegalCommand()
{
   bool isMultipleCPSystem = false;

   ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);

   if (result == ACS_CS_API_NS::Result_Success)    //function currently returns only success
   {
      if (isMultipleCPSystem)
      {
         return NSCommands::Exit_Success;
      }
      else
      {
         return NSCommands::Exit_Illegal_Command;
      }
   }
   else if (result == ACS_CS_API_NS::Result_NoAccess)
   {
      return NSCommands::Exit_Unable_Connect_Server;
   }
   else
   {
      return NSCommands::Exit_Error_Executing;
   }
}

NSCommands::Exit_Code ACS_CS_Command_Util::isOccupiedName(const std::string stringToCheck)
{
   set<string> invalidNameSet;
   string stringToCheckUpperCase = ACS_CS_Command_Util::stringToUpper(stringToCheck);

   //Add existing aliases to the SET
   ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();

   if (cpTable)
   {
      ACS_CS_API_IdList cpIds;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpIds);  //get a list of CP identities

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int j = 0; j < cpIds.size(); j++)
         {
            ACS_CS_API_Name cpAlias;
            string cpAliasStr;

            CPID cpId = cpIds[j];
            bool isAlias = false;

            result = cpTable->getCPAliasName(cpId, cpAlias, isAlias);   //get CP alias

            if (result == ACS_CS_API_NS::Result_Success)
            {
               size_t cpAliasLength = cpAlias.length();
               char* cpAliasChar = new char[cpAliasLength];

               if (cpAliasChar)
               {
                  result = cpAlias.getName(cpAliasChar, cpAliasLength);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     string cpAliasString = ACS_CS_Command_Util::stringToUpper(cpAliasChar);
                     invalidNameSet.insert(cpAliasString);       //adds current alias in upper case to the SET
                  }
                  else           //error, stop further execution
                  {
                     delete [] cpAliasChar;
                     ACS_CS_API::deleteCPInstance(cpTable);
                     return NSCommands::Exit_Error_Executing;
                  }

                  delete [] cpAliasChar;
               }
               else              //error, stop further execution
               {
                  ACS_CS_API::deleteCPInstance(cpTable);
                  return NSCommands::Exit_Error_Executing;
               }
            }
            else if (result == ACS_CS_API_NS::Result_NoValue)
            {
               //do nothing (no value to add to the SET)
            }
            else     //unsuccessful new, stop further execution
            {
               ACS_CS_API::deleteCPInstance(cpTable);
               return NSCommands::Exit_Error_Executing;
            }
         }
      }
      else if (result == ACS_CS_API_NS::Result_NoAccess)
      {
         ACS_CS_API::deleteCPInstance(cpTable);
         return NSCommands::Exit_Unable_Connect_Server;
      }
      else
      {
         ACS_CS_API::deleteCPInstance(cpTable);
         return NSCommands::Exit_Error_Executing;
      }

      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else
      return NSCommands::Exit_Error_Executing;

   //Add existing CP groups to the SET
   ACS_CS_API_CPGroup* cpGroup = ACS_CS_API::createCPGroupInstance();

   if (cpGroup)
   {
      ACS_CS_API_NameList nameList;

      ACS_CS_API_NS::CS_API_Result result = cpGroup->getGroupNames(nameList);  //get a list of CP groups

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int k = 0; k < nameList.size(); k++)
         {
            ACS_CS_API_Name cpGroupName = nameList[k];

            size_t cpGroupNameLength = cpGroupName.length();
            char* cpGroupNameChar = new char[cpGroupNameLength];

            if (cpGroupNameChar)
            {
               result = cpGroupName.getName(cpGroupNameChar, cpGroupNameLength);

               if (result == ACS_CS_API_NS::Result_Success)
               {
                  string cpGroupNameString = ACS_CS_Command_Util::stringToUpper(cpGroupNameChar);
                  invalidNameSet.insert(cpGroupNameString);   //adds current group name in upper case to the SET
               }
               else           //error, stop further execution
               {
                  delete [] cpGroupNameChar;
                  ACS_CS_API::deleteCPGroupInstance(cpGroup);
                  return NSCommands::Exit_Error_Executing;
               }

               delete [] cpGroupNameChar;
            }
            else     //unsuccessful new, stop further execution
            {
               ACS_CS_API::deleteCPGroupInstance(cpGroup);
               return NSCommands::Exit_Error_Executing;
            }
         }
      }
      else if (result == ACS_CS_API_NS::Result_NoAccess)
      {
         ACS_CS_API::deleteCPGroupInstance(cpGroup);
         return NSCommands::Exit_Unable_Connect_Server;
      }
      else  //unable to get group names
      {
         ACS_CS_API::deleteCPGroupInstance(cpGroup);
         return NSCommands::Exit_Error_Executing;
      }

      ACS_CS_API::deleteCPGroupInstance(cpGroup);
   }
   else
      return NSCommands::Exit_Error_Executing;


   size_t stringCount = invalidNameSet.count(stringToCheckUpperCase);   //check if given name in upper case already exist

   if (stringCount == 1)      //check if given name was found
      return NSCommands::Exit_Reserved_Occupied_Name;
   else
      return NSCommands::Exit_Success;
}

NSCommands::Exit_Code ACS_CS_Command_Util::isOccupiedCPName(const std::string stringToCheck)
{
   set<string> invalidNameSet;
   string stringToCheckUpperCase = ACS_CS_Command_Util::stringToUpper(stringToCheck);

   //Add existing aliases to the SET
   ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();

   if (cpTable)
   {
      ACS_CS_API_IdList cpIds;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpIds);  //get a list of CP identities

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int j = 0; j < cpIds.size(); j++)
         {
            ACS_CS_API_Name cpAlias;
            string cpAliasStr;

            CPID cpId = cpIds[j];
            bool isAlias = false;

            result = cpTable->getCPAliasName(cpId, cpAlias, isAlias);   //get CP alias

            if (result == ACS_CS_API_NS::Result_Success)
            {
               size_t cpAliasLength = cpAlias.length();
               char* cpAliasChar = new char[cpAliasLength];

               if (cpAliasChar)
               {
                  result = cpAlias.getName(cpAliasChar, cpAliasLength);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     string cpAliasString = ACS_CS_Command_Util::stringToUpper(cpAliasChar);
                     invalidNameSet.insert(cpAliasString);       //adds current alias in upper case to the SET
                  }
                  else           //error, stop further execution
                  {
                     delete [] cpAliasChar;
                     ACS_CS_API::deleteCPInstance(cpTable);
                     return NSCommands::Exit_Error_Executing;
                  }

                  delete [] cpAliasChar;
               }
               else              //error, stop further execution
               {
                  ACS_CS_API::deleteCPInstance(cpTable);
                  return NSCommands::Exit_Error_Executing;
               }
            }
            else if (result == ACS_CS_API_NS::Result_NoValue)
            {
               //do nothing (no value to add to the SET)
            }
            else     //unsuccessful new, stop further execution
            {
               ACS_CS_API::deleteCPInstance(cpTable);
               return NSCommands::Exit_Error_Executing;
            }
         }
      }
      else if (result == ACS_CS_API_NS::Result_NoAccess)
      {
         ACS_CS_API::deleteCPInstance(cpTable);
         return NSCommands::Exit_Unable_Connect_Server;
      }
      else
      {
         ACS_CS_API::deleteCPInstance(cpTable);
         return NSCommands::Exit_Error_Executing;
      }

      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else
      return NSCommands::Exit_Error_Executing;


   size_t stringCount = invalidNameSet.count(stringToCheckUpperCase);   //check if given name in upper case already exist

   if (stringCount == 1)      //check if given name was found
      return NSCommands::Exit_Reserved_Occupied_Name;
   else
      return NSCommands::Exit_Success;
}

NSCommands::Exit_Code ACS_CS_Command_Util::isOccupiedCPGroupName(const std::string stringToCheck)
{
   set<string> invalidNameSet;
   string stringToCheckUpperCase = ACS_CS_Command_Util::stringToUpper(stringToCheck);

   //Add existing CP groups to the SET
   ACS_CS_API_CPGroup* cpGroup = ACS_CS_API::createCPGroupInstance();

   if (cpGroup)
   {
      ACS_CS_API_NameList nameList;

      ACS_CS_API_NS::CS_API_Result result = cpGroup->getGroupNames(nameList);  //get a list of CP groups

      if (result == ACS_CS_API_NS::Result_Success)
      {
         for (unsigned int k = 0; k < nameList.size(); k++)
         {
            ACS_CS_API_Name cpGroupName = nameList[k];

            size_t cpGroupNameLength = cpGroupName.length();
            char* cpGroupNameChar = new char[cpGroupNameLength];

            if (cpGroupNameChar)
            {
               result = cpGroupName.getName(cpGroupNameChar, cpGroupNameLength);

               if (result == ACS_CS_API_NS::Result_Success)
               {
                  string cpGroupNameString = ACS_CS_Command_Util::stringToUpper(cpGroupNameChar);
                  invalidNameSet.insert(cpGroupNameString);   //adds current group name in upper case to the SET
               }
               else           //error, stop further execution
               {
                  delete [] cpGroupNameChar;
                  ACS_CS_API::deleteCPGroupInstance(cpGroup);
                  return NSCommands::Exit_Error_Executing;
               }

               delete [] cpGroupNameChar;
            }
            else     //unsuccessful new, stop further execution
            {
               ACS_CS_API::deleteCPGroupInstance(cpGroup);
               return NSCommands::Exit_Error_Executing;
            }
         }
      }
      else if (result == ACS_CS_API_NS::Result_NoAccess)
      {
         ACS_CS_API::deleteCPGroupInstance(cpGroup);
         return NSCommands::Exit_Unable_Connect_Server;
      }
      else  //unable to get group names
      {
         ACS_CS_API::deleteCPGroupInstance(cpGroup);
         return NSCommands::Exit_Error_Executing;
      }

      ACS_CS_API::deleteCPGroupInstance(cpGroup);
   }
   else
      return NSCommands::Exit_Error_Executing;


   size_t stringCount = invalidNameSet.count(stringToCheckUpperCase);   //check if given name in upper case already exist

   if (stringCount == 1)      //check if given name was found
      return NSCommands::Exit_Duplicated_CP_Group_Name;
   else
      return NSCommands::Exit_Success;
}

bool ACS_CS_Command_Util::isApBoardDefined(uint16_t apId)
{
    bool foundBoard = false;

    ACS_CS_API_HWC *tableHWC = ACS_CS_API::createHWCInstance();

    if (tableHWC)
    {
        ACS_CS_API_BoardSearch *boardSearch = tableHWC->createBoardSearchInstance();

        if (boardSearch)
        {
            // search for sysid (apid)
            boardSearch->setSysId(apId);

            ACS_CS_API_IdList apList;

            if (tableHWC->getBoardIds(apList, boardSearch) == ACS_CS_API_NS::Result_Success) {
                // search is complete
                if (apList.size() > 0)
                    foundBoard = true;
            }

            tableHWC->deleteBoardSearchInstance(boardSearch);
        }

        ACS_CS_API::deleteHWCInstance(tableHWC);
    }

    return foundBoard;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_Command_Util::getAPList(ACS_CS_API_IdList& apList)
{
   set<APID> apIdSet;
   set<APID>::iterator itSet;

   ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;

   ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

   if (hwc)
   {
      ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();

      if (boardSearch)
      {
         ACS_CS_API_IdList boardList;

         result = hwc->getBoardIds(boardList, boardSearch);  //search for all board ids (wildcard search)

         if (result == ACS_CS_API_NS::Result_Success)
         {
            for (unsigned int i = 0; i < boardList.size(); i++)   //step through all the boards in the hwc table
            {
               BoardID boardId = boardList[i];
               SysID sysId = 0;

               result = hwc->getSysId(sysId, boardId);    //get the system id of current board

               if (result == ACS_CS_API_NS::Result_Success)
               {
                  if (sysId > ACS_CS_Protocol::SysType_AP) //check if system type is AP
                     apIdSet.insert(sysId);  //add sysId to the SET
               }
               else if (result == ACS_CS_API_NS::Result_NoValue)  //no value isn't a fault, the board just lacks a system id
                  result = ACS_CS_API_NS::Result_Success;
               else           //no entry, no access or other failure, not meaningful to continue
                  break;
            }
         }

         ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
      }

      ACS_CS_API::deleteHWCInstance(hwc);
   }

   if (result == ACS_CS_API_NS::Result_Success)
   {
      unsigned int apListSize = (unsigned int)apIdSet.size();
      apList.setSize(apListSize);

      int i = 0;

      for (itSet = apIdSet.begin(); itSet != apIdSet.end(); itSet++)
      {
         apList.setValue(*itSet, i++);
      }
   }

   return result;
}

std::string ACS_CS_Command_Util::stringToUpper(const std::string& stringToConvert)
{
   //Changes each element of the string to upper case
   string convertedString = stringToConvert;

   for(unsigned int i = 0; i < stringToConvert.length(); i++)
   {
      convertedString[i] = toupper(stringToConvert[i]);
   }

   return convertedString;    //return the converted string
}

NSCommands::Exit_Code ACS_CS_Command_Util::isCmdAllowedInCurrentProfile(
   const NSCommands::Command cmd,
   bool& allowed)
{
   allowed = false;
   unsigned int currProfile;

   const NSCommands::Exit_Code result =
      ACS_CS_Command_Util::getCurrentOmProfile(currProfile);

   if (NSCommands::Exit_Success != result)
   {
      return result;
   }

   // Check if BLADE or CLUSTER profile
   if (ACS_CS_API_OmProfileChange::BladeProfile == currProfile)
   {
      // No restriction on commands in BLADE profile
      allowed = true;
   }
   else if(currProfile > ACS_CS_API_OmProfileChange::BladeProfile)
   {
      switch (cmd)
      {
         case NSCommands::CPGDEF :
            /* NO BREAK */
         case NSCommands::CPGRM :
            allowed = false;
            break;
         default:
            allowed = true;
            break;
      }
   }
   return NSCommands::Exit_Success;
}

NSCommands::Exit_Code ACS_CS_Command_Util::getCurrentOmProfile(
   unsigned int& currProfile)
{
   ACS_CS_API_OmProfileChange profile;

   const ACS_CS_API_NS::CS_API_Result result =
      ACS_CS_API_NetworkElement::getOmProfile(profile);

   switch (result)
   {
      case ACS_CS_API_NS::Result_Success:
         currProfile = profile.omProfileCurrent;
         break;
      case ACS_CS_API_NS::Result_NoAccess:
         return NSCommands::Exit_Unable_Connect_Server;
         /* NO BREAK */
      default:
         return NSCommands::Exit_Error_Executing;
         /* NO BREAK */
   }
   return NSCommands::Exit_Success;
}

bool ACS_CS_Command_Util::isIpAddress(const std::string ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    return result != 0;
}
