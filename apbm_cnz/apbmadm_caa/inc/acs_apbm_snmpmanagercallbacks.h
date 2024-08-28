#ifndef HEADER_GUARD_CLASS__acs_apbm_snmpmanagercallbacks
#define HEADER_GUARD_CLASS__acs_apbm_snmpmanagercallbacks acs_apbm_snmpmanagercallbacks

/** @file acs_apbm_snmpmanagercallbacks.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2011-10-27
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2011-10-27 | xnicmut      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <iostream>
#include "acs_apbm_snmpsessioncallback.h"

namespace axe_eth_shelf = acs_apbm_snmp::axe::eth::shelf;

class acs_apbm_snmploggercallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline explicit acs_apbm_snmploggercallback (const char * message_ = 0) : acs_apbm_snmpsessioncallback(), message(message_) {}

	inline acs_apbm_snmploggercallback (
			const char * board_magazine_str_,
			int32_t board_slot_,
			const char * switch_board_ip_,
			int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_), message(0) {}

	inline acs_apbm_snmploggercallback (
			const char * message_,
			const char * board_magazine_str_,
			int32_t board_slot_,
			const char * switch_board_ip_,
			int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_), message(message_) {}


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_snmpmanager Destructor
	 */
	virtual inline ~acs_apbm_snmploggercallback () {}


	//===========//
	// Functions //
	//===========//
protected:
	inline virtual int read_scbrp (netsnmp_variable_list * var_list) const { return read_scx(var_list); }
	virtual int read_scx (netsnmp_variable_list * var_list) const;


	//===========//
	// Operators //
	//===========//
public:
//	virtual int operator() (int operation, netsnmp_session * session, int request_id, netsnmp_pdu * pdu) const;


	//========//
	// Fields //
	//========//
public:
	const char * message;
};


class acs_apbm_getboarddatacallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getboarddatacallback (
			char (& product_number_) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1], // 25 ASCII characters with null end character provided by the caller
			char (& product_revision_state_) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1], // 8 ASCII characters with null end character provided by the caller
			char (& product_name_) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1], // 13 ASCII characters with null end character provided by the caller
			char (& serial_number_) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1], // 14 ASCII characters with null end character provided by the caller
			char (& manufacture_date_) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1], // 9 ASCII characters with null end character provided by the caller
			char (& vendor_) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1], // 32 ASCII characters with null end character provided by the caller
			const char * board_magazine_str_,
			int32_t board_slot_,
			const char * switch_board_ip_,
			int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_),
	  product_number(product_number_), product_revision_state(product_revision_state_),
		product_name(product_name_), serial_number(serial_number_), manufacture_date(manufacture_date_), vendor(vendor_) /*,
		board_magazine_str(board_magazine_str_), board_slot(board_slot_), switch_board_ip(switch_board_ip_), is_cba(is_cba_)*/
	{}


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getboarddatacallback () {}


	//===========//
	// Functions //
	//===========//
protected:
	//	int read_boarddata_scbrp (netsnmp_variable_list * var_list) const;
	virtual int read_scbrp (netsnmp_variable_list * var_list) const;

	//	int read_boarddata_scx (netsnmp_variable_list * var_list) const;
	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	void get_field_from_data_record (char * buffer_dest, const char * buffer_src, size_t buffer_len) const;


	//===========//
	// Operators //
	//===========//
public:
//	virtual int operator() (int operation, netsnmp_session * session, int request_id, netsnmp_pdu * pdu) const;


	//========//
	// Fields //
	//========//
public:
	char (& product_number) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NUMBER_SIZE + 1]; // 25 ASCII characters with null end character provided by the caller
	char (& product_revision_state) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_REVISION_STATE_SIZE + 1]; // 8 ASCII characters with null end character provided by the caller
	char (& product_name) [acs_apbm_snmp::SNMP_BOARD_DATA_PRODUCT_NAME_SIZE + 1]; // 13 ASCII characters with null end character provided by the caller
	char (& serial_number) [acs_apbm_snmp::SNMP_BOARD_DATA_SERIAL_NUMBER_SIZE + 1]; // 14 ASCII characters with null end character provided by the caller
	char (& manufacture_date) [acs_apbm_snmp::SNMP_BOARD_DATA_MANUFACTURE_DATE_SIZE + 1]; // 9 ASCII characters with null end character provided by the caller
	char (& vendor) [acs_apbm_snmp::SNMP_BOARD_DATA_VENDOR_SIZE + 1]; // 32 ASCII characters with null end character provided by the caller

	static const char * BOARD_DATA_FIELD_NAMES [];
};


class acs_apbm_getboardipmidatacallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getboardipmidatacallback (
			axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status_, // xshmcBoardIPMIUpgradeStatus
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_),
	  ipmi_firmware_type(0), ipmi_product_number(0),
	  ipmi_product_revision(0), ipmi_upgrade_status(&ipmi_upgrade_status_) /*,
		board_magazine_str(board_magazine_str_), board_slot(board_slot_), switch_board_ip(switch_board_ip_), is_cba(is_cba_)*/
	{}

	inline acs_apbm_getboardipmidatacallback (
			int16_t & ipmi_firmware_type_,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
			char (& ipmi_product_number_) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
			char (& ipmi_product_revision_) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_),
	  ipmi_firmware_type(&ipmi_firmware_type_), ipmi_product_number(ipmi_product_number_),
	  ipmi_product_revision(ipmi_product_revision_), ipmi_upgrade_status(0) /*,
		board_magazine_str(board_magazine_str_), board_slot(board_slot_), switch_board_ip(switch_board_ip_), is_cba(is_cba_)*/
	{}

	inline acs_apbm_getboardipmidatacallback (
			int16_t & ipmi_firmware_type_,	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
			char (& ipmi_product_number_) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1], // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
			char (& ipmi_product_revision_) [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1],	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]
			axe_eth_shelf::ipmi_upgrade_status_t & ipmi_upgrade_status_, // xshmcBoardIPMIUpgradeStatus
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_),
	  ipmi_firmware_type(&ipmi_firmware_type_), ipmi_product_number(ipmi_product_number_),
	  ipmi_product_revision(ipmi_product_revision_), ipmi_upgrade_status(&ipmi_upgrade_status_) /*,
		board_magazine_str(board_magazine_str_), board_slot(board_slot_), switch_board_ip(switch_board_ip_), is_cba(is_cba_)*/
	{}


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getboardipmidatacallback () {}


	//===========//
	// Functions //
	//===========//
protected:
	//	int read_data_scbrp (netsnmp_variable_list * var_list) const;
	virtual int read_scbrp (netsnmp_variable_list * var_list) const;

	//	int read_data_scx (netsnmp_variable_list * var_list) const;
	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_values (const oid * (& oid_names) [3], size_t (& oid_names_sizes) [3], netsnmp_variable_list * var_list) const;

	int read_ipmi_data (netsnmp_variable_list * var) const;
	int read_ipmi_upgrade (netsnmp_variable_list * var) const;
	int read_ipmi_upgrade_status (netsnmp_variable_list * var) const;


	//===========//
	// Operators //
	//===========//
public:
//	virtual int operator() (int operation, netsnmp_session * session, int request_id, netsnmp_pdu * pdu) const;


	//========//
	// Fields //
	//========//
public:
	int16_t * ipmi_firmware_type;	// xshmcBoardIPMIData[0] 	 : [1 ASCII character] 0 =UPG (Upgrade) 1 =FB  (Fallback)
	char * ipmi_product_number; // [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_NUMBER_SIZE + 1]; // xshmcBoardIPMIData[1-25]  : [25 ASCII characters]
	char * ipmi_product_revision; // [acs_apbm_snmp::SNMP_BOARD_IPMI_DATA_PRODUCT_REVISION_SIZE + 1];	// xshmcBoardIPMIData[26-33] : [8 ASCII characters]

	axe_eth_shelf::ipmi_upgrade_status_t * ipmi_upgrade_status; // xshmcBoardIPMIUpgradeStatus
};


class acs_apbm_getboardledsstatuscallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getboardledsstatuscallback (
			axe_eth_shelf::led_status_t & led_,
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_, axe_eth_shelf::led_color_t led_color_, uint16_t board_fbn_, bool is_4led_supported_=false, bool is_new_gea_supported_=false)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_, board_fbn_, is_4led_supported_, is_new_gea_supported_),
	  green_led(0), red_led(0)/*,
		board_magazine_str(board_magazine_str_), board_slot(board_slot_), switch_board_ip(switch_board_ip_), is_cba(is_cba_)*/
	{ 
          
            switch(led_color_) {
            
            case axe_eth_shelf::LED_COLOR_YELLOW:  
                mia_led = &led_; //yellow
                blue_led = NULL;    
                break;
            case axe_eth_shelf::LED_COLOR_RED:    
                red_led = &led_;
                mia_led = blue_led = NULL;
                break;
            case axe_eth_shelf::LED_COLOR_BLUE:
                if(is_4led_supported_ || is_new_gea_supported_)  { //GEP4 and blue led
                    blue_led = &led_;
                    mia_led = NULL;    
                }
                break;
            case axe_eth_shelf::LED_COLOR_GREEN:
                green_led = &led_;
                mia_led = blue_led = NULL;
                break;
            }
	}

	inline acs_apbm_getboardledsstatuscallback (
			axe_eth_shelf::led_status_t & green_led_,
			axe_eth_shelf::led_status_t & red_led_,
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_, uint16_t board_fbn_, bool is_4led_supported_=false, bool is_new_gea_supported_=false)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_, board_fbn_, is_4led_supported_, is_new_gea_supported_),
	  green_led(&green_led_), red_led(&red_led_), mia_led(0), blue_led(0)/*,
		board_magazine_str(board_magazine_str_), board_slot(board_slot_), switch_board_ip(switch_board_ip_), is_cba(is_cba_)*/
	{}

        //For 3 LEDs hardware
        inline acs_apbm_getboardledsstatuscallback (
			axe_eth_shelf::led_status_t & green_led_,
			axe_eth_shelf::led_status_t & red_led_,
			axe_eth_shelf::led_status_t & mia_led_, 
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_, uint16_t board_fbn_, bool is_4led_supported_=false, bool is_new_gea_supported_=false)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_, board_fbn_, is_4led_supported_, is_new_gea_supported_),
	  green_led(&green_led_), red_led(&red_led_), mia_led(&mia_led_), blue_led(0) /*,
		board_magazine_str(board_magazine_str_), board_slot(board_slot_), switch_board_ip(switch_board_ip_), is_cba(is_cba_)*/
	{}


        //For 4 Leds hardware
	inline acs_apbm_getboardledsstatuscallback (
			axe_eth_shelf::led_status_t & green_led_,
			axe_eth_shelf::led_status_t & red_led_,
			axe_eth_shelf::led_status_t & yellow_led_,  // status
			axe_eth_shelf::led_status_t & blue_led_, //maintenance
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_, uint16_t board_fbn_, bool is_4leds_supported_, bool is_new_gea_supported_)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_, board_fbn_, is_4leds_supported_, is_new_gea_supported_),
	  green_led(&green_led_), red_led(&red_led_), mia_led(&yellow_led_), blue_led(&blue_led_) /*,
		board_magazine_str(board_magazine_str_), board_slot(board_slot_), switch_board_ip(switch_board_ip_), is_cba(is_cba_)*/
	{}


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getboardledsstatuscallback () {}


	//===========//
	// Functions //
	//===========//
protected:
//	int read_data_scbrp (netsnmp_variable_list * var_list) const;
	virtual int read_scbrp (netsnmp_variable_list * var_list) const;

//	int read_data_scx (netsnmp_variable_list * var_list) const;
	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_leds_data (const oid * ( &led_oids) [3], size_t ( &led_oid_sizes) [3], netsnmp_variable_list * var_list) const;
	int read_4leds_data (const oid * ( &led_oids) [4], size_t ( &led_oid_sizes) [4], netsnmp_variable_list * var_list) const;


	//===========//
	// Operators //
	//===========//
public:
//	virtual int operator() (int operation, netsnmp_session * session, int request_id, netsnmp_pdu * pdu) const;


	//========//
	// Fields //
	//========//
public:
        // 3 LEDs 
	axe_eth_shelf::led_status_t * green_led;
	axe_eth_shelf::led_status_t * red_led;
	axe_eth_shelf::led_status_t * mia_led;
        
	axe_eth_shelf::led_status_t * blue_led; // 4 LED

	static const char * LED_NAMES [];
	static const char * FOUR_LED_NAMES [];
};


class acs_apbm_getboardbiosrunmodecallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getboardbiosrunmodecallback (
			axe_eth_shelf::bios_boot_run_mode_t & bios_run_mode_,
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_), bios_run_mode(&bios_run_mode_)
	{}


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getboardbiosrunmodecallback () {}


	//===========//
	// Functions //
	//===========//
protected:
	virtual int read_scbrp (netsnmp_variable_list * var_list) const;

	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;


	//========//
	// Fields //
	//========//
public:
	axe_eth_shelf::bios_boot_run_mode_t * bios_run_mode;
};


class acs_apbm_getshelfmgrcontrolstatecallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getshelfmgrcontrolstatecallback (
			int & state_,
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_), state(&state_)
	{}


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getshelfmgrcontrolstatecallback () {}


	//===========//
	// Functions //
	//===========//
protected:
	virtual int read_scbrp (netsnmp_variable_list * var_list) const;

	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;


	//========//
	// Fields //
	//========//
public:
	int * state;
};
// front partner port state
class acs_apbm_getscxfrontpartnerportstatecallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getscxfrontpartnerportstatecallback (
			int & partnerPortstate,
			const char * board_magazine_str_, int32_t port_number, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, port_number, switch_board_ip_, is_cba_), partnerPortstate(&partnerPortstate)
	{}

	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getscxfrontpartnerportstatecallback () {}


	//===========//
	// Functions //
	//===========//
protected:

	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;


	//========//
	// Fields //
	//========//
public:
	int * partnerPortstate;
};

class acs_apbm_getshelfneighbourcallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getshelfneighbourcallback (
			int & shelfneighbour_,
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_), shelfneighbour(&shelfneighbour_)
	{}


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getshelfneighbourcallback () {}


	//===========//
	// Functions //
	//===========//
protected:
	virtual int read_scbrp (netsnmp_variable_list * var_list) const;

	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;


	//========//
	// Fields //
	//========//
public:
	int * shelfneighbour;
};



class acs_apbm_getboardpowerstatuscallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getboardpowerstatuscallback (
			acs_apbm_snmp::bladePwr_status_t & power_status_,
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_), power_status(&power_status_)
	{}


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getboardpowerstatuscallback () {}


	//===========//
	// Functions //
	//===========//
protected:
	virtual int read_scbrp (netsnmp_variable_list * var_list) const;

	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;


	//========//
	// Fields //
	//========//
public:
	acs_apbm_snmp::bladePwr_status_t * power_status;
};


/*******************************************************************************
* This class has the callback funtions to read front port status from the mib.
*/

class acs_apbm_getfrontportstatuscallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getfrontportstatuscallback (
			acs_apbm_snmp::frontPort_status_t & port_status,
			const char * board_magazine_str_,int32_t port_number, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_,port_number, switch_board_ip_, is_cba_), port_status(&port_status)
	{}
	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getfrontportstatuscallback () {}

    //===========//
	// Functions //
	//===========//
protected:

	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;

	//========//
	// Fields //
	//========//
public:
	acs_apbm_snmp::frontPort_status_t * port_status;
};

/*******************************************************************************
* This class has the callback funtions to read admin status of front ports from the mib.
*/

class acs_apbm_getadminstatefrontportstatuscallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getadminstatefrontportstatuscallback (
			acs_apbm_snmp::frontPort_status_t & port_status,
			const char * board_magazine_str_,int32_t port_number, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_,port_number, switch_board_ip_, is_cba_), port_status(&port_status)
	{}
	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getadminstatefrontportstatuscallback () {}

    //===========//
	// Functions //
	//===========//
protected:

	virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;

	//========//
	// Fields //
	//========//
public:
	acs_apbm_snmp::frontPort_status_t * port_status;
};


/*******************************************************************************
* This class has the callback funtions to read blade_led_type from the mib.
*/


class acs_apbm_getbladeledtypecallback : public acs_apbm_snmpsessioncallback {
        //==============//
        // Constructors //
        //==============//
public:
        inline acs_apbm_getbladeledtypecallback(
                        acs_apbm_snmp::blade_led_type_t & blade_led_type_,
                        const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_ )
        : acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_), blade_led_type(&blade_led_type_)
        {}


        //============//
        // Destructor //
        //============//
public:
        virtual inline ~acs_apbm_getbladeledtypecallback() {}


        //===========//
        // Functions //
        //===========//
protected:
        virtual int read_scbrp (netsnmp_variable_list * var_list) const;          // For SCB-RP boards.

        virtual int read_scx (netsnmp_variable_list * var_list) const;            // For SCX boards.

private:
        int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;

        //========//
        // Fields //
        //========//
public:
        acs_apbm_snmp::blade_led_type_t * blade_led_type;
};

/********************************************************************************/
class acs_apbm_getboardfataleventlogcallback : public acs_apbm_snmpsessioncallback {
        //==============//
        // Constructors //
        //==============//
public:
        inline acs_apbm_getboardfataleventlogcallback (
            char (& fatalEventLogInfo_) [acs_apbm_snmp::SNMP_BOARD_FATAL_EVENT_LOG_SIZE + 1],
                        const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_
        )
        : acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_), fatalEventLogInfo(fatalEventLogInfo_)
        {}


        //============//
        // Destructor //
        //============//
public:
        virtual inline ~acs_apbm_getboardfataleventlogcallback () {}


        //===========//
        // Functions //
        //===========//
protected:
        virtual int read_scbrp (netsnmp_variable_list * var_list) const;

        virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
        int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;


        //========//
        // Fields //
        //========//
public:
        char * fatalEventLogInfo;
};

class acs_apbm_getmanagerIpAddresscallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getmanagerIpAddresscallback (
			char (& ipAddress_ )[acs_apbm_snmp::SNMP_IPADDRESS_FIELD_SIZE],
			const char * board_magazine_str_, int32_t board_slot_, const char * switch_board_ip_, int is_cba_
	)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_)
	{

::strncpy(managerIpaddr, ipAddress_, acs_apbm_snmp::SNMP_IPADDRESS_FIELD_SIZE);}


	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getmanagerIpAddresscallback () {}


        //===========//
        // Functions //
        //===========//
protected:
        virtual int read_scbrp (netsnmp_variable_list * var_list) const;

        virtual int read_scx (netsnmp_variable_list * var_list) const;

private:
	int read_value (const oid * oid_name, size_t oid_names_size, netsnmp_variable_list * var) const;


	//========//
	// Fields //
	//========//
public:
	mutable acs_apbm_snmp::axe::eth::shelf::ipAddress_t managerIpaddr;
};


class acs_apbm_getHWIGenericCallback : public acs_apbm_snmpsessioncallback {
	//==============//
	// Constructors //
	//==============//
public:
	inline acs_apbm_getHWIGenericCallback (
			const char * board_magazine_str_,
			int32_t board_slot_,
			const char * switch_board_ip_,
			int is_cba_,
			char * hwiOutValue_,
			const int &hwiOutValueSize_,
			const bool &isHexStr_)
	: acs_apbm_snmpsessioncallback(board_magazine_str_, board_slot_, switch_board_ip_, is_cba_),
	  hwiOutValue(hwiOutValue_) , hwiOutValueSize(hwiOutValueSize_), isHexStr(isHexStr_) {}
	//============//
	// Destructor //
	//============//
public:
	virtual inline ~acs_apbm_getHWIGenericCallback() {}

	//===========//
	// Functions //
	//===========//
protected:
	//	int read_data_scbrp (netsnmp_variable_list * var_list) const;
	virtual int read_scbrp (netsnmp_variable_list * var_list) const;

	//	int read_data_scx (netsnmp_variable_list * var_list) const;
	virtual int read_scx (netsnmp_variable_list * var_list) const;

	//========//
	// Fields //
	//========//
public:
	char * hwiOutValue;
	int hwiOutValueSize;
	bool isHexStr;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_snmpmanagercallbacks

