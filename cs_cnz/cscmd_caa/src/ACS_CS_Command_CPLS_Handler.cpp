/*
 * ACS_CS_Command_CPLS_Handler.cpp
 *
 *  Created on: Aug 21, 2013
 *      Author: eanform
 */

#include "ACS_CS_Command_CPLS_Handler.h"

#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>

#include "ACS_CS_Command_Util.h"
#include "ACS_CS_Command_Exitcodes.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Registry.h"
using std::stringstream;
using std::endl;
using std::ios;
using std::left;
using std::setiosflags;
using std::resetiosflags;
using std::setw;
using std::cout;
using std::string;

//constructor
ACS_CS_Command_CPLS_Handler::ACS_CS_Command_CPLS_Handler(int argc, char* argv[])
:  boolLongList(false)
{
   this->argCount = argc;
   this->argValue = argv;
   this->exitCode = new ACS_CS_Command_Exitcodes(NSCommands::CPLS);
}

//destructor
ACS_CS_Command_CPLS_Handler::~ACS_CS_Command_CPLS_Handler()
{
   if (this->exitCode)
   {
      delete this->exitCode;
      this->exitCode = 0;
   }
}

bool ACS_CS_Command_CPLS_Handler::handleCommand()
{
   NSCommands::Exit_Code returnValue = ACS_CS_Command_Util::isLegalCommand();

   if (returnValue == NSCommands::Exit_Success)
   {
      if (! ACS_CS_API_Util::isCSRunning())
         return this->setExitCodeAndReturnFalse(NSCommands::Exit_Error_Executing);

      returnValue = this->checkOptions(this->argCount, this->argValue);

      if (returnValue == NSCommands::Exit_Success)
      {
         returnValue = this->CPTablePrint();

         if (returnValue != NSCommands::Exit_Success)
            return this->setExitCodeAndReturnFalse(returnValue);

         return this->setExitCodeAndReturnTrue(NSCommands::Exit_Success);
      }
      else
         return this->setExitCodeAndReturnFalse(returnValue);
   }
   else
      return this->setExitCodeAndReturnFalse(returnValue);
}

NSCommands::Exit_Code ACS_CS_Command_CPLS_Handler::checkOptions(int argc, char* argv[])
{
	this->boolLongList = false;

	if (argc > 2)  //check if more options than one (2 due to command name and option)
		return NSCommands::Exit_Incorrect_Usage;

	else if (argc == 2)  //check if number of options is one
	{
		if ((string)argv[1] == "-l")  //check if option is "-l"
		{
			int shelf_arch;
			ACS_CS_Registry::getNodeArchitecture(shelf_arch);
			if (shelf_arch == NODE_VIRTUALIZED)
			{
				return NSCommands::Exit_Illegal_Option ;
			}

			this->boolLongList = true;
		}
		else
			return NSCommands::Exit_Incorrect_Usage;
	}

	return NSCommands::Exit_Success;
}

NSCommands::Exit_Code ACS_CS_Command_CPLS_Handler::getExitCode()
{
   return this->exitCode->getExitCode();
}

string ACS_CS_Command_CPLS_Handler::getExitCodeDescription()
{
   return this->exitCode->getExitCodeDescription();
}

bool ACS_CS_Command_CPLS_Handler::setExitCodeAndReturnTrue(NSCommands::Exit_Code exitCodeToSet)
{
   this->exitCode->setExitCode(exitCodeToSet);
   return true;
}

bool ACS_CS_Command_CPLS_Handler::setExitCodeAndReturnFalse(NSCommands::Exit_Code exitCodeToSet)
{
   this->exitCode->setExitCode(exitCodeToSet);
   return false;
}

bool ACS_CS_Command_CPLS_Handler::isLongList() const
{
   return this->boolLongList;
}

NSCommands::Exit_Code ACS_CS_Command_CPLS_Handler::CPTablePrint()
{
   ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();

   if (cpTable)
   {
      ACS_CS_API_IdList cpIds;

      ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpIds);  //get a list of CP identities

      if (result == ACS_CS_API_NS::Result_Success)
      {
         stringstream cpTableStringStream;

         if (this->isLongList())
         {
            cpTableStringStream<<endl<<"CP IDENTIFICATION TABLE"<<endl;
            cpTableStringStream<<endl<<setiosflags(ios::left)<<setw(5)<<"CPID"<<setw(7)<<"CPNAME"<<
               setw(8)<<"ALIAS"<<setw(9)<<"APZSYS"<<setw(7)<<"CPTYPE"<<setw(11)<<"MAG"<<setw(5)<<"SLOT"<<
               setw(7)<<"FBN"<<setw(6)<<"CPSIDE"<<resetiosflags(ios::left)<<endl;
         }
         else
         {
            cpTableStringStream<<endl<<"CP IDENTIFICATION TABLE"<<endl;
            cpTableStringStream<<endl<<setiosflags(ios::left)<<setw(5)<<"CPID"<<setw(7)<<"CPNAME"<<
               setw(8)<<"ALIAS"<<setw(9)<<"APZSYS"<<setw(7)<<"CPTYPE"<<resetiosflags(ios::left)<<endl;
         }

         ACS_CS_API_HWC* hwcTable = NULL; // This is only used inside the cpid loop below, but it is better to
                                          // initialize it here in order to avoid creating multiple sockets
         if (this->isLongList())
         {
            //Get HWC table information
            hwcTable = ACS_CS_API::createHWCInstance();
            if (NULL == hwcTable)
            {
               ACS_CS_API::deleteCPInstance(cpTable);
               return NSCommands::Exit_Error_Executing;
            }
         }

         //convert into vector
         std::vector<unsigned short> cpListSorted;
         for (int k = 0; static_cast<int> (cpIds.size()) > k; ++k) {
        	 cpListSorted.push_back(cpIds[k]);
         }
         //sort CP table id list
         std::sort (cpListSorted.begin(),cpListSorted.end());

         for (std::vector<unsigned short>::iterator it = cpListSorted.begin() ; it != cpListSorted.end(); ++it)
         {
            ACS_CS_API_Name cpAlias;
            string cpAliasStr;
            ACS_CS_API_Name cpName;
            string cpNameStr;
            unsigned short apzSystem;
            string apzSystemString;
            unsigned short cpType;
            string cpTypeString;

            CPID cpId = *it;

            //Get default CP name
            result = ACS_CS_API_NetworkElement::getDefaultCPName(cpId, cpName);

            if (result == ACS_CS_API_NS::Result_Success)
            {
               size_t cpNameLength = cpName.length();
               char* cpNameChar = new char[cpNameLength];

               if (cpNameChar)
               {
                  result = cpName.getName(cpNameChar, cpNameLength);

                  if (result == ACS_CS_API_NS::Result_Success)
                     cpNameStr = cpNameChar;
                  else  //unable to get name
                  {
                     delete [] cpNameChar;
                     ACS_CS_API::deleteCPInstance(cpTable);
                     return NSCommands::Exit_Error_Executing;
                  }

                  delete [] cpNameChar;
               }
               else  //unsuccessful new (cpNameChar)
               {
                  ACS_CS_API::deleteCPInstance(cpTable);
                  return NSCommands::Exit_Error_Executing;
               }
            }
            else  //unable to get default CP name
            {
               ACS_CS_API::deleteCPInstance(cpTable);
               return NSCommands::Exit_Error_Executing;
            }

			bool isAlias = false;
			result = cpTable->getCPAliasName(cpId, cpAlias, isAlias); //Get CP alias

            if (result == ACS_CS_API_NS::Result_Success)
            {
            	size_t cpAliasLength = cpAlias.length();
               char* cpAliasChar = new char[cpAliasLength];

               if (cpAliasChar)
               {
                  result = cpAlias.getName(cpAliasChar, cpAliasLength);

                  if (result == ACS_CS_API_NS::Result_Success)
                     cpAliasStr = cpAliasChar;
                  else  //unable to get name
                  {
                     delete [] cpAliasChar;
                     ACS_CS_API::deleteCPInstance(cpTable);
                     return NSCommands::Exit_Error_Executing;
                  }

                  delete [] cpAliasChar;
               }
               else  //unsuccessful new (cpAliasChar)
               {
                  ACS_CS_API::deleteCPInstance(cpTable);
                  return NSCommands::Exit_Error_Executing;
               }
            }
            else if (result == ACS_CS_API_NS::Result_NoValue)  //no CP alias has been defined
               cpAliasStr = "-";
            else  //unable to get CP alias
            {
               ACS_CS_API::deleteCPInstance(cpTable);
               return NSCommands::Exit_Error_Executing;
            }

            if (cpNameStr == cpAliasStr)     //if CP name and alias is the same, alias should be printed as dash
               cpAliasStr = "-";

            //Get APZ system
            result = cpTable->getAPZSystem(cpId, apzSystem);

            if (result == ACS_CS_API_NS::Result_Success)
            {
               if ((apzSystem >= 21200 && apzSystem <= 21299) || (apzSystem >= 21400 && apzSystem <= 21499))
               {
                  stringstream tmpAPZSys;
                  tmpAPZSys<<"APZ"<<apzSystem;
                  apzSystemString = tmpAPZSys.str();
               }
               else
                  apzSystemString = "INVALID";
            }
            else if (result == ACS_CS_API_NS::Result_NoValue)
               apzSystemString = "-";
            else  //unable to get APZ system
            {
               ACS_CS_API::deleteCPInstance(cpTable);
               return NSCommands::Exit_Error_Executing;
            }

            //Get CP type
            result = cpTable->getCPType(cpId, cpType);

            if (result == ACS_CS_API_NS::Result_Success)
            {
               if (cpType >= 21200 && cpType <= 21299)
               {
                  stringstream tmpCPType;
                  tmpCPType<<cpType;
                  cpTypeString = tmpCPType.str();
               }
               else
                  cpTypeString = "INVALID";
            }
            else if (result == ACS_CS_API_NS::Result_NoValue)
               cpTypeString = "-";
            else  //unable to get CP type
            {
               ACS_CS_API::deleteCPInstance(cpTable);
               return NSCommands::Exit_Error_Executing;
            }

            cpTableStringStream<<setw(4)<<cpId<<setw(1)<<"";
            cpTableStringStream<<setiosflags(ios::left);
            cpTableStringStream<<setw(7)<<cpNameStr;
            cpTableStringStream<<setw(8)<<cpAliasStr;
            cpTableStringStream<<setw(9)<<apzSystemString;
            cpTableStringStream<<setw(7)<<cpTypeString;
            cpTableStringStream<<resetiosflags(ios::left);


            if (! this->isLongList())
               cpTableStringStream<<endl;
            else
            {
               ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();

               if (boardSearch)
               {
                  ACS_CS_API_IdList boardIds;

                  boardSearch->reset();
                  boardSearch->setSysId(cpId);     //get boards with current sysId

                  result = hwcTable->getBoardIds(boardIds, boardSearch);

                  if (result == ACS_CS_API_NS::Result_Success)
                  {
                     uint32_t magUL;
                     unsigned short slotUS, fbnUS, sideUS;
                     string magStr, slotStr, fbnStr, cpSideStr;
                     stringBoardIDMap idStrBoardIDMap;
                     stringBoardIDMap::iterator it;
                     BoardID boardId;
                     ACS_CS_API_NS::CS_API_Result resultHWC;

                     //Create a string of mag and slot for each board, add the
                     //string and the corresponding boardId to a map to get a
                     //sorted list regarding the columns mag and slot
                     for (unsigned int j = 0; j < boardIds.size(); j++)   //get mag and slot value for each board found
                     {
                        stringstream magStringStream, slotStringStream;
                        string idStr;

                        boardId = boardIds[j];

                        resultHWC = hwcTable->getMagazine(magUL, boardId); //get magazine value

                        if (resultHWC != ACS_CS_API_NS::Result_Success)
                        {
                           ACS_CS_API::deleteCPInstance(cpTable);
                           ACS_CS_API::deleteHWCInstance(hwcTable);
                           ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
                           return NSCommands::Exit_Error_Executing;
                        }

                        resultHWC = hwcTable->getSlot(slotUS, boardId);    //get slot value

                        if (resultHWC != ACS_CS_API_NS::Result_Success)
                        {
                           ACS_CS_API::deleteCPInstance(cpTable);
                           ACS_CS_API::deleteHWCInstance(hwcTable);
                           ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
                           return NSCommands::Exit_Error_Executing;
                        }

                        magStringStream<<(magUL & 0xFF)<<"."<<((magUL >> 8) &
                           0xFF)<<"."<<((magUL >> 16) & 0xFF)<<"."<<(magUL >> 24 & 0xFF); //convert mag to dotted decimal form
                        magStr = magStringStream.str();

                        if (slotUS < 10)
                           slotStringStream<<"0"<<slotUS;   //add a zero ahead of the one digit number to make the sorting work
                        else
                           slotStringStream<<slotUS;

                        slotStr = slotStringStream.str();

                        idStr = magStr + "_" + slotStr;  //create the string containing mag and slot (eg. 2.4.0.5_8)

                        idStrBoardIDMap.insert(stringBoardIDMap::value_type(idStr, boardId));   //add the string and boardId to the map
                     }  //end for loop

                     bool firstRow = true;

                     //Loop through the map from the beginning to the end, fetch the
                     //magazine, slot, functional board name and side value for each
                     //boardId and add them formatted to the stringstream to print
                     for (it = idStrBoardIDMap.begin(); it != idStrBoardIDMap.end(); ++it)   //loop through MAP
                     {
                        stringstream magStringStream, slotStringStream;

                        boardId = it->second;

                        resultHWC = hwcTable->getMagazine(magUL, boardId); //get magazine value

                        if (resultHWC != ACS_CS_API_NS::Result_Success)
                        {
                           ACS_CS_API::deleteCPInstance(cpTable);
                           ACS_CS_API::deleteHWCInstance(hwcTable);
                           ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
                           return NSCommands::Exit_Error_Executing;
                        }

                        resultHWC = hwcTable->getSlot(slotUS, boardId);    //get slot value

                        if (resultHWC != ACS_CS_API_NS::Result_Success)
                        {
                           ACS_CS_API::deleteCPInstance(cpTable);
                           ACS_CS_API::deleteHWCInstance(hwcTable);
                           ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
                           return NSCommands::Exit_Error_Executing;
                        }

                        resultHWC = hwcTable->getFBN(fbnUS, boardId);      //get FBN value

                        if (resultHWC != ACS_CS_API_NS::Result_Success)
                        {
                           ACS_CS_API::deleteCPInstance(cpTable);
                           ACS_CS_API::deleteHWCInstance(hwcTable);
                           ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
                           return NSCommands::Exit_Error_Executing;
                        }

                        resultHWC = hwcTable->getSide(sideUS, boardId);    //get side value

                        if (resultHWC == ACS_CS_API_NS::Result_Success)
                        {
                           switch (sideUS)
                           {
                           case ACS_CS_Protocol::Side_A  :  cpSideStr = "A"; break;
                           case ACS_CS_Protocol::Side_B  :  cpSideStr = "B"; break;
                           default                       :  cpSideStr = "-";
                           }
                        }
                        else if (resultHWC == ACS_CS_API_NS::Result_NoValue)
                           cpSideStr = "-";
                        else
                        {
                           ACS_CS_API::deleteCPInstance(cpTable);
                           ACS_CS_API::deleteHWCInstance(hwcTable);
                           ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
                           return NSCommands::Exit_Error_Executing;
                        }

                        magStringStream<<(magUL & 0xFF)<<"."<<((magUL >> 8) &
                           0xFF)<<"."<<((magUL >> 16) & 0xFF)<<"."<<(magUL >> 24 & 0xFF); //convert mag to dotted decimal form
                        magStr = magStringStream.str();

                        slotStringStream<<slotUS;
                        slotStr = slotStringStream.str();

                        switch (fbnUS)
                        {
                        case ACS_CS_Protocol::FBN_APUB   :  fbnStr = "APUB"; break;
                        case ACS_CS_Protocol::FBN_CPUB   :  fbnStr = "CPUB"; break;
                        case ACS_CS_Protocol::FBN_Disk   :  fbnStr = "Disk"; break;
                        case ACS_CS_Protocol::FBN_DVD    :  fbnStr = "DVD"; break;
                        case ACS_CS_Protocol::FBN_GEA    :  fbnStr = "GEA"; break;
                        case ACS_CS_Protocol::FBN_MAUB   :  fbnStr = "MAUB"; break;
                        case ACS_CS_Protocol::FBN_RPBIS  :  fbnStr = "RPBI-S"; break;
                        case ACS_CS_Protocol::FBN_SCBRP  :  fbnStr = "SCB-RP"; break;
                        default                          :  fbnStr = "-";
                        }

                        if (firstRow)
                        {
                           cpTableStringStream<<setiosflags(ios::left);
                           cpTableStringStream<<setw(11)<<magStr;
                           cpTableStringStream<<resetiosflags(ios::left);
                           firstRow = false;
                        }
                        else
                        {
                           cpTableStringStream<<setiosflags(ios::left);
                           cpTableStringStream<<setw(36)<<""<<setw(11)<<magStr;
                           cpTableStringStream<<resetiosflags(ios::left);
                        }

                        cpTableStringStream<<setw(4)<<slotStr<<setw(1)<<"";
                        cpTableStringStream<<setiosflags(ios::left);
                        cpTableStringStream<<setw(7)<<fbnStr;
                        cpTableStringStream<<resetiosflags(ios::left);
                        cpTableStringStream<<setw(6)<<cpSideStr<<endl;

                     }  //end for loop
                  }
                  else if (result == ACS_CS_API_NS::Result_NoAccess)
                  {
                     ACS_CS_API::deleteCPInstance(cpTable);
                     ACS_CS_API::deleteHWCInstance(hwcTable);
                     ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
                     return NSCommands::Exit_Error_Executing;
                  }
                  else
                  {
                     ACS_CS_API::deleteCPInstance(cpTable);
                     ACS_CS_API::deleteHWCInstance(hwcTable);
                     ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
                     return NSCommands::Exit_Error_Executing;
                  }
               }
               else  //create boardSearch failed
               {
                  ACS_CS_API::deleteCPInstance(cpTable);
                  ACS_CS_API::deleteHWCInstance(hwcTable);
                  return NSCommands::Exit_Error_Executing;
               }
            }
         }

         cout<<cpTableStringStream.str(); //print the table
      }
      else if (result == ACS_CS_API_NS::Result_NoAccess)
      {
         ACS_CS_API::deleteCPInstance(cpTable);
         return NSCommands::Exit_Error_Executing;
      }
      else
      {
         ACS_CS_API::deleteCPInstance(cpTable);
         return NSCommands::Exit_Error_Executing;
      }

      ACS_CS_API::deleteCPInstance(cpTable);
      return NSCommands::Exit_Success;
   }
   else  //create cpTable failed
      return NSCommands::Exit_Error_Executing;
}
