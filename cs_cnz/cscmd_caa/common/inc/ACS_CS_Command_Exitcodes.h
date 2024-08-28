#include <map>
#include <string>
#include "ACS_CS_Command_Common.h"

class ACS_CS_Command_Exitcodes
{
 public:
   //constructors
   ACS_CS_Command_Exitcodes(NSCommands::Command command);

   //destructor
   virtual ~ACS_CS_Command_Exitcodes();

   //functions for handling exit codes and descriptions for them
   void setExitCode(NSCommands::Exit_Code exitCode);
   NSCommands::Exit_Code getExitCode();
   std::string getExitCodeDescription();

 private:
   typedef std::map<unsigned int, std::string> ExitCodeMapType;
   //function for initializing all exit codes and their descriptions
   void initializeExitCodeDescriptions(NSCommands::Command command);

   //members
   ExitCodeMapType exitCodeMap;   //map holding the exit code descriptions
   NSCommands::Exit_Code exitCode;                    //variable holding current exit code
};
