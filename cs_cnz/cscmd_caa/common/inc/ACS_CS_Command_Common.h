#ifndef ACS_CS_Command_Common_h
#define ACS_CS_Command_Common_h

namespace NSCommands
{
   // NOTE: if you add/delete an entry here, make sure to make the
   //    corresponding change in file ACS_CS_Command_Exitcodes.cpp !!!

   //Command Exit Codes
   enum Exit_Code
   {
      Exit_Success                         = 0,
      Exit_Error_Executing                 = 1,
      Exit_Incorrect_Usage                 = 2,
      Exit_NoReply_Cs                      =  7,
      Exit_Hardware_NotPresent             =  14,
      Exit_Reserved_Occupied_Name          = 20,
      Exit_Incorrect_Alias                 = 32,
      Exit_Incorrect_APZ_System            = 33,
      Exit_Illegal_CP_Group_Name           = 34,
      Exit_Duplicated_CP_Group_Name        = 35,
      Exit_CP_Group_Not_Defined            = 36,
      Exit_Predefined_Group_Not_Removable  = 37,
      Exit_Illegal_Service_Name            = 38,
      Exit_Duplicated_Function_Definition  = 39,
      Exit_Function_Not_Defined            = 40,
      Exit_Illegal_NEID                    = 41,
      Exit_Illegal_Domain_Name             = 42,
      Exit_Incorrect_CP_Type               = 43,
      Exit_Incorrect_ClusterMode           = 44,
      Exit_Incorrect_Profile               = 45,
      Exit_Missing_File                    = 46,
      Exit_ClusterOpMode_Switch_In_Progress = 47,
      Exit_Waiting_For_Dependent_Services  = 48,
      Exit_Profile_Switch_In_Progress      = 49,

	
      Exit_FuncBoard_NotAllowed		   = 112,
      Exit_AP_Not_Defined                  = 113,
      Exit_114                             = 114,
      Exit_Illegal_Command                 = 115,
      Exit_Illegal_Option                  = 116,
      Exit_Unable_Connect_Server           = 117,
      Exit_CP_Not_Defined                  = 118,
      Exit_Connection_Failure              = 119,

      Exit_Unspecified_Error               = 999,
   };

   //Supported commands
   enum Command
   {
      CPLS      = 0,
      CPCH      = 1,
      CPGLS     = 2,
      CPGDEF    = 3,
      CPGRM     = 4,
      NELS      = 5,
      NECH      = 6,
      FDLS      = 7,
      FDDEF     = 8,
      FDRM      = 9,
      APZVLANLS = 10,
      DEFAULT   = 65535
   };
}  //End of namespace

#endif
