#include "ACS_CS_Command_Exitcodes.h"

#include <sstream>

using std::string;

//constructor
ACS_CS_Command_Exitcodes::ACS_CS_Command_Exitcodes(NSCommands::Command command)
   :  exitCode(NSCommands::Exit_Success)
{
   this->initializeExitCodeDescriptions(command);   //initialize the exit code descriptions
}

//destructor
ACS_CS_Command_Exitcodes::~ACS_CS_Command_Exitcodes()
{
}

void ACS_CS_Command_Exitcodes::initializeExitCodeDescriptions(NSCommands::Command command)
{
   exitCodeMap[NSCommands::Exit_Success]         = "Success";
   exitCodeMap[NSCommands::Exit_Error_Executing] = "Error when executing (general fault)";

   
//   exitCodeMap[NSCommands::Exit_Board_NotAllowed]                  = "CP is not defined"; //CPCH,CPGDEF,FDDEF
   exitCodeMap[NSCommands::Exit_NoReply_Cs]                      = "No Reply from Configuration Service";
   exitCodeMap[NSCommands::Exit_Hardware_NotPresent]             = "Hardware not present"; 
   switch (command)
   {
      case NSCommands::CPCH            :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = (string)"Incorrect usage\n" +
         "Usage: cpch -a alias [-s apzsys] [-t cptype] cpid\n\n" +
         "Usage: cpch -s apzsys [-a alias] [-t cptype] cpid\n\n" +
         "Usage: cpch -t cptype [-a alias] [-s apzsys] cpid\n\n" +
         "Usage: cpch -r cpid"; break;
      case NSCommands::CPLS            :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: cpls [-l]"; break;
      case NSCommands::CPGLS           :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: cpgls"; break;
      case NSCommands::CPGDEF          :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: cpgdef -cp cpname... cpgroupname"; break;
      case NSCommands::CPGRM           :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: cpgrm cpgroupname"; break;
      case NSCommands::FDDEF           :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: fddef -d domain -s service ap"; break;
      case NSCommands::FDRM            :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: fdrm -d domain -s service ap"; break;
      case NSCommands::FDLS            :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: fdls"; break;
      case NSCommands::NECH            :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: nech [-i neid | -m copm | -p profile [-t scope]]"; break;
      case NSCommands::NELS            :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: nels"; break;
      case NSCommands::APZVLANLS       :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage\nUsage: apzvlanls [-n cpname] [-s stack] [-v vlan]"; break;
      default                          :  exitCodeMap[NSCommands::Exit_Incorrect_Usage] = "Incorrect usage";
   }
   switch (command)
   {
      case NSCommands::CPCH   :  exitCodeMap[NSCommands::Exit_Reserved_Occupied_Name] = "Reserved name or occupied alias"; break; //CPCH
      default                 :  exitCodeMap[NSCommands::Exit_Reserved_Occupied_Name] = "Reserved or occupied name";              //CPGDEF
   }

   exitCodeMap[NSCommands::Exit_Incorrect_Alias]                 = "Incorrect alias"; //CPCH
   exitCodeMap[NSCommands::Exit_FuncBoard_NotAllowed]            = "Not Allowed for this Functional Board Name";
   exitCodeMap[NSCommands::Exit_Incorrect_APZ_System]            = "Incorrect APZ system"; //CPCH
   exitCodeMap[NSCommands::Exit_Illegal_CP_Group_Name]           = "Illegal CP group name"; //CPGDEF
   exitCodeMap[NSCommands::Exit_Duplicated_CP_Group_Name]        = "Duplicated CP group name"; //CPGDEF
   exitCodeMap[NSCommands::Exit_CP_Group_Not_Defined]            = "CP group not defined"; //CPGRM
   exitCodeMap[NSCommands::Exit_Predefined_Group_Not_Removable]  = "Predefined CP group not removable"; //CPGRM
   exitCodeMap[NSCommands::Exit_Illegal_Service_Name]            = "Illegal service name"; //FDDEF
   exitCodeMap[NSCommands::Exit_Duplicated_Function_Definition]  = "Duplicated function"; //FDDEF
   exitCodeMap[NSCommands::Exit_Function_Not_Defined]            = "Function not defined"; //FDRM
   exitCodeMap[NSCommands::Exit_Illegal_NEID]                    = "Illegal Network Element Identity"; //NECH
   exitCodeMap[NSCommands::Exit_Illegal_Domain_Name]             = "Illegal domain name"; //FDDEF
   exitCodeMap[NSCommands::Exit_Incorrect_CP_Type]               = "Incorrect CP type"; //CPCH
   exitCodeMap[NSCommands::Exit_Incorrect_ClusterMode]           = "Incorrect cluster operation mode"; //NECH
   exitCodeMap[NSCommands::Exit_Incorrect_Profile]               = "Incorrect profile"; //NECH
   exitCodeMap[NSCommands::Exit_Missing_File]                    = "Command Classification File missing"; //NECH
   exitCodeMap[NSCommands::Exit_ClusterOpMode_Switch_In_Progress] = "Not allowed during Cluster Operation Mode switch"; //NECH
   exitCodeMap[NSCommands::Exit_Profile_Switch_In_Progress]       = "Not allowed during OaM Profile configuration change"; //NECH
   exitCodeMap[NSCommands::Exit_Waiting_For_Dependent_Services]  = "Function busy"; // NECH
   exitCodeMap[NSCommands::Exit_AP_Not_Defined]                  = "AP is not defined"; //FDRM
   exitCodeMap[NSCommands::Exit_Illegal_Command]                 = "Illegal command in this system configuration"; //CPCH,CPLS,CPGDEF,CPGRM,CPGLS,NECH,APZVLANLS
   exitCodeMap[NSCommands::Exit_Illegal_Option]                  = "Illegal option in this system configuration"; //FDDEF,NECH
   exitCodeMap[NSCommands::Exit_Unable_Connect_Server]           = "Unable to connect to server"; //CPCH,CPLS,CPGDEF,CPGRM,CPGLS,NECH,NELS,FDDEF,FDRM,FDLS,APZVLANLS
   exitCodeMap[NSCommands::Exit_CP_Not_Defined]                  = "CP is not defined"; //CPCH,CPGDEF,FDDEF
   exitCodeMap[NSCommands::Exit_Connection_Failure]              = "Connection failure";
   
}
NSCommands::Exit_Code ACS_CS_Command_Exitcodes::getExitCode()
{
   return this->exitCode;
}

string ACS_CS_Command_Exitcodes::getExitCodeDescription()
{
   ExitCodeMapType::const_iterator pos = this->exitCodeMap.find(this->exitCode);
   if( pos != exitCodeMap.end() )
   {
      return pos->second;
   }
   else
   {
      return exitCodeMap[NSCommands::Exit_Unspecified_Error];
   }

}

void ACS_CS_Command_Exitcodes::setExitCode(NSCommands::Exit_Code exitCodeToSet)
{
   this->exitCode = exitCodeToSet;
}
