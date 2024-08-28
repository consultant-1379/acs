/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2012
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file acs_cs_moddcommands.cpp
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XNHAPHA
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2012-05-18  XMAGLEX  Removed all code not needed for hwcls since all other commands have been removed
 * 2013-11-05  XNHAPHA  Add opensession -a|-f [username] format to support change request O&M traffic
 *                      relay to AP2
 * 2014-04-08  XNHAPHA  Fix the TRs: HS46371 and HS42399
 *
 ****************************************************************************/

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>

#include "ACS_CS_Protocol.h"
#include "ACS_CS_Common_Util.h"

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImRepository.h"
#include "acs_apgcc_paramhandling.h"
#include "acs_cs_moddcommands.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"
#include "ACS_CS_API_BoardSearch_Implementation.h"
#include "ACS_DSD_Client.h"
#include "ACS_CS_Registry.h"

#define AP2_CLUSTER_IP_ETHA_ADDRESS       "192.168.169.34"
#define AP2_CLUSTER_IP_ETHB_ADDRESS       "192.168.170.34"
#define ROOT_DIRECTORY_SFTP               "/data/opt"
#define NBI_DIRECTORY_SFTP                "ap/nbi_fuse"

using namespace std;
//constructor

ACS_TRA_trace* CommandTrace::TraceObject = 0;

pid_t PARENT_COM_CLI_PID = 1;

MountFile m_sftpFileList[] =
{
   // file list in /bin directory
   {"/usr/bin/sftp", "bin/sftp"},
   {"/usr/bin/ssh", "usr/bin/ssh"},
   {"/bin/bash", "bin/bash"},
   {"/bin/sh", "bin/sh"},
   {"/bin/ls", "bin/ls"},
   {"/bin/mkdir", "bin/mkdir"},
   {"/usr/bin/getent", "bin/getent"}

};
  
MountFile m_LibDirectoryList[] =
{
   {"/lib64/", "lib64/"},
   {"/usr/lib64/", "usr/lib64/"}
};


ACS_CS_MODDCommands::ACS_CS_MODDCommands(string commandName)
   : commStruct(0),
   printStruct(0),
   printOutVector(0)
{
	
   this->commStruct = new commandStruct;  //creates a pointer to the struct commandStruct
   this->clearStruct(this->commStruct);   //sets default values for the variables in the struct
   this->commStruct->currCommand = stringToUpper(commandName); //assigns the current command name to the struct var
   this->setExitCodes(this->commStruct->currCommand);    //sets the exit code descriptions for the current command

}

//destructor
ACS_CS_MODDCommands::~ACS_CS_MODDCommands()
{
   delete this->commStruct;
   this->commStruct = 0;

   int printOutVectorSize = (int)this->printOutVector.size();
   for (int i = 0; i < printOutVectorSize; i++)
   {
      this->printStruct = this->printOutVector.back();
      if (this->printStruct)
      {
            delete this->printStruct;
         this->printStruct = 0;
      }
      this->printOutVector.pop_back();
   }
}

bool ACS_CS_MODDCommands::handleHWCLS(int argc, char* argv[])
{
   if (this->checkEnv())
   {
   	if (this->checkFormat(argc, argv))
   	{
      		if (this->getHWCTable())
      		{
    	 		this->sortTable();
 		        if (this->printTable())
		            return true;
      		}
   	}
   }
   return false;
}

bool ACS_CS_MODDCommands::checkEnv()
{
	const char* env_p = std::getenv("HWCLS");

	if(!env_p)
	{
		int shelf_arch;
		ACS_CS_Registry::getNodeArchitecture(shelf_arch);
		if (shelf_arch == NODE_VIRTUALIZED)
		{
			return faultyOption(NSCommand::Exit_Illegal_Command);
		}
	}

	return true;

}
bool ACS_CS_MODDCommands::checkFormat(int argc, char* argv[])
{
   //analyzes the options given in the command
   if (! this->checkOptions(argc, argv))  
      return false;

   //analyzes the option-arguments given in the command
   if (! this->checkOptionArguments(argc, argv))
      return false;

   return true;
}

bool ACS_CS_MODDCommands::checkArgMag(char* arg)
{
   istringstream istr(arg);

   this->reverseDottedDecStr(istr); //reverses the order of the dotted decimal string to match the mag addr format on the server

   if (numOfChar(istr,'.') != 3) // If not 3 dots, faulty format
      return false;

   if (! dottedDecToLong(istr, this->commStruct->magazine, 0, 15, 0, 0, 0, 15, 0, 15))
      return false;

   return this->commStruct->boolMagazine = true;
}

bool ACS_CS_MODDCommands::checkArgSlot(char* arg)
{
   istringstream istr(arg);

   if (! this->checkFigures(istr))  //checks that only figures (0-9) are used in the option arg.
      return false;

   if (! (istr >> this->commStruct->slot))   //try to convert operand to slot number
      return false;

   if (this->commStruct->slot == 27)
	   return false;

   if (this->commStruct->slot > 28)
	   return false;

   return this->commStruct->boolSlot = true;
}

bool ACS_CS_MODDCommands::checkArgFBN(char* arg)
{
   istringstream istr(arg);
   string fbnString = "";

   if (! (istr >> fbnString)) // Try to convert operand to functional board name
      return false;

   fbnString = stringToUpper(fbnString);
   if (! (fbnString == "APUB" || fbnString == "CPUB" || fbnString == "DISK" || 
         fbnString == "DVD" || fbnString == "GEA" || fbnString == "MAUB" || 
         fbnString == "RPBI-S" || fbnString == "SCB-RP" || fbnString == "GESB" || //CR GESB
         fbnString == "SCXB" || fbnString == "EPB1" || fbnString == "IPTB" ||
         fbnString == "CMXB" || fbnString == "EVOET" || fbnString == "IPLB" || fbnString == "SMXB"))
      return false;

   if (fbnString == "APUB")
      this->commStruct->fbn = ACS_CS_Protocol::FBN_APUB;
   else if (fbnString == "CPUB")
      this->commStruct->fbn = ACS_CS_Protocol::FBN_CPUB;
   else if (fbnString == "DISK")
      this->commStruct->fbn = ACS_CS_Protocol::FBN_Disk;
   else if (fbnString == "DVD")
      this->commStruct->fbn = ACS_CS_Protocol::FBN_DVD;
   else if (fbnString == "GEA")
      this->commStruct->fbn = ACS_CS_Protocol::FBN_GEA;
   else if (fbnString == "MAUB")
      this->commStruct->fbn = ACS_CS_Protocol::FBN_MAUB;
   else if (fbnString == "RPBI-S")
      this->commStruct->fbn = ACS_CS_Protocol::FBN_RPBIS;
   else if (fbnString == "SCB-RP")
      this->commStruct->fbn = ACS_CS_Protocol::FBN_SCBRP;
   else if (fbnString == "GESB")
      this->commStruct->fbn = ACS_CS_Protocol::FBN_GESB; //CR GESB
   else if (fbnString == "SCXB")
	   this->commStruct->fbn = ACS_CS_Protocol::FBN_SCXB;
   else if (fbnString == "EPB1")
   	   this->commStruct->fbn = ACS_CS_Protocol::FBN_EPB1;
   else if (fbnString == "IPTB")
   	   this->commStruct->fbn = ACS_CS_Protocol::FBN_IPTB;
   else if (fbnString == "EVOET")
	   this->commStruct->fbn = ACS_CS_Protocol::FBN_EvoET;
   else if (fbnString == "CMXB")
   	   this->commStruct->fbn = ACS_CS_Protocol::FBN_CMXB;
   else if (fbnString == "IPLB")
	   this->commStruct->fbn = ACS_CS_Protocol::FBN_IPLB;
   else if (fbnString == "SMXB")
           this->commStruct->fbn = ACS_CS_Protocol::FBN_SMXB;
   else
      return false;  

   return this->commStruct->boolFbn = true;
}

bool ACS_CS_MODDCommands::checkArgSysType(char* arg, bool& cpBlade)
{
   istringstream istr(arg);
   string systemTypeString = "";
   cpBlade = false;

   if( ! (istr >> systemTypeString) )     // Try to convert operand to system type
      return false;

   systemTypeString = stringToUpper(systemTypeString);
   if (! (systemTypeString == "BC" || systemTypeString == "CP" || systemTypeString == "AP"))
      return false;

   if (systemTypeString == "BC")
   {
      this->commStruct->systemType = ACS_CS_Protocol::SysType_BC;
      cpBlade=true;
   }
   else if (systemTypeString == "CP")
      this->commStruct->systemType = ACS_CS_Protocol::SysType_CP;
   else if (systemTypeString == "AP")
      this->commStruct->systemType = ACS_CS_Protocol::SysType_AP;
   else
      return false;

   return this->commStruct->boolSystemType = true;
}

bool ACS_CS_MODDCommands::checkArgSysNum(char* arg)
{
   istringstream istr(arg);

   if (! this->checkFigures(istr))  //checks that only figures (0-9) are used in the option arg.
      return false;

   if(! (istr >> this->commStruct->systemNum) )    // Try to convert operand to system number
      return false;

   if (this->commStruct->systemNum > 999)
      return false;
   
   return this->commStruct->boolSystemNum = true;
}

bool ACS_CS_MODDCommands::checkArgSide(char* arg)
{
   istringstream istr(arg);
   string sideString = "";

   if(! (istr >> sideString) )      // Try to convert operand to side
      return false;

   sideString = stringToUpper(sideString);
   if (! (sideString == "A" || sideString == "B" ))
      return false;

   if (sideString == "A")
      this->commStruct->side = ACS_CS_Protocol::Side_A;
   else if (sideString == "B")
      this->commStruct->side = ACS_CS_Protocol::Side_B;
   else
      return false;

   return this->commStruct->boolSide = true;
}

bool ACS_CS_MODDCommands::checkArgDHCP(char* arg)
{
   istringstream istr(arg);
   string dhcpReqString = "";

   if (! (istr >> dhcpReqString))   // Try to convert operand to DHCP request method (string)
      return false;

   dhcpReqString = stringToUpper(dhcpReqString);
   if (! (dhcpReqString == "NORMAL" || dhcpReqString == "CLIENT" || dhcpReqString == "NONE"))
      return false;

   if (dhcpReqString == "NORMAL")
      this->commStruct->dhcpReq = ACS_CS_Protocol::DHCP_Normal;
   else if (dhcpReqString == "CLIENT")
      this->commStruct->dhcpReq = ACS_CS_Protocol::DHCP_Client;
   else if (dhcpReqString == "NONE")
      this->commStruct->dhcpReq = ACS_CS_Protocol::DHCP_None;
   else
      return false;

   return this->commStruct->boolDhcpReq = true;
}

bool ACS_CS_MODDCommands::checkArgOutputFormat(char* arg)
{
   istringstream istr(arg);
   string formatString = "";

   if (! (istr >> formatString)) // Try to convert operand to output format (string)
      return false;

   formatString = stringToUpper(formatString);
   if (! (formatString == "SHORT" || formatString == "LONG" || formatString == "XML"))
      return false;

   if (formatString == "SHORT")
      this->commStruct->outputFormat = NSCommand::Output_Format_SHORT;
   else if (formatString == "LONG")
      this->commStruct->outputFormat = NSCommand::Output_Format_LONG;
   else if (formatString == "XML")
      this->commStruct->outputFormat = NSCommand::Output_Format_XML;
   else
      return false;  

   return this->commStruct->boolOutputFormat = true;
}

bool ACS_CS_MODDCommands::dottedDecToLong(istringstream &istr, uint32_t &value,
                              uint32_t lowerLimit1, uint32_t upperLimit1,
                              uint32_t lowerLimit2, uint32_t upperLimit2,
                              uint32_t lowerLimit3, uint32_t upperLimit3,
                              uint32_t lowerLimit4, uint32_t upperLimit4)
{
   istr.clear();     // return to the beginning
   (void)istr.seekg(0);    //      of the stream
   char c;
   string strAddr = "";
   unsigned short dotNum = 0;
   value = 0;
   
   do {
      c = istr.get();
      if (c == '.' || c == EOF)
      {
         dotNum++;
         istringstream converter(strAddr);

         if (! this->checkFigures(converter))   //checks that only figures (0-9) are used in the option arg.
            return false;

         strAddr = "";
         unsigned long plug;
         if (! (converter >> plug)) //check if string is possible to convert to long
         {
            value = 0;
            return false;
         }

         else
         {
            if (dotNum == 1)
            {
               if (! (((plug >= lowerLimit1) && (plug <= upperLimit1)) || (plug == 255)))  //CR GESB
               {
                  value = 0;
                  return false;
               }
            }

            else if (dotNum == 2)
            {
               if (! ((plug >= lowerLimit2) && (plug <= upperLimit2)))
               {
                  value = 0;
                  return false;
               }
            }

            else if (dotNum == 3)
            {
               if (! (((plug >= lowerLimit3) && (plug <= upperLimit3)) || (plug == 255)))  //CR GESB
               {
                  value = 0;
                  return false;
               }
            }

            else if (dotNum == 4)
            {
               if (! (((plug >= lowerLimit4) && (plug <= upperLimit4)) || (plug == 255)))  //CR GESB
               {
                  value = 0;
                  return false;
               }
            }

            value = plug | value;
                  
            if (dotNum != 4)     // if not last byte, shift 1 byte to the left
               value = value << 8;
         }
      }

      else
         strAddr = strAddr + c;
   } while (c != EOF);

   return true;
}

unsigned short ACS_CS_MODDCommands::numOfChar(istringstream &istr, char searchChar)
{
   istr.clear();        // return to the beginning
   (void)istr.seekg(0); //      of the stream
   char c;
   unsigned short numChar = 0;

   do {
      c = istr.peek();
      if (c == searchChar)
         numChar = numChar + 1;
      
      (void)istr.seekg((int)(istr.tellg()) + 1);
   } while (c != EOF);

   istr.clear();     // return to the beginning
   (void)istr.seekg(0);    //      of the stream
   return numChar;
}

string ACS_CS_MODDCommands::getArgument(istringstream &istr, uint16_t optionNum)
{
   istr.clear();        // return to the beginning
   (void)istr.seekg(0); //      of the stream
   char c;
   unsigned short numCommas = 0;
   string optionString = "";

   do {
      c = istr.peek();
      if (c != ',')
      {
         if (numCommas == optionNum)
            optionString = optionString + c;
      }
      else
         numCommas = numCommas + 1;
      
      (void)istr.seekg((int)(istr.tellg()) + 1);
   } while (c != EOF);
   
   istr.clear();        // return to the beginning
   (void)istr.seekg(0); //      of the stream
   return optionString;
}

bool ACS_CS_MODDCommands::faultyOption(NSCommand::Exit_Code exitCode, string faultyOptionArg)
{
   clearStruct(this->commStruct);         //clears the contents of the struct
   
   this->commStruct->exitCode = exitCode; //assigns the exit code to the commandstruct
   this->commStruct->exitCodeDescriptionArg = faultyOptionArg; //assigns faulty option/option arg to the commandstruct

   return false;
}

bool ACS_CS_MODDCommands::faultyOption(NSCommand::Exit_Code exitCode)
{
   (void)this->faultyOption(exitCode, "");

   return false;
}

//change each element of the string to upper case
string ACS_CS_MODDCommands::stringToUpper(string strToConvert)
{                                   
   for(unsigned int i = 0; i < strToConvert.length(); i++)
   {
      strToConvert[i] = toupper(strToConvert[i]);
   }
   return strToConvert;       //return the converted string
}

//analyzes the options given in the command
bool ACS_CS_MODDCommands::checkOptions(int argc, char* argv[])
{
   TRACE("checkOptions()");
   
   string arg = "";
   string optionString = "";
   string commandString = this->commStruct->currCommand;

   if (commandString == "HWCSYNC")
   {
      if (argc > 1)
         return faultyOption(NSCommand::Exit_Incorrect_Usage);
   }

   //create a string of given options
   for (int i = 1; i < argc; i++)   //loop through option list
   {
      arg = argv[i];          //get current option

      if (arg.substr(0,1) == "-" && arg.size() == 2)  //look for options (size 2 and starts with "-", e.g. "-m")
         optionString = optionString + arg.substr(1,1);  //create a string of options given
   }

   NSCommand::Exit_Code exitCode;
   string illegalOption = "";

   if (commandString == "HWCLS")
   {
      if (! checkHwcLsOptions(optionString, exitCode, illegalOption))
         return faultyOption(exitCode, illegalOption);
   }

   if (commandString == "OPENSESSION")
   {     
      if (! checkOpenSessionOptions(optionString, argc, exitCode, illegalOption))
         return faultyOption(exitCode, illegalOption);
   }

   //check for more than one occurrence of an option
   if (optionString.find("m") != optionString.rfind("m") || optionString.find("s") != optionString.rfind("s") ||
       optionString.find("n") != optionString.rfind("n") || optionString.find("t") != optionString.rfind("t") ||
       optionString.find("y") != optionString.rfind("y") || optionString.find("e") != optionString.rfind("e") ||
       optionString.find("d") != optionString.rfind("d") || optionString.find("x") != optionString.rfind("x"))
      return faultyOption(NSCommand::Exit_Incorrect_Usage);

   return true;
}

bool ACS_CS_MODDCommands::checkHwcLsOptions(string optionString, NSCommand::Exit_Code &exitCode, string &illegalOption)
{
   //check that only valid options have been used
   for (unsigned short i = 0; i < optionString.length(); i++)
   {
      string currOption = optionString.substr(i,1);

      if (currOption != "m" && currOption != "s" && 
         currOption != "n" && currOption != "t" && 
         currOption != "y" && currOption != "e" && 
         currOption != "d" && currOption != "x")
      {
         exitCode = NSCommand::Exit_Illegal_Option;
         illegalOption = "-" + currOption;
         return false;
      }
   }

   return true;
}

//analyzes the option arguments given in the command
bool ACS_CS_MODDCommands::checkOptionArguments(int argc, char* argv[])
{
   string arg = "";
   string commandString = this->commStruct->currCommand;
   bool cpBlade = false;

   //check that all options have option arguments (except -f)
   for (int j = 1; j < argc; j++)   // Loop through option list
   {
	   arg = argv[j];          // Get current option

	   if (arg.substr(0,1) == "-" && arg.size() == 2)  //look for options (size 2 and starts with "-", e.g. "-m")
	   {
		   if (j < (argc - 1))
		   {
			   j++;
			   arg = argv[j]; //get the option arg
			   if (arg.substr(0,1) == "-" && arg.size() == 2)  //check if option is followed by option
				   return faultyOption(NSCommand::Exit_Incorrect_Usage);
		   }
	   }
   }

   char * mag_arg = 0;

   for (int i = 1; i < argc; i++)      //loop through option list
   {
      arg = argv[i];       //get current option

      if ((i + 1) >= argc)    //check if command ends with option instead of option argument
      {
            return faultyOption(NSCommand::Exit_Incorrect_Usage);                         
      }
      
      if (arg == "-m")           // Check option -m, magazine address
      {
         if (! checkArgMag(mag_arg = argv[i+1])) //CR GESB
            return faultyOption(NSCommand::Exit_Incorrect_Mag, argv[i+1]);
      }
      
      else if (arg == "-s")         // Check option -s, slot position
      {
         if (! checkArgSlot(argv[i+1]))
            return faultyOption(NSCommand::Exit_Incorrect_Slot, argv[i+1]);
      }
      
      else if (arg == "-n")         // Check option -n, functional board name
      {
         if (! checkArgFBN(argv[i+1]))
            return faultyOption(NSCommand::Exit_Incorrect_FBN, argv[i+1]);
      }

      else if (arg == "-t")      // Check option -t, system type
      {
         if (! checkArgSysType(argv[i+1],cpBlade))
            return faultyOption(NSCommand::Exit_Incorrect_SysType, argv[i+1]);
      }

      else if (arg == "-y")      // Check option -y, system number
      {
         if (! checkArgSysNum(argv[i+1]))
            return faultyOption(NSCommand::Exit_Incorrect_SysNum, argv[i+1]);
      }

      else if (arg == "-e")         // Check option -e, side identifier
      {
         if (! checkArgSide(argv[i+1]))
            return faultyOption(NSCommand::Exit_Incorrect_Side, argv[i+1]);
      }
      
      else if (arg == "-d")         // Check option -d, DHCP request method
      {
         if (! checkArgDHCP(argv[i+1]))
            return faultyOption(NSCommand::Exit_Incorrect_DHCPReq, argv[i+1]);
      }

      else if (arg == "-x")         // Check option -x, output format
      {
         if (! checkArgOutputFormat(argv[i+1]))
            return faultyOption(NSCommand::Exit_Incorrect_OutputFormat, argv[i+1]);
      }

      else                    // Illegal option
         return faultyOption(NSCommand::Exit_Incorrect_Usage);

      
      i++;  //jump to next option (skip the option argument)
   }

   //CR GESB
   if (this->commStruct->boolMagazine
      && this->commStruct->boolFbn
      && (this->commStruct->fbn ^ ACS_CS_Protocol::FBN_GESB)
      && (((this->commStruct->magazine & 0xFF000000) == 0xFF000000)
            || ((this->commStruct->magazine & 0x0000FF00) == 0x0000FF00)
            || ((this->commStruct->magazine & 0x000000FF) == 0x000000FF)))
   {
      return faultyOption(NSCommand::Exit_Incorrect_Mag, mag_arg ? mag_arg : "");
   }

   if (this->commStruct->currCommand == "HWCLS" && this->commStruct->boolOutputFormat == false) //set default value for
   {                                                                       //option -x in hwcls
      this->commStruct->outputFormat = NSCommand::Output_Format_SHORT;
      this->commStruct->boolOutputFormat = true;
   }

   return true;
}

void ACS_CS_MODDCommands::clearStruct(commandStruct* cStruct)
{
   cStruct->currCommand = "";
   
   cStruct->boolMagazine = false;
   cStruct->magazine = 0;
   cStruct->boolSlot = false;
   cStruct->slot = 0;
   cStruct->boolFbn = false;
   cStruct->fbn = 0;
   cStruct->boolSystemType = false;
   cStruct->systemType = 0;
   cStruct->boolSystemNum = false;
   cStruct->systemNum = 0;
   cStruct->boolSeqNum = false;
   cStruct->seqNum = 0;
   cStruct->boolSide = false;
   cStruct->side = 0;
   cStruct->boolDhcpReq = false;
   cStruct->dhcpReq = 0;
   cStruct->boolOutputFormat = false;
   cStruct->outputFormat = 0;
   cStruct->exitCode = 0;
   cStruct->exitCodeDescriptionArg = "";
   cStruct->openSessionFormat1= false;
   cStruct->openSessionFormat2= false;
   cStruct->openSessionFormat3= false;
   cStruct->openSessionType = NSCommand::TELNET;   // Default Open Session Type is telnet
   cStruct->username = "";
}

void ACS_CS_MODDCommands::setExitCodes(string currCommand)
{
   exitCodeMap[0] = "Success";
   exitCodeMap[1] = "Error when executing (general fault)";

   if (currCommand == "HWCADD")
      exitCodeMap[2] = (string)"Incorrect usage\n" + 
                       "Usage: hwcadd -m magazine -s slot -n fbn [-u seqNum] [-e side] [-d dhcpReq]\n\n" +
                       "Usage: hwcadd -m magazine -s slot -n fbn -t systemType -y systemNum [-u seqNum] [-e side] [-d dhcpReq]\n\n" +
                       "Usage: hwcadd -m magazine -s slot -n fbn -a ipAlias -b netmask [-u seqNum] [-e side] [-d dhcpReq]\n\n" +
                       "Usage: hwcadd -m magazine -s slot -n fbn -t systemType -y systemNum -a ipAlias -b netmask [-u seqNum] [-e side] [-d dhcpReq]";
   else if (currCommand == "HWCLS")
      exitCodeMap[2] = "Incorrect usage\nUsage: hwcls [-m magazine] [-s slot] [-n fbn] [-t systemType] [-y systemNum] [-e side] [-d dhcpReq] [-x format]";
   else if (currCommand == "HWCRM")
      exitCodeMap[2] = "Incorrect usage\nUsage: hwcrm -m magazine -s slot [-f]";
   else if (currCommand == "HWCSYNC")
      exitCodeMap[2] = "Incorrect usage\nUsage: hwcsync";
   else if (currCommand == "OPENSESSION")
   {
      identifyApplicableFormat(this->commStruct->openSessionFormat1, 
                               this->commStruct->openSessionFormat2,
                               this->commStruct->openSessionFormat3);
      
      exitCodeMap[2] = "Incorrect usage\n";
      
      if(this->commStruct->openSessionFormat1 == true)
      {
         exitCodeMap[2] += (string) "Usage: opensession fbn\n";
      }
      
      if(this->commStruct->openSessionFormat2 == true)
      {
         exitCodeMap[2] += (string) "Usage: opensession -m magazine -s slot\n";
      }
      
      if(this->commStruct->openSessionFormat3 == true)
      {
         exitCodeMap[2] += (string) "Usage: opensession -a [username]\nUsage: opensession -f [username]\nUsage: opensession -t [username]\n";
      }
      
      // exitCodeMap for Exit_Incorrect_Usage_Full
      exitCodeMap[NSCommand::Exit_Incorrect_Usage_Full] = (string)"Incorrect usage\n" + 
                                                                  "Usage: opensession fbn\n" +
                                                                  "Usage: opensession -m magazine -s slot\n" +
                                                                  "Usage: opensession -a [username]\n" +
                                                                  "Usage: opensession -f [username]\n" +
																  "Usage: opensession -t [username]\n";
   }
   else
      exitCodeMap[2] = "Incorrect usage";

   exitCodeMap[7] = "No reply from Configuration Service";
   exitCodeMap[5] = "Illegal option ";
   exitCodeMap[10] = "Only allowed from AP1";
   exitCodeMap[14]= "Hardware not present";   
   exitCodeMap[15] = "Slot already occupied";
   exitCodeMap[16] = "No such entry";
   exitCodeMap[21] = "Incorrect magazine address ";
   exitCodeMap[22] = "Incorrect slot position ";
   exitCodeMap[23] = "Incorrect functional board name ";
   exitCodeMap[24] = "Incorrect system number ";
   exitCodeMap[25] = "Incorrect sequence number ";
   exitCodeMap[26] = "Incorrect side ";
   exitCodeMap[27] = "Incorrect IP alias ";
   exitCodeMap[28] = "Incorrect netmask ";
   exitCodeMap[29] = "Incorrect DHCP request method ";
   exitCodeMap[30] = "Incorrect system type ";
   exitCodeMap[31] = "Incorrect output format ";
   exitCodeMap[32] = "Sequence number for blade CP outside range";
   exitCodeMap[33] = "Functional board name not allowed for this SS";
   
   exitCodeMap[112] = "Not allowed for this Functional Board Name";
   exitCodeMap[115] = "Illegal command in this system configuration";

   //exitCodeMap[102] = "Unable to contact server"; // APG43 exit code
   exitCodeMap[NSCommand::Exit_Unable_Contact_Server] = "Unable to connect to server"; // Blade Cluster exit code
   exitCodeMap[119] = "Connection failure ";
   exitCodeMap[NSCommand::Exit_No_Config_Info] = "System configuration information not available";
}

unsigned short ACS_CS_MODDCommands::getExitCode()
{
   return this->commStruct->exitCode;
}

string ACS_CS_MODDCommands::getExitCodeDescription(const unsigned short exitCode)
{
   return this->exitCodeMap[exitCode];
}

string ACS_CS_MODDCommands::getExitCodeDescriptionArg()
{
   return this->commStruct->exitCodeDescriptionArg;
}

void ACS_CS_MODDCommands::hwclsFilter(std::set<const ACS_CS_ImBase *> &objects, std::map<std::string, std::string> parentMap)
{
	std::set<const ACS_CS_ImBase *>::const_iterator it;
	std::set<const ACS_CS_ImBase *>::const_iterator it_next;

	for (it = objects.begin(), it_next = it; it != objects.end(); it = it_next)
	{
		++it_next;
		bool matching = true;
		const ACS_CS_ImBlade * blade = reinterpret_cast<const ACS_CS_ImBlade *>(*it);

		if(!blade)
			return;

		if (matching && this->commStruct->boolMagazine)
		{
			std::string parent = getParentFromRdn(blade->rdn);
			map<std::string, std::string>::iterator map_it;
			map_it = parentMap.find(parent);

			if(map_it != parentMap.end())
			{
				struct in_addr addr;
				addr.s_addr = commStruct->magazine;
				string dot_ip = inet_ntoa(addr);

				string address = map_it->second;

				if(dot_ip.compare(address) != 0)
					matching = false;
			}
		}

		if (matching && this->commStruct->boolSlot)
		{
			if(this->commStruct->slot != blade->slotNumber)
			{
				matching = false;
			}
		}

		if (matching && this->commStruct->boolFbn)
		{
			if(this->commStruct->fbn != blade->functionalBoardName)
				matching = false;
		}

		if (matching && this->commStruct->boolSystemType)
		{
			if(this->commStruct->systemType != blade->systemType)
				matching = false;
		}

		if (matching && this->commStruct->boolSystemNum)
		{
			if(this->commStruct->systemNum != blade->systemNumber)
				matching = false;
		}

		if (matching && this->commStruct->boolSide)
		{
			if(this->commStruct->side != blade->side)
				matching = false;
		}

		if (matching && this->commStruct->boolDhcpReq)
		{
			if(this->commStruct->dhcpReq != blade->dhcpOption)
				matching = false;
		}

		if(!matching)
			objects.erase(it);
	}
}

string ACS_CS_MODDCommands::getParentFromRdn(string rdn)
{
	string parent = "";
	size_t pos = rdn.find(",");

	if (pos != string::npos){
		parent = rdn.substr(pos+1);
	}

	return parent;
}

bool ACS_CS_MODDCommands::getHWCTable()
{
	size_t numOfBoards = 0;
	bool boolTable = false; //flag indicating if any board has been fetched
	this->commStruct->exitCode = NSCommand::Exit_Unable_Contact_Server;     //default error code

	// We need Shelf/Magazine address for each board
	// - Fetch them once and store in this map
	std::map<std::string, std::string> parentMap;

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	std::vector<std::string> dnList;
	std::vector<std::string>::iterator dnList_it;

	result = immHandle.Init();

	if(result != ACS_CC_SUCCESS)
		return false;

	// Use of APGCC classes instead of ACS_CS_ImModel that load whole model -> time consuming
	// - Now only fetch Blade objects!
	result = immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_AP_BLADE.c_str(), dnList);
	result = immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_CP_BLADE.c_str(), dnList);
	result = immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_OTHER_BLADE.c_str(), dnList);

	//ACS_CS_ImModel model_;
	std::set<const ACS_CS_ImBase *> objects;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	// Fetch the objects who's rdns where returned above
	for(dnList_it = dnList.begin(); dnList_it != dnList.end(); dnList_it++){

		string dn = *dnList_it;
		const ACS_CS_ImBase * blade = immReader->getObject(dn, immHandle);

		// Insert to object/blade vector and fetch parent Shelf address
		if(blade)
		{
			objects.insert(blade);
			string parentRdn = getParentFromRdn(blade->rdn);
			std::map<std::string, std::string>::iterator map_it;

			map_it = parentMap.find(parentRdn);

			if(map_it == parentMap.end()){
				const ACS_CS_ImBase * shelfBase = immReader->getObject(parentRdn, immHandle);

				if(shelfBase){
					const ACS_CS_ImShelf *shelf = reinterpret_cast<const ACS_CS_ImShelf *>(shelfBase);
					parentMap.insert(pair<std::string, std::string>(parentRdn, shelf->address));
					delete shelfBase;
				}
			}
		}
	}

	delete immReader;
	immHandle.Finalize();
	std::set<const ACS_CS_ImBase *>::const_iterator it;

	hwclsFilter(objects, parentMap);
	numOfBoards = objects.size();

	size_t numOfAttributes = 0;
	bool boolOtherFailure = false;   //flag indicating if no more server requests should be sent

	if ((this->commStruct->outputFormat && this->commStruct->outputFormat == NSCommand::Output_Format_LONG) ||  //long format or XML printout ordered
			(this->commStruct->outputFormat && this->commStruct->outputFormat == NSCommand::Output_Format_XML))
		numOfAttributes = 15;
	else
		numOfAttributes = 7;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImBlade * blade = reinterpret_cast<const ACS_CS_ImBlade *>(*it);

		this->printStruct = new printOut;
		bool missingEntry = false;

		size_t totalNumOfAttributes = 0;
		ACS_CS_Attribute **attributes = NULL;

		if(this->getAttributes(&attributes, blade, totalNumOfAttributes, parentMap)) //Convert to ACS_CS_Attributes to keep code from old model
		{
			// current attribute to fetch
			size_t attributeIndex = -1;
			uint16_t attributeIdentifier = -1;

			for (size_t attId = 0; attId < numOfAttributes; attId++)
			{
				switch (attId)
				{
				case 0: attributeIdentifier = ACS_CS_Protocol::Attribute_Magazine; break;
				case 1: attributeIdentifier = ACS_CS_Protocol::Attribute_Slot; break;
				case 2: attributeIdentifier = ACS_CS_Protocol::Attribute_SysType; break;
				case 3: attributeIdentifier = ACS_CS_Protocol::Attribute_SysNo; break;
				case 4: attributeIdentifier = ACS_CS_Protocol::Attribute_FBN; break;
				}

				if ((this->commStruct->outputFormat && this->commStruct->outputFormat == NSCommand::Output_Format_LONG) ||  //long format or XML printout ordered
						(this->commStruct->outputFormat && this->commStruct->outputFormat == NSCommand::Output_Format_XML))
				{
					switch (attId)
					{
					case 5: attributeIdentifier = ACS_CS_Protocol::Attribute_Side; break;
					case 6: attributeIdentifier = ACS_CS_Protocol::Attribute_SeqNo; break;
					case 7: attributeIdentifier = ACS_CS_Protocol::Attribute_IP_EthA; break;
					case 8: attributeIdentifier = ACS_CS_Protocol::Attribute_IP_EthB; break;
					case 9: attributeIdentifier = ACS_CS_Protocol::Attribute_Alias_EthA; break;
					case 10: attributeIdentifier = ACS_CS_Protocol::Attribute_Netmask_Alias_EthA; break;
					case 11: attributeIdentifier = ACS_CS_Protocol::Attribute_Alias_EthB; break;
					case 12: attributeIdentifier = ACS_CS_Protocol::Attribute_Netmask_Alias_EthB; break;
					case 13: attributeIdentifier = ACS_CS_Protocol::Attribute_DHCP; break;
					case 14: attributeIdentifier = ACS_CS_Protocol::Attribute_Blade_Product_Number; break;
					}
				}

				else	//short format printout ordered
				{
					switch (attId)
					{
					case 5: attributeIdentifier = ACS_CS_Protocol::Attribute_IP_EthA; break;
					case 6: attributeIdentifier = ACS_CS_Protocol::Attribute_IP_EthB; break;
					}
				}

				// default result code is that the entry does not exist
				int attributeResult = ACS_CS_Protocol::Result_No_Value; //ACS_CS_Protocol::Result_No_Such_Entry;

				// find the requested attribute in the list of attributes we got
				for (size_t i = 0;i < totalNumOfAttributes; i++)
				{
					if (attributes[i]->getIdentifier() == attributeIdentifier)
					{
						// found the attribute! now check if the attribute has a value or if it's empty
						attributeResult = attributes[i]->getValueLength() <= 0 ?
								ACS_CS_Protocol::Result_No_Value : ACS_CS_Protocol::Result_Success;

						// save index to were it was found
						attributeIndex = i;
						break;
					}
				}

				switch (attributeResult)   //check result code for the getValue request
				{
				case ACS_CS_Protocol::Result_Success:		//request performed successfully
				{
					char* tempBuffer = new char[sizeof(long)];	//creates buffer for storing the response value

					if (tempBuffer)
					{
						// fetch the attribute
						attributes[attributeIndex]->getValue(tempBuffer, attributes[attributeIndex]->getValueLength());

						if (attributeIdentifier == ACS_CS_Protocol::Attribute_Magazine)
						{
							int MAGSIZE = attributes[attributeIndex]->getValueLength();
							char* magBuffer = new char[MAGSIZE+1];

							if(magBuffer)
							{
								attributes[attributeIndex]->getValue(magBuffer, attributes[attributeIndex]->getValueLength());

								magBuffer[MAGSIZE] = 0;

								in_addr address;
								inet_aton(magBuffer, &address);
								uint32_t tempValue = ntohl(address.s_addr);

								this->printStruct->UL_MAG = tempValue;
								this->printStruct->MAG = magBuffer;

								delete [] magBuffer;
							}
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_Slot)
						{
							stringstream ss;
							uint16_t tempValue = *(reinterpret_cast<uint16_t*>(tempBuffer));  //converts the char* to unsigned short*
							//and stores the value in a variable
							ss << tempValue;
							this->printStruct->US_SLOT = tempValue;
							this->printStruct->SLOT = ss.str();
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_SysType)
						{
							uint16_t sysType = *(reinterpret_cast<uint16_t*>(tempBuffer));
							if (sysType == ACS_CS_Protocol::SysType_BC)
								this->printStruct->SYSTYPE = "BC";
							else if (sysType == ACS_CS_Protocol::SysType_CP)
								this->printStruct->SYSTYPE = "CP";
							else if (sysType == ACS_CS_Protocol::SysType_AP)
								this->printStruct->SYSTYPE = "AP";
							else
								this->printStruct->SYSTYPE = "-";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_SysNo)
						{

							uint16_t tempValue = *(reinterpret_cast<uint16_t*>(tempBuffer));	//converts the char* to unsigned short*

							if(tempValue == 0){
								this->printStruct->SYSNUM = "-";
							}
							else{
								stringstream ss;
								ss << tempValue;
								this->printStruct->SYSNUM = ss.str();
							}
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_FBN)
						{
							uint16_t FBN = *(reinterpret_cast<uint16_t*>(tempBuffer));

							if (FBN == ACS_CS_Protocol::FBN_SCBRP)
								this->printStruct->FBN = "SCB-RP";
							else if (FBN == ACS_CS_Protocol::FBN_RPBIS)
								this->printStruct->FBN = "RPBI-S";
							else if (FBN == ACS_CS_Protocol::FBN_CPUB)
								this->printStruct->FBN = "CPUB";
							else if (FBN == ACS_CS_Protocol::FBN_MAUB)
								this->printStruct->FBN = "MAUB";
							else if (FBN == ACS_CS_Protocol::FBN_APUB)
								this->printStruct->FBN = "APUB";
							else if (FBN == ACS_CS_Protocol::FBN_Disk)
								this->printStruct->FBN = "Disk";
							else if (FBN == ACS_CS_Protocol::FBN_DVD)
								this->printStruct->FBN = "DVD";
							else if (FBN == ACS_CS_Protocol::FBN_GEA)
								this->printStruct->FBN = "GEA";
							else if (FBN == ACS_CS_Protocol::FBN_GESB) //CR GESB
								this->printStruct->FBN = "GESB";
							else if (FBN == ACS_CS_Protocol::FBN_SCXB)
								this->printStruct->FBN = "SCXB";
							else if (FBN == ACS_CS_Protocol::FBN_EPB1)
								this->printStruct->FBN = "EPB1";
							else if (FBN == ACS_CS_Protocol::FBN_EvoET)
								this->printStruct->FBN = "EvoET";
							else if (FBN == ACS_CS_Protocol::FBN_IPTB)
								this->printStruct->FBN = "IPTB";
							else if (FBN == ACS_CS_Protocol::FBN_CMXB)
								this->printStruct->FBN = "CMXB";
							else if (FBN == ACS_CS_Protocol::FBN_IPLB)
								this->printStruct->FBN = "IPLB";
							else if (FBN == ACS_CS_Protocol::FBN_SMXB)
								this->printStruct->FBN = "SMXB";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_Side)
						{
							uint16_t side = *(reinterpret_cast<uint16_t*>(tempBuffer));

							if (side == ACS_CS_Protocol::Side_A)
								this->printStruct->SIDE = "A";
							else if (side == ACS_CS_Protocol::Side_B)
								this->printStruct->SIDE = "B";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_SeqNo)
						{
							stringstream ss;
							uint16_t tempValue = *(reinterpret_cast<uint16_t*>(tempBuffer));  //converts the char* to unsigned short*
							//and stores the value in a variable
							ss << tempValue;
							this->printStruct->SEQNUM = ss.str();
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_IP_EthA)
						{
							this->printStruct->IPA = tempBuffer;
							if(this->printStruct->IPA.compare("0.0.0.0") == 0)
								this->printStruct->IPA = "-";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_IP_EthB)
						{
							this->printStruct->IPB = tempBuffer;
							if(this->printStruct->IPB.compare("0.0.0.0") == 0)
								this->printStruct->IPB = "-";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_Alias_EthA)
						{
							this->printStruct->ALIASA = tempBuffer;
							if(this->printStruct->ALIASA.compare("0.0.0.0") == 0)
								this->printStruct->ALIASA = "-";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_Alias_EthB)
						{
							this->printStruct->ALIASB = tempBuffer;
							if(this->printStruct->ALIASB.compare("0.0.0.0") == 0)
								this->printStruct->ALIASB = "-";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_Netmask_Alias_EthA)
						{
							this->printStruct->MASKA = tempBuffer;
							if(this->printStruct->MASKA.compare("0.0.0.0") == 0)
								this->printStruct->MASKA = "-";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_Netmask_Alias_EthB)
						{
							this->printStruct->MASKB = tempBuffer;
							if(this->printStruct->MASKB.compare("0.0.0.0") == 0)
								this->printStruct->MASKB = "-";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_DHCP)
						{
							uint16_t dhcp = *(reinterpret_cast<uint16_t*>(tempBuffer));

							if (dhcp == ACS_CS_Protocol::DHCP_None)
								this->printStruct->DHCP = "None";
							else if (dhcp == ACS_CS_Protocol::DHCP_Normal)
								this->printStruct->DHCP = "Normal";
							else if (dhcp == ACS_CS_Protocol::DHCP_Client)
								this->printStruct->DHCP = "Client";
						}

						else if (attributeIdentifier == ACS_CS_Protocol::Attribute_Blade_Product_Number)
						{
							this->printStruct->PRODID = tempBuffer;
						}

						boolTable = true;
						delete [] tempBuffer;
					}
					break;
				}

				case ACS_CS_Protocol::Result_No_Value:        //no value for sought attribute and board id
				{
					switch (attributeIdentifier)
					{
					case ACS_CS_Protocol::Attribute_Magazine:			this->printStruct->MAG = "-"; break;
					case ACS_CS_Protocol::Attribute_Slot:				this->printStruct->SLOT = "-"; break;
					case ACS_CS_Protocol::Attribute_SysType:			this->printStruct->SYSTYPE = "-"; break;
					case ACS_CS_Protocol::Attribute_SysNo:				this->printStruct->SYSNUM = "-"; break;
					case ACS_CS_Protocol::Attribute_FBN:				this->printStruct->FBN = "-"; break;
					case ACS_CS_Protocol::Attribute_Side:				this->printStruct->SIDE = "-"; break;
					case ACS_CS_Protocol::Attribute_SeqNo:				this->printStruct->SEQNUM = "-"; break;
					case ACS_CS_Protocol::Attribute_IP_EthA:			this->printStruct->IPA = "-"; break;
					case ACS_CS_Protocol::Attribute_IP_EthB:			this->printStruct->IPB = "-"; break;
					case ACS_CS_Protocol::Attribute_Alias_EthA:			this->printStruct->ALIASA = "-"; break;
					case ACS_CS_Protocol::Attribute_Netmask_Alias_EthA:	this->printStruct->MASKA = "-"; break;
					case ACS_CS_Protocol::Attribute_Alias_EthB:			this->printStruct->ALIASB = "-"; break;
					case ACS_CS_Protocol::Attribute_Netmask_Alias_EthB:	this->printStruct->MASKB = "-"; break;
					case ACS_CS_Protocol::Attribute_DHCP:				this->printStruct->DHCP = "-"; break;
					case ACS_CS_Protocol::Attribute_Blade_Product_Number:				this->printStruct->PRODID = "-"; break;
					}

					break;
				}

				case ACS_CS_Protocol::Result_No_Such_Entry:   //no entry in the HWC table for given board id
				{                                   //board has probably been removed after getBoardIds
					missingEntry = true;
					break;
				}

				default:	//all other faults
				{
					boolOtherFailure = true;
					break;
				}
				}

				if (missingEntry || boolOtherFailure) //break execution of attribute loop, continue with next board instead
					break;
			}

			if (!missingEntry) //unless the boardId was missing in the HWC table
				if (this->printStruct->US_SLOT < 29) //filter out CPUB and MAUB boards for APZ 212 3x
						this->printOutVector.push_back(this->printStruct); //add the current board's struct to the vector

			// free memory instances of ACS_CS_Attribute from the getAttributes() call
			for (size_t i = 0;i < totalNumOfAttributes; i++)
			{
				if (attributes[i])
					delete attributes[i];
				attributes[i] = 0;
			}

			if (attributes)
				delete[] attributes;

			if (boolOtherFailure) //break execution of board loop and clear boolTable
			{ //flag since nothing should be printed
				boolTable = false;
				break;
			}
		}

		delete blade;

	} // end of for(...)

	if (boolTable) {
		this->commStruct->exitCode = NSCommand::Exit_Success;
		return true; //hwcTable fetched and stored in vector
	}

	if (numOfBoards == 0) //if getBoardIds couldn't find any boards,
	{ //change exit code to success
		this->commStruct->exitCode = NSCommand::Exit_Success;
		return true; //hwcTable empty, no entries matched the search criteria, empty table will be printed
	}

	return false;
}

//function verifying that the string (arg) only contains figures (1-9)
bool ACS_CS_MODDCommands::checkFigures(istringstream &istr)
{
   istr.clear();     // return to the beginning
   (void)istr.seekg(0);    //      of the stream
   char c = istr.get(); //get first char in the istringstream

   while (c != EOF)
   {
      if ((c < '0') || (c > '9'))
         return false;

      c = istr.get();
   } 

   istr.clear();     // return to the beginning
   (void)istr.seekg(0);    //      of the stream
   return true;
}

void ACS_CS_MODDCommands::reverseDottedDecStr(istringstream &istr)
{
   istr.clear();     // return to the beginning
   (void)istr.seekg(0);    //      of the stream
   istringstream tempIstr;
   string tempString = "";
   char c;

   do
   {
      c = istr.get();      //get char from istringstream

      if (c == '.' || c == EOF)
      {
         (void)tempIstr.seekg(0);
         
         if (c == '.')
                tempIstr.str(c + tempString + tempIstr.str());
         else
            tempIstr.str(tempString + tempIstr.str());   

         tempString = "";
      }
      else
         tempString = tempString + c;
   } while (c != EOF);

   istr.str(tempIstr.str());

   istr.clear();     // return to the beginning
   (void)istr.seekg(0);    //      of the stream
}

bool ACS_CS_MODDCommands::printTable()
{
   stringstream hwcTable;
   int printOutVectorSize = (int)this->printOutVector.size();

   if (this->commStruct->boolOutputFormat && this->commStruct->outputFormat == NSCommand::Output_Format_LONG)
   {
      hwcTable<<endl<<"HARDWARE CONFIGURATION TABLE"<<endl;

      for (int i = 0; i < printOutVectorSize; i++)
      {
         this->printStruct = this->printOutVector.at(i);

         hwcTable<<endl<<setiosflags(ios::left)<<setw(14)<<"MAG"<<setw(5)<<"SLOT"<<setw(8)<<"SYSTYPE"<<setw(8)<<
                "SYSNUM"<<setw(7)<<"FBN"<<setw(9)<<"SIDE"<<setw(6)<<"SEQNUM"<<resetiosflags(ios::left)<<endl;

         hwcTable<<setiosflags(ios::left)<<setw(14)<<this->printStruct->MAG<<resetiosflags(ios::left);
         hwcTable<<setw(4)<<this->printStruct->SLOT<<setw(1)<<"";
         hwcTable<<setiosflags(ios::left)<<setw(8)<<this->printStruct->SYSTYPE;
         hwcTable<<setw(8)<<this->printStruct->SYSNUM;
         hwcTable<<setw(7)<<this->printStruct->FBN;
         hwcTable<<setw(9)<<this->printStruct->SIDE;
         hwcTable<<setw(6)<<this->printStruct->SEQNUM<<resetiosflags(ios::left)<<endl;

         hwcTable<<endl<<setiosflags(ios::left)<<setw(19)<<"IPA"<<setw(16)<<"IPB"<<setw(16)<<"ALIASA"<<
            setw(16)<<"MASKA"<<resetiosflags(ios::left)<<endl;

         hwcTable<<setiosflags(ios::left)<<setw(19)<<this->printStruct->IPA;
         hwcTable<<setw(16)<<this->printStruct->IPB;
         hwcTable<<setw(16)<<this->printStruct->ALIASA;
         hwcTable<<setw(16)<<this->printStruct->MASKA<<resetiosflags(ios::left)<<endl;

         hwcTable<<endl<<setiosflags(ios::left)<<setw(19)<<"ALIASB"<<setw(16)<<"MASKB"<<setw(7)<<"DHCP"<<setw(16)<<"PRODID"<<
            resetiosflags(ios::left)<<endl;

         hwcTable<<setiosflags(ios::left)<<setw(19)<<this->printStruct->ALIASB;
         hwcTable<<setw(16)<<this->printStruct->MASKB;
         hwcTable<<setw(7)<<this->printStruct->DHCP;
         hwcTable<<setw(16)<<this->printStruct->PRODID<<resetiosflags(ios::left);

         if ((i + 1) != printOutVectorSize)
            hwcTable<<endl<<endl;
      }
   }

   else if (this->commStruct->boolOutputFormat && this->commStruct->outputFormat == NSCommand::Output_Format_SHORT)
   {
      hwcTable<<endl<<"HARDWARE CONFIGURATION TABLE"<<endl;
      
      if (printOutVectorSize > 0)
         hwcTable<<endl<<setiosflags(ios::left)<<setw(14)<<"MAG"<<setw(5)<<"SLOT"<<setw(8)<<"SYSTYPE"<<setw(7)<<
            "SYSNUM"<<setw(7)<<"FBN"<<setw(16)<<"IPA"<<setw(16)<<"IPB"<<resetiosflags(ios::left);

      for (int i = 0; i < printOutVectorSize; i++)
      {
         this->printStruct = this->printOutVector.at(i);

         hwcTable<<endl;
         hwcTable<<setiosflags(ios::left)<<setw(14)<<this->printStruct->MAG<<resetiosflags(ios::left);
         hwcTable<<setw(4)<<this->printStruct->SLOT<<setw(1)<<"";
         hwcTable<<setiosflags(ios::left)<<setw(8)<<this->printStruct->SYSTYPE;
         hwcTable<<setw(7)<<this->printStruct->SYSNUM;
         hwcTable<<setw(7)<<this->printStruct->FBN;
         hwcTable<<setw(16)<<this->printStruct->IPA;
         hwcTable<<setw(16)<<this->printStruct->IPB<<resetiosflags(ios::left);
      }
   }

   else if (this->commStruct->boolOutputFormat && this->commStruct->outputFormat == NSCommand::Output_Format_XML)
   {
      hwcTable<<endl<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
      hwcTable<<"<!DOCTYPE HardwareConfigurationTable ["<<endl;
      hwcTable<<"<!ELEMENT HardwareConfigurationTable (board*)>"<<endl;
      hwcTable<<"<!ATTLIST HardwareConfigurationTable ver CDATA #FIXED \"1.0\">"<<endl;
      hwcTable<<"<!ELEMENT board (sysid?,ipaddress?)>"<<endl;
      hwcTable<<"<!ATTLIST board mag CDATA #REQUIRED>"<<endl;
      hwcTable<<"<!ATTLIST board slot (0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|28) #REQUIRED>"<<endl;
      hwcTable<<"<!ATTLIST board fbn (APUB|CMXB|CPUB|Disk|DVD|EPB1|EvoET|GEA|GESB|IPLB|IPTB|MAUB|RPBI-S|SCB-RP|SCXB|SMXB) #REQUIRED>"<<endl;
      hwcTable<<"<!ATTLIST board side (A|B) #IMPLIED>"<<endl;
      hwcTable<<"<!ATTLIST board seqnum CDATA #IMPLIED>"<<endl;
      hwcTable<<"<!ATTLIST board dhcp (None|Normal|Client) #IMPLIED>"<<endl;
      hwcTable<<"<!ELEMENT sysid EMPTY>"<<endl;
      hwcTable<<"<!ATTLIST sysid systype (AP|BC|CP) #REQUIRED>"<<endl;
      hwcTable<<"<!ATTLIST sysid sysnum CDATA #REQUIRED>"<<endl;
      hwcTable<<"<!ELEMENT product EMPTY>"<<endl;
      hwcTable<<"<!ATTLIST product prodid CDATA #REQUIRED>"<<endl;
      hwcTable<<"<!ELEMENT ipaddress (ip,alias?)>"<<endl;
      hwcTable<<"<!ELEMENT ip EMPTY>"<<endl;
      hwcTable<<"<!ATTLIST ip ipa CDATA #REQUIRED>"<<endl;
      hwcTable<<"<!ATTLIST ip ipb CDATA #REQUIRED>"<<endl;
      hwcTable<<"<!ELEMENT alias EMPTY>"<<endl;
      hwcTable<<"<!ATTLIST alias aliasa CDATA #REQUIRED>"<<endl;
      hwcTable<<"<!ATTLIST alias maska CDATA #REQUIRED>"<<endl;
      hwcTable<<"<!ATTLIST alias aliasb CDATA #REQUIRED>"<<endl;
      hwcTable<<"<!ATTLIST alias maskb CDATA #REQUIRED>"<<endl;
      hwcTable<<"]>"<<endl;
      
      hwcTable<<"<HardwareConfigurationTable ver=\"1.0\">"<<endl;

      for (int i = 0; i < printOutVectorSize; i++)
      {
         this->printStruct = this->printOutVector.at(i);

         hwcTable<<setw(2)<<""<<setiosflags(ios::left)<<setw(14)<<"<board mag"<<resetiosflags(ios::left)<<setw(2)<<"=\""<<this->printStruct->MAG<<"\""<<endl;
         hwcTable<<setiosflags(ios::left)<<setw(9)<<""<<setw(7)<<"slot"<<resetiosflags(ios::left)       <<setw(2)<<"=\""<<this->printStruct->SLOT<<"\""<<endl;
         hwcTable<<setiosflags(ios::left)<<setw(9)<<""<<setw(7)<<"fbn"<<resetiosflags(ios::left)        <<setw(2)<<"=\""<<this->printStruct->FBN<<"\"";
         
         if (this->printStruct->SIDE != "-")
            hwcTable<<endl<<setiosflags(ios::left)<<setw(9)<<""<<setw(7)<<"side"<<resetiosflags(ios::left)       <<setw(2)<<"=\""<<this->printStruct->SIDE<<"\"";
         
         if (this->printStruct->SEQNUM != "-")
            hwcTable<<endl<<setiosflags(ios::left)<<setw(9)<<""<<setw(7)<<"seqnum"<<resetiosflags(ios::left)     <<setw(2)<<"=\""<<this->printStruct->SEQNUM<<"\"";
         
         if (this->printStruct->DHCP != "-")
            hwcTable<<endl<<setiosflags(ios::left)<<setw(9)<<""<<setw(7)<<"dhcp"<<resetiosflags(ios::left)       <<setw(2)<<"=\""<<this->printStruct->DHCP<<"\"";


         hwcTable<<">"<<endl;
         
         if (this->printStruct->SYSTYPE != "-" && this->printStruct->SYSNUM != "-")
         {
            hwcTable<<setiosflags(ios::left)<<setw(4)<<""<<"<sysid systype=\""<<this->printStruct->SYSTYPE<<"\""<<resetiosflags(ios::left)<<endl;
            hwcTable<<setiosflags(ios::left)<<setw(11)<<""<<setw(7)<<"sysnum"<<"=\""<<this->printStruct->SYSNUM<<"\"></sysid>"<<resetiosflags(ios::left)<<endl;
         }
         
         //////
         if (this->printStruct->PRODID != "-")
         {
        	 hwcTable<<setiosflags(ios::left)<<setw(4)<<""<<"<product prodid=\""<<this->printStruct->PRODID<<"\"></product>"<<resetiosflags(ios::left)<<endl;
         }

         if (this->printStruct->IPA != "-" && this->printStruct->IPB != "-")
         {
            hwcTable<<setw(4)<<""<<"<ipaddress>"<<endl;
            hwcTable<<setiosflags(ios::left)<<setw(6)<<""<<"<ip ipa=\""<<this->printStruct->IPA<<"\""<<resetiosflags(ios::left)<<endl;
            hwcTable<<setiosflags(ios::left)<<setw(10)<<""<<"ipb=\""<<this->printStruct->IPB<<"\"></ip>"<<resetiosflags(ios::left)<<endl;

            if (this->printStruct->ALIASA != "-" && this->printStruct->MASKA != "-" &&
               this->printStruct->ALIASB != "-" && this->printStruct->MASKB != "-")
            {
               hwcTable<<setiosflags(ios::left)<<setw(6)<<""<<"<alias aliasa=\""<<this->printStruct->ALIASA<<"\""<<resetiosflags(ios::left)<<endl;
               hwcTable<<setiosflags(ios::left)<<setw(13)<<""<<"maska =\""<<this->printStruct->MASKA<<"\""<<resetiosflags(ios::left)<<endl;
               hwcTable<<setiosflags(ios::left)<<setw(13)<<""<<"aliasb=\""<<this->printStruct->ALIASB<<"\""<<resetiosflags(ios::left)<<endl;
               hwcTable<<setiosflags(ios::left)<<setw(13)<<""<<"maskb =\""<<this->printStruct->MASKB<<"\"></alias>"<<resetiosflags(ios::left)<<endl;
            }
            
            hwcTable<<setw(4)<<""<<"</ipaddress>"<<endl;
         }
         
         hwcTable<<setw(2)<<""<<"</board>"<<endl;
      }

      hwcTable<<"</HardwareConfigurationTable>";
   }

   else
   {
      this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
      return false;  
   }

   cout<<hwcTable.str()<<endl;

   return true;
}

void ACS_CS_MODDCommands::sortTable()
{
   vector<printOut*> tempPrintOutVector;     //temp vector holding the printout structs
   vector<printOut*>::iterator tempIterator;

   int leastVal = 0;
   int loop = (int)this->printOutVector.size() - 1;

   if (loop <= 0)    //vector only contains 1 board, i.e. list sorted
      return;

   for (int i = 0; i < loop; i++)
   {
      for (int j = 0; j < (int)this->printOutVector.size() - 1; j++)
      {
    	 if (this->printOutVector.at(leastVal)->UL_MAG > this->printOutVector.at(j+1)->UL_MAG)
            leastVal = j + 1;
         
         else if (this->printOutVector.at(leastVal)->UL_MAG == this->printOutVector.at(j+1)->UL_MAG)
         {
            if (this->printOutVector.at(leastVal)->US_SLOT > this->printOutVector.at(j+1)->US_SLOT)
               leastVal = j + 1;
         }
      }

      this->printStruct = this->printOutVector.at(leastVal);   //gets the board with the least mag and slot addr from the vector
      tempPrintOutVector.push_back(this->printStruct);      //add the board with the least mag and slot addr last in the tempvector
      tempIterator = this->printOutVector.begin();
      tempIterator = tempIterator + leastVal;
      (void)this->printOutVector.erase(tempIterator);       //erases the board with the least mag and slot addr from the vector
      leastVal = 0;  //clear the leastVal in order to start with first vector element in next loop

      if (this->printOutVector.size() == 1)        //if there's only one board left in the vector
      {
         this->printStruct = this->printOutVector.at(0); //gets the board from the vector
         tempPrintOutVector.push_back(this->printStruct);         //add the board in the tempvector
         tempIterator = this->printOutVector.begin();
         (void)this->printOutVector.erase(tempIterator);          //erases the board from the vector  
      }
   }

   this->printOutVector.assign(tempPrintOutVector.begin(), tempPrintOutVector.end());  //assigns the teintmpVector to the original
}

bool ACS_CS_MODDCommands::getAttributes(ACS_CS_Attribute ***array, const ACS_CS_ImBlade * blade, size_t &size, std::map<std::string, std::string> parentMap)
{
	ACS_CS_Attribute * a = 0;
	uint16_t UNDEFINED = 65535;
	std::vector<ACS_CS_Attribute *> attributeVector;

	std::string parent = getParentFromRdn(blade->rdn);
	map<std::string, std::string>::iterator it;
	it = parentMap.find(parent);

	if(it != parentMap.end()){
		a = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Magazine);

		string address = it->second;

		a->setValue(address.c_str(), address.length());
		attributeVector.push_back(a);
	}

	if(blade->functionalBoardName != UNDEF_FUNCTIONALBOARDNAME){
		a = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_FBN);
		a->setValue(reinterpret_cast<const char*>(&blade->functionalBoardName), sizeof(blade->functionalBoardName));
		attributeVector.push_back(a);
	}

	if(blade->slotNumber != UNDEFINED){
		a = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Slot);
		a->setValue(reinterpret_cast<const char*>(&blade->slotNumber), sizeof(blade->slotNumber));
		attributeVector.push_back(a);
	}

	if(blade->systemType != UNDEF_SYSTEMTYPE){
		a = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_SysType);
		a->setValue(reinterpret_cast<const char*>(&blade->systemType), sizeof(blade->systemType));
		attributeVector.push_back(a);
	}

	if(blade->systemNumber != UNDEFINED){
		a = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_SysNo);
		a->setValue(reinterpret_cast< const char*>(&blade->systemNumber), sizeof(blade->systemNumber));
		attributeVector.push_back(a);
	}

	if(blade->side != UNDEF_SIDE){
		a = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Side);
		a->setValue(reinterpret_cast<const char*>(&blade->side), sizeof(blade->side));
		attributeVector.push_back(a);
	}

	if(blade->sequenceNumber != -1){
		a = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_SeqNo);
		a->setValue(reinterpret_cast<const char*>(&blade->sequenceNumber), sizeof(blade->sequenceNumber));
		attributeVector.push_back(a);
	}

	if(blade->dhcpOption != UNDEF_DHCP){
		a = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_DHCP);
		a->setValue(reinterpret_cast<const char*>(&blade->dhcpOption), sizeof(blade->dhcpOption));
		attributeVector.push_back(a);
	}

	if (blade->type == OTHERBLADE_T) {
		const ACS_CS_ImOtherBlade * otherBlade = dynamic_cast<const ACS_CS_ImOtherBlade *>(blade);

		if (otherBlade && otherBlade->bladeProductNumber.size() > 0) {
			char *buffer = new char[30];
			int size = sprintf(buffer, "%s", otherBlade->bladeProductNumber.c_str());

			ACS_CS_Attribute * attr = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Blade_Product_Number);
			attr->setValue(buffer, size+1);
			attributeVector.push_back(attr);

			delete []buffer;
		}
	}

	if(ACS_CS_Common_Util::isIpAddress(blade->ipAddressEthA)){
		char *buffer = new char[16];
		int size = sprintf(buffer, "%s", blade->ipAddressEthA.c_str());
		//cout << "A Slot " << blade->slotNumber << " IP: " << blade->ipAddressEthA << endl;

		ACS_CS_Attribute * attr = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_IP_EthA);
		attr->setValue(buffer, size+1);
		attributeVector.push_back(attr);

		delete []buffer;
	}

	if(ACS_CS_Common_Util::isIpAddress(blade->ipAddressEthB)){
		char *buffer = new char[16];
		int size = sprintf(buffer, "%s", blade->ipAddressEthB.c_str());
		//cout << "B Slot " << blade->slotNumber << " IP: " << blade->ipAddressEthB << endl;
		ACS_CS_Attribute * attr = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_IP_EthB);
		attr->setValue(buffer, size+1);
		attributeVector.push_back(attr);
		delete buffer;
	}

	if(ACS_CS_Common_Util::isIpAddress(blade->ipAliasEthA)){
		char *buffer = new char[16];
		int size = sprintf(buffer, "%s", blade->ipAliasEthA.c_str());
		ACS_CS_Attribute * attr = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Alias_EthA);
		attr->setValue(buffer, size+1);
		attributeVector.push_back(attr);
		delete []buffer;
	}

	if(ACS_CS_Common_Util::isIpAddress(blade->ipAliasEthB)){
		char *buffer = new char[16];
		int size = sprintf(buffer, "%s", blade->ipAliasEthB.c_str());
		ACS_CS_Attribute * attr = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Alias_EthB);
		attr->setValue(buffer, size+1);
		attributeVector.push_back(attr);
		delete []buffer;
	}

	if(ACS_CS_Common_Util::isIpAddress(blade->aliasNetmaskEthA)){
		char *buffer = new char[16];
		int size = sprintf(buffer, "%s", blade->aliasNetmaskEthA.c_str());
		ACS_CS_Attribute * attr = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Netmask_Alias_EthA);
		attr->setValue(buffer, size+1);
		attributeVector.push_back(attr);
		delete []buffer;
	}

	if(ACS_CS_Common_Util::isIpAddress(blade->aliasNetmaskEthB)){
		char *buffer = new char[16];
		int size = sprintf(buffer, "%s", blade->aliasNetmaskEthB.c_str());
		ACS_CS_Attribute * attr = new ACS_CS_Attribute(ACS_CS_Protocol::Attribute_Netmask_Alias_EthB);
		attr->setValue(buffer, size+1);
		attributeVector.push_back(attr);
		delete []buffer;
	}

	*array = new ACS_CS_Attribute*[attributeVector.size()];
	size = attributeVector.size();

	for (uint16_t i = 0; i < attributeVector.size(); i++) {
		(*array)[i] = attributeVector[i];
	}

	return true;
}

//*********************************************************************************
// Description:
//    This function checks the opensession command's format
//
// Parameters:
//    Input    argc     number of arguments
//             argv     argument strings
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::checkOpenSessionFormat(int argc, char* argv[])
{  
   TRACE("checkOpenSessionFormat()");
   
   if( (this->commStruct->openSessionFormat1 == true) ||
       (this->commStruct->openSessionFormat2 == true) ||
       (this->commStruct->openSessionFormat3 == true))
   {
      //analyzes the options given in the command
      if (! this->checkOptions(argc, argv))  
         return false;

      //analyzes the option-arguments given in the command
      if (! this->checkOpenSessionOptionArguments(argc, argv))
         return false;

      if(this->commStruct->openSessionFormat2==true)
      {   
         if(!this->checkOpenSessionBoardSearch())
            return false;
      }
   
      return true;
   }
   else
   {
      if (this->commStruct->isAPZTypeAvail == false || this->commStruct->isAPGCountAvail == false)
         this->commStruct->exitCode = NSCommand::Exit_No_Config_Info;
      else
         this->commStruct->exitCode = NSCommand::Exit_Incorrect_Usage;
         
      return false;
   }
}

//*********************************************************************************
// Description:
//    This function checks the opensession command's options
//
// Parameters:
//    Input    optionString      Option String extracted from input command
//
//    Output   exitCode          Command exit code
//             illegalOption     Illegal Option only applicable for 
//                               Exit_Illegal_Option exit code
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::checkOpenSessionOptions(string optionString, int argc, NSCommand::Exit_Code &exitCode, string &illegalOption)
{
   TRACE("checkOpenSessionOptions()");
   
   bool option_m = false, option_s = false, option_f = false, option_a = false, option_t = false;
   int option_count = 0;
   exitCode = NSCommand::Exit_Incorrect_Usage;
   illegalOption = "";
   string tempIllegalOption = "";
   
   //check that only valid options have been used 
   for (unsigned short i = 0; i < optionString.length(); i++)
   {
      option_count++;
      
      string currOption = optionString.substr(i,1);
      if(currOption == "m")
    	  option_m = true;
      else if(currOption == "s")
    	  option_s = true;
      else if(currOption == "f")
    	  option_f = true;
      else if(currOption == "a")
    	  option_a = true;
      else if(currOption == "t")
    	  option_t = true;
      else
      {
         tempIllegalOption = "-" + currOption;
         break;
      }
   }
   
   // Check if the options are valid with the formats.
   switch (option_count)
   {
   case 2:     
      // Incorrect format - With 2 options, there must be 5 arguments i.e opensession -m magazine -s slot
      if (argc != 5)
      {
          exitCode = NSCommand::Exit_Incorrect_Usage_Full;
          return false;
      }
   
      if (this->commStruct->openSessionFormat2 == true)
      {
         if (option_s == true && option_m == true)
         {
            this->commStruct->openSessionFormat1 = false;
            this->commStruct->openSessionFormat3 = false;
            return true;
         }
         else if (tempIllegalOption != "")
         {
            illegalOption = tempIllegalOption;
            exitCode = NSCommand::Exit_Illegal_Option;
            return false;
         }
      }
      
      if (this->commStruct->isAPZTypeAvail == false)
         exitCode = NSCommand::Exit_No_Config_Info;

      return false;
      
   case 1:
      // In correct format - With 1 option, there must be 2 or 3 arguments i.e opensession -f/-a [username]
      if ( (argc != 2) && (argc != 3)) 
      {
          exitCode = NSCommand::Exit_Incorrect_Usage_Full;
          return false;
      }
      
      if (this->commStruct->openSessionFormat3 == true)
      {
         this->commStruct->openSessionFormat1 = false;
         this->commStruct->openSessionFormat2 = false;
         
         if (option_f == true)
         {
            this->commStruct->openSessionType = NSCommand::SFTP;
            return true;
         }
         else if (option_a == true)
         {
            this->commStruct->openSessionType = NSCommand::SSH;
            return true;
         }
         else if (option_t == true)
         {
        	 pid_t parentPid = getppid();
        	 bool validPid = (parentPid > 0)? true: false;
        	 for (int i=0; i < 2 && validPid; i++)
        	 {
        		 validPid = getParentProcessPid(parentPid,parentPid);
        	 }

        	 if (parentPid == PARENT_COM_CLI_PID)
        	 {
        		 illegalOption = "-t in this session type";
        		 exitCode = NSCommand::Exit_Illegal_Option;
        		 return false;
        	 }
        	 else
        	 {
        		 this->commStruct->openSessionType = NSCommand::SSH_4422;
        		 return true;
        	 }
         }
         else if (tempIllegalOption != "")
         {
            illegalOption = tempIllegalOption;
            exitCode = NSCommand::Exit_Illegal_Option;
            return false;
         }
      }

      if (this->commStruct->isAPGCountAvail == false)
         exitCode = NSCommand::Exit_No_Config_Info;
         
      return false;

   case 0:
       // Incorrect format - with no option, there must be 2 arguments i.e opensession fbn
      if (argc != 2)
      {
          exitCode = NSCommand::Exit_Incorrect_Usage_Full;
          return false;
      }
      
      if (this->commStruct->openSessionFormat1 == true)
      {
         this->commStruct->openSessionFormat2 = false;
         this->commStruct->openSessionFormat3 = false;
         return true;
      }

      if (this->commStruct->isAPZTypeAvail == false)
         exitCode = NSCommand::Exit_No_Config_Info;
         
      return false;

   default:
      exitCode = NSCommand::Exit_Incorrect_Usage_Full;
      return false;
   }
}

//*********************************************************************************
// Description:
//    This function checks the opensession command's arguments
//
// Parameters:
//    Input    argc     number of arguments
//             argv     argument strings
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::checkOpenSessionOptionArguments(int argc, char* argv[])
{
   TRACE("checkOpenSessionOptionArguments()");
   
   string arg = "";
   string commandString = this->commStruct->currCommand;

   ACS_DSD_Client dsdClient;
   ACS_DSD_Node   ownNode;
   if (dsdClient.get_local_node(ownNode))
   {
      TRACE("get_local_node() failed");
      return faultyOption(NSCommand::Exit_Error_Executing);
   }

   switch (argc)
   {        
      case 2:
         // format: opensession fbn
         if (this->commStruct->openSessionFormat1 == true)
         {
            arg = argv[1];
            if (arg.substr(0,1) == "-")
                   return faultyOption(NSCommand::Exit_Incorrect_Usage);

            if(arg.compare("GESB-I-A") == 0 )
            {
               this->commStruct->strIpEthA = "192.168.169.132";
               this->commStruct->strIpEthB = "192.168.170.132";
               this->commStruct->fbn = ACS_CS_Protocol::FBN_GESB;
               return true;   
            }
            else if (arg.compare("GESB-E-A") == 0 )
            {
               this->commStruct->strIpEthA = "192.168.169.134";
               this->commStruct->strIpEthB = "192.168.170.134";
               this->commStruct->fbn = ACS_CS_Protocol::FBN_GESB;
               return true;   
            }
            else if (arg.compare("GESB-I-B") == 0)
            {
               this->commStruct->strIpEthA = "192.168.169.133";
               this->commStruct->strIpEthB = "192.168.170.133";
               this->commStruct->fbn = ACS_CS_Protocol::FBN_GESB;
               return true;   
            }
            else if (arg.compare("GESB-E-B") == 0)
            {
               this->commStruct->strIpEthA = "192.168.169.135";
               this->commStruct->strIpEthB = "192.168.170.135";
               this->commStruct->fbn = ACS_CS_Protocol::FBN_GESB;
               return true;   
            }
            else
            {
               if (checkOpenSessionArgFBN(argv[1]))
                  return faultyOption(NSCommand::Exit_FuncBoard_NotAllowed); 
               else
                  return faultyOption(NSCommand::Exit_Incorrect_FBN); 
               
            }
         }
         
         // format: opensession -a|-f
         if (this->commStruct->openSessionFormat3 == true)
         {
            if (ownNode.system_id != 2001)      // Check if the current node is AP1
               return faultyOption(NSCommand::Exit_OnlyAllowed_From_AP1); 
               
            this->commStruct->strIpEthA = AP2_CLUSTER_IP_ETHA_ADDRESS;
            this->commStruct->strIpEthB = AP2_CLUSTER_IP_ETHB_ADDRESS;
            return true;
         }
         
         return faultyOption(NSCommand::Exit_Incorrect_Usage);

      case 5:
         // format: opensession -m magazine -s slot
         if (this->commStruct->openSessionFormat2 == true)
         {
            //check that all options have option arguments 
            for (int j = 1; j < argc; j++)   // Loop through option list
            {
               arg = argv[j];          // Get current option

               if (arg.substr(0,1) == "-" && arg.size() == 2)  //look for options (size 2 and starts with "-", e.g. "-m")
               {
                  if (j < (argc - 1))
                  {
                     j++;
                     arg = argv[j]; //get the option arg
                     if (arg.substr(0,1) == "-" && arg.size() == 2)  //check if option is followed by option
                        return faultyOption(NSCommand::Exit_Incorrect_Usage);
                  }
               }
            }

            char * mag_arg = 0;

            for (int i = 1; i < argc; i++)      //loop through option list
            {
               arg = argv[i];       //get current option

               if (arg == "-m")           // Check option -m, magazine address
               {
                  if ((i + 1) >= argc)    //check if command ends with option instead of option argument
                  {
                      return faultyOption(NSCommand::Exit_Incorrect_Usage);               
                  }

                  if (! checkArgMag(mag_arg = argv[i+1])) //CR GESB
                     return faultyOption(NSCommand::Exit_Incorrect_Mag, argv[i+1]);
               }
               else if (arg == "-s")         // Check option -s, slot position
               {
                     
                  if ((i + 1) >= argc)    //check if command ends with option instead of option argument
                  {
                     return faultyOption(NSCommand::Exit_Incorrect_Usage);                         
                  }

                  if (! checkArgSlot(argv[i+1]))
                     return faultyOption(NSCommand::Exit_Incorrect_Slot, argv[i+1]);
               }
               else                    // Illegal option
                  return faultyOption(NSCommand::Exit_Incorrect_Usage);

               
               i++;  //jump to next option (skip the option argument)
            }
            return true;
         }

         return faultyOption(NSCommand::Exit_Incorrect_Usage);       
         
      case 3:
         // format: opensession -a|-f [username]
         if (this->commStruct->openSessionFormat3 == true)
         {
            if (ownNode.system_id != 2001)      // Check if the current node is AP1
               return faultyOption(NSCommand::Exit_OnlyAllowed_From_AP1); 
            
            for (int i = 1; i < argc; i++)
            {
               if (argv[i][0] != '-')
                  this->commStruct->username = argv[i];
            }
            
            if (this->commStruct->username == "")
               return faultyOption(NSCommand::Exit_Incorrect_Usage);

            this->commStruct->strIpEthA = AP2_CLUSTER_IP_ETHA_ADDRESS;
            this->commStruct->strIpEthB = AP2_CLUSTER_IP_ETHB_ADDRESS;
            return true;
         }
         
         return faultyOption(NSCommand::Exit_Incorrect_Usage);
         
      default:
         return faultyOption(NSCommand::Exit_Incorrect_Usage);
   }
}


bool ACS_CS_MODDCommands::checkOpenSessionArgFBN(char* arg)
{
   TRACE("checkOpenSessionArgFBN()");
   
   istringstream istr(arg);
   string fbnString = "";

   if (! (istr >> fbnString)) // Try to convert operand to functional board name
      return false;

   fbnString = stringToUpper(fbnString);
   if (! (fbnString == "APUB" || fbnString == "CPUB" || fbnString == "DISK" || 
         fbnString == "DVD" || fbnString == "GEA" || fbnString == "MAUB" || 
         fbnString == "RPBI-S" || fbnString == "SCB-RP" || //CR GESB
         fbnString == "SCXB" || fbnString == "EPB1" || fbnString == "IPTB" ||
         fbnString == "CMXB" || fbnString == "EVOET" || fbnString == "SMXB"))
      return false;
   return true;
}
bool ACS_CS_MODDCommands::checkOpenSessionBoardSearch()
{
   TRACE("checkOpenSessionBoardSearch()");
   
   ACS_CS_API_BoardSearch *boardSearch = 0;;
   ACS_CS_API_HWC * hwc = 0;;	
   hwc = ACS_CS_API::createHWCInstance();
   boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();
   boardSearch->setMagazine(this->commStruct->magazine);
   boardSearch->setSlot(this->commStruct->slot);
   ACS_CS_API_IdList boardList;
   ACS_CS_API_NS::CS_API_Result returnValue = hwc->getBoardIds(boardList, boardSearch);
   if (returnValue == ACS_CS_API_NS::Result_Success)
   {
      if (boardList.size() == 1)
      {
      	BoardID boardId = boardList[0];
        unsigned short fbn = 0;
	returnValue = hwc->getFBN(fbn, boardId);
	if (returnValue == ACS_CS_API_NS::Result_Success)
	{
		switch (fbn)
		{
		case ACS_CS_Protocol::FBN_SMXB:
		{
			this->commStruct->openSessionType = NSCommand::SSH;
			setUserName(commStruct->username);
		}

		case ACS_CS_Protocol::FBN_GESB:
		{
			this->commStruct->fbn = fbn;
			uint32_t ipEthA;
			uint32_t ipEthB;
			returnValue = hwc->getIPEthA(ipEthA, boardId);
			if (returnValue == ACS_CS_API_NS::Result_Success)
			{
				convComstrToIp(ipEthA,this->commStruct->strIpEthA);
			}
			else
			{
				if (boardSearch) ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
				if (hwc) ACS_CS_API::deleteHWCInstance(hwc);
				return faultyOption(NSCommand::Exit_No_Config_Info);
			}
			returnValue = hwc->getIPEthB(ipEthB, boardId);
			if (returnValue == ACS_CS_API_NS::Result_Success)
			{
				convComstrToIp(ipEthB,this->commStruct->strIpEthB);
			}
			else
			{
				if (boardSearch) ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
				if (hwc) ACS_CS_API::deleteHWCInstance(hwc);
				return faultyOption(NSCommand::Exit_No_Config_Info);
			}
		}
		if (boardSearch) ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
		if (hwc) ACS_CS_API::deleteHWCInstance(hwc);
		break;
		default:
		{
			if (boardSearch) ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
			if (hwc) ACS_CS_API::deleteHWCInstance(hwc);
			return faultyOption(NSCommand::Exit_FuncBoard_NotAllowed);
		}
		}
	}
       else
       {
	//Cannot retrieve FBN from CS		
	  if (boardSearch) ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
   	  if (hwc) ACS_CS_API::deleteHWCInstance(hwc);		
	  return faultyOption(NSCommand::Exit_No_Config_Info);
       }
    }
    else
    {
    	//Board List is empty
	if (boardSearch) ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
	if (hwc) ACS_CS_API::deleteHWCInstance(hwc);
	return faultyOption(NSCommand::Exit_Hardware_NotPresent);
    }
   }
   else if ((returnValue == ACS_CS_API_NS::Result_NoValue) || (returnValue == 	ACS_CS_API_NS::Result_NoEntry))
   {
	if (boardSearch) ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
	if (hwc) ACS_CS_API::deleteHWCInstance(hwc);
	return faultyOption(NSCommand::Exit_Hardware_NotPresent);
   }
   else
   {
       //GetBoardIds failed, error from CS
       if (boardSearch) ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
       if (hwc) ACS_CS_API::deleteHWCInstance(hwc);	
       return faultyOption(NSCommand::Exit_No_Config_Info);
   }
   return true;
}
void ACS_CS_MODDCommands::convComstrToIp(uint32_t ipEth,string &strIpEth)
{
   struct in_addr addr;
   addr.s_addr = ipEth;
   string dot_ip = inet_ntoa(addr);
   istringstream istr(dot_ip.c_str());
   reverseDottedDecStr(istr);
   istr>>strIpEth;
   //cout <<"string format"<<strIpEth<<endl;
}

//*********************************************************************************
// Description:
//    This function identify applicable format for opensession command:
//       - Format1   fbn      APZ 212 50
//       - Format2   -m - s   APZ 212 55 and APZ 212 60
//       - Format3   -a|-f    System with more than 2 APs
//
// Parameters:
//    Output:  format1  true(applicable) false(not applicable)
//             format2  true(applicable) false(not applicable)
//             format3  true(applicable) false(not applicable)
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::identifyApplicableFormat(bool &format1, bool &format2, bool &format3)
{
   TRACE("identifyApplicableFormat()");
   
   format1 = false;
   format2 = false;
   format3 = false;
   this->commStruct->isAPZTypeAvail = true;
   this->commStruct->isAPGCountAvail = true;
   
   int returnValue = 4; //Set default to APZ21260
   bool isMulti = false;
   bool retval = isMultipleCpSystem(isMulti);
   if(!retval)
   {
      this->commStruct->isAPZTypeAvail = false;
   }
   else
   {
      if(isMulti)
      {
         CPID cpid = 0;
         uint16_t system = 4;
         ACS_CS_API_NS::CS_API_Result returnValue;
         
         returnValue = ACS_CS_API_NetworkElement::getClockMaster(cpid);
        
         if (returnValue == ACS_CS_API_NS::Result_Success)
         {
            ACS_CS_API_CP *apiCp = ACS_CS_API::createCPInstance();
            returnValue = apiCp->getAPZSystem(cpid, system);
            ACS_CS_API::deleteCPInstance(apiCp);
         }
         else
            TRACE("ERROR: getClockMaster() failed - result = %d", returnValue);
         
         if (returnValue == ACS_CS_API_NS::Result_Success)
         {        
            if (system == 21250) 
               format1 = true;
               
            if ((system == 21255) || (system == 21260))
               format2 = true;
         }
         else
         {
            TRACE("ERROR: getAPZSystem() failed - result = %d", returnValue);
            this->commStruct->isAPZTypeAvail = false;
         }
      }   
      else 
      {  
         //use imm functions
         acs_apgcc_paramhandling acsParamHandling;
      
         ACS_CC_ReturnType result;
         //result = acsParamHandling.getParameter("apConfigurationId=1", "cpAndProtocolType", &returnValue);
         result = acsParamHandling.getParameter(ACS_CS_ImmMapper::RDN_APZFUNCTIONS, ACS_CS_ImmMapper::ATTR_APZ_PROTOCOL_TYPE, &returnValue);
         if (result != ACS_CC_SUCCESS)
         {
            TRACE("ERROR: getParameter() failed - result = %d", result);
            
            //Get env variable - used for testing
            char * envVar = getenv("APZTYPE");
            if(envVar != 0 && strcmp(envVar, ""))
            {
               returnValue = atoi(envVar);
            }
            else
            {
               TRACE("ERROR: getenv(APZTYPE) failed");
               //cout<<"Failed to read apzProtocolType and env variable APZTYPE. Default value:"<<endl;
               this->commStruct->isAPZTypeAvail = false;
            }
         }
         else
         {
            if(returnValue > 3)
               format2 = true;//APZ21255/60
               
            if(returnValue == 3)
               format1 = true;//APZ21250
         }
      }
   }
   
   //Check number of AP in the system
   uint32_t apgCount;
      
   if (ACS_CS_API_NetworkElement::getAPGCount(apgCount) == ACS_CS_API_NS::Result_Success)
   {
      if (apgCount >= 2)
         format3 = true;
   }
   else
   {
      TRACE("ERROR: getAPGCount() failed");
      this->commStruct->isAPGCountAvail = false;
   }

   return true;
}

bool ACS_CS_MODDCommands::isMultipleCpSystem(bool &isMulti)
{
  TRACE("isMultipleCPSystem()");
  
  ACS_CS_API_NS::CS_API_Result resultcs = ACS_CS_API_NetworkElement::isMultipleCPSystem(isMulti);

  if (resultcs == ACS_CS_API_NS::Result_Success)
  {
     if (isMulti)
     {
	// Multi CP system
	return true;
     }	
     else
     {
	//Single Cp System
	return true;
     }
  }
  else
  {
      TRACE("ERROR: isMultipleCPSystem() failed - result = %d", resultcs);
      return false;
  }
}

//*********************************************************************************
// Description:
//    This function handles openning a Telnet session
//
// Parameters:
//    -
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::handleTelnetSession()
{
   TRACE("handleTelnetSession()");
   
   int exitCode = 0;
   bool isConnected = false;
   string str1 = "telnet -E "+this->commStruct->strIpEthA;
   string str2 = "telnet -E "+this->commStruct->strIpEthB;
   int c =0;
   bool noprint = false;
   bool firsttime =true;

   FILE* pipe = popen(str1.c_str(), "r");
   std::cout << "Trying "<< this->commStruct->strIpEthA<<"...\n"<<"press ctr-c to try another subnet\n" << flush;
   if(NULL != pipe)
   {
      char buffer[128]={0};
      int i =0;
      while(!feof(pipe))
      {
    	// get the cmd output
	c = getc(pipe);
	if(c != EOF)
	{
	   if((firsttime == true) && (c == 'T') && (i == 0))
	   {
		noprint = true;
	   }
	   if(firsttime == true)
	      firsttime = false;
	   if(noprint == false)
	   {
	      buffer[i++] = c;
	      if ( c == ' ')
	      {
                 string temp(buffer);
	 	 //      std::cout << "\n buffer = " << temp.c_str() << std::endl;
		 if(temp.find("Connected") != std::string::npos)
		 isConnected = true;
		 strcpy(buffer,"");
		 i = 0;
	      }
	      if ( c == '\n')
	      {
	      	i= 0;
	        strcpy(buffer,"");
	      }
	      fputc(c,stdout);
	      fflush(stdout);
	   }
	   if ((noprint == true ) && (c == '\n'))
	   	noprint = false;
	}
     }
   }
   // wait cmd termination
   exitCode = pclose(pipe);
   // get the exit code from the exit status
   if (isConnected == false)
   {
      bool firsttime1 = true;
      bool noprint1 = false;
      //      std::cout << " \n try on other subnet..." << std::endl;
      FILE* pipe1 = popen(str2.c_str(), "r");
      std::cout << "\nTrying "<<this->commStruct->strIpEthB<<"...\n" << flush;
      if(NULL != pipe1)
      {
      	char buffer1[128]={0};
        int i =0;
	while(!feof(pipe1))
	{
	   // get the cmd output
	   c = getc(pipe1);
	   if(c != EOF)
	   {
	      if((firsttime1 == true) && (c == 'T') && (i == 0))
	      {
	         noprint1 = true;
	      }
	      if(firsttime1 == true)
	       firsttime1 = false;
	      if(noprint1 == false)
	      {
	        buffer1[i++] = c;
	     	if ( c == ' ')
	     	{
		   string temp(buffer1);
		   //      std::cout << "\n buffer = " << temp.c_str() << std::endl;
		   if(temp.find("Connected") != std::string::npos)
		     isConnected = true;
		   strcpy(buffer1,"");
		   i = 0;
	        }
	     	if ( c == '\n')
	     	{
	          i= 0;
		  strcpy(buffer1,"");
	        }
	        fputc(c,stdout);
	        fflush(stdout);
	      }
	      if ((noprint1 == true ) && (c == '\n'))
	         noprint1 = false;
	    }
	}
      }
      exitCode = pclose(pipe1);
      // get the exit code from the exit status
   }
   if(isConnected != true)
        exitCode = NSCommand :: Exit_Connection_Failure;
   this->commStruct->exitCode = exitCode;
   return isConnected;
}

//*********************************************************************************
// Description:
//    This function handles openning a secure session (either ssh or sftp)
//
// Parameters:
//    -
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::handleSshSession()
{
   TRACE("handleSshSession()");
   
   int exitCode = 0;
   bool isConnected = false;  
   char commandIpEthA[512], commandIpEthB[512];


   //===========================
   // Create the command string
   //===========================
   {
      string commandName;
      string fullCommandIpEthA;
      string fullCommandIpEthB;
      
      switch (this->commStruct->openSessionType)
      {
      case NSCommand::SSH:
         commandName = "ssh";
         break;
         
      case NSCommand::SSH_4422:
    	  commandName = "ssh -p 4422";
    	  break;

      default:
         this->commStruct->exitCode = NSCommand::Exit_Incorrect_Usage;
         return false;
      }
      
      // This option is used to ignore the fingerprint of the host key confirmation
      // To Debug add the option: -o LogLevel=DEBUG3
      commandName += " -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no ";
      
      if (this->commStruct->username == "")
      {
         fullCommandIpEthA = commandName + this->commStruct->strIpEthA + " 2>&1";
         fullCommandIpEthB = commandName + this->commStruct->strIpEthB + " 2>&1";
      }
      else
      {
         fullCommandIpEthA = commandName + this->commStruct->username + "@" + this->commStruct->strIpEthA + " 2>&1";
         fullCommandIpEthB = commandName + this->commStruct->username + "@" + this->commStruct->strIpEthB + " 2>&1";
      }
      
      strcpy(commandIpEthA, fullCommandIpEthA.c_str());
      strcpy(commandIpEthB, fullCommandIpEthB.c_str());
   }
   
   //=======================
   // Open a secure session 
   //=======================
   bool firstTime =true;
   bool printEnable = false;
   FILE* pipe;
   char connectedIPaddr[128];
   
   for (int j = 0; j < 2; j++)
   {
      if (firstTime == true)  // Connecting to IP address in Plan A
      {
         std::cout << "Trying "<< this->commStruct->strIpEthA<<"...\n"<<"press ctr-c to try another subnet\n" << flush;
         strncpy(connectedIPaddr, this->commStruct->strIpEthA.c_str(), 128);
         pipe = popen(commandIpEthA, "r");
         firstTime = false;
      }
      else                    // Connecting to IP address in Plan B
      {
         std::cout << "\nTrying "<<this->commStruct->strIpEthB<<"...\n" << flush;
         strncpy(connectedIPaddr, this->commStruct->strIpEthB.c_str(), 128);
         pipe = popen(commandIpEthB, "r");
      }
                
      if(NULL != pipe)
      {
         char buffer[256] = {0};
         int i = 0, c = 0;
         
         while(!feof(pipe))
         {
            // get the cmd output
            c = getc(pipe);
            if(c != EOF)
            {
               // Verify if the connection is successful
               if (isConnected == false)
               {
                  buffer[i++] = c;
                  if (c == '\n')
                  {
                     string temp(buffer);

                     if (temp.find("Warning:") != std::string::npos)
                     {
                        isConnected = true;
                        std::cout << "Connected to "<< connectedIPaddr << "\n" << flush;
                     }

                     strcpy(buffer,"");
                     i = 0;
                  }
               }

               // Dectect if output from ssh/sftp can be printed
               if ((isConnected == true) && (printEnable == false))
               {
                     printEnable = true;
               }
               
               if (printEnable == true)
               {
                  fputc(c,stdout);
                  fflush(stdout);
               }
            }
         }
         
         // wait cmd termination
         pclose(pipe);
      }
      
      if (isConnected == true)
         break;
   }
   
   if (isConnected != true)
       exitCode = NSCommand :: Exit_Connection_Failure;
   
   this->commStruct->exitCode = exitCode;
   return isConnected;
}

//*********************************************************************************
// Description:
//    This function handles openning a sftp session
//
// Parameters:
//    -
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::handleSftpSession()
{
   TRACE("handleSftpSession()");

   int exitCode = 0;
   bool isConnected = false;
   char commandIpEthA[512], commandIpEthB[512], commandStr[512];

   //=====================================
   // Mount and Chroot to jail opensession
   //=====================================

   if (!this->mountDirectorForSftp())
   {
      TRACE("ERROR: mountDirectorForSftp() failed");
      this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
      return false;
   }

   if (chroot(ROOT_DIRECTORY_SFTP))
   {
      TRACE("ERROR: chroot(%s) failed - %s", ROOT_DIRECTORY_SFTP, strerror(errno));
      chdir(ROOT_DIRECTORY_SFTP);
      this->umountDirectorForSftp();
      this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
      return false;
   }

   if (chdir(NBI_DIRECTORY_SFTP))
   {
      TRACE("ERROR: chdir(%s) failed - %s", NBI_DIRECTORY_SFTP, strerror(errno));
      chdir(ROOT_DIRECTORY_SFTP);
      this->umountDirectorForSftp();
      this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
      return false;
   }

   //===========================
   // Create the command string
   //===========================
   {
      string commandName;
      string fullCommandIpEthA;
      string fullCommandIpEthB;

      commandName += "sftp -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no ";

      if (this->commStruct->username == "")
      {
         fullCommandIpEthA = commandName + this->commStruct->strIpEthA + " 2>&1";
         fullCommandIpEthB = commandName + this->commStruct->strIpEthB + " 2>&1";
      }
      else
      {
         fullCommandIpEthA = commandName + this->commStruct->username + "@" + this->commStruct->strIpEthA + " 2>&1";
         fullCommandIpEthB = commandName + this->commStruct->username + "@" + this->commStruct->strIpEthB + " 2>&1";
      }

      strcpy(commandIpEthA, fullCommandIpEthA.c_str());
      strcpy(commandIpEthB, fullCommandIpEthB.c_str());
   }

   //=======================
   // Open a SFTP session
   //=======================
   bool firstTime =true;
   bool printEnable = false;
   bool input_allow = false;
   char connectedIPaddr[128];

   for (int j = 0; j < 2; j++)
   {
      if (firstTime == true)  // Connecting to IP address in Plan A
      {
         TRACE("handleSftpSession() - First try");
         cout << "Trying "<< this->commStruct->strIpEthA<<"..."<< endl;
         cout << "press ctr-c to try another subnet" << endl;
         strncpy(connectedIPaddr, this->commStruct->strIpEthA.c_str(), 128);
         strcpy(commandStr, commandIpEthA);
         firstTime = false;
         TRACE("handleSftpSession() - First started");
      }
      else                    // Connecting to IP address in Plan B
      {
         TRACE("handleSftpSession() - Second try");
         cout <<"Trying "<<this->commStruct->strIpEthB<<"..." << endl;
         strncpy(connectedIPaddr, this->commStruct->strIpEthB.c_str(), 128);
         strcpy(commandStr, commandIpEthB);
         TRACE("handleSftpSession() - Second started");
      }

      //=======================================
      // Creat pipes connecting to SFTP process
      //=======================================
      int command_pfd[2];                             // Pipe file descriptors
      int printout_pfd[2];                             // Pipe file descriptors

      char *arg[11] =
      {
         (char*)"sh",
         (char*)"-c",
         commandStr,
         NULL
      };

      if (pipe(command_pfd) == -1)                    // Create the pipe
      {
         TRACE("pipe(command_pfd) failed - %s", strerror(errno));
         this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
         return false;
      }

      if (pipe(printout_pfd) == -1)                    // Create the pipe
      {
         TRACE("pipe(printout_pfd) failed - %s", strerror(errno));
         this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
         return false;
      }

      //====================
      // Create SFTP process
      //====================
      pid_t sftp_pid = fork();
      switch (sftp_pid)
      {
      case -1:
         TRACE("fork() failed - %s", strerror(errno));
         this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
         return false;

      case 0:             /* sftp  - reads from pipe */
         TRACE("SFTP process");

         close(command_pfd[1]);
         close(printout_pfd[0]);

         close(STDIN_FILENO);
         close(STDOUT_FILENO);
         close(STDERR_FILENO);

         if (dup2(command_pfd[0], STDIN_FILENO) == -1)
         {
            TRACE(" dup2(command_pfd[0], STDIN_FILENO) failed");
            this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
            return false;
         }

         if (dup2(printout_pfd[1], STDOUT_FILENO) == -1)
         {
            TRACE(" dup2(printout_pfd[1], STDOUT_FILENO) failed");
            this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
            return false;
         }

         if (dup2(printout_pfd[1], STDERR_FILENO) == -1)
         {
            TRACE(" dup2(printout_pfd[1], STDERR_FILENO) failed");
            this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
            return false;
         }

         TRACE("START execl() - command = %s %s %s", arg[0], arg[1], arg[2]);
         if (execvp(arg[0], arg))
         {
            TRACE("execl() failed - %s - command = %s", strerror(errno), arg[0]);
            this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
            return false;
         }

         exit(0);

      default:            /* opensession - writes to pipe */
         TRACE("opensession process");

         close(command_pfd[0]);
         close(printout_pfd[1]);

         break;
      }

      //==========================================================================
      // OPENSESSION process - Handle input from terminal and output from sftp
      //==========================================================================

      {
         char buffer[256] = {0};
         char input_buf[256] = {0};
         int i = 0, k = 0;
         int input_cnt = 0;
         char c, in_char;
         fd_set rfds;
         struct timeval tv;
         int retval;
         bool is_prompt_printed = false;
         ssize_t read_result;

         bool localPwd = false;
         bool localShellCommand = false;

         bool checkCurrentLocalPath = false;
         char input_lcd_buf[256] = {0};
         const std::string local_pwd_message = "Local working directory: ";
         const std::string defaut_pwd_path = "/ap/nbi_fuse";
         const std::string pwd_cmd = "pwd";
         const std::string lcd_cmd = "lcd";

         while(1)
         {
           bool is_output(false);

           if (waitpid(sftp_pid, NULL, WNOHANG) != 0)
           {
               TRACE("SFTP process exit");
               break;
           }

           /* Watch stdin (fd 0) to see when it has input. */
           FD_ZERO(&rfds);
           if (input_allow)
              FD_SET(STDIN_FILENO, &rfds);

           FD_SET(printout_pfd[0], &rfds);

           /* Wait up to five seconds. */
           tv.tv_sec = 5;
           tv.tv_usec = 0;

           retval = select(printout_pfd[0] + 1, &rfds, NULL, NULL, &tv);
           switch (retval)
           {
              case -1:
               TRACE("select() failed - %s", strerror(errno));
               break;

              case 0:
//               TRACE("select() timeout");
               break;

              default:
               if (FD_ISSET(STDIN_FILENO, &rfds))
               {

                  read_result = read(STDIN_FILENO, &in_char, 1);
                  if (read_result != 0)
                  {
                     input_buf[input_cnt++] = in_char;
                     input_buf[input_cnt] = '\0';
                     if (in_char == '\n')
                     {
                    	 bool skipCmd = false;
                    	 string temp = removeInvalidCharsFromCmd(input_buf);

                    	 if ((temp.substr(0,4)).compare("lpwd") == 0 || (temp.substr(0,4)).compare("!pwd") == 0)
                    	 {
                    		 if (temp.length() == 4 || (temp.length() > 5 && temp.find(" ")!= std::string::npos))
                    		 {
                    			 TRACE("Handling of customized local PWD command !!!");
                    			 if (temp.find("!") != std::string::npos)
                    			 {
                    				 localShellCommand = true;
                    			 }
                    			 localPwd = true;
                    		 }
                    	 }

                    	 if ((temp.substr(0,3)).compare("lcd") == 0)
                    	 {
                    		 if ((temp.length() > 4 && temp.find(" ")!= std::string::npos))
                    		 {
                    			 TRACE("Handling of customized local CD command !!!");

                    			 checkCurrentLocalPath = true;

                    			 //copy command to execute after verification
                    			 strcpy(input_lcd_buf,input_buf);

                    			 //perform lpwd command in order to retrive the current folder path
                    			 write(command_pfd[1], "lpwd\n", 5);

                    			 //postpone lcd cmd execution in order to verify the current local path
                    			 skipCmd = true;
                    		 }
                    	 }

                    	 if (!skipCmd)
                    	 {
                    		 ssize_t result;
                    		 result = write(command_pfd[1], input_buf, input_cnt);
                    		 if ( result == -1 )
                    		 {
                    			 TRACE("write(command_pfd[1] = %d, input_buf, input_cnt + 1) failed - %s",command_pfd[1], strerror(errno));
                    			 this->commStruct->exitCode = NSCommand::Exit_Error_Executing;
                    			 return false;
                    		 }

                    		 // Send "\n" to sftp to trigger prompt sftp>
                    		 write(command_pfd[1], "\n", 1);
                    		 is_prompt_printed = false;
                    	 }

                    	 strcpy(input_buf,"");
                    	 input_cnt = 0;
                     }
                  }
               }


               if (FD_ISSET(printout_pfd[0], &rfds))
               {
                  if (read(printout_pfd[0], &c, 1) == 1)
                  {
                     is_output = true;
                  }
               }

               break;
            }

            // Printout handling
            if (is_output)
            {
               // Verify if the connection is successful
               if (isConnected == false)
               {
                  buffer[i++] = c;
                  if (c == '\n')
                  {
                     string temp(buffer);

                     if (temp.find("Warning:") != std::string::npos)
                     {
                        isConnected = true;
                        cout << "Connected to "<< connectedIPaddr << endl;
                        TRACE("handleSftpSession() - Connected to %s", connectedIPaddr);
                     }

                     strcpy(buffer,"");
                     i = 0;
                  }
               }

               // Dectect if output from ssh/sftp can be printed
               if ((isConnected == true) && (printEnable == false))
               {
                  buffer[i++] = c;
                  buffer[i] = '\0';
                  if (c == '\n')
                  {
                     string temp(buffer);
                     if (temp.find("Connected to ") != std::string::npos)
                     {
                        input_allow = true;
                        printEnable = true;

                        // Clean up non-required directories after sftp session establishes
                        chdir("/");
                        this->umountDirectorForSftp();

                        // Send "\n" to sftp to trigger prompt sftp>
                        write(command_pfd[1], "\n", 1);
                     }

                     strcpy(buffer,"");
                     i = 0;
                  }
               }

               if (printEnable == true)
               {
                  buffer[k++] = c;
                  buffer[k] = '\0';
                  if (c == '\n')
                  {
                     string temp(buffer);

                     if (temp.find("sftp> \n") != std::string::npos)
                     {
                        if (!is_prompt_printed)
                        {
                            std::cout << "sftp> " << flush;
                        }

                        is_prompt_printed = true;
                     }
                     else if (temp.find("sftp> ") == std::string::npos) // This is result of command
                     {
                    	 TRACE("temp(buffer) (%s)", temp.c_str());

                    	 //manage lpwd or !pwd commands
                    	 if (localPwd)
                    	 {
                    		 std::string toCompare;

                    		 //build pwd output to compare
                    		 if (!localShellCommand)
                    		 {
                    			 toCompare += local_pwd_message;
                    		 }

                    		 toCompare += defaut_pwd_path;

                    		 //erase new line from output temp string
                    		 temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());

                    		 //build output in case the current path is different from the default one (/ap/nbi_fuse)
                    		 if (temp.compare(toCompare))
                    		 {
                    			 std::string::size_type i = temp.find(toCompare);

                        		 //remove /ap/nbi_fuse from local path printout
                    			 if (i != std::string::npos)
                    				 temp.erase(i, toCompare.length());

                    			 TRACE("new temp path (%s)", temp.c_str());
                    		 }
                    		 else
                    		 {
                    			 //overwrite current default path from /ap/nbi_fuse to /
                    			 temp = "/";
                    		 }

                    		 //adding local message
                    		 if (!localShellCommand)
                    		 {
                    			 cout << local_pwd_message;
                    		 }

                    		 cout << temp << endl;
                    		 cout << "\r" << flush;

                    		 localShellCommand = false;
                    		 localPwd = false;
                    	 }
                    	 //manage lcd command to avoid to exit from /ap/nbi_fuse
                    	 else if (checkCurrentLocalPath)
                    	 {
                    		 bool execute = false;
                    		 string cmdRequested = removeInvalidCharsFromCmd(input_lcd_buf);

                    		 //erase new line from output temp string (lpwd)
                    		 temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());

                    		 if (cmdRequested.compare(lcd_cmd.c_str()) == 0)
                    		 {
                    			 execute = true;

                    		 }
                    		 else if (cmdRequested.find("lcd ..") != std::string::npos)
                    		 {
                    			 //count occurrences from local command and local path
                    			 size_t n_up = (std::count(cmdRequested.begin(), cmdRequested.end(), '.')/2);
                    			 size_t n_down = std::count(temp.begin(), temp.end(), '/');

                    			 //check if the lcd .. command can be executed avoiding to exit from nbi_fuse
                    			 if ((n_down-n_up) < 2)
                    			 {
                    				 std::string buildCmd = lcd_cmd + " " + defaut_pwd_path;
                    				 strcpy(input_lcd_buf, buildCmd.c_str());
                    			 }

                    			 execute = true;
                    		 }
                    		 else
                    		 {
                    			 std::string buildCmd;

                    			 //erase lcd from requested command
                    			 std::string::size_type i = cmdRequested.find(lcd_cmd);
                    			 if (i != std::string::npos)
                    				 cmdRequested.erase(i, lcd_cmd.length()+1);//plus space after lcd cmd

                    			 if (cmdRequested.find("/") != std::string::npos)
                    			 {
                    				 buildCmd = lcd_cmd + " " + defaut_pwd_path + cmdRequested;
                    			 }
                    			 else
                    			 {
                    				 buildCmd = lcd_cmd + " " + cmdRequested;
                    			 }

                    			 strcpy(input_lcd_buf, buildCmd.c_str());
                    			 execute = true;
                    		 }


                    		 if (execute)
                    		 {
                    			 //perform the lcd command
                    			 write(command_pfd[1], input_lcd_buf, sizeof(input_lcd_buf));
                    			 write(command_pfd[1], "\n", 1);
                    			 is_prompt_printed = false;
                    		 }

                    		 std::cout << "sftp> " << flush;

                    		 strcpy(input_lcd_buf,"");
                    		 checkCurrentLocalPath = false;
                    	 }
                    	 else
                    	 {
                    		 std::cout << buffer << flush;
                    	 }
                     }

                     strcpy(buffer,"");
                     k = 0;
                  }
               }
            }
         }
      }

      TRACE("handleSftpSession() - Connection closed");

      if (isConnected == true){
         break;
      }
   }

   if (isConnected != true){
       exitCode = NSCommand :: Exit_Connection_Failure;
   }

   chdir("/");
   this->umountDirectorForSftp();


   TRACE("handleSftpSession() - Exit");

   this->commStruct->exitCode = exitCode;
   return isConnected;
}

std::string ACS_CS_MODDCommands::trimCmd(const std::string& str, const std::string& whitespace)
{
	std::string::size_type strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	std::string::size_type strEnd = str.find_last_not_of(whitespace);
	std::string::size_type strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::string ACS_CS_MODDCommands::removeInvalidCharsFromCmd(const std::string& str, const std::string& fill, const std::string& whitespace)
{
    // trim first
	std::string result = trimCmd(str, whitespace);

    // replace sub ranges
	std::string::size_type beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos)
    {
    	std::string::size_type endSpace = result.find_first_not_of(whitespace, beginSpace);
    	std::string::size_type range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        std::string::size_type newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}

//*********************************************************************************
// Description:
//    This function handles openning a session specified in commStruct->openSessionType
//
// Parameters:
//    -
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::handleOpenSession()
{
   TRACE("handleOpenSession()");
   
   switch (this->commStruct->openSessionType)
   {
      case NSCommand::TELNET:
         return handleTelnetSession();
         
      case NSCommand::SFTP:
    	  return handleSftpSession();

      case NSCommand::SSH:
      case NSCommand::SSH_4422:
         return handleSshSession();
        
      default:
         this->commStruct->exitCode = NSCommand::Exit_Incorrect_Usage;
         return false;
   }
}

//*********************************************************************************
// Description:
//    This function handles creating and mounting required directory to run sftp 
//    before jailing the opensession. Clean up is executed with rmdir to remove
//    failed mount and check if the directory was mounted. If it was mounted, no
//    action is proceeded.
//
// Parameters:
//    -
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::mountDirectorForSftp(void)
{
   TRACE("mountDirectorForSftp()");
   
   int result;
   int fd;
      
   if (chdir(ROOT_DIRECTORY_SFTP))
   {
      TRACE("ERROR: chdir( %s ) failed - %s", ROOT_DIRECTORY_SFTP, strerror(errno));
      return false;
   }
         
   // Create directory
   if (mkdir("usr", S_IWUSR |S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IXOTH))
      TRACE("mkdir( usr ) failed - %s", strerror(errno));
      
   if (mkdir("usr/lib64", S_IWUSR |S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IXOTH))
      TRACE("mkdir( usr/lib64 ) failed - %s", strerror(errno));
      
   if (mkdir("usr/bin", S_IWUSR |S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IXOTH))
      TRACE("mkdir( usr/bin ) failed - %s", strerror(errno));
      
   if (mkdir("lib64", S_IWUSR | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IXOTH))
      TRACE("mkdir( lib64 ) failed - %s", strerror(errno));

   if (mkdir("bin", S_IWUSR | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IXOTH))
      TRACE("mkdir( bin ) failed - %s", strerror(errno));

   // Mount binary files
   for (unsigned int i = 0; i < (sizeof(m_sftpFileList) / sizeof(MountFile)); i++)
   {
      fd = open(m_sftpFileList[i].target, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
      if (fd != -1)
      {
         off_t curr = lseek(fd, 0, SEEK_END);
         close(fd);
         
         if (curr == 0) // file size == 0 (i.e the file is not mounted)
         {
            if(mount(m_sftpFileList[i].source, m_sftpFileList[i].target, 0, MS_BIND, 0))
            {
               TRACE("ERROR: mount( %s ) failed - %s", m_sftpFileList[i].target, strerror(errno));
               return false;
            }
         }
      }
      else if (errno != ETXTBSY)
      {
         TRACE("ERROR: open( %s ) failed - %s - errno = %d", m_sftpFileList[i].target, strerror(errno), errno);
         return false;
      }
   }
   
   // Mount lib files
   for (unsigned int i = 0; i < (sizeof(m_LibDirectoryList) / sizeof(MountFile)); i++)
   {
      DIR *dirp;
      struct dirent *dp;

      dirp = opendir(m_LibDirectoryList[i].source);
      if (dirp == NULL)
         TRACE("ERROR: opendir( %s ) failed - %s - errno = %d", m_LibDirectoryList[i].source, strerror(errno), errno);
      else
      {
         /* For each entry in this directory, create and mount files */
         for (;;) 
         {
            string source = m_LibDirectoryList[i].source;
            string target = m_LibDirectoryList[i].target;
            
            errno = 0; /* To distinguish error from end-of-directory */
            dp = readdir(dirp);
            if (dp == NULL)
               break;
               
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
               continue; /* Skip . and .. */
            
            if (dp->d_type != DT_REG && dp->d_type != DT_LNK)
               continue; /* Only create and mount regular file and symbolic link. Skip others types */
               
            // create and mount files
            source += dp->d_name;
            target += dp->d_name;
            
            fd = open(target.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            if (fd != -1)
            {
               off_t curr = lseek(fd, 0, SEEK_END);
               close(fd);
               
               if (curr == 0) // file size == 0 (i.e the file is not mounted)
               {
                  if(mount(source.c_str(), target.c_str(), 0, MS_BIND, 0))
                  {
                     if (errno == ENOTDIR)
                        TRACE("mount( %s ) failed - %s", target.c_str(), strerror(errno));
                     else
                     {
                        TRACE("ERROR: mount( %s ) failed - %s", target.c_str(), strerror(errno));
                        return false;
                     }
                  }
               }
            }
            else if (errno != ETXTBSY)
            {
               TRACE("ERROR: open( %s ) failed - %s - errno = %d", target.c_str(), strerror(errno), errno);
               return false;
            }
         }
         
         if (errno != 0)
         {
            TRACE("ERROR: readdir failed - %s - errno = %d", strerror(errno), errno);
            return false;
         }
            
         if (closedir(dirp) == -1)
         {
            TRACE("ERROR: closedir failed - %s - errno = %d", strerror(errno), errno);
            return false;
         }
      }
   }
        
   // mount /dev directory
   result = rmdir("dev");
   if (!result || (result && (errno != EBUSY)))
   {
      mkdir("dev", S_IRUSR | S_IXUSR);
      if(mount("/dev", "dev", 0, MS_BIND, 0))
      {
         TRACE("ERROR: mount(dev) failed - %s", strerror(errno));
         return false;
      }
   }
      
   // mount /etc directory
   result = rmdir("etc");
   if (!result || (result && (errno != EBUSY)))
   {
      mkdir("etc", S_IRUSR | S_IXUSR);
      if(mount("/etc", "etc", 0, MS_BIND, 0))
      {
         TRACE("ERROR: mount(etc) failed - %s", strerror(errno));
         return false;
      }
   }
   
   // mount /var/run/nscd for checking LDAP accounts
   result = rmdir("var/run/nscd");
   if (!result || (result && (errno != EBUSY)))
   {
      mkdir("var", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("var/run", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("var/run/nscd", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      if(mount("/var/run/nscd", "var/run/nscd", 0, MS_BIND, 0))
      {
         TRACE("ERROR: mount(var/run/nscd) failed - %s", strerror(errno));
         return false;
      }
   }

   // mount /var/lib/sss/pipes for checking LDAP accounts in SLES12
   {
	   mkdir("var/lib", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	   mkdir("var/lib/sss", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	   mkdir("var/lib/sss/pipes", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

	   if(mount("/var/lib/sss/pipes", "var/lib/sss/pipes", 0, MS_BIND, 0))
	   {
		   TRACE("ERROR: mount(var/lib/sss/pipes) failed - %s", strerror(errno));
		   return false;
	   }
   }
   
   // mount /cluster/storage/system/config/sec-apr9010539/var/db for checking LDAP accounts
   result = rmdir("cluster/storage/system/config/sec-apr9010539/var/db");
   if (!result || (result && (errno != EBUSY)))
   {
      mkdir("cluster", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage/system", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage/system/config", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage/system/config/sec-apr9010539", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage/system/config/sec-apr9010539/var", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage/system/config/sec-apr9010539/var/db", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      if(mount("/cluster/storage/system/config/sec-apr9010539/var/db", "cluster/storage/system/config/sec-apr9010539/var/db", 0, MS_BIND, 0))
      {
         TRACE("ERROR: mount(cluster/storage/system/config/sec-apr9010539/var/db) failed - %s", strerror(errno));
         return false;
      }
   }
   
   // mount /cluster/sw_package/APG for /sw_package/APG symbolic link
   result = rmdir("cluster/sw_package/APG");
   if (!result || (result && (errno != EBUSY)))
   {
      mkdir("cluster/sw_package", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/sw_package/APG", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      if(mount("/cluster/sw_package/APG", "cluster/sw_package/APG", 0, MS_BIND, 0))
      {
         TRACE("ERROR: mount(cluster/sw_package/APG) failed - %s", strerror(errno));
         return false;
      }
   }

   // mount /cluster/storage/no-backup/coremw/StorageManagement
   result = rmdir("cluster/storage/no-backup/coremw/SoftwareManagement");
   if (!result || (result && (errno != EBUSY)))
   {
      mkdir("cluster", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage/no-backup", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage/no-backup/coremw", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      mkdir("cluster/storage/no-backup/coremw/SoftwareManagement", S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

      if(mount("/cluster/storage/no-backup/coremw/SoftwareManagement", "cluster/storage/no-backup/coremw/SoftwareManagement", 0, MS_BIND, 0))
      {
         TRACE("ERROR: mount(cluster/storage/no-backup/coremw/SoftwareManagement) failed - %s", strerror(errno));
         return false;
      }
   }
   
   return true;
}

//*********************************************************************************
// Description:
//    This function handles unmounting and removing required directory to run sftp 
//    before jailing the opensession. If the directory is required for running sftp
//    unmounting fails
//
//    Note: Directories required for an opened session are not unmounted.
//
// Parameters:
//    -
//
// Return Value:
//    true     Success
//    false    Fail
//*********************************************************************************
bool ACS_CS_MODDCommands::umountDirectorForSftp(void)
{      
   TRACE("umountDirectorForSftp()");
   
   if (!umount("var/run/nscd"))
   {
      rmdir("var/run/nscd");
      rmdir("var/run");
   }

   if (!umount("var/lib/sss/pipes"))
   {
	   rmdir("var/lib/sss/pipes");
	   rmdir("var/lib/sss");
	   rmdir("var/lib");
	   rmdir("var");
   }

   if (!umount("cluster/storage/system/config/sec-apr9010539/var/db"))
   {
      rmdir("cluster/storage/system/config/sec-apr9010539/var/db");
      rmdir("cluster/storage/system/config/sec-apr9010539/var");
      rmdir("cluster/storage/system/config/sec-apr9010539");
      rmdir("cluster/storage/system/config");
      rmdir("cluster/storage/system");
      rmdir("cluster/storage");
   }
     
   if (!umount("etc"))
      rmdir("etc");
      
   if (!umount("dev"))
      rmdir("dev");
      
   if (!umount("bin/sftp")) // Check if there is openned session.
   {
      // Unmount binary files
      for (unsigned int i = 0; i < (sizeof(m_sftpFileList) / sizeof(MountFile)); i++)
      {
         umount(m_sftpFileList[i].target);
         unlink(m_sftpFileList[i].target);
      }
      
      // Unmout Lib files
      for (unsigned int i = 0; i < (sizeof(m_LibDirectoryList) / sizeof(MountFile)); i++)
      {
         DIR *dirp;
         struct dirent *dp;

         dirp = opendir(m_LibDirectoryList[i].target);
         if (dirp == NULL)
            TRACE("ERROR: opendir( %s ) failed - %s - errno = %d", m_LibDirectoryList[i].source, strerror(errno), errno);
         else
         {
            /* For each entry in this directory, create and mount files */
            for (;;) 
            {
               string target = m_LibDirectoryList[i].target;
               
               errno = 0; /* To distinguish error from end-of-directory */
               dp = readdir(dirp);
               if (dp == NULL)
                  break;
                  
               if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
                  continue; /* Skip . and .. */
               
               // create and mount files
               target += dp->d_name;
               
               umount(target.c_str());
               unlink(target.c_str());
            }
            
            if (errno != 0)
            {
               TRACE("ERROR: readdir failed - %s - errno = %d", strerror(errno), errno);
               return false;
            }
               
            if (closedir(dirp) == -1)
            {
               TRACE("ERROR: closedir failed - %s - errno = %d", strerror(errno), errno);
               return false;
            }
         }
      }

      // Remove directories
      rmdir("bin");
      rmdir("lib64");
      rmdir("usr/bin");
      rmdir("usr/lib64");
      rmdir("usr");
      
      // The following directories are unmount when there is no openned session.
      if (!umount("cluster/sw_package/APG"))
      {
         rmdir("cluster/sw_package/APG");
         rmdir("cluster/sw_package");
         rmdir("cluster");
      }

      // The following directories are unmount when there is no openned session.
      if (!umount("cluster/storage/no-backup/coremw/SoftwareManagement"))
      {
         rmdir("cluster/storage/no-backup/coremw/SoftwareManagement");
         rmdir("cluster/storage/no-backup/coremw");
         rmdir("cluster/storage/no-backup");
         rmdir("cluster/storage");
         rmdir("cluster");
      }
   }
      
   return true;
}

bool ACS_CS_MODDCommands::getParentProcessPid(pid_t processId, pid_t &parentProcessId)
{
	bool result = false;
	string command = "";
	string output = "";

	if (processId > 0) {
		ostringstream outStream;
		outStream << "cat /proc/" << processId << "/status | grep PPid";
		command = outStream.str();

		if (runCommand(command, output)) {
			// Output format is "PPid : <id>". Lets extract the <id>
			int p = output.find(":");
			if (p > 0) {
				// Extract id string
				output = output.substr(p + 1);
				istringstream inStream(output);
				inStream >> parentProcessId;
				result = (!inStream.bad());
			} else {
				result = false;
			}
		}
	}
	return result;
}

bool ACS_CS_MODDCommands::runCommand(const string command, string& output)
{
	FILE *fp;
	char readLine[10000];
	output = "";

	/* Open the command for reading. */
	fp = popen(command.c_str(), "r");
	if (fp == 0) {
		return false;
	}

	/* Read the output a line at a time and store it. */
	while (fgets(readLine, sizeof(readLine) - 1, fp) != 0) {

		size_t newbuflen = strlen(readLine);

		if ( (readLine[newbuflen - 1] == '\r') || (readLine[newbuflen - 1] == '\n') ) {
			readLine[newbuflen - 1] = '\0';
		}

		if ( (readLine[newbuflen - 2] == '\r') || (readLine[newbuflen - 2] == '\n') ) {
			readLine[newbuflen - 2] = '\0';
		}

		output += readLine;
	}

	/* close */
	pclose(fp);

	return true;
}
