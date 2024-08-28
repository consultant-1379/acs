#ifndef HEADER_GUARD_CLASS__acs_apbm_api_imp
#define HEADER_GUARD_CLASS__acs_apbm_api_imp acs_apbm_api_imp

/** @file acs_apbm_acs_apbm_api_imp.h
 *	@brief
 *	@author xgiufer (Giuliano Ferraioli)
 *	@date 2011-08
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
 *	| R-001 | 	  -		 | xnicmut      | Creation        					  |
 *	+=======+============+==============+=====================================+
 *	| R-002 | 	  -		 | xgiufer      | First Revision			          |
 *	+=======+============+==============+=====================================+
 */


/* @class acs_apbm_api_imp acs_apbm_api_imp.h
 *	@brief
 *	@author xgiufer (Giuliano Ferraioli)
 *	@date -
 *	@version 1.04
 *
 *	acs_apbm_boardsessionhandler class detailed description
 */

#include <string>
#include <vector>
#include <list>
#include <errno.h>
#include <ace/Time_Value.h>
//#include <poll.h>

#include "acs_apbm_types.h"
#include "acs_apbm_trapmessage.h"
#include "acs_apbm_trapmessage_imp.h"
#include <ACS_DSD_Client.h>
#include <ACS_DSD_Macros.h>
#include "acs_apbm_macros.h"

#include "acs_apbm_primitivedatahandler.h"
#include "acs_apbm_configurationhelper.h"


typedef acs_apbm_primitivedatahandler<1024> acs_apbm_primitive;

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_api_imp

class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	__CLASS_NAME__ ();

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual ~__CLASS_NAME__ ();

	//===========//
	// Functions //
	//===========//
public:	
	bool gep5_gep7;
       	bool is_gep5_gep7();
	bool hwtype_gep7;
	bool is_hwtype_gep7();
	//===============//
	// Public Method //
	//===============//

	 /**@brief subscribe_trap method
	 *
	 *	On success returns handle.
	 *	@param[in] bitmap detailed description
	 *	@param[in/out] trap_handle detailed description
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int subscribe_trap (int bitmap, acs_apbm::trap_handle_t & trap_handle);

	 /**	@brief unsubscribe_trap method
	 *
	 *	On success unsubscribe from trap receiving service.
	 *	@param[in] trap_handle detailed description
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int unsubscribe_trap (acs_apbm::trap_handle_t trap_handle);

	 /**	@brief get_trap method
	 *
	 *	Returns the DSD API Session object to be used for accepting a new client connection.
	 *	@param[in] trap_handle detailed description
	 *	@param[in/out] trap_message detailed description
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int get_trap (acs_apbm::trap_handle_t trap_handle, acs_apbm_trapmessage & trap_message);

	 /**	@brief get_own_slot method
	 *
	 *	Returns the DSD API Session object to be used for accepting a new client connection.
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
  	int get_own_slot ();


  	int get_switch_board_info( const acs_apbm::architecture_type_t architecture, uint32_t magazine, int32_t  & slot, char (&ipna_str) [16], char (&ipnb_str) [16], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state, acs_apbm::fbn_t & fbn);

  	//  methods from HWM_API
	/** @brief Check if a board is present
	 *
	 *	Check if a specified board is available in the AP HW configuration.
	 *
	 *	@param[in] board_name
	 *		The board name. Valid values are those defined in the acs_apbm::board_name_t
	 *		enumeration:
	 *			- DVD: External media board
	 *			- GEA: Alarm board
	 *	@return The function returns one of the following integer codes
	 *		 0	Yes, the board is available in the current configuration.
	 *		 1	No, the board is not available in the current configuration.
	 *		 2	Pending, the board is part of the HW configuration but is not
	 *				available at the moment. E.g. it can be faulty or being
	 *				repaired and can not be used.
	 *		-1	Error
	 *	@remarks Remarks
	 */
	int get_board_presence (const acs_apbm::board_name_t board_name);

	/** Check if a specified board is connected to an other AP in the system.
	 *
	 *	@param[in] board_name
	 *		The board name. Valid values are those defined in the acs_apbm::board_name_t
	 *		enumeration:
	 *			- DVD: External media board
	 *			- GEA: Alarm board
	 *	@param[out] ap_ids
	 *		Array of AP numbers for APs where the board is connected: 1 for AP1, 2 for AP2, etc.
	 *	@return The function returns one of the following integer codes
	 *		 0	OK.
	 *		-1	Error
	 *	@remarks Remarks
	 */
	int get_board_location (const acs_apbm::board_name_t board_name, std::list<int> & ap_ids);

	/** Get the status for an optional board.
	 *
	 *	@param[in] board_name
	 *		The board name. Valid values are those defined in the acs_apbm::board_name_t
	 *		enumeration:
	 *			- DVD: External media board
	 *			- GEA: Alarm board
	 *	@return The function returns one of the following integer codes
	 *		 0	Working, the board is working OK.
	 *		 1	Faulty, the board is faulty.
	 *		 2	Blocked, the board is being blocked and is being repaired.
	 *		 3	Missing, the board is temporarily missing.
	 *		-1	Error
	 *	@remarks Remarks
	 */
	int get_board_status (const acs_apbm::board_name_t board_name);

	/** Set the status for an optional board.
	 *
	 *	@param[in] board_name
	 *		The board name. Valid values are those defined in the acs_apbm::board_name_t
	 *		enumeration:
	 *			- DVD: External media board
	 *			- GEA: Alarm board
	 *	@param[in] status
	 *		Specify the status to be set. Valid values are:
	 *			 0	OK
	 *			-1	Faulty
	 *	@return The function returns one of the following integer codes
	 *		 0	OK
	 *		-1	Error
	 *	@remarks Remarks
	 */
	int set_board_status (const acs_apbm::board_name_t board_name, const acs_apbm::reported_board_status_t status);

	/** ipmifw_upgrade upgrade ipmi fw of the  board located in magazine 'magazine' and slot 'slot'
		 *
		 *
		 *	@param[in] magazine the magazine number in which the board to be updated is located
		 *	@param[in]: slot
		 *	@param[in]: comport  the com port to use for upgrading
		 *	 possible values:
		 *		acs_apbm::NO_COM (0) 	use IPMI bus for upgrading the board
		 *		acs_apbm::COM2  (1)     use Internal APUB com port
		 *		acs_apbm::COM3 (2)      use SATA/USB 1 port to update GEA board
		 *
		 *	 status
		 *		Specify the status to be set. Valid values are:
		 *			 0	OK
		 *			-1	Faulty
		 *	@return The function returns one of the following integer codes
		 *		 0	OK
		 *		-1	Error
		 *	@remarks Remarks
	 **/
	int ipmifw_upgrade(uint32_t magazine, int32_t slot, const char ipmipkg_path [ACS_APBM_IPMIPKG_NAME_MAX_SIZE], uint8_t comport );

	int get_ipmifw_status(uint32_t magazine, int32_t  slot, uint8_t & ipmifw_status, char (& lastupg_date) [ACS_APBM_DATA_TIME_SIZE] );

	int get_ipmifw_data(uint32_t magazine, int32_t  slot, uint8_t & ipmifw_type, char (& product_number) [ACS_APBM_IPMI_PRODUCT_NUMBER] ,  char (& revision) [ACS_APBM_IPMI_REVISION]);

private:

	//===========//
	// Operators //
	//===========//
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);



	//================//
	// Private Method //
	//================//

	/**	@brief init connection method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int connect(unsigned timeout=2000);
	int connect_unix();


	/**	@brief send_primitive_subscribe_request method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int send_primitive_subscribe_request(const int primitive_id, const int primitive_version,const int bitmap);

	/**	@brief send_primitive_unsubscribe_request method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int send_primitive_unsubscribe_request(const int primitive_id, const int primitive_version);

	/**	@brief send_primitive_get_trap_request method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int send_primitive_get_trap_request(const int primitive_id,const int primitive_version);

	/**	@brief send_primitive_switch_board_data_request method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */

	int send_primitive_switch_board_data_request(const int primitive_id, const int primitive_version, acs_apbm::architecture_type_t architecture, uint32_t magazine);

	/**	@brief send_primitive_get_own_slot_request method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */

	int send_primitive_get_own_slot_request(const int primitive_id, const int primitive_version);

	/**	@brief send_primitive_board_location_request method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */

	int send_primitive_board_location_request(const int primitive_id, const int primitive_version, const acs_apbm::board_name_t board_name /*,List <int> &apNoList */);

	/**	@brief send_primitive_board_status_request method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */

	int send_primitive_board_status_request(const int primitive_id, const int primitive_version, const acs_apbm::board_name_t board_name);

	/**	@brief send_primitive_set_board_status_request method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */

	int send_primitive_set_board_status_request(const int primitive_id, const int primitive_version, const acs_apbm::board_name_t board_name, int8_t board_status);

	/**	@brief send_primitive_board_presence_request method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */

	int send_primitive_board_presence_request(const int primitive_id, const int primitive_version, const acs_apbm::board_name_t board_name);

	/**	@brief send_primitive_get_ipmifw_status_request method
		 *
		 *	@return Return Description
		 *	@Remarks Remarks Remarks
		 */
	int send_primitive_get_ipmifw_status_request(const int primitive_id, const int primitive_version, uint32_t magazine, int32_t slot);

	/**	@brief send_primitive_ipmifw_upgrade_request method
		 *
		 *	@return Return Description
		 *	@Remarks Remarks Remarks
		 */
	int send_primitive_ipmifw_upgrade_request(const int primitive_id, const int primitive_version, uint32_t magazine, int32_t slot, uint8_t comport, const char ipmipkg_name [ACS_APBM_IPMIPKG_NAME_MAX_SIZE]);

	/**	@brief send_primitive_get_ipmifw_data_request method
		 *
		 *	@return Return Description
		 *	@Remarks Remarks Remarks
		 */
	int send_primitive_get_ipmifw_data_request(const int primitive_id, const int primitive_version, uint32_t magazine, int32_t slot);

	/**     @brief unpack_primitive_get_sel_trap method
         *
         *      @return Return Description
         *      @Remarks Remarks Remarks
         */
	int unpack_primitive_get_sel_trap(const acs_apbm_primitivedatahandler<> & pdh, int & mag_plug_number,int & slot_pos, char * messages);

	/**	@brief unpack_primitive_get_sel_trap method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int unpack_primitive_get_board_presence_trap(const acs_apbm_primitivedatahandler<> & pdh, int & mag_plug_number, int & slot_pos, int & hwBoardPresence, int & busType, char * messages);


	/**	@brief unpack_primitive_get_sel_trap method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int unpack_primitive_get_sensor_state_change_trap(const acs_apbm_primitivedatahandler<> & pdh, int & mag_plug_number, int & slot_pos, int & sensorType, int & sensorID, int & sensorTypeCode, char * sensorEventData);

	/**	@brief unpack_primitive_get_scb_data method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int unpack_primitive_get_scb_data(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code, int32_t & slot, char (& ipna_str) [acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS], char (& ipnb_str) [acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state);

	/**	@brief unpack_primitive_get_scxb_data method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int unpack_primitive_get_Egem2L2Switch_data(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code, uint32_t & magazine, int32_t & slot, char (& ipna_str) [acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS], char (& ipnb_str) [acs_apbm::PCP_FIELD_SIZE_IPV4_ADDRESS], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state, acs_apbm::fbn_t & fbn);


        /**     @brief unpack_primitive_get_own_slot method
         *
         *      @return Return Description
         *      @Remarks Remarks Remarks
         */
	int unpack_primitive_get_own_slot(const acs_apbm_primitivedatahandler<> & pdh, int32_t & slot);

    /**     @brief unpack_primitive_board_presence_reply method
         *
         *      @return Return Description
         *      @Remarks Remarks Remarks
         */
	int unpack_primitive_board_presence_reply(const acs_apbm_primitivedatahandler<> & pdh, int8_t & board_presence);

	/**     @brief unpack_primitive_board_status_reply method
	 *
	 *      @return Return Description
	 *      @Remarks Remarks Remarks
	 */
	int unpack_primitive_board_status_reply(const acs_apbm_primitivedatahandler<> & pdh, int8_t & board_status);

	/**     @brief unpack_primitive_board_location method
	 *
	 *      @return Return Description
	 *      @Remarks Remarks Remarks
	 */
	int unpack_primitive_board_location_reply(const acs_apbm_primitivedatahandler<> & pdh , int32_t & err_code, int32_t & ap_sys_no );

	/**     @brief unpack_primitive_set_board_status method
	 *
	 *      @return Return Description
	 *      @Remarks Remarks Remarks
	 */
	int unpack_primitive_set_board_status_reply(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code);

	/**    @brief unpack_primitive_get_ipmifw_status_reply method
	*
	*      @return Return Description
	*      @Remarks Remarks Remarks
	*/
	int unpack_primitive_get_ipmifw_status_reply(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code, uint8_t & ipmifw_status, char (& lastupg_date) [ACS_APBM_DATA_TIME_SIZE]);

	/**    @brief unpack_primitive_get_ipmifw_status_reply method
		*
		*      @return Return Description
		*      @Remarks Remarks Remarks
	*/
	int unpack_primitive_ipmifw_upgrade_reply(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code);

	/**    @brief unpack_primitive_get_ipmifw_data_reply method
		*
		*      @return Return Description
		*      @Remarks Remarks Remarks
	*/
	int unpack_primitive_get_ipmifw_data_reply(const acs_apbm_primitivedatahandler<> & pdh, int32_t & err_code, uint8_t & ipmifw_type, char (& product_number) [ACS_APBM_IPMI_PRODUCT_NUMBER], char (& revision) [ACS_APBM_IPMI_REVISION]);
	/**	@brief receive_primitive_notification method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_notification();

	/**	@brief receive_primitive_get_trap method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_get_trap(acs_apbm_trapmessage & trap_msg);

	/**	@brief receive_primitive_scxb_data method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_switch_board_data(uint32_t magazine, int32_t & slot, char (& ipna_str) [16], char (& ipnb_str) [16], acs_apbm::state_t & shelf_mgr_state, acs_apbm::neighbour_state_t & neighbour_state, acs_apbm::fbn_t & fbn);

	/**	@brief receive_primitive_get_own_slot method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_get_own_slot(int32_t & slot);

	/**	@brief receive_primitive_board_presence method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_board_presence(int8_t & board_presence);

	/**	@brief receive_primitive_board_status method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_board_status(int8_t & board_status);

	/**	@brief receive_primitive_board_location method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_board_location(int & ap_sys_no);

	/**	@brief receive_primitive_set_board_status_reply method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_set_board_status_reply(int32_t & err_code);

	/**	@brief receive_primitive_get_ipmifw_status_reply method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_get_ipmifw_status_reply(uint8_t & ipmifw_status, char (& lastupg_date) [ACS_APBM_DATA_TIME_SIZE]);

	/**	@brief receive_primitive_ipmifw_upgrade_reply method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_ipmifw_upgrade_reply(int32_t & err_code, uint8_t comport );

	/**	@brief receive_primitive_get_ipmifw_data_reply method
	 *
	 *	@return Return Description
	 *	@Remarks Remarks Remarks
	 */
	int receive_primitive_get_ipmifw_data_reply(int32_t & err_code, uint8_t & ipmifw_type, char (& product_number) [ACS_APBM_IPMI_PRODUCT_NUMBER], char (& revision) [ACS_APBM_IPMI_REVISION]);
	//========//
	// Fields //
	//========//
private:
	ACS_DSD_Session _session;
	ACS_DSD_Client _client_connector;
	static ACS_DSD_Node _node;
};

#endif // HEADER_GUARD_CLASS__acs_apbm_api_imp
