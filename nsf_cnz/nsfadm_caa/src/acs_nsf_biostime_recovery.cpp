/*=================================================================== */
   /**
   @file acs_nsf_biostime_recovery.cpp

   Class method implementation for Biostime recovery application.

   This module contains the implementation of class declared in
   the acs_nsf_biostime_recovery.h module.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.
        **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <syslog.h>
#include <string.h>
#include <ACS_APGCC_Util.H>
#include <time.h>
#include <ace/ACE.h>
#include <ace/Capabilities.h>
#include <acs_nsf_biostime_recovery.h>
// BEGIN  TR:H053106
std::string nsfTimeStampFile(ACS_NSF_TIMESTAMP_FILEPATH);
// END  TR:H053106
std::string nsf_restore_flag_file;
const std::string nsf_restore_flag_file_a(ACS_NSF_RESTORE_FLAG_FILEPATH_NODEA);
const std::string nsf_restore_flag_file_b(ACS_NSF_RESTORE_FLAG_FILEPATH_NODEB);
/*=====================================================================
		ROUTINE DECLARATION SECTION
  =====================================================================*/

/*===================================================================
   ROUTINE: NSF_Biostime_Recovery::NSF_Biostime_Recovery()
=================================================================== */

NSF_Biostime_Recovery::NSF_Biostime_Recovery()
{

/*=====================================================================
	Default Initialization
  =====================================================================*/

    savedTime = 0;
    systemTime = 0;
    fd = -1;
}


/*=====================================================================
   ROUTINE: ~NSF_Biostime_Recovery 
  =====================================================================*/

NSF_Biostime_Recovery::~NSF_Biostime_Recovery()
{
}

/*=====================================================================
   ROUTINE: CheckForTimestampFile  
  =====================================================================*/

int NSF_Biostime_Recovery::CheckForTimestampFile()
{
    /*
     * First, check if the NSF restore flag is present under PSO clear area.
     * If it is not present, this command is executed after a restore operation
     * and the NSF timestamp file will be not present.
     * So, to avoid useless errors, log the information and exists.
     */

	struct stat stat_info;
	/*
	if (ACE_OS::stat(nsf_restore_flag_file.c_str(), &stat_info)) {
		// NSF restore flag doesn't exist, nothing to do!
		const std::string msg("INFO: This command has been executed after a restore, nothing to do!");
		AddToLogFile(msg);
        CloseLogFile();
		return 1;
	}*/
	//Moved this part to the main function

    ACE_INT32 file_status;
    // BEGIN  TR:H053106
    file_status = ACE_OS::stat(nsfTimeStampFile.c_str(), &stat_info);
    // END  TR:H053106
    if ( file_status == 0 )
    {
        return 0;
    }
    else
    {
	AddToLogFile("ERROR: main: NSF system time stamp file not existing\r\n");
        CloseLogFile();
        return -1;
    }
}

/*=====================================================================
   ROUTINE: GetFileTime   
  =====================================================================*/

bool NSF_Biostime_Recovery::GetFileTime()
{
	// BEGIN  TR:H053106
    FILE* fp  = ACE_OS::fopen(nsfTimeStampFile.c_str(),ACE_TEXT("r"));
    // END  TR:H053106
    if (!fp)
    {
        AddToLogFile("ERROR: main: Unable to read the NSF system time stamp file");
        CloseLogFile();
        return false;
    }
    ACE_TString fileTime;
    fileTime.set(0,0);
    ACE_INT32 ch;
    while ( (ch = ACE_OS::fgetc(fp)) != EOF && ch != ACE_TEXT('\n'))
    {
	fileTime += (ACE_TCHAR) ch;
    }
    if (ch == EOF && fileTime.length() == 0)
    {
        AddToLogFile("ERROR: main: Unable to find time in the NSF system time stamp file");
        CloseLogFile();
        ACE_OS::fclose(fp);
        return false;
    }
    else
    {
	std::string msg = "INFO: main: NSF saved system time is ";
	msg.append(fileTime.c_str());
        AddToLogFile(msg);
        ACE_OS::fclose(fp);
        savedTime = atoi(fileTime.c_str());	
        return true;
    }
}



/*=====================================================================
   ROUTINE: CheckForPrivileges    
  =====================================================================*/

bool NSF_Biostime_Recovery::CheckForPrivileges()
{
    uid_t uid;
    uid = ACE_OS::getuid();
    
    if( uid == 0)
    {
	return true;
    }
    else
    {
        AddToLogFile("ERROR: main: Insufficient privileges to adjust the system time");
        CloseLogFile();
	return false;
    }

}



/*=====================================================================
   ROUTINE: GetCurrentTime    
  =====================================================================*/

void NSF_Biostime_Recovery::GetCurrentTime()
{
    // Get current time in epoch format
	char buffer1[100];
	ACE_OS::memset(buffer1, 0, sizeof(buffer1));

    time_t sysTime;
    sysTime = time(NULL);
    systemTime = (uintmax_t)sysTime;
    ACE_OS::itoa(systemTime, buffer1, 10);
    //string message(ACE_OS::itoa(systemTime));
	std::string msg = "INFO: main: NSF current system time is ";
	msg.append(buffer1);
    AddToLogFile(msg);

}



/*=====================================================================
   ROUTINE: CheckCurrentTimeWithFileTime    
  =====================================================================*/

bool NSF_Biostime_Recovery::CheckCurrentTimeWithFileTime()
{
	if( systemTime < (savedTime - 86400))
	{
		std::string change_time = "date -s @";
		std::stringstream temp;
		temp <<savedTime;
		change_time.append(temp.str());
		::syslog(LOG_NOTICE, "Changing the system date: systemTime is '%d', savedTime is '%d'!", systemTime, savedTime);
		int result = ACE_OS::system(change_time.c_str());
		if (result != 0)
		{
			AddToLogFile("ERROR: main: Unable to set system time");
			CloseLogFile();
			return false;
		}
		::syslog(LOG_NOTICE, "Changing the hardware clock time: systemTime is '%d', savedTime is '%d'!", systemTime, savedTime);
		result = ACE_OS::system("hwclock --systohc --utc");
		if (result != 0)
		{
			AddToLogFile("ERROR: main: Unable to set system hardware clock");
			CloseLogFile();
			return false;
		}
		TimeoutByHostname();
		return true;
	}
	else
	{
		AddToLogFile("INFO: main: System time is already updated. NO POWER LOSS!!");
		CloseLogFile();
		return true;
	}
}



/*=====================================================================
   ROUTINE: TimeoutByHostname    
  =====================================================================*/


void NSF_Biostime_Recovery::TimeoutByHostname( void ) 
{

  ACE_TCHAR name[32];
  time_t systime;

  gethostname(name, sizeof(name));

  std::string msg="INFO: timeoutByHostname: Hostname is:"; 
  msg.append(name);
	
  AddToLogFile(msg);
	
  ACE_INT32 NodeNum;
  NodeNum= atoi(&name[5]);
  if (NodeNum == 1)
  {
    AddToLogFile("INFO: timeoutByHostname: Hostname ends with A(1). No Sleep to do.");
  }
  else
  {
    msg="INFO: timeoutByHostname: Sleep time to wait is: ";
    std::stringstream temp;
    temp <<SLEEP_TIME;
    msg.append(temp.str());
    AddToLogFile(msg);

    systime = time(NULL);

    msg="INFO: timeoutByHostname: Time before sleeping:";
    temp.str("");
    temp << systime;
    msg.append(temp.str()); 
    AddToLogFile(msg);

    ACE_OS::sleep(SLEEP_TIME);


    systime = time(NULL);
    msg="INFO: timeoutByHostname: Time After sleeping:";
    temp.str("");
    temp << systime ;
    msg.append(temp.str()); 
    AddToLogFile(msg);
  }
}


/*=====================================================================
   ROUTINE: OpenLogFile    
  =====================================================================*/

void NSF_Biostime_Recovery::OpenLogFile()
{
    fd = ACE_OS::open(ACE_TEXT(LOG_FILE_PATH),O_CREAT | O_TRUNC | O_WRONLY );


}

// BEGIN  TR:H053106
/*===================================================================
   ROUTINE: getNode
=================================================================== */
NODE_NAME NSF_Biostime_Recovery::getNode()
{
//	AddToLogFile("Entering getNode");

#if 0
	ACS_DSD_Client myDSDClient;
	ACS_DSD_Node DSDNode;
	ACE_TCHAR curNode[32];
	int result = myDSDClient.get_local_node(DSDNode);

	if( result < 0 )
	{
		ERROR("%s", "Error occured while getting node name");
		ERROR("%s", "Exiting getNode");
		return UNDEFINED;
	}

	ACE_OS::strcpy( curNode, DSDNode.node_name);

	if(curNode[3] == 'A')
	{
		DEBUG("%s", "Node A");
		DEBUG("%s", "Exiting getNode");
		return NODE_A;
	}
	else if(curNode[3] == 'B')
	{
		DEBUG("%s", "Node B");
		DEBUG("%s", "Exiting getNode");
		return NODE_B;
	}
	else
	{
		DEBUG("%s", "Undefined Node Name");
		ERROR("%s", "Exiting getNode");
		return UNDEFINED;
	}

#endif

	FILE *fp = 0;
	char buff[512];
	int nodeId  = 0;

	/* popen creates a pipe so we can read the output
        of the program we are invoking */
	if (!(fp = popen("cat /etc/opensaf/slot_id" ,"r")))
	{
		AddToLogFile("ERROR: while reading from popen in getNode");
		return UNDEFINED;
	}

	/* read the output of route, one line at a time */
	if(fgets(buff, sizeof(buff), fp) != NULL )
	{
		nodeId = atoi(&buff[0]);
	}
	/* close the pipe */
	pclose(fp);

	if (nodeId == 1)
	{
		return NODE_A;
	}
	else if (nodeId == 2)
	{
		return NODE_B;
	}
//	AddToLogFile("Exiting getNode");
	return UNDEFINED;
}
// END  TR:H053106

NODE_NAME NSF_Biostime_Recovery::getNodeid()
{
	std::string p_local_node_id_path = "/etc/cluster/nodes/this/id";
	std::string p_remote_node_id_path = "/etc/cluster/nodes/peer/id";
	ifstream ifs;
	char node_id[16] = {0};
	ifs.open(p_local_node_id_path.c_str());

	if ( ifs.good())
		ifs.getline(node_id, 16);
	ifs.close();
	if ( strcmp(node_id,"1") == 0){ // Nodo A
		return NODE_A;
	}
	else if ( strcmp(node_id,"2") == 0){ // Node B
		return NODE_B;
	}
	else
	{
		ifs.open(p_remote_node_id_path.c_str());
		if ( ifs.good())
			ifs.getline(node_id, 16);
		ifs.close();
		if ( strcmp(node_id,"1") == 0){ // Nodo A
			return NODE_B;
		}
		else if ( strcmp(node_id,"2") == 0){ // Node B
			return NODE_A;
		}
	}
	//still node name is not fetched properly
	std::string p_local_node_hostname_path = "/etc/cluster/nodes/this/hostname";
	std::string p_remote_node_hostname_path = "/etc/cluster/nodes/peer/hostname";
	ifs.open(p_local_node_hostname_path.c_str());

	if ( ifs.good())
		ifs.getline(node_id, 16);
	ifs.close();

	if (strstr(node_id,"SC-2-1")) // Node A
		return NODE_A;
	else if (strstr(node_id,"SC-2-2"))
		return NODE_B;
	ifs.open(p_remote_node_hostname_path.c_str());
	if ( ifs.good())
		ifs.getline(node_id, 16);
	ifs.close();
	if (strstr(node_id,"SC-2-1")) // Node A
		return NODE_B;
	else if (strstr(node_id,"SC-2-2"))
		return NODE_A;

	return UNDEFINED;
}

/*=====================================================================
   ROUTINE: AddToLogFile    
  =====================================================================*/

void NSF_Biostime_Recovery::AddToLogFile(std::string message)
{
    if (fd != ACE_INVALID_HANDLE)
    {
       message.append("\r\n");
       ACE_OS::write(fd, message.c_str(), strlen(message.c_str()));
    }
}



/*=====================================================================
   ROUTINE: CloseLogFile
  =====================================================================*/


void NSF_Biostime_Recovery::CloseLogFile()
{
    ACE_OS::close(fd);
}

/*=====================================================================
   ROUTINE: Main routine
  =====================================================================*/
ACE_INT32 ACE_TMAIN()
{
	NSF_Biostime_Recovery biosTime;

	biosTime.OpenLogFile();

	// BEGIN  TR:H053106
	if(biosTime.getNode() == NODE_A)
	{
		nsfTimeStampFile.append("NODEA_");
		nsfTimeStampFile.append(ACS_NSF_TIMESTAMP_FILENAME);
		nsf_restore_flag_file = nsf_restore_flag_file_a;
	}
	else if(biosTime.getNode() == NODE_B)
	{
		nsfTimeStampFile.append("NODEB_");
		nsfTimeStampFile.append(ACS_NSF_TIMESTAMP_FILENAME);
		nsf_restore_flag_file = nsf_restore_flag_file_b;
	}
	else
	{
		if(biosTime.getNodeid() == NODE_A)
		{
			nsfTimeStampFile.append("NODEA_");
			nsfTimeStampFile.append(ACS_NSF_TIMESTAMP_FILENAME);
			nsf_restore_flag_file = nsf_restore_flag_file_a;
		}
		else if(biosTime.getNodeid() == NODE_B)
		{
			nsfTimeStampFile.append("NODEB_");
			nsfTimeStampFile.append(ACS_NSF_TIMESTAMP_FILENAME);
			nsf_restore_flag_file = nsf_restore_flag_file_b;
		}
		else{

			biosTime.AddToLogFile("ERROR: NODE name received by nsf_biostime_recovery is UNDEFINED");
			return -1;
		}
	}

	//Check if it is a VM environment or if a restore is performed
	struct stat stat_info;// BEGIN OF HX95348
	if (ACE_OS::stat(nsf_restore_flag_file.c_str(), &stat_info)) {
		// NSF restore flag doesn't exist, nothing to do!
		const std::string msg("INFO: This command has been executed after a restore or it is a VM environment, nothing to do!");
		biosTime.AddToLogFile(msg);
		return 0;
	}//END OF HX95348

	std::string msg= "INFO: main: Reading saved system time from file ";
	msg.append(nsfTimeStampFile.c_str());
	// END  TR:H053106
	biosTime.AddToLogFile(msg);

	// Check if the NSF timestamp file exists
	int call_result = 0;
	if ((call_result = biosTime.CheckForTimestampFile()) < 0) {
		return -1;
	}
	/*	else if (call_result > 0) {
		// The BIOS Time recovery has been called after a restore, nothing to do!
		return 0;
	}*/

	// Get file time
	if (!biosTime.GetFileTime())
	{
		return -1;
	}


	// Check for privileges to adjust the system time
	if (!biosTime.CheckForPrivileges())
	{
		return -1;
	}

	// Get current time in epoch seconds
	biosTime.GetCurrentTime();

	// Check the current time with the file time
	if (!biosTime.CheckCurrentTimeWithFileTime())
	{
		return -1;
	}


	return 0;
}



