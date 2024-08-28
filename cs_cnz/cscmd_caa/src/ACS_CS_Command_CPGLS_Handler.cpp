/*
 * ACS_CS_Command_CPGLS_Handler.cpp
 *
 *  Created on: Aug 21, 2013
 *      Author: eanform
 */

#include "ACS_CS_Command_CPGLS_Handler.h"

#include <iomanip>
#include <sstream>
#include "ACS_CS_Command_Util.h"
#include "ACS_CS_Command_Exitcodes.h"
#include "ACS_CS_API.h"
#include "ACS_CS_Util.h"

using std::cout;
using std::endl;
using std::setw;
using std::string;
using std::stringstream;

//constructor
ACS_CS_Command_CPGLS_Handler::ACS_CS_Command_CPGLS_Handler(int argc, char* argv[])
{
   this->argCount = argc;
   this->argValue = argv;
   this->exitCode = new ACS_CS_Command_Exitcodes(NSCommands::CPGLS);
}

//destructor
ACS_CS_Command_CPGLS_Handler::~ACS_CS_Command_CPGLS_Handler()
{
   if (this->exitCode)
   {
      delete this->exitCode;
      this->exitCode = 0;
   }
}

bool ACS_CS_Command_CPGLS_Handler::handleCommand()
{
   NSCommands::Exit_Code returnValue = ACS_CS_Command_Util::isLegalCommand();

   if (returnValue == NSCommands::Exit_Success)
   {
      if (! ACS_CS_API_Util::isCSRunning())
         return this->setExitCodeAndReturnFalse(NSCommands::Exit_Error_Executing);

      returnValue = this->checkOptions(this->argCount, this->argValue);

      if (returnValue == NSCommands::Exit_Success)
      {
         returnValue = this->CPGroupTablePrint();

         if (returnValue != NSCommands::Exit_Success) //check if table wasn't printed
            return this->setExitCodeAndReturnFalse(returnValue);

         return this->setExitCodeAndReturnTrue(NSCommands::Exit_Success);  //table successfully printed
      }
      else  //incorrect usage of command options
         return this->setExitCodeAndReturnFalse(returnValue);
   }
   else  //illegal command in this system configuration or no access to CS
      return this->setExitCodeAndReturnFalse(returnValue);
}

NSCommands::Exit_Code ACS_CS_Command_CPGLS_Handler::checkOptions(int argc, char* /*argv*/[])
{
   if (argc > 1)  //check if any option
      return NSCommands::Exit_Incorrect_Usage;

   return NSCommands::Exit_Success;
}

NSCommands::Exit_Code ACS_CS_Command_CPGLS_Handler::getExitCode()
{
   return this->exitCode->getExitCode();
}

string ACS_CS_Command_CPGLS_Handler::getExitCodeDescription()
{
   return this->exitCode->getExitCodeDescription();
}

bool ACS_CS_Command_CPGLS_Handler::setExitCodeAndReturnTrue(NSCommands::Exit_Code exitCodeToSet)
{
   this->exitCode->setExitCode(exitCodeToSet);
   return true;
}

bool ACS_CS_Command_CPGLS_Handler::setExitCodeAndReturnFalse(NSCommands::Exit_Code exitCodeToSet)
{
   this->exitCode->setExitCode(exitCodeToSet);
   return false;
}

NSCommands::Exit_Code ACS_CS_Command_CPGLS_Handler::CPGroupTablePrint()
{
   ACS_CS_API_CPGroup* cpGroup = ACS_CS_API::createCPGroupInstance();
   ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();

   if (cpGroup && cpTable)
   {
      const unsigned int maxRowLength = 79;
      unsigned int currentLength = 0;
      unsigned int indent = 0;
      stringstream cpGroupStringStream;
      cpGroupStringStream<<endl<<"PREDEFINED CP GROUPS"<<endl<<endl;

      // Query for current O&M profile and extract corresponding CP GROUP members
      unsigned int currProfile;

      NSCommands::Exit_Code returnCode =
         ACS_CS_Command_Util::getCurrentOmProfile(currProfile);

      if (NSCommands::Exit_Success != returnCode)
      {
         ACS_CS_API::deleteCPGroupInstance(cpGroup);
         ACS_CS_API::deleteCPInstance(cpTable);
         return NSCommands::Exit_Error_Executing;
      }

      //Add group ALL or OPGROUP based on current O&M profile
      ACS_CS_API_IdList cpIds;
      ACS_CS_API_Name cpGroupName;

      if (ACS_CS_API_OmProfileChange::BladeProfile == currProfile) //BLADE profile
      {
         cpGroupName = ACS_CS_API_Name(ACS_CS_NS::CP_GROUP_ALL);
      }
      else // CLUSTER profile
      {
         cpGroupName = ACS_CS_API_Name(ACS_CS_NS::CP_GROUP_OPGROUP);
      }

      ACS_CS_API_NS::CS_API_Result result = cpGroup->getGroupMembers(cpGroupName, cpIds);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         if (ACS_CS_API_OmProfileChange::BladeProfile == currProfile)
         {
            cpGroupStringStream<<ACS_CS_NS::CP_GROUP_ALL<<": ";
            currentLength = indent = (unsigned int)string(ACS_CS_NS::CP_GROUP_ALL).length() + 2;
         }
         else
         {
            cpGroupStringStream<<ACS_CS_NS::CP_GROUP_OPGROUP<<": ";
            currentLength = indent = (unsigned int)string(ACS_CS_NS::CP_GROUP_OPGROUP).length() + 2;
         }

         for (unsigned int i = 0; i < cpIds.size(); i++) //loop through CPs belonging to the group
         {
            CPID cpId = cpIds[i];
            ACS_CS_API_Name cpName;
            //string cpNameStr;
            bool isAlias = false;

            result = cpTable->getCPAliasName(cpId, cpName, isAlias);

            if (result == ACS_CS_API_NS::Result_Success || result == ACS_CS_API_NS::Result_NoValue)
            {
               if (result == ACS_CS_API_NS::Result_NoValue) //if no alias, get the CP name
               {
            	   result = ACS_CS_API_NetworkElement::getDefaultCPName(cpId, cpName);
            	   if (result != ACS_CS_API_NS::Result_Success)
            	   {
            		   ACS_CS_API::deleteCPInstance(cpTable);
            		   return NSCommands::Exit_Error_Executing;
            	   }
               }

               size_t nameLength = cpName.length();
               char* nameChar = new char[nameLength];
               if (nameChar)
               {
                  result = cpName.getName(nameChar, nameLength);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     if (i == 0)
                     {
                        cpGroupStringStream<<nameChar;
                        currentLength = currentLength + string(nameChar).length();
                     }
                     else
                     {
                        if ((currentLength + 1 + string(nameChar).length()) < maxRowLength)
                        {
                           cpGroupStringStream<<","<<nameChar;
                           currentLength = currentLength + 1 + string(nameChar).length();
                        }
                        else
                        {
                           cpGroupStringStream<<","<<endl<<setw(indent)<<""<<nameChar;
                           currentLength = indent + string(nameChar).length();
                        }
                     }
                  }
                  else  //function getName failed
                  {
                     delete [] nameChar;
                     ACS_CS_API::deleteCPGroupInstance(cpGroup);
                     ACS_CS_API::deleteCPInstance(cpTable);
                     return NSCommands::Exit_Error_Executing;
                  }

                  delete [] nameChar;
               }
               else  //unsuccessful new (nameChar)
               {
                  ACS_CS_API::deleteCPGroupInstance(cpGroup);
                  ACS_CS_API::deleteCPInstance(cpTable);
                  return NSCommands::Exit_Error_Executing;
               }
            }
            else  //function getCPAliasName failed
            {
               ACS_CS_API::deleteCPGroupInstance(cpGroup);
               ACS_CS_API::deleteCPInstance(cpTable);
               return NSCommands::Exit_Error_Executing;
            }
         }  //end for-loop

         cpGroupStringStream<<endl;
      }
      else if (result == ACS_CS_API_NS::Result_NoEntry)
      {
         if (ACS_CS_API_OmProfileChange::BladeProfile == currProfile)
         {
            cpGroupStringStream<<ACS_CS_NS::CP_GROUP_ALL<<":"<<endl;
         }
         else
         {
            cpGroupStringStream<<ACS_CS_NS::CP_GROUP_OPGROUP<<":"<<endl;
         }
      }
      else  //function getGroupMembers failed
      {
         ACS_CS_API::deleteCPGroupInstance(cpGroup);
         ACS_CS_API::deleteCPInstance(cpTable);
         return NSCommands::Exit_Error_Executing;
      }

      //Add group ALLBC
      ACS_CS_API_Name cpGroupNameALLBC(ACS_CS_NS::CP_GROUP_ALLBC);

      result = cpGroup->getGroupMembers(cpGroupNameALLBC, cpIds);

      if (result == ACS_CS_API_NS::Result_Success)
      {
         cpGroupStringStream<<ACS_CS_NS::CP_GROUP_ALLBC<<": ";

         currentLength = indent = (unsigned int)string(ACS_CS_NS::CP_GROUP_ALLBC).length() + 2;

         for (unsigned int i = 0; i < cpIds.size(); i++)
         {
            CPID cpId = cpIds[i];
            ACS_CS_API_Name cpName;
            //string cpNameStr;
            bool isAlias = false;

            result = cpTable->getCPAliasName(cpId, cpName, isAlias);

            if (result == ACS_CS_API_NS::Result_Success || result == ACS_CS_API_NS::Result_NoValue)
            {
               if (result == ACS_CS_API_NS::Result_NoValue) //if no alias, get the CP name
               {
            	   result = ACS_CS_API_NetworkElement::getDefaultCPName(cpId, cpName);
            	   if (result != ACS_CS_API_NS::Result_Success)
            	   {
            		   ACS_CS_API::deleteCPInstance(cpTable);
            		   return NSCommands::Exit_Error_Executing;
            	   }
               }

               size_t nameLength = cpName.length();
               char* nameChar = new char[nameLength];
               if (nameChar)
               {
                  result = cpName.getName(nameChar, nameLength);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     if (i == 0)
                     {
                        cpGroupStringStream<<nameChar;
                        currentLength = currentLength + string(nameChar).length();
                     }
                     else
                     {
                        if ((currentLength + 1 + string(nameChar).length()) < maxRowLength)
                        {
                           cpGroupStringStream<<","<<nameChar;
                           currentLength = currentLength + 1 + string(nameChar).length();
                        }
                        else
                        {
                           cpGroupStringStream<<","<<endl<<setw(indent)<<""<<nameChar;
                           currentLength = indent + string(nameChar).length();
                        }
                     }
                  }
                  else  //function getName failed
                  {
                     delete [] nameChar;
                     ACS_CS_API::deleteCPGroupInstance(cpGroup);
                     ACS_CS_API::deleteCPInstance(cpTable);
                     return NSCommands::Exit_Error_Executing;
                  }

                  delete [] nameChar;
               }
               else  //unsuccessful new (nameChar)
               {
                  ACS_CS_API::deleteCPGroupInstance(cpGroup);
                  ACS_CS_API::deleteCPInstance(cpTable);
                  return NSCommands::Exit_Error_Executing;
               }
            }
            else  //function getCPAliasName failed
            {
               ACS_CS_API::deleteCPGroupInstance(cpGroup);
               ACS_CS_API::deleteCPInstance(cpTable);
               return NSCommands::Exit_Error_Executing;
            }
         }  //end for-loop

         cpGroupStringStream<<endl;
      }
      else if (ACS_CS_API_NS::Result_NoEntry == result)
      {
         cpGroupStringStream<<ACS_CS_NS::CP_GROUP_ALLBC<<":"<<endl;
      }
      else  //function getGroupMembers failed
      {
         ACS_CS_API::deleteCPGroupInstance(cpGroup);
         ACS_CS_API::deleteCPInstance(cpTable);
         return NSCommands::Exit_Error_Executing;
      }

      if (ACS_CS_API_OmProfileChange::BladeProfile == currProfile)
      {
         //Add operator defined groups
         cpGroupStringStream<<endl<<"OPERATOR DEFINED CP GROUPS"<<endl<<endl;

         ACS_CS_API_NameList nameList;

         result = cpGroup->getGroupNames(nameList);

         if (result == ACS_CS_API_NS::Result_Success)
         {
            for (unsigned int i = 0; i < nameList.size(); i++)
            {
               ACS_CS_API_Name cpGroupName = nameList[i];

               size_t cpGroupNameLength = cpGroupName.length();
               char* cpGroupNameChar = new char[cpGroupNameLength];

               if (cpGroupNameChar)
               {
                  result = cpGroupName.getName(cpGroupNameChar, cpGroupNameLength);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     string cpGroupNameString = cpGroupNameChar;

                     //Skip predefined groups as they have already been printed
                     if (cpGroupNameString == ACS_CS_NS::CP_GROUP_ALL ||
                        cpGroupNameString == ACS_CS_NS::CP_GROUP_ALLBC ||
                        cpGroupNameString == ACS_CS_NS::CP_GROUP_OPGROUP)
                     {
                        delete [] cpGroupNameChar;
                        continue;
                     }

                     cpGroupStringStream<<cpGroupNameChar<<": ";

                     currentLength = indent = (unsigned int)string(cpGroupNameChar).length() + 2;

                     result = cpGroup->getGroupMembers(cpGroupName, cpIds);

                     if (result == ACS_CS_API_NS::Result_Success)
                     {
                        for (unsigned int j = 0; j < cpIds.size(); j++)
                        {
                           CPID cpId = cpIds[j];
                           ACS_CS_API_Name cpName;
                          // string cpNameStr;
                           bool isAlias = false;

                           result = cpTable->getCPAliasName(cpId, cpName, isAlias);

                           if (result == ACS_CS_API_NS::Result_Success || result == ACS_CS_API_NS::Result_NoValue)
                           {
                              if (result == ACS_CS_API_NS::Result_NoValue) //if no alias, get the CP name
                              {
                           	   result = ACS_CS_API_NetworkElement::getDefaultCPName(cpId, cpName);
                           	   if (result != ACS_CS_API_NS::Result_Success)
                           	   {
                           		   ACS_CS_API::deleteCPInstance(cpTable);
                           		   return NSCommands::Exit_Error_Executing;
                           	   }
                              }

                              size_t nameLength = cpName.length();
                              char* nameChar = new char[nameLength];
                              if (nameChar)
                              {
                                 result = cpName.getName(nameChar, nameLength);

                                 if (result == ACS_CS_API_NS::Result_Success)
                                 {
                                    if (j == 0)
                                    {
                                       cpGroupStringStream<<nameChar;
                                       currentLength = currentLength + string(nameChar).length();
                                    }
                                    else
                                    {
                                       if ((currentLength + 1 + string(nameChar).length()) < maxRowLength)
                                       {
                                          cpGroupStringStream<<","<<nameChar;
                                          currentLength = currentLength + 1 + string(nameChar).length();
                                       }
                                       else
                                       {
                                          cpGroupStringStream<<","<<endl<<setw(indent)<<""<<nameChar;
                                          currentLength = indent + string(nameChar).length();
                                       }
                                    }
                                 }
                                 else  //function getName failed
                                 {
                                    delete [] nameChar;
                                    delete [] cpGroupNameChar;
                                    ACS_CS_API::deleteCPGroupInstance(cpGroup);
                                    ACS_CS_API::deleteCPInstance(cpTable);
                                    return NSCommands::Exit_Error_Executing;
                                 }

                                 delete [] nameChar;
                              }
                              else  //unsuccessful new (nameChar)
                              {
                                 delete [] cpGroupNameChar;
                                 ACS_CS_API::deleteCPGroupInstance(cpGroup);
                                 ACS_CS_API::deleteCPInstance(cpTable);
                                 return NSCommands::Exit_Error_Executing;
                              }
                           }
                           else  //function getCPAliasName failed
                           {
                              delete [] cpGroupNameChar;
                              ACS_CS_API::deleteCPGroupInstance(cpGroup);
                              ACS_CS_API::deleteCPInstance(cpTable);
                              return NSCommands::Exit_Error_Executing;
                           }
                        }  //end for-loop

                        cpGroupStringStream<<endl;
                     }
                     else  //function getGroupMembers failed
                     {
                        delete [] cpGroupNameChar;
                        ACS_CS_API::deleteCPGroupInstance(cpGroup);
                        ACS_CS_API::deleteCPInstance(cpTable);
                        return NSCommands::Exit_Error_Executing;
                     }
                  }
                  else  //function getName failed
                  {
                     delete [] cpGroupNameChar;
                     ACS_CS_API::deleteCPGroupInstance(cpGroup);
                     ACS_CS_API::deleteCPInstance(cpTable);
                     return NSCommands::Exit_Error_Executing;
                  }

                  delete [] cpGroupNameChar;
               }
               else  //unsuccessful new (cpGroupNameChar)
               {
                  ACS_CS_API::deleteCPGroupInstance(cpGroup);
                  ACS_CS_API::deleteCPInstance(cpTable);
                  return NSCommands::Exit_Error_Executing;
               }
            }  //end for-loop
         }
         else  //function getGroupNames failed
         {
            ACS_CS_API::deleteCPGroupInstance(cpGroup);
            ACS_CS_API::deleteCPInstance(cpTable);
            return NSCommands::Exit_Error_Executing;
         }
      }

      //Print the table
      cout<<cpGroupStringStream.str();

      ACS_CS_API::deleteCPGroupInstance(cpGroup);
      ACS_CS_API::deleteCPInstance(cpTable);
   }
   else  //create cpGroup and/or cpTable failed
   {
      if (cpGroup)
      {
         ACS_CS_API::deleteCPGroupInstance(cpGroup);
      }

      if (cpTable)
      {
         ACS_CS_API::deleteCPInstance(cpTable);
      }

      return NSCommands::Exit_Error_Executing;
   }

   return NSCommands::Exit_Success;
}
