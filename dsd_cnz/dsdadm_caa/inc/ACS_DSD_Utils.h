#ifndef ACS_DSD_UTILS_H_
#define ACS_DSD_UTILS_H_

/** @file ACS_DSD_utils.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-27
 *	@version 0.1
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
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2010-11-27 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <stdint.h>
#include <stddef.h>
#include <string>
#include "ACS_DSD_MacrosConstants.h"
#include "ACS_DSD_MacrosConfig.h"

#define ACS_DSD_STARTUP_CONNECTION_CHECK_TIMER_CONTEXT reinterpret_cast<const void *>(0xC6110FA8BC)
#define ACS_DSD_IMM_COPY_POLLING_CONTEXT reinterpret_cast<const void *>(0x444AFBAD)
#define ACS_DSD_STATE_UPDATE_POLLING_CONTEXT reinterpret_cast<const void *>(0x123EAABC)

#ifdef ACS_DSD_SERVER_MCHECK_TRACE_ENABLED
extern FILE * mcheck_file_p;
#define ACS_DSD_SERVER_MCHECK_TRACE_PRINT(m,c) ::fprintf(mcheck_file_p, "%s: %d\n", m, c)
#else
#define ACS_DSD_SERVER_MCHECK_TRACE_PRINT(m,c)
#endif

class ACS_DSD_Node;

namespace ACS_DSD_Utils
{
 /** @brief Copies a ACS_DSD_ServicePrimitive string field into a destination buffer supplied by caller
 *
 *	 Helper function used in the implementation of ACS_DSD_ServicePrimitive derived classes, for getting string fields values.
 *
 *	@param[in] primitive_field  string field to be copied
 *	@param[in] dest_buff destination buffer
 *	@param[in] dest_buff_size destination buffer size
 *	@return Return Description
 *	@remarks   It assumes that <primitive_field> is of type uint8_t[DIM], where DIM is a constant known at compile time
 */
//int get_primitive_string_field(uint8_t *primitive_field, char *dest_buff, size_t dest_buff_size);

template <size_t size>
int get_primitive_string_field(uint8_t (& primitive_field) [size], char *dest_buff, size_t dest_buff_size)
{
	if(!dest_buff)
		return -1;		// invalid call

	if(dest_buff_size < size)
		return -2;		// supplied buffer is not large enough

	// copy field value in the destination buffer
	memcpy(dest_buff, primitive_field, size);

	return 0;
}

template <size_t size>
int get_primitive_string_field(char (& primitive_field) [size], char *dest_buff, size_t dest_buff_size)
{
	if(!dest_buff)
		return -1;		// invalid call

	if(dest_buff_size < size)
		return -2;		// supplied buffer is not large enough

	// copy field value in the destination buffer
	memcpy(dest_buff, primitive_field, size);

	return 0;
}

/** @brief Set a ACS_DSD_ServicePrimitive string field using the data provided by caller in the source buffer
 *
 *	 Helper function used in the implementation of ACS_DSD_ServicePrimitive derived classes, for setting string fields values.
 *
 *	@param[in] primitive_field  field to be set
 *	@param[in] src_buff source buffer
 *	@param[in] n_bytes_to_copy number of bytes to be copied
 *	@return Return Description
 *	@remarks   It assumes that <primitive_field> is of type uint8_t[DIM], where DIM is a constant known at compile time
 */
//int set_primitive_string_field(uint8_t *primitive_field, const char *src_buff, size_t n_bytes_to_copy);

template <size_t size>
int set_primitive_string_field(uint8_t (& primitive_field) [size], const char *src_buff, size_t n_bytes_to_copy)
{
	if(!src_buff)
		return -1;	// invalid call

	if( n_bytes_to_copy > size)
		return -2; 	// too many bytes supplied by the caller: the primitive field is not large enough

	if( (n_bytes_to_copy == size) && (src_buff[n_bytes_to_copy-1] != '\0') )
		return -3;	// if the number of bytes to be copied is equal to the primitive field size, then the last supplied character must be a NULL character !

	// fill primitive field with the passed bytes
	memcpy(primitive_field, src_buff, n_bytes_to_copy);

	// ... to be sure that the primitive_field terminates with a NULL character
	if(n_bytes_to_copy < size)
		primitive_field[n_bytes_to_copy] = '\0';

	return 0;
}

template <size_t size>
int set_primitive_string_field(char (& primitive_field) [size], const char *src_buff, size_t n_bytes_to_copy)
{
	if(!src_buff)
		return -1;	// invalid call

	if( n_bytes_to_copy > size)
		return -2; 	// too many bytes supplied by the caller: the primitive field is not large enough

	if( (n_bytes_to_copy == size) && (src_buff[n_bytes_to_copy-1] != '\0') )
		return -3;	// if the number of bytes to be copied is equal to the primitive field size, then the last supplied character must be a NULL character !

	// fill primitive field with the passed bytes
	memcpy(primitive_field, src_buff, n_bytes_to_copy);

	// ... to be sure that the primitive_field terminates with a NULL character
	if(n_bytes_to_copy < size)
		primitive_field[n_bytes_to_copy] = '\0';

	return 0;
}

/// Get Info about the NODE on which the caller is running
int get_node_info(ACS_DSD_Node & node_info, std::string & error_descr);

int get_partner_node_info(ACS_DSD_Node & node_info, std::string & error_descr);
//******************************************************************************
// get_AP_name
// APidentity (CP-AP protocol) ==> AP name
//******************************************************************************
std::string get_AP_name(char CP_APidentity);

int get_ApNodesName(int32_t system_id, char node_name[][acs_dsd::CONFIG_NODE_NAME_SIZE_MAX]);

// get the system type related to the provided system_id
acs_dsd::SystemTypeConstants get_system_type(int32_t system_id);

//  convert an integer containing an IP address in a string
int get_ip_addr_str(uint32_t ip_addr, char *buff, int len);

// remove the unix sockets created by DSD Server
int clean_dsd_saps(const char * saps_path );
}

#endif /* ACS_DSD_UTILS_H_ */
