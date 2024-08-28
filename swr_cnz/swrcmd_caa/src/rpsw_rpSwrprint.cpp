#include <iostream>
#include <thread>
#include <iomanip>
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAX_NO_RPVM 50
#define SIZE 512


static char rpVMSwVersion[MAX_NO_RPVM][SIZE] = {{0}};

enum ErrorCodes
{
  EXECUTED = 0,
  EXECUTING_ERROR = 1,
  INCORRECT_USAGE=2,
  INTERNAL_ERROR=18,
  ILLEGAL_COMMAND=115
};

/* method declarations */
void printRpVmSwVersion(void);
static char* getSwVersion(char* rpstring);
static char* getparmtoolOutStr(char* buffString,const char* parameter);
void fetchRpVmSwVersionInfo(int rpvmno);
bool isVirtualBscEnvironment(int &retvalue);
void PrintUsage(void);
bool checkRSAKeyFileAccess(int &retVal);


//******************************************************************************
// printRpVmSwVersion
// Print the RPVM Software Versions
//******************************************************************************
void printRpVmSwVersion(void)
{
  std::cout<< setiosflags(std::ios::left) << "RPVM" << std::setw(6) << "" << "SWVERSION" << std::endl;
  for(int i=0;i<MAX_NO_RPVM;i++){
    if(strcmp(rpVMSwVersion[i],"NOT AVAILABLE") == 0) continue;
    std::cout << "RPVM"<< (i+1) << (((i+1) < 10) ? std::setw(5) : std::setw(4)) << "" << rpVMSwVersion[i] ;
  }
  return;

}

//******************************************************************************
// getSwVersion
// Extract RPVM Software Version string
//******************************************************************************
static char* getSwVersion(char* rpstring)
{
   char* swver = NULL;

   swver = strstr((char*)rpstring, "SwVersion=");

   if (swver != NULL)
   {
      swver+= strlen("SwVersion=");
      if((strncmp(swver,"RPVM-", 5)) == 0)
        swver+= strlen("RPVM-");
   }

   return swver;
}

//******************************************************************************
// getparamtoolOutStr
// Extract paramtool output string
// Argument: buffString   -paramtools output string buffer is argument
// Argument: paramenter   -parameter string to extract corresponding parmtool output
// return char * to parmtool output string corresponding to paramenter value passed 
//******************************************************************************
static char* getparmtoolOutStr(char* buffString,const char* parameter)
{
   char* resultStr = NULL;

   resultStr= strstr((char*)buffString, parameter);

   if (resultStr!= NULL)
   {
      resultStr += strlen(parameter);
   }

   return resultStr;
}

//******************************************************************************
// fetchRpVmSwVersionInfo
// Argument: rpvmno   --Pass RPVM number as argument
// Fetch RPVM Software Version string from specified RPVM number
//******************************************************************************
void fetchRpVmSwVersionInfo(int rpvmno)
{
  const size_t RPNOMAX=3; //considering Maximum RPVM number will not cross two digit 
  char buf[SIZE] ={0};
  char cmdstr[SIZE] ={0};
  char strpvmno[RPNOMAX] ={0};
  /******************************SSH option description****************************************
   -q : Suppresses all errors and warnings
   -4 : Allows IPv4 addresses only (makes connection faster)
   -o Batchmode=yes : user interaction such as password prompts and host key confirmation
                     requests will be disabled
   -o ServerAliveInterval=4 : Sets timeout interval in seconds before sending msg
   -o ServerAliveCountMax=2 : Sets the number of server alive messages sent 
   -o ConnectTimeout=3 : Stpecifies the timeout (in seconds) used when connecting to the SSH server
   -o StrictHostKeyChecking=no : ssh will automatically add new hostkeys to the user known
                                hosts files and allow connections to hosts with changed
                                hostkeys to proceed
   -i <public key> : used  to speficy public authentication key to connect with server                                                                                    
   *********************************************************************************************/
  
  const char *sshcmd = "/usr/bin/ssh -q -4 -o BatchMode=yes -o ServerAliveInterval=4 -o ServerAliveCountMax=2 -o ConnectTimeout=3 -o StrictHostKeyChecking=no -i /var/home/ts_users/.ssh/id_rsa ts_rp@rpvm";
  const char *catcmd = " cat /boot/sw_inventory.txt";
  size_t bytes;
  char* rpSwVersion= NULL;
  strcpy(rpVMSwVersion[rpvmno -1],"NOT AVAILABLE");

  //Prepare command string to be execute
  strcpy(cmdstr,sshcmd);
  sprintf(strpvmno, "%d", rpvmno);
  strcat(cmdstr,strpvmno);
  strcat(cmdstr,catcmd);
  
  // open pipe to execute command string and read RPVM Swversion
  FILE *fp=popen(cmdstr, "r");
  if(fp){
    while((bytes=fread(buf, sizeof(char), SIZE, fp))){
       rpSwVersion=getSwVersion(buf);
       if(rpSwVersion != NULL){
          char *version = strtok(rpSwVersion," /n");
          strcpy(rpVMSwVersion[rpvmno -1],version); //store SWVersion in rpVMSwVersion array element
          break;
       }
     }
     pclose(fp);
   }
}

//******************************************************************************
// isVirtualBscEnvironment
// Argument: retvalue  --command execution retun value as reference argument
// check if its a vBSC environment or not
// return: bool value (true or false)
//******************************************************************************

bool isVirtualBscEnvironment(int &retvalue)
{
  char buf[SIZE] ={0};
  size_t bytes={0};
  bool isVirtual=false, isBSC=false;
  retvalue=EXECUTED;
  bool isvBSC=false;
  const char *cmdshelfarchstr =  "/opt/ap/apos/bin/parmtool/parmtool get --item-list shelf_architecture,apt_type 2> /dev/null";
  FILE *fp=popen(cmdshelfarchstr, "r");
  if(fp){
    if((bytes=fread(buf, sizeof(char), SIZE, fp))){        
      char* shelf_arch = getparmtoolOutStr(buf, "shelf_architecture=");
      if(shelf_arch != NULL)
      {
        if (strncmp(shelf_arch,"VIRTUALIZED",11) !=0)
        {
           retvalue=ILLEGAL_COMMAND;
        }
        else {
           isVirtual= true; //virtual env
           char* apttype = getparmtoolOutStr(buf, "apt_type=");
           if (apttype != NULL){
              
              if (strncmp(apttype,"BSC",3) !=0)
              {
                 retvalue=ILLEGAL_COMMAND;
              }
              else {
                 isBSC=true;
              }
            }
            else{
                 retvalue=EXECUTING_ERROR;
            }
       }
     }
     else{
       retvalue=INTERNAL_ERROR;
     }
   }
   else{
     retvalue=INTERNAL_ERROR;
   }
   pclose(fp);
 }
 else {
   retvalue=EXECUTING_ERROR;
 }
 if(isVirtual && isBSC)
 {
    isvBSC=true;
 }
  return isvBSC;
}

//******************************************************************************
// PrintUsage
// Print the usage message to output stream
//******************************************************************************
void PrintUsage(void)
{
  std::cout<< "Incorrect usage" << std::endl;
  std::cout<< "Usage: rpswprint" << std::endl;
} // End of PrintUsage


//******************************************************************************
// checkRSAKeyFileAccess
// check RSA key access permissions which will be used for SSH
// Argument retVal: set the error code in case of failure
// return : bool (true/false): rsa key exists or not
//******************************************************************************

bool checkRSAKeyFileAccess(int &retVal)
{
   struct stat stats ;
   memset(&stats,0,sizeof(stats));
   bool ispermited =false;
   retVal=INTERNAL_ERROR;
   if(stat("/var/home/ts_users/.ssh/id_rsa",&stats) == 0)
   {
     if ((stats.st_mode & S_IRUSR) && (stats.st_mode & S_IWUSR)){
       ispermited =true;
       retVal=EXECUTED;
     }
   }
   return ispermited;
}   
    
//Main method

int main(int argc, char** /*argv[]*/)
{

  if (argc > 1) //no option supported
  {
      PrintUsage();
      return INCORRECT_USAGE;
  }
  
  int retval=EXECUTED;

  if(isVirtualBscEnvironment(retval))
  {
     if(checkRSAKeyFileAccess(retval))
     {
        std::thread * threadpointer = new std::thread[MAX_NO_RPVM];
        for(int i=0;i<MAX_NO_RPVM;i++)
        {
           threadpointer[i] = std::thread(fetchRpVmSwVersionInfo, i+1); //spawn one thread for each RPVM
        }
        for (int i = 0; i < MAX_NO_RPVM; i++)
        {
          // Join will block our main thread, and so the program won't exit until
          // all thread completed.
          threadpointer[i].join();
        }
        delete [] threadpointer;
        
        // print RP Versions
        printRpVmSwVersion();
        
        retval= EXECUTED;
    }
    else {
        std::cout<< "Internal error: file access error"  << std::endl;
        return INTERNAL_ERROR; //18
    }
  }
  if(retval !=EXECUTED)
  {
    switch(retval)
     {
       case ILLEGAL_COMMAND:                                      //115
            std::cout<< "Illegal command in this system configuration" << std::endl;
            break;
       case INCORRECT_USAGE:                                      //2
            PrintUsage();
            break;
       case INTERNAL_ERROR:                                       //18
            std::cout<< "Internal error: unable to check node architecture"  << std::endl;  
            break;
       case EXECUTING_ERROR:                                       //1
       default:
            std::cout << "Error when executing (general fault)" << std::endl;
            break;
     }

  }
  return retval;
}
