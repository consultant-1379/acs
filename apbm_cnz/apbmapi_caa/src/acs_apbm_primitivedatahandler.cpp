/*
 * acs_apbm_primitivedatahandler.cpp
 *
 *  Created on: Sep 20, 2011
 *      Author: xgiufer
 */

#include "acs_apbm_primitivedatahandler.h"

//number of parameters of primitives
const uint8_t ACS_APBM_PCP_PRIMITIVE_FIELDS_COUNTS[acs_apbm::PCP_SUPPORTED_VERSION_COUNT][acs_apbm::PCP_PRIMITIVES_RANGE_COUNT] = {
	{ // Version 1
		 //PRIM ID:	0  1  2  3  4  5  6  7  8   9 10  11  12  13  14  15  16 17 18 19 20 21 22 23 24 25 26 27 28  29 30 31 32 33 34 35 36
					3, 2, 3, 2, 5, 7, 8, 4, 8, 10, 2,  2,  2,  2,  5,  2,  4, 2, 4, 3, 4, 3, 4, 3, 4, 4, 3, 4, 5, 6, 3, 4, 6, 2, 4, 7, 3
	}
};

const uint16_t ACS_APBM_PCP_PRIMITIVE_FIELDS_SIZES[acs_apbm::PCP_SUPPORTED_VERSION_COUNT][acs_apbm::PCP_PRIMITIVES_RANGE_COUNT][acs_apbm::PCP_FIELD_COUNT_MAX] = {
	{ //Version 1
		//**DEFAULT*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},
		/*PRIM  0*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_SLOT_BITMAP},	//SUBSCRIBE
		/*PRIM  1*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},	//UNSUBSCRIBE
		/*PRIM  2*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_NOTIFY_NUMBER},	//NOTIFY
		/*PRIM  3*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},	//GETTRAP REQUEST
		/*PRIM  4*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_MAG_PLUG_NUMBER,acs_apbm::PCP_FIELD_SIZE_SLOT_POS, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG},  //GET SEL TRAP
		/*PRIM  5*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_MAG_PLUG_NUMBER,acs_apbm::PCP_FIELD_SIZE_SLOT_POS, acs_apbm::PCP_FIELD_SIZE_HW_BOARD_PRESENCE, acs_apbm::PCP_FIELD_SIZE_BUS_TYPE,acs_apbm::PCP_FIELD_SIZE_TRAP_MSG},  //GET BOARD PRESENCE TRAP
		/*PRIM  6*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_MAG_PLUG_NUMBER,acs_apbm::PCP_FIELD_SIZE_SLOT_POS, acs_apbm::PCP_FIELD_SIZE_SENSOR_TYPE, acs_apbm::PCP_FIELD_SIZE_SENSOR_ID, acs_apbm::PCP_FIELD_SIZE_SENSOR_TYPE_CODE, acs_apbm::PCP_FIELD_SIZE_TRAP_MSG},   //GET SENSOR STATE CHANGE TRAP
		/*PRIM  7*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_ARCHITECTURE_TYPE,acs_apbm::PCP_FIELD_SIZE_MAGAZINE},	//HWM CMD REQUEST
		/*PRIM  8*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE,acs_apbm::PCP_FIELD_SIZE_SLOT_POS,acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS,acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS,acs_apbm::PCP_FIELD_SIZE_MASTER_STATUS,acs_apbm::PCP_FIELD_SIZE_NEIGHBOUR_STATUS},	//SCB DATA REPLAY
		/*PRIM  9*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE,acs_apbm::PCP_FIELD_SIZE_MAGAZINE,acs_apbm::PCP_FIELD_SIZE_SLOT_POS,acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS,acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS,acs_apbm::PCP_FIELD_SIZE_MASTER_STATUS,acs_apbm::PCP_FIELD_SIZE_NEIGHBOUR_STATUS,acs_apbm::PCP_FIELD_SIZE_FBN},	//SCX DATA REPLAY
		/*PRIM  10*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},   //TODO
		/*PRIM  11*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},   //TODO
		/*PRIM  12*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},   //TODO
		/*PRIM  13*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},   //GET BIOS VERSION REQUEST
		/*PRIM  14*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE,acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_NAME,acs_apbm::PCP_FIELD_SIZE_BIOS_PRODUCT_VERSION},   //GET BIOS VERSION REPLAY
		/*PRIM  15*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},   //GET DISK STATUS REQUEST
		/*PRIM  16*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE,acs_apbm::PCP_FIELD_SIZE_DISK_CONNECTED},   //GETT DISK STATUS REPLAY
		/*PRIM  17*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},   //GET OWN SLOT REQUEST
		/*PRIM  18*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE,acs_apbm::PCP_FIELD_SIZE_SLOT_POS},   //GET OWN SLOT REPLAY
		/*PRIM  19*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_BOARD_NAME},   //BOARD PRESENCE REQUEST
		/*PRIM  20*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE,acs_apbm::PCP_FIELD_SIZE_BOARD_PRESENCE},   // BOARD PRESENCE REPLAY
		/*PRIM  21*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_BOARD_NAME},   //BOARD LOCATION REQUEST
		/*PRIM  22*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE, acs_apbm::PCP_FIELD_SIZE_AP_SYS_NO},   // BOARD LOCATION REPLAY
		/*PRIM  23*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_BOARD_NAME},   //BOARD STATUS REQUEST
		/*PRIM  24*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE, acs_apbm::PCP_FIELD_SIZE_BOARD_STATUS},   // BOARD STATUS REPLAY
		/*PRIM  25*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_BOARD_NAME, acs_apbm::PCP_FIELD_SIZE_BOARD_STATUS},   //BOARD SET STATUS REQUEST
		/*PRIM  26*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE},   // BOARD SET STATUS REPLAY
		/*PRIM  27*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_MAG_PLUG_NUMBER, acs_apbm::PCP_FIELD_SIZE_SLOT_POS},   //IPMIFW STATUS REQUEST
		/*PRIM  28*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE, acs_apbm::PCP_FIELD_SIZE_IPMIFW_STATUS, acs_apbm::PCP_FIELD_SIZE_IPMIFW_DATETIME},   //IPMIFW STATUS REPLAY
		/*PRIM  29*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_MAG_PLUG_NUMBER, acs_apbm::PCP_FIELD_SIZE_SLOT_POS, acs_apbm::PCP_FIELD_SIZE_IPMIFW_COMPORT, acs_apbm::PCP_FIELD_SIZE_IPMIFW_PKGNAME},   //IPMIFW UPGRADE REQUEST
		/*PRIM  30*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE},   //IPMIFW UPGRADE REPLAY
		/*PRIM  31*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_MAG_PLUG_NUMBER, acs_apbm::PCP_FIELD_SIZE_SLOT_POS},   //IPMIFW DATA REQUEST
		/*PRIM  32*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE, acs_apbm::PCP_FIELD_SIZE_IPMIFW_TYPE, acs_apbm::PCP_FIELD_SIZE_IPMIFW_PRODUCT_NUMBER, acs_apbm::PCP_FIELD_SIZE_IPMIFW_REVISION},   //IPMIFW DATA REPLAY
		/*PRIM  33*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION},   //GET DISK STATUS REQUEST
		/*PRIM  34*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE,acs_apbm::PCP_FIELD_SIZE_DISK_CONNECTED},   //GETT DISK STATUS REPLAY
		/*PRIM  35*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE,acs_apbm::PCP_FIELD_SIZE_NIC_STATUS, acs_apbm::PCP_FIELD_SIZE_NIC_NAME, acs_apbm::PCP_FIELD_SIZE_NIC_IPV4_ADDRESS, acs_apbm::PCP_FIELD_SIZE_NIC_MAC_ADDRESS},   //SET NIC INFO REQUEST
		/*PRIM  36*/{acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE},      //SET NIC INFO REPLY
		///*PRIM 37*/ {acs_apbm::PCP_FIELD_SIZE_PRIMITIVE_ID,acs_apbm::PCP_FIELD_SIZE_VERSION,acs_apbm::PCP_FIELD_SIZE_PROT_ERROR_CODE,acs_apbm::PCP_FIELD_SIZE_MAGAZINE,acs_apbm::PCP_FIELD_SIZE_SLOT_POS,acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS,acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS,acs_apbm::PCP_FIELD_SIZE_MASTER_STATUS,acs_apbm::PCP_FIELD_SIZE_NEIGHBOUR_STATUS,acs_apbm::PCP_FIELD_SIZE_FBN}  //GET SMX BOARD STATUS
	}
};


const copier<void> _null_copier_object;
namespace {
	const copier<byte_type> _byte_copier;
	const copier<byte_buffer_type> _byte_buffer_type_copier;
	const copier<string_type> _string_copier;
	const copier<two_bytes_host_order_type> _two_bytes_host_order_copier;
	const copier<two_bytes_net_order_type> _two_bytes_net_order_copier;
	const copier<four_bytes_host_order_type> _four_bytes_host_order_copier;
	const copier<four_bytes_net_order_type> _four_bytes_net_order_copier;
	const copier<eight_bytes_little_endian_order_type> _eight_bytes_little_endian_order_copier;
}


const copy_caller ACS_APBM_PCP_PRIMITIVE_FIELDS_COPIER_CALLERS[acs_apbm::PCP_SUPPORTED_VERSION_COUNT][acs_apbm::PCP_PRIMITIVES_RANGE_COUNT][acs_apbm::PCP_FIELD_COUNT_MAX] = {
	{ //Version 1
		//*DEFAULT 0*/ {copy_caller(),copy_caller()},
		/*PRIM 0*/  {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier)}, 							//SUBSCRIBE
		/*PRIM 1*/  {copy_caller(),copy_caller()},																		//UNSUBSCRIBE
		/*PRIM 2*/  {copy_caller(),copy_caller(),copy_caller(_two_bytes_net_order_copier)},												//NOTIFY
		/*PRIM 3*/  {copy_caller(),copy_caller()},																		//GET TRAP REQUEST
		/*PRIM 4*/  {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_byte_buffer_type_copier)}, //GET SEL TRAP
		/*PRIM 5*/  {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_byte_buffer_type_copier)}, //GET BOARD PRESENCE TRAP
		/*PRIM 6*/  {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_byte_buffer_type_copier)}, //GET SENSOR STATE CHANGE
		/*PRIM 7*/  {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier)},
		/*PRIM 8*/  {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_string_copier),copy_caller(_string_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier)},
		/*PRIM 9*/  {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_string_copier),copy_caller(_string_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier)},
		/*PRIM 10*/ {copy_caller(),copy_caller()},
		/*PRIM 11*/ {copy_caller(),copy_caller()},
		/*PRIM 12*/ {copy_caller(),copy_caller()},
		/*PRIM 13*/ {copy_caller(),copy_caller()},
		/*PRIM 14*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_string_copier),copy_caller(_string_copier)},
		/*PRIM 15*/ {copy_caller(),copy_caller()},
		/*PRIM 16*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_byte_copier)},
		/*PRIM 17*/ {copy_caller(),copy_caller()},
		/*PRIM 18*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier)},
		/*PRIM 19*/ {copy_caller(),copy_caller(),copy_caller(_byte_copier)},
		/*PRIM 20*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_byte_copier)},
		/*PRIM 21*/ {copy_caller(),copy_caller(),copy_caller(_byte_copier)}, // Location request
		/*PRIM 22*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier)},
		/*PRIM 23*/ {copy_caller(),copy_caller(),copy_caller(_byte_copier)},
		/*PRIM 24*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_byte_copier)},
		/*PRIM 25*/ {copy_caller(),copy_caller(),copy_caller(_byte_copier),copy_caller(_byte_copier)},
		/*PRIM 26*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier)},
		/*PRIM 27*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier)},
		/*PRIM 28*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier), copy_caller(_byte_copier),copy_caller(_byte_buffer_type_copier)},
		/*PRIM 29*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_byte_copier), copy_caller(_string_copier)},
		/*PRIM 30*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier)},
		/*PRIM 31*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier)},
		/*PRIM 32*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier), copy_caller(_byte_copier), copy_caller(_string_copier),copy_caller(_string_copier)},
		/*PRIM 33*/ {copy_caller(),copy_caller()},
		/*PRIM 34*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_byte_copier)},
		/*PRIM 35*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier), copy_caller(_string_copier),copy_caller(_string_copier),copy_caller(_string_copier)},
		/*PRIM 36*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier)},
		///*PRIM 37*/ {copy_caller(),copy_caller(),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_string_copier),copy_caller(_string_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier),copy_caller(_four_bytes_net_order_copier)}
	}
};

