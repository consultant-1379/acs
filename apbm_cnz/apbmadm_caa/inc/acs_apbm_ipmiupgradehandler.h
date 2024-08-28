/*
 * ipmiupghandler.h
 *
 *  Created on: May 21, 2012
 *      Author: xgiufer
 */

#ifndef IPMIUPGHANDLER_H_
#define IPMIUPGHANDLER_H_

#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"
#include <acs_apbm_ipmiapi_impl.h>

#define   IPMI_FW_UPG           0
#define   IPMI_FW_FB            1
#define   IPMI_FW_NOT_EX        2
#define   IPMI_FW_READ_RUNNING  3
#define   IPMI_FW_RUNNING       0x0F

#define 	ACTIVE_FLASH_MODE 4
#include <string>
using namespace std;
/* gep1/gep2 */
//static int IPMI_UPG_START = 0xF000;
//static int SUPPORT_NODEBUSY = 1;

/* assume GEP1 HW, change my mind in runtime if wrong */
static const int NB_MAX_RETRIES = 20;	// Node Busy signal from IMC
static const int NB_WAIT_TIME = 50000;	// 50msec time gap between consecutive requests incase of NodeBusy

static const int IPMI_UPG_LENGTH = 0x32000;
static const int MAX_NO_OF_RESEND = 5;

/* hide the messy parts in macros and make it even more difficult to read;-) */

#define GEP_TYPE_HDR_CHK(address, len) (pip_protocol_version <= PIP_PROTO_1_1 ? \
                                        ( IPMI_UPG_LENGTH >= IPMI_UPG_START ) && (( address + ( len - 10 ) / 2 ) <= IPMI_UPG_START + IPMI_UPG_LENGTH ) : \
                                        ( address + ( len - 10 ) / 2 ) <= IPMI_UPG_START + IPMI_UPG_LENGTH )

#define GEP_TYPE_REC5_CHECK(val) (pip_protocol_version <= PIP_PROTO_1_1 ? val == '0' : val == '3')


enum firmware_restart_versions {
	FW_RESTART_EX = 0x00,
	FW_RESTART_FB = 0x01,
	FW_RESTART_NOT_EX = 0x02,
	FW_RESTART_RUNNING = 0x0F,
};

enum pip_protocol_versions {
	PIP_PROTO_1_1 = 0x0101,
	PIP_PROTO_1_2 = 0x0102,
};

#define DEFAULT_IPMI_DEVICE "/dev/eri_ipmi"
#define DEFAULT_EXT_IPMI_DEVICE "/dev/ttyUSB0"


namespace ipmi_upgrade_handler_ns {

	enum internal_error_code {
		ERR_NO_ERROR = 													0,
		ERR_IN_EXECUTION = 											-1,
		ERR_CS_CREATE_HWC_INSTANCE = 						-2,
		ERR_CS_CREATE_BOARD_SEARCH_INSTANCE = 	-3,
		ERR_CS_GET_BOARD_ID = 									-4,
		ERR_CS_NO_SWITCH_BOARDS = -5,
		ERR_CS_NO_APUB_BOARDS = -6,
		ERR_CS_GET_SLOT = -7 ,
		ERR_CS_GET_IP_ETH = -8,
		ERR_CS_GET_MAGAZINE = -9,
		ERR_LOCAL_MAGAZINE_NUMBER_EXEPECTED = -10,
		ERR_GETTING_NODE_ARCHTECTURE = -11,
		ERR_NO_ECHO= -12,
		ERR_IPMIAPI_NOT_EXIST = -13,
		ERR_IPMIAPI_NOT_INIZIALIZED = -14,
		ERR_IPMIAPI_ALREADY_INIZIALIZED = -15,
		ERR_INVALID_COMMUNICATION_MODE = -16,
		ERR_INVALID_FILENAME = -17,
		ERR_IPMI_RESTART = -18,
		ERR_GET_IPMI_FIRMWARE  = -19,
		ERR_GET_PIP_PROTOCOL = -20,
		ERR_GET_HARDWARE_VERSION = -21,
		ERR_S0_UPGRADE_FIRMWARE = -22,
		ERR_S1_UPGRADE_FIRMWARE = -23,
		ERR_SET_ACTIVE_MODE = -24,
		ERR_IPMI_ERASE_FIRMWARE = -25

	};
	enum ipmi_upgrade_handler_configuration {
		NOT_CBA_ARCHITECTURE = 0,
		CBA_ARCHITECTURE = 1
	};

	enum communication_mode {
		COMMUNICATION_MODE_LOCAL_COM2_PORT =  0,
		COMMUNICATION_MODE_EXTERN_COM3_PORT = 1
	};

}

class acs_apbm_ipmiupgradehandler {

public:
	/* CONSTRUCTOR */
	acs_apbm_ipmiupgradehandler();
	/* DESTRUCTOR */
	~acs_apbm_ipmiupgradehandler();

	/* PUBLIC METHODS */
	int execute();

	int initialize(const char * com_port, const char * mot);

	int close();

	static inline const char * get_local_ipmi_device()
	{
		const char * local_ipmi_device = DEFAULT_IPMI_DEVICE;
		return local_ipmi_device;
	}
	static inline const char * get_extern_ipmi_device()
	{
		const char * extern_ipmi_device = DEFAULT_EXT_IPMI_DEVICE;
		return extern_ipmi_device;
	}

  int get_communication_mode();

  int get_ipmi_hw_version(unsigned char & device_rev);

	int get_ipmi_fw_running();

	int get_pip_protocol_version();

	int set_flash_mode_active();

	int fw_erase_finished(unsigned char ch_state);

	int firmware_upgrade(char * motorola_file);

/* firmware_upgrade_hpm function supports the .hpm format of IPMI FW on GEP7 */
	int firmware_upgrade_hpm(std::string hpm_file);
	
	int firmware_upgrade_bin(std::string bin_file);

		/* PRIVATE METHODS*/
private:

	int init_ipmi_device(const char * dev_name);

	int read_one_S19_line(FILE * const pFile, int *const len,
			                       unsigned char *const out, long *const addr,
			                       int *const startData, int doJustCheck);
	int open_file_for_reading(FILE ** const ppFile, const char *const fileName);

	/*void inline check_length(int len, int right_len)	{
		if (len != right_len)	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ERROR: Incorrect length in reply\n");*/

	int restart_ipmi_fw(int firmware_version);

private:
	char _com_port[32];
	char _filename[512];
	acs_apbm_ipmiapi_impl * _ipmi_api;

};

#endif /* IPMIUPGHANDLER_H_ */
