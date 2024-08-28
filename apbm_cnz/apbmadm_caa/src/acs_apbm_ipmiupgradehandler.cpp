/*
 * ipmiupghandler.cpp
 *
 *  Created on: May 21, 2012
 *      Author: xgiufer
 */

#include <new>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <acs_apbm_api.h>
#include "acs_apbm_ipmiupgradehandler.h"
//using namespace std;
#define DEBUG_MODE
#define MAX_ERASE_RETRY 3


static int initialized = 0;
	/* CONSTRUCTOR */

acs_apbm_ipmiupgradehandler::acs_apbm_ipmiupgradehandler(): _com_port(),_filename(), _ipmi_api(0)
{
	strncpy(_com_port, DEFAULT_IPMI_DEVICE, 32);
}
  /* DESTRUCTOR */

acs_apbm_ipmiupgradehandler::~acs_apbm_ipmiupgradehandler() {
}

	/* ************** *
	 * PUBLIC METHODS *
	 * ************** */


int acs_apbm_ipmiupgradehandler::execute(){
	std::string _filenametype;
	std::string tmp;
	acs_apbm_api api;
	if (initialized)
	{
		if(api.is_gep5_gep7())
		{
			int res = 0;
			tmp = string(_filename);
			_filenametype = tmp.substr(tmp.size()-4, 4);
			if(_filenametype == ".bin")
			{
				ACS_APBM_LOG(LOG_LEVEL_INFO,"FILENAME is bin format!");
		        	if((res = firmware_upgrade_bin(tmp))!= ipmi_upgrade_handler_ns::ERR_NO_ERROR)
				{
#ifdef DEBUG_MODE
		                        ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error in firmware_upgrade. result:%d",res);
#endif
                		        return ipmi_upgrade_handler_ns::ERR_IN_EXECUTION;
                		}
			}
			else
			{
				if(_filenametype == ".hpm")
				{
					ACS_APBM_LOG(LOG_LEVEL_INFO,"FILENAME is hpm format!");
					if((res = firmware_upgrade_hpm(tmp))!= ipmi_upgrade_handler_ns::ERR_NO_ERROR)
                			{
#ifdef DEBUG_MODE
			                        ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error in firmware_upgrade. result:%d",res);
#endif
                        			return ipmi_upgrade_handler_ns::ERR_IN_EXECUTION;
                			}
				}
			}
			return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
		}
		else //for other GEP
		{
			int res = init_ipmi_device(_com_port);
			if(res != ipmi_upgrade_handler_ns::ERR_NO_ERROR){
#ifdef DEBUG_MODE
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error in init ipmi device. result:%d",res);
#endif
			return ipmi_upgrade_handler_ns::ERR_IN_EXECUTION;
                	}
			if( (res = firmware_upgrade(_filename)) != ipmi_upgrade_handler_ns::ERR_NO_ERROR){
#ifdef DEBUG_MODE
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error in firmware_upgrade. result:%d",res);
#endif
				return ipmi_upgrade_handler_ns::ERR_IN_EXECUTION;
			}
		}
		return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
	} else
		return ipmi_upgrade_handler_ns::ERR_IPMIAPI_NOT_INIZIALIZED;
}

int acs_apbm_ipmiupgradehandler::initialize(const char * com_port, const char * mot){

	if(initialized){
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Maybe upgrading on going! Refusing update!");
		return ipmi_upgrade_handler_ns::ERR_IPMIAPI_ALREADY_INIZIALIZED;
	}
	if (!com_port || !*com_port)
		return ipmi_upgrade_handler_ns::ERR_INVALID_COMMUNICATION_MODE;
	strncpy(_com_port, com_port, 32);
	//if no change, default COM2 = eri_ipmi
#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"COM_MODE with port %s ",_com_port);
#endif
	if (!mot || !*mot)
		return ipmi_upgrade_handler_ns::ERR_INVALID_FILENAME;
#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"FILENAME %s ",mot);
#endif
	strncpy(_filename, mot, 512);

	initialized = 1;

	return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
}

int acs_apbm_ipmiupgradehandler::get_communication_mode(){
#ifdef DEBUG_MODE
ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Retrieve communication mode!");
	#endif
	if (strcmp(_com_port, DEFAULT_IPMI_DEVICE)==0)
  	return ipmi_upgrade_handler_ns::COMMUNICATION_MODE_LOCAL_COM2_PORT;
  else if (strcmp(_com_port, DEFAULT_EXT_IPMI_DEVICE)==0)
  	return ipmi_upgrade_handler_ns::COMMUNICATION_MODE_EXTERN_COM3_PORT;
  else return ipmi_upgrade_handler_ns::ERR_INVALID_COMMUNICATION_MODE;
}

int acs_apbm_ipmiupgradehandler::get_ipmi_hw_version(unsigned char & device_rev)
{
#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Retrieve IPMI HARDWARE VERSION from ipmiapi->get_ipmi_hw_version(..!");
#endif
	unsigned char tmp[8] = {0};
  int result = 0;
	result = _ipmi_api->get_ipmi_hardware_version(1, &device_rev);

#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Dumping: ");
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%02X ",device_rev);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"");
#endif
	if (result == 0) {
		return tmp[2];
	} else {
		return ipmi_upgrade_handler_ns::ERR_GET_HARDWARE_VERSION; //ERROR
	}

}


int acs_apbm_ipmiupgradehandler::get_ipmi_fw_running()
{
#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Retrieve IPMI FIRMWARE RUNNING  from ipmiapi->get_ipmi_fw(..!");
#endif
	unsigned char tmp[8] = {0};
  int result = 0;
	result = _ipmi_api->get_ipmi_fwinfo(tmp, IPMI_FW_READ_RUNNING);

#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Dumping: ");
	for(int i=0; i < 3; i++)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%02X ",tmp[i]);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"");
#endif

	if (result == 0) {
		return tmp[2];
	} else {
		return ipmi_upgrade_handler_ns::ERR_GET_IPMI_FIRMWARE; //ERROR
	}
}

int acs_apbm_ipmiupgradehandler::get_pip_protocol_version(){

#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Retrieve IPMI PROTOCOL VERSION from ipmiapi->get_pip_protocol_version(..!");
#endif
	unsigned char tmp[8] = {0};
  int result = 0;
	result = _ipmi_api->get_pip_protocol_version(tmp);

#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Dumping: ");
	for(int i=0; i < 3; i++)
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"%02X ",tmp[i]);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"");
#endif

	if (result == 0) {
		return (tmp[0] << 8 | tmp[1]);
	} else {
		return ipmi_upgrade_handler_ns::ERR_GET_PIP_PROTOCOL; //ERROR
	}
	return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
}

int acs_apbm_ipmiupgradehandler::set_flash_mode_active(){

#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Try to set_flash_mode_active(..! ");
#endif
  int result = 0;
	result = _ipmi_api->flash_mode_activate(4);

	if (result != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Activation ERROR result = %d", result);
		return ipmi_upgrade_handler_ns::ERR_SET_ACTIVE_MODE;
	}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Activation OK");
		return 0;

}

int acs_apbm_ipmiupgradehandler::fw_erase_finished(unsigned char ch_state){

#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Retrieve IPMI FW ERASE from ipmiapi->fw_erase_finished(..!");
#endif
  int result = 0;
	result = _ipmi_api->ipmi_firmware_erase_finished(&ch_state);

	if (result != 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Firwmare Erase not Finished!... ERROR result =%d", result);
		return ipmi_upgrade_handler_ns::ERR_IPMI_ERASE_FIRMWARE;
	}
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Firmware Erase Finished!... OK ch_state= %d",ch_state);
		return 0;
}

int acs_apbm_ipmiupgradehandler::firmware_upgrade_bin(std::string bin_file)
{
	ACS_APBM_LOG(LOG_LEVEL_INFO, "firmware_upgrade_bin");	
	std::string cmd = "eri-ipmitool fupg_bin " + bin_file;
	if(system(cmd.c_str())==0) 
	{
		ACS_APBM_LOG(LOG_LEVEL_INFO, "IPMI UPGRADE DONE FOR .bin file");
	}

	return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
}
int acs_apbm_ipmiupgradehandler::firmware_upgrade_hpm(std::string hpm_file)
{
        std::string fw_filename;
        std::string tmp;
        std::string dirPath = "cd /data/APZ/data/boot && ";
        int res = 0;
        tmp = string(hpm_file);
        fw_filename = tmp.substr(20);
        std::string cmd = dirPath + "ipmitool hpm upgrade " + fw_filename  + " activate";

        //int ret = chdir("/data/APZ/data/boot");
        //if(ret != 0)
       // {
        //       ACS_APBM_LOG(LOG_LEVEL_INFO, "Change directory to /data/APZ/data/boot failed");	
        //}
        if(res = system(cmd.c_str())==0)
        {
               ACS_APBM_LOG(LOG_LEVEL_WARN, "Upgrade command passed with exit status=%d",WEXITSTATUS(res));
        }
        else
        {
                ACS_APBM_LOG(LOG_LEVEL_WARN, "Upgrade command failed with exit status=%d",WEXITSTATUS(res));
        }
        int ret = chdir("/");
        if(ret != 0)
        {
               ACS_APBM_LOG(LOG_LEVEL_INFO, "Change directory to root failed");
        }
        return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
}


int acs_apbm_ipmiupgradehandler::firmware_upgrade(char * motorola_file)
 {
	FILE *file;
	int len;
        int  res;
        int  dataStart;
        int fw_running;
        int  readres;
        int  doJustCheck;
	long address;
	unsigned char tmp[200];

	ACS_APBM_LOG(LOG_LEVEL_INFO,"                 ********************************");
	ACS_APBM_LOG(LOG_LEVEL_INFO,"                 **** Time to Flash IPMI UPG ****");
	ACS_APBM_LOG(LOG_LEVEL_INFO,"                 ****    DO NOT RESET !!!!!  ****");
	ACS_APBM_LOG(LOG_LEVEL_INFO,"                 *** DO NOT REMOVE BOARD !!!! ***");
	ACS_APBM_LOG(LOG_LEVEL_INFO,"                 ********************************");

	/* PHASE 1 : CHECK FILE */
 /*** open file ***/
	res = open_file_for_reading(&file, motorola_file);
	if (res != 0){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Error in reading file!");
		return ipmi_upgrade_handler_ns::ERR_INVALID_FILENAME;
	}
	doJustCheck = 1;
	readres =
	read_one_S19_line(file, &len, tmp + 4, &address, &dataStart, doJustCheck);
	if (readres == 250) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Wrong file; feof failed");
		return ipmi_upgrade_handler_ns::ERR_INVALID_FILENAME;//exit(0X9);
	} else if (readres == 254) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Wrong file; Not s-record header");
		return ipmi_upgrade_handler_ns::ERR_INVALID_FILENAME;//exit(0X9);
	}
	doJustCheck = 0;
	rewind(file);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"File: '%s' opened and checked ok", motorola_file);

	/* PHASE 2 : VERIFY UPG STATE */

	/*** switch to fallback ***/
	/* check if we run UPG or not
	 */

	if ((fw_running = get_ipmi_fw_running()) == -1) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Couldn't get running firmware type");
		return ipmi_upgrade_handler_ns::ERR_GET_IPMI_FIRMWARE;
	}
sleep (1);
	//TODO - Verifica la stampa forse e il 3 elemento tmp[]
		if (/*tmp[6] != IPMI_FW_FB*/1) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Switch to FB...");

			if (restart_ipmi_fw(FW_RESTART_FB)) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR,	"Couldn't restart in fallback, bailing!");
				return ipmi_upgrade_handler_ns::ERR_IPMI_RESTART;
			}
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"done.");
		}

sleep (10);

//		// Receive IPMI FW Reset Status
//		unsigned char fwType;
//		int result = _ipmi_api->ipmi_firmware_reset_status(&fwType);
//		if (result != 0)
//		{
//			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Failed to receive IPMI FW Reset Status. Result: %d",result);
//			return result;
//		}
//		#ifdef DEBUG_MODE
//		ACS_APBM_LOG(LOG_LEVEL_DEBUG,"ipmi firmware reset status: '%u'",fwType);
//		#endif

		/* Now in fallback, get the pip protocol version - if not fallback impossible to active flash*/
		int pip_protocol_version = 0x0101;
		pip_protocol_version = get_pip_protocol_version();
		if (pip_protocol_version < 0) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Couldn't get pip protocol version! Bailing...");
			restart_ipmi_fw(FW_RESTART_EX);
			return ipmi_upgrade_handler_ns::ERR_GET_PIP_PROTOCOL;
		}

		/* setup hw specific stuff - compile time setup was GEP1 */
		/* if GEP2, do minor adjustments */
//		if (pip_protocol_version > PIP_PROTO_1_1) {
//			IPMI_UPG_START = 0x78D00000;
//			SUPPORT_NODEBUSY = 0;
//		}
//
sleep (1);
		unsigned char dev_rev;
		if (get_ipmi_hw_version(dev_rev) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR,	"Couldn't get ipmi hw version!");
			return ipmi_upgrade_handler_ns::ERR_GET_HARDWARE_VERSION;
		}
sleep (1);
		/*** set active mode ***/
		if(set_flash_mode_active() < 0){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Couldn't set flash mode active!...");
			return ipmi_upgrade_handler_ns::ERR_SET_ACTIVE_MODE;
		}

		if (dev_rev == 1){
			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"GEP2...");
			sleep(60); //GEP2
			unsigned char chstate;
			int retry = 0;
			do
			{
				if(fw_erase_finished(chstate) < 0)
				{
					retry++;
					if(retry == MAX_ERASE_RETRY){
						ACS_APBM_LOG(LOG_LEVEL_ERROR,"Couldn't fw erase finished!");
						return ipmi_upgrade_handler_ns::ERR_IPMI_ERASE_FIRMWARE;
					}
					continue;
				}
				//OK
				break;
			} while (retry != MAX_ERASE_RETRY);
		}

	/* PHASE 3 : UPGRADE and PRAY*/
			std::ifstream myfile(motorola_file);
			std::string line;
			int linenumber = 0;
			ACS_APBM_LOG(LOG_LEVEL_INFO,"Flashing started");
			//fflush(stdout);
			int result = 0;
			while (! myfile.eof() )
			{
				getline (myfile,line);

				//Verify that first character is S to avoid file ending/starting characters
				if(strcmp(line.substr(0,1).c_str(), "S") == 0)
				{
		         // The S0 record needs a special handling on GEP1
		         if (strcmp(line.substr(0,2).c_str(), "S0") == 0 && dev_rev == 0)
		         {
		        	 result = _ipmi_api->flash_s0record((const unsigned char*)line.c_str(), (const unsigned int)line.size());
		     			if (result != 0)
					   {
						   ACS_APBM_LOG(LOG_LEVEL_ERROR,"Flashing failed.");
						   return ipmi_upgrade_handler_ns::ERR_S0_UPGRADE_FIRMWARE;
					   }
		            continue; // avoid the processing of S0 record twice on GEP1
		         }

		         result = _ipmi_api->flash_srecord((const unsigned char*)line.c_str(), (const unsigned int)line.size());
		         if (result != 0)
		         {
		        	 ACS_APBM_LOG(LOG_LEVEL_ERROR,"Flashing failed.");
		        	 return ipmi_upgrade_handler_ns::ERR_S1_UPGRADE_FIRMWARE;
		         }

				}
				linenumber++;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"...send line number=%d",linenumber);
				if (linenumber % 500 == 0)
				{
					/* flashing OK, do some nice looking dots...! */
					ACS_APBM_LOG(LOG_LEVEL_DEBUG,".");
					//fflush(stdout);
				}
			}
			myfile.close();
			ACS_APBM_LOG(LOG_LEVEL_INFO,"Finished! Flashing successfully executed");
			//start ipmi in UPG mode
			if (restart_ipmi_fw(FW_RESTART_EX)) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR,	"Couldn't restart in UPG mode!");
				return ipmi_upgrade_handler_ns::ERR_IPMI_RESTART;
			}
//			//give it some time to restart
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Waiting Restart 60 sec...!");
			//sleep(600);

		   // Receive IPMI FW Reset Status
//			result = _ipmi_api->ipmi_firmware_reset_status(&fwType);
//			if (result != 0)
//			{
//				ACS_APBM_LOG(LOG_LEVEL_DEBUG,"Failed to receive IPMI FW Reset Status. Result: %d",result);
//				return result;
//			}
//#ifdef DEBUG_MODE
//			ACS_APBM_LOG(LOG_LEVEL_DEBUG,"ipmi firmware reset status: '%u'",fwType);
//#endif
//

	return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
 }


	/* *************** *
	 * PRIVATE METHODS *
	 * *************** */



int acs_apbm_ipmiupgradehandler::init_ipmi_device(const char* device_name)
{
	int result_ = ipmi_upgrade_handler_ns::ERR_NO_ERROR;
	if(!device_name || !*device_name)
		return ipmi_upgrade_handler_ns::ERR_IPMIAPI_NOT_INIZIALIZED;

	if(!_ipmi_api && !(_ipmi_api = new (std::nothrow) acs_apbm_ipmiapi_impl())){
#ifdef DEBUG_MODE
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Wrong initialization of ipmiapi object! /n");
#endif
		return ipmi_upgrade_handler_ns::ERR_IPMIAPI_NOT_EXIST;
	}
	if((result_ = _ipmi_api->ipmiapi_init(device_name)) != ipmi_upgrade_handler_ns::ERR_NO_ERROR){
#ifdef DEBUG_MODE
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "error in ipmi_api->ipmiapi_init(...! - result %d /n" , result_);
#endif
		return ipmi_upgrade_handler_ns::ERR_IPMIAPI_NOT_INIZIALIZED;
	}
	return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
}

int acs_apbm_ipmiupgradehandler::close()
{
	initialized = 0;
	if (_ipmi_api)
		delete _ipmi_api;

	return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
}

int acs_apbm_ipmiupgradehandler::read_one_S19_line(FILE * const pFile, int *const len,
			                       unsigned char *const out, long *const addr,
			                       int *const startData, int doJustCheck)
{

	int i1;
	int i2;

	if (feof(pFile)) {
		return 250;
	}

	if (fgets((char *)out, 100, pFile) == NULL) {
		return 252;
	}

	for (i1 = 0, i2 = 0; out[i1] != 0; i1++) {
		if (out[i1] >= '0') {
			out[i2++] = out[i1];
		}
	}

	out[i2] = 0;

	*len = strlen((char *)out);

	if ((*len < 10) || (out[0] != 'S')) {
		return 254;
	}

	if (doJustCheck) {
		if (out[1] != '0') {
			return 254;
		}
		return 0;
	}

	switch (out[1]) {
	case '1':

		sscanf((char *)out + 4, "%4lX", addr);
		*startData = 8;

		break;

	case '2':

		sscanf((char *)out + 4, "%6lX", addr);
		*startData = 10;

		break;

	case '3':

		sscanf((char *)out + 4, "%8lX", addr);
		*startData = 12;

		break;

	case '5':
	case '9':

		sscanf((char *)out + 4, "%4lX", addr);
		*startData = 0;

		break;

	case '7':

		sscanf((char *)out + 4, "%8lX", addr);
		*startData = 0;

		break;

	case '8':

		sscanf((char *)out + 4, "%6lX", addr);
		*startData = 0;

		break;

	default:

		*startData = 0;
		*addr = 0;

		break;
	}

	return 0;

}

int acs_apbm_ipmiupgradehandler::open_file_for_reading(FILE ** const ppFile, const char *const fileName)
{

	if (fileName == 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "No filename given!");
		return (0x2);
	}

	if (!(*ppFile = fopen(fileName, "rt"))) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "File \"%s\" does not exist!",
			fileName);
		return (0x4);
	}

	if (*ppFile != NULL) {
		fseek(*ppFile, 0, SEEK_END);
		long lSize = ftell(*ppFile);
		rewind(*ppFile);

		if (lSize == 0) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "File \"%s\" is empty!",
				fileName);
			fclose(*ppFile);
			return 0x6;
		}
		return 0;
	} else {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "File \"%s\" could not be opened!",
			fileName);
		return 0x8;
	}
}

int acs_apbm_ipmiupgradehandler::restart_ipmi_fw(int firmware_version)
{
	int ret = 0, protocol_version, fw_running;


#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"in restart_ipmi_fw -> get_pip_protocol_version");
#endif
	if ((protocol_version = get_pip_protocol_version()) == -1) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Couldn't get protocol version");
		return -1;
	}

#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_DEBUG,"in restart_ipmi_fw -> get_pip_protocol_version");
#endif

	if ((fw_running = get_ipmi_fw_running()) == -1) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Couldn't get running firmware type");
		return -1;
	}

	if (protocol_version <= PIP_PROTO_1_1 ||
	    (protocol_version <= PIP_PROTO_1_2 && fw_running == 00)) {
	    //int old_firmware = 1;
	}

//TODO - VERIFICA MUTEX SU PIPAPI
//	if (register_for_incoming_cmd(&reset_handler)) {
//		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Couldn't register for ipmi reset, bailing");
//		return -1;
//	}

// IPMI_FW_UPG           0
// IPMI_FW_FB            1
// IPMI_FW_NOT_EX        2
// IPMI_FW_READ_RUNNING  3
// IPMI_FW_RUNNING       0x0F

#ifdef DEBUG_MODE
	ACS_APBM_LOG(LOG_LEVEL_INFO,"...sending ipmi restart!");
#endif

	if( (ret = _ipmi_api->ipmi_restart((unsigned char)firmware_version) < 0))
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Couldn't restart ipmi with firmware version %d return=%d",firmware_version, ret);
		return -1;
	}

return ipmi_upgrade_handler_ns::ERR_NO_ERROR;
}
