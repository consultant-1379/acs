namespace {
	const char * const _not_defined_mesg = "<ERROR MESG NOT DEFINED>";

	enum {
		TEXT_OK	=	0,
		TEXT_ACCEPTOR_NOT_CLOSED,
		TEXT_ACCEPTOR_NOT_OPEN,
		TEXT_STREAM_ALREADY_IN_USE,
		TEXT_WRONG_STREAM_TYPE,
		TEXT_WRONG_ADDRESS_TYPE,
		TEXT_REMOVE_NOT_ALLOWED,
		TEXT_IP4_ADDRESS_DOTTED_NOT_VALID,
		TEXT_INTERNAL_NET_CONFIG_NOT_LOADED,
		TEXT_SERVICE_MODE_UNKNOWN,
		TEXT_SERVER_NOT_CLOSED,
		TEXT_SERVICE_ACCESS_POINT_NOT_VALID,
		TEXT_SERVICE_ACCESS_POINT_OUT_OF_RANGE,
		TEXT_SOME_NETWORKS_UNAVAILABLE,
		TEXT_SOME_INTERNAL_ACCEPTORS_NOT_OPEN,
		TEXT_SOME_NETWORKS_AND_ACCEPTORS_UNAVAILABLE,
		TEXT_SERVER_NOT_OPEN,
		TEXT_SESSION_ALREADY_IN_USE,
		TEXT_TIMEOUT_EXPIRED,
		TEXT_NODE_NOT_FOUND,

		TEXT_PCP_PRIMITIVE_UNDEFINED,
		TEXT_PCP_VERSION_NOT_SUPPORTED,
		TEXT_PCP_DATA_BUFFER_OVERFLOW,
		TEXT_PCP_BAD_VERSION_IN_PRIMITIVE_DATA,
		TEXT_PCP_BAD_PRIMITIVE_IN_PRIMITIVE_DATA,
		TEXT_PCP_UNEXPECTED_PRIMITIVE_RECEIVED,
		TEXT_PCP_PRIMITIVE_DATA_NOT_READY_TO_SEND,

		TEXT_PCP_UNEXPECTED_ERROR_CODE,

		TEXT_PCP_ERROR_CODE_NODE_NOT_VALID,
		TEXT_PCP_ERROR_CODE_FILE_STORAGE_FAILURE,
		TEXT_PCP_ERROR_CODE_ADDRESS_NOT_VALID,
		TEXT_PCP_ERROR_CODE_APPLICATION_NOT_VALID,
		TEXT_PCP_ERROR_CODE_NAME_NOT_UNIQUE,
		TEXT_PCP_ERROR_CODE_APPLICATION_NOT_REGISTERED,
		TEXT_PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED,
		TEXT_PCP_ERROR_CODE_APPLICATION_ALREADY_REGISTERED,
		TEXT_PCP_ERROR_CODE_NO_IP_ADDRESS_AVAILABLE,
		TEXT_PCP_ERROR_CODE_NO_PORT_AVAILABLE,
		TEXT_PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE,
		TEXT_PCP_ERROR_CODE_REGISTER_FAILURE,
		TEXT_PCP_ERROR_CODE_UNREGISTER_FAILURE,
		TEXT_PCP_ERROR_CODE_CONNECTION_ADDRESS_NUM_MISMATCH,
		TEXT_PCP_ERROR_CODE_BAD_IP_ADDRESSES_COUNT,
		TEXT_PCP_ERROR_CODE_PROTOCOL_VERSION_NOT_SUPPORTED,
		TEXT_PCP_ERROR_CODE_ILLEGAL_ELEMENT_VALUE,
		TEXT_PCP_ERROR_CODE_HOST_UNREACHABLE,
		TEXT_PCP_ERROR_CODE_OPERATION_NOT_ALLOWED,

		TEXT_SYSTEM_ERROR,
		TEXT_SYSTEM_OPEN,
		TEXT_SYSTEM_ACCEPT,
		TEXT_SYSTEM_CLOSE,
		TEXT_SYSTEM_REMOVE,
		TEXT_SYSTEM_GET_ADDRESS,
		TEXT_SYSTEM_CONNECT,
		TEXT_SYSTEM_RECEIVE,
		TEXT_SYSTEM_SEND,
		TEXT_SYSTEM_GET_OPTION,
		TEXT_SYSTEM_SET_OPTION,
		TEXT_SYSTEM_OPEN_DIRECTORY,
		TEXT_SYSTEM_OUT_OF_MEMORY,
		TEXT_SYSTEM_READ,
		TEXT_SYSTEM_CREATE,
		TEXT_SYSTEM_EPOLL_CTL_ADD,
		TEXT_SYSTEM_IO_MULTIPLEXING_ACTIVATE,
		TEXT_SYSTEM_EPOLL_WAIT,
		TEXT_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE,
		TEXT_SYSTEM_POLL,

		TEXT_HOST_CONFIG_LOAD,
		TEXT_GET_PROCESS_INFORMATION,
		TEXT_WRONG_ACCEPTOR_TYPE,
		TEXT_LOAD_AP_NODES_FROM_CS,
		TEXT_FIND_MY_AP_NODE_HOST_INFO,
		TEXT_AP_CONFIGURATION_MISMATCH_FROM_CS,
		TEXT_CONFIG_NODE_IP_CONFLICT,
		TEXT_REMOTE_SYSTEM_TYPE_UNKNOWN,
		TEXT_SERVICE_MODE_IS_PRIVATE,
		TEXT_PARAMETER_EMPTY_OR_NULL,
		TEXT_SERVICE_PUBLISH,
		TEXT_NOT_ENOUGH_SPACE,
		TEXT_ACCEPTORS_ADDRESSES_NOT_FOUND,
		TEXT_PEER_CLOSED_CONNECTION,
		TEXT_OBJECT_CORRUPTED,
		TEXT_SERVER_ALREADY_PUBLISHED,
		TEXT_SESSION_DISCONNECTED,
		TEXT_BAD_PARAMETER_VALUE,
		TEXT_GET_NODE_STATE,
		TEXT_CS_API_CALL_ERROR,
		TEXT_FIND_MY_AP_PARTNER_NODE_HOST_INFO,
		TEXT_SAF_IMM_OM_INIT,
		TEXT_SAF_IMM_OM_GET,
		TEXT_SAF_IMM_NULL_ATTRIBUTE_VALUE,
		TEXT_BAD_NODE_STATE,
		TEXT_BAD_SYSTEM_ID,
		TEXT_ADDRESS_EMPTY_OR_NULL,
		TEXT_OCP_SENDING_ACCEPT_INDICATION,
		TEXT_OCP_RECEIVING_ACCEPT_CONFIRMATION,
		TEXT_OCP_ACCEPT_CONFIRM_BAD_RESPONSE,
		TEXT_OCP_SEND_REQUEST_BAD_PRIM_ID_IN_HEADER,
		TEXT_OCP_RECEIVED_HEADER_SIZES_MISMATCH,
		TEXT_OCP_BUFFER_SIZE_EXCEEDS_MAXIMUM_ALLOWED,
		TEXT_SAF_IMM_BAD_ATTRIBUTE_VALUE,
		TEXT_NODE_NETWORK_CONFIGURATION,
		TEXT_INDEX_OUT_OF_RANGE,
		TEXT_SAF_IMM_EMPTY_ATTRIBUTE_VALUE,

		TEXT_CS_CREATE_HWC_INSTANCE,
		TEXT_CS_CREATE_BOARD_SEARCH_INSTANCE,
		TEXT_CS_GET_BOARD_IDS,
		TEXT_CS_GET_SYSTEM_ID,
		TEXT_CS_GET_SYSTEM_NUMBER,
		TEXT_CS_GET_SYSTEM_SEQUENCE,
		TEXT_CS_GET_SIDE,
		TEXT_CS_GET_IP_ETH_A,
		TEXT_CS_GET_IP_ETH_B,
		TEXT_CS_GET_ALIAS_ETH_A,
		TEXT_CS_GET_ALIAS_ETH_B,
		TEXT_CS_SET_ALARM_MASTER,
		TEXT_CS_SET_CLOCK_MASTER,

		TEXT_CP_STATE_NOTIFIER_NOT_DETACHED,
		TEXT_SENDER_NOT_AVAILABLE,
		TEXT_RECEIVER_NOT_AVAILABLE,
		TEXT_CP_STATE_NOTIFIER_NOT_ATTACHED
	};

	const char * _texts[] = {
		"OK",
		"Acceptor not closed",																																	//TEXT_ACCEPTOR_NOT_CLOSED
		"Acceptor not open",																																		//TEXT_ACCEPTOR_NOT_OPEN
		"I/O stream already in use",																														//TEXT_STREAM_ALREADY_IN_USE
		"Wrong I/O stream type",																																//TEXT_WRONG_STREAM_TYPE
		"Wrong address type",																																		//TEXT_WRONG_ADDRESS_TYPE
		"Remove not allowed",																																		//TEXT_REMOVE_NOT_ALLOWED
		"IPv4 address in numbers-and-dots notation not valid",																	//TEXT_IP4_ADDRESS_DOTTED_NOT_VALID
		"Internal network configuration not loaded",																						//TEXT_INTERNAL_NET_CONFIG_NOT_LOADED
		"Service mode unknown",																																	//TEXT_SERVICE_MODE_UNKNOWN
		"Server not closed",																																		//TEXT_SERVER_NOT_CLOSED
		"Service access point identifier not valid",																						//TEXT_SERVICE_ACCESS_POINT_NOT_VALID
		"Service access point identifier out of range",																					//TEXT_SERVICE_ACCESS_POINT_OUT_OF_RANGE
		"Some networks are not available for TCP/IP communications",														//TEXT_SOME_NETWORKS_UNAVAILABLE
		"Some internal acceptors not open correctly",																						//TEXT_SOME_INTERNAL_ACCEPTORS_NOT_OPEN
		"Some networks unavailable and some internal acceptors not open correctly",							//TEXT_SOME_NETWORKS_AND_ACCEPTORS_UNAVAILABLE
		"Server not open",																																			//TEXT_SERVER_NOT_OPEN
		"Session already in use",																																//TEXT_SESSION_ALREADY_IN_USE
		"Timeout expired",																																			//TEXT_TIMEOUT_EXPIRED
		"Node not found",																																				//TEXT_NODE_NOT_FOUND

		"Primitive Communication Protocol: Primitive not defined",															//TEXT_PCP_PRIMITIVE_UNDEFINED
		"Primitive Communication Protocol: Version not supported",															//TEXT_PCP_VERSION_NOT_SUPPORTED
		"Primitive Communication Protocol: Data buffer overflow",																//TEXT_PCP_DATA_BUFFER_OVERFLOW
		"Primitive Communication Protocol: Bad version found in primitive data buffer",					//TEXT_PCP_BAD_VERSION_IN_PRIMITIVE_DATA
		"Primitive Communication Protocol: Bad primitive value found in primitive data buffer",	//TEXT_PCP_BAD_PRIMITIVE_IN_PRIMITIVE_DATA
		"Primitive Communication Protocol: Unexpected primitive received",											//TEXT_PCP_UNEXPECTED_PRIMITIVE_RECEIVED
		"Primitive Communication Protocol: Primitive data buffer not ready to send",						//TEXT_PCP_PRIMITIVE_DATA_NOT_READY_TO_SEND

		"Primitive Communication Protocol: Unexpected error code received",											//TEXT_PCP_UNEXPECTED_ERROR_CODE

		"Primitive Communication Protocol: Node name not valid or illegal number",									//TEXT_PCP_ERROR_CODE_NODE_NOT_VALID,
		"Primitive Communication Protocol: File storage failure",																		//TEXT_PCP_ERROR_CODE_FILE_STORAGE_FAILURE,
		"Primitive Communication Protocol: Address not available, try later (APAP)",								//TEXT_PCP_ERROR_CODE_ADDRESS_NOT_VALID,
		"Primitive Communication Protocol: Application not available (APAP)",												//TEXT_PCP_ERROR_CODE_APPLICATION_NOT_VALID,
		"Primitive Communication Protocol: Name is not unique (APAP)",															//TEXT_PCP_ERROR_CODE_NAME_NOT_UNIQUE,
		"Primitive Communication Protocol: Application/Service not registered",											//TEXT_PCP_ERROR_CODE_APPLICATION_NOT_REGISTERED,
		"Primitive Communication Protocol: Address request failed",																	//TEXT_PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED,
		"Primitive Communication Protocol: Application/Service already registered",									//TEXT_PCP_ERROR_CODE_APPLICATION_ALREADY_REGISTERED,
		"Primitive Communication Protocol: Not enough available IP addresses",											//TEXT_PCP_ERROR_CODE_NO_IP_ADDRESS_AVAILABLE,
		"Primitive Communication Protocol: No available port numbers",															//TEXT_PCP_ERROR_CODE_NO_PORT_AVAILABLE,
		"Primitive Communication Protocol: Illegal connection type",																//TEXT_PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE,
		"Primitive Communication Protocol: Register failed",																				//TEXT_PCP_ERROR_CODE_REGISTER_FAILURE,
		"Primitive Communication Protocol: Unregister failed",																			//TEXT_PCP_ERROR_CODE_UNREGISTER_FAILURE,
		"Primitive Communication Protocol: Mismatch between connections count and addresses count",	//TEXT_PCP_ERROR_CODE_CONNECTION_ADDRESS_NUM_MISMATCH,
		"Primitive Communication Protocol: Erroneous number of IP addresses specified",							//TEXT_PCP_ERROR_CODE_BAD_IP_ADDRESSES_COUNT,
		"Primitive Communication Protocol: Protocol version not supported",													//TEXT_PCP_ERROR_CODE_PROTOCOL_VERSION_NOT_SUPPORTED,
		"Primitive Communication Protocol: Illegal element value",																	//TEXT_PCP_ERROR_CODE_ILLEGAL_ELEMENT_VALUE,
		"Primitive Communication Protocol: Host unreachable",																				//TEXT_PCP_ERROR_CODE_HOST_UNREACHABLE,
		"Primitive Communication Protocol: Operation not allowed",																	//TEXT_PCP_ERROR_CODE_OPERATION_NOT_ALLOWED

		"System error",																																					//TEXT_SYSTEM_ERROR
		"System error during an open",																													//TEXT_SYSTEM_OPEN
		"System error during an accept",																												//TEXT_SYSTEM_ACCEPT
		"System error during a close",																													//TEXT_SYSTEM_CLOSE
		"System error during a remove",																													//TEXT_SYSTEM_REMOVE
		"System error getting an address",																											//TEXT_SYSTEM_GET_ADDRESS
		"System error during a connect",																												//TEXT_SYSTEM_CONNECT
		"System error during a receive",																												//TEXT_SYSTEM_RECEIVE
		"System error during a send",																														//TEXT_SYSTEM_SEND
		"System error getting options",																													//TEXT_SYSTEM_GET_OPTION
		"System error setting options",																													//TEXT_SYSTEM_SET_OPTION
		"System error opening a directory",																											//TEXT_SYSTEM_OPEN_DIRECTORY
		"System out of memory",																																	//TEXT_SYSTEM_OUT_OF_MEMORY
		"System error during a read",																														//TEXT_SYSTEM_READ
		"System error while creating a system object",																					//TEXT_SYSTEM_CREATE
		"System error adding objects to epoll handle",																					//TEXT_SYSTEM_EPOLL_CTL_ADD
		"System error activating I/O multiplexing interface",																		//TEXT_SYSTEM_IO_MULTIPLEXING_ACTIVATE
		"System error waiting on epoll handle", 																								//TEXT_SYSTEM_EPOLL_WAIT
		"Cannot acquire a lock over a synchronization object",																	//TEXT_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE
		"System error calling poll function",																										//TEXT_SYSTEM_POLL

		"Error loading host configuration",																											//TEXT_HOST_CONFIG_LOAD
		"Error getting process information",																										//TEXT_GET_PROCESS_INFORMATION
		"Wrong acceptor type",																																	//TEXT_WRONG_ACCEPTOR_TYPE
		"Error loading AP nodes configuration info from CS",																		//TEXT_LOAD_AP_NODES_FROM_CS
		"Error finding my ap node configuration info",																					//TEXT_FIND_MY_AP_NODE_HOST_INFO
		"AP nodes mismatch configuration found into the CS",																		//TEXT_AP_CONFIGURATION_MISMATCH_FROM_CS
		"IP conflicting in the system nodes configuration",																			//TEXT_CONFIG_NODE_IP_CONFLICT
		"Remote node has an unknown system type value",																					//TEXT_REMOTE_SYSTEM_TYPE_UNKNOWN
		"Service mode is private",																															//TEXT_SERVICE_MODE_IS_PRIVATE
		"Null or empty parameter provided",																											//TEXT_PARAMETER_EMPTY_OR_NULL
		"Errors publishing a service",																													//TEXT_SERVICE_PUBLISH
		"Not enough space",																																			//TEXT_NOT_ENOUGH_SPACE
		"Internal acceptor's addresses not found",																							//TEXT_ACCEPTORS_ADDRESSES_NOT_FOUND
		"Peer-side closed the connection",																											//TEXT_PEER_CLOSED_CONNECTION
		"Object corrupted",																																			//TEXT_OBJECT_CORRUPTED
		"Server already published",																															//TEXT_SERVER_ALREADY_PUBLISHED
		"The session is disconnected",																													//TEXT_SESSION_DISCONNECTED
		"Bad parameter value",																																	//TEXT_BAD_PARAMETER_VALUE
		"Error getting node state",																															//TEXT_GET_NODE_STATE
		"An error occurred or received calling a function in the ACS CS API",										//TEXT_CS_API_CALL_ERROR
		"Error finding my ap partner node configuration info",																	//TEXT_FIND_MY_AP_PARTNER_NODE_HOST_INFO
		"Error while initializing an object manager towards SAF IMM platform",									//TEXT_SAF_IMM_OM_INIT
		"Error getting data from SAF IMM platform by an object manager",												//TEXT_SAF_IMM_OM_GET
		"A null attribute value got from SAF IMM metabase",																			//TEXT_SAF_IMM_NULL_ATTRIBUTE_VALUE
		"Bad node state",																																				//TEXT_BAD_NODE_STATE
		"Bad system id",																																				//TEXT_BAD_SYSTEM_ID
		"Address empty or null",																																//TEXT_ADDRESS_EMPTY_OR_NULL
		"Error while sending the OCP accept indication packet to a CP/BC",											//TEXT_OCP_SENDING_ACCEPT_INDICATION
		"Error while receiving the OCP accept confirmation packet from a CP/BC",								//TEXT_OCP_RECEIVING_ACCEPT_CONFIRMATION
		"Bad OCP Accept Confirmation response received: message size unexpected or connection rejected due to an error",					//TEXT_OCP_ACCEPT_CONFIRM_BAD_RESPONSE
		"The OCP send request message received from a CP/BC has an unexpected primitive id in its header, it is not equal to 6",	//TEXT_OCP_SEND_REQUEST_BAD_PRIM_ID_IN_HEADER
#ifdef ACS_DSD_WARNING_CUSTOM_MESSAGES_ENABLED
#warning "Aggiustare il messaggio precedente per quanto riguarda quel numero 6 in accordo a quanto registrato sul foglio ToDO 2"
#endif
		"The OCP send request message received from a CP/BC contains bad OCP and DSD sizes in its header: [OCP size] != [DSD size + 4]",	//TEXT_OCP_RECEIVED_HEADER_SIZES_MISMATCH
		"The buffer size exceeds the maximum size allowed into an OCP message",									//TEXT_OCP_BUFFER_SIZE_EXCEEDS_MAXIMUM_ALLOWED
		"A bad attribute value got from SAF IMM metabase",																			//TEXT_SAF_IMM_BAD_ATTRIBUTE_VALUE
		"This host/node has a network configuration that does not match any IP address in DSD library lists",	//TEXT_NODE_NETWORK_CONFIGURATION
		"Index out of range",																																		//TEXT_INDEX_OUT_OF_RANGE
		"One or more empty attributes into an object instance from IMM",												//TEXT_SAF_IMM_EMPTY_ATTRIBUTE_VALUE

		"CS API ERROR: CS api failed to create an HWC object",																	//TEXT_CS_CREATE_HWC_INSTANCE
		"CS API ERROR: CS api failed to create a board search object",													//TEXT_CS_CREATE_BOARD_SEARCH_INSTANCE
		"CS API ERROR: CS api failed to get board IDs",																					//TEXT_CS_GET_BOARD_IDS
		"CS API ERROR: CS api failed to get system ID",																					//TEXT_CS_GET_SYSTEM_ID
		"CS API ERROR: CS api failed to get system number",																			//TEXT_CS_GET_SYSTEM_NUMBER
		"CS API ERROR: CS api failed to get system sequence number",														//TEXT_CS_GET_SYSTEM_SEQUENCE
		"CS API ERROR: CS api failed to get node side",																					//TEXT_CS_GET_SIDE
		"CS API ERROR: CS api failed to get IP address of ethA",																//TEXT_CS_GET_IP_ETH_A
		"CS API ERROR: CS api failed to get IP address of ethB",																//TEXT_CS_GET_IP_ETH_B
		"CS API ERROR: CS api failed to get address of IP alias A",															//TEXT_CS_GET_ALIAS_ETH_A
		"CS API ERROR: CS api failed to get address of IP alias B",															//TEXT_CS_GET_ALIAS_ETH_B
		"CS API ERROR: CS api failed to set alarm master CP",																		//TEXT_CS_SET_ALARM_MASTER
		"CS API ERROR: CS api failed to set clock master CP",																		//TEXT_CS_SET_CLOCK_MASTER

		"CP state notifier object is not detached",																							//TEXT_CP_STATE_NOTIFIER_NOT_DETACHED
		"No sender objects available",																													//TEXT_SENDER_NOT_AVAILABLE
		"No receiver objects available",																												//TEXT_RECEIVER_NOT_AVAILABLE
		"CP state notifier object is not attached"																							//TEXT_CP_STATE_NOTIFIER_NOT_ATTACHED
	};
}

extern const char * const ACS_DSD_AP_NODE_STATE_STR[] = {"ACTIVE", "PASSIVE", "UNDEFINED"};
extern const char * const ACS_DSD_CP_NODE_STATE_STR[] = {"EXECUTION", "STANDBY", "UNDEFINED"};

const char * ACS_DSD_WarningTexts[] = {
	_texts[TEXT_OK],																			//WAR_NO_WARNINGS															=	0
	_texts[TEXT_ACCEPTOR_NOT_CLOSED],											//WAR_ACCEPTOR_NOT_CLOSED											=	1
	_texts[TEXT_ACCEPTOR_NOT_OPEN],												//WAR_ACCEPTOR_NOT_OPEN												=	2
	_texts[TEXT_STREAM_ALREADY_IN_USE],										//WAR_STREAM_ALREADY_IN_USE										=	3
	_texts[TEXT_WRONG_STREAM_TYPE],												//WAR_WRONG_STREAM_TYPE												=	4
	_texts[TEXT_WRONG_ADDRESS_TYPE],											//WAR_WRONG_ADDRESS_TYPE											=	5
	_texts[TEXT_REMOVE_NOT_ALLOWED],											//WAR_REMOVE_NOT_ALLOWED											=	6
	_texts[TEXT_IP4_ADDRESS_DOTTED_NOT_VALID],						//WAR_IP4_ADDRESS_DOTTED_NOT_VALID						=	7
	_texts[TEXT_INTERNAL_NET_CONFIG_NOT_LOADED],					//WAR_INTERNAL_NET_CONFIG_NOT_LOADED					=	8
	_texts[TEXT_SERVICE_MODE_UNKNOWN],										//WAR_SERVICE_MODE_UNKNOWN										= 9
	_texts[TEXT_SERVER_NOT_CLOSED],												//WAR_SERVER_NOT_CLOSED												=	10
	_texts[TEXT_SERVICE_ACCESS_POINT_NOT_VALID],					//WAR_SERVICE_ACCESS_POINT_NOT_VALID					=	11
	_texts[TEXT_SERVICE_ACCESS_POINT_OUT_OF_RANGE],				//WAR_SERVICE_ACCESS_POINT_OUT_OF_RANGE				=	12
	_texts[TEXT_SOME_NETWORKS_UNAVAILABLE],								//WAR_SOME_NETWORKS_UNAVAILABLE								=	13
	_texts[TEXT_SOME_INTERNAL_ACCEPTORS_NOT_OPEN],				//WAR_SOME_INTERNAL_ACCEPTORS_NOT_OPEN				= 14
	_texts[TEXT_SOME_NETWORKS_AND_ACCEPTORS_UNAVAILABLE],	//WAR_SOME_NETWORKS_AND_ACCEPTORS_UNAVAILABLE	=	15
	_texts[TEXT_SERVER_NOT_OPEN],													//WAR_SERVER_NOT_OPEN													=	16
	_texts[TEXT_SESSION_ALREADY_IN_USE],									//WAR_SESSION_ALREADY_IN_USE									=	17
	_texts[TEXT_TIMEOUT_EXPIRED],													//WAR_TIMEOUT_EXPIRED													=	18
	_texts[TEXT_NODE_NOT_FOUND],													//WAR_NODE_NOT_FOUND													=	19

	_texts[TEXT_PCP_PRIMITIVE_UNDEFINED],		//WAR_PCP_PRIMITIVE_UNDEFINED						=	20
	_texts[TEXT_PCP_VERSION_NOT_SUPPORTED],	//WAR_PCP_VERSION_NOT_SUPPORTED					=	21
	_texts[TEXT_PCP_DATA_BUFFER_OVERFLOW],	//WAR_PCP_DATA_BUFFER_OVERFLOW					=	22

	_not_defined_mesg,	//NOT DEFINED - 23
	_not_defined_mesg,	//NOT DEFINED - 24

	_texts[TEXT_SYSTEM_CLOSE]		//WAR_SYSTEM_CLOSE	=	25
};

const char * ACS_DSD_ErrorTexts[] = {
	_texts[TEXT_OK],																//ERR_NO_ERRORS														=	0
	_texts[TEXT_SYSTEM_ERROR],											//ERR_SYSTEM_ERROR												=	-1
	_texts[TEXT_SYSTEM_OPEN],												//ERR_SYSTEM_OPEN													=	-2
	_texts[TEXT_SYSTEM_ACCEPT],											//ERR_SYSTEM_ACCEPT												=	-3
	_texts[TEXT_SYSTEM_CLOSE],											//ERR_SYSTEM_CLOSE												=	-4
	_texts[TEXT_SYSTEM_REMOVE],											//ERR_SYSTEM_REMOVE												=	-5
	_texts[TEXT_SYSTEM_GET_ADDRESS],								//ERR_SYSTEM_GET_ADDRESS									=	-6
	_texts[TEXT_SYSTEM_CONNECT],										//ERR_SYSTEM_CONNECT											=	-7
	_texts[TEXT_SYSTEM_RECEIVE],										//ERR_SYSTEM_RECEIVE											=	-8
	_texts[TEXT_SYSTEM_SEND],												//ERR_SYSTEM_SEND													=	-9
	_texts[TEXT_SYSTEM_GET_OPTION],									//ERR_SYSTEM_GET_OPTION										=	-10
	_texts[TEXT_SYSTEM_SET_OPTION],									//ERR_SYSTEM_SET_OPTION										=	-11
	_texts[TEXT_SYSTEM_OPEN_DIRECTORY],							//ERR_SYSTEM_OPEN_DIRECTORY								=	-12
	_texts[TEXT_SYSTEM_OUT_OF_MEMORY],							//ERR_SYSTEM_OUT_OF_MEMORY								=	-13
	_texts[TEXT_SYSTEM_READ],												//ERR_SYSTEM_READ													=	-14
	_texts[TEXT_SYSTEM_CREATE],											//ERR_SYSTEM_CREATE												=	-15
	_texts[TEXT_SYSTEM_EPOLL_CTL_ADD],							//ERR_SYSTEM_EPOLL_CTL_ADD								=	-16
	_texts[TEXT_SYSTEM_IO_MULTIPLEXING_ACTIVATE],		//ERR_SYSTEM_IO_MULTIPLEXING_ACTIVATE			=	-17
	_texts[TEXT_SYSTEM_EPOLL_WAIT],									//ERR_SYSTEM_EPOLL_WAIT										=	-18
	_texts[TEXT_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE],	//ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE		=	-19

	_texts[TEXT_SERVICE_ACCESS_POINT_NOT_VALID],					//ERR_SERVICE_ACCESS_POINT_NOT_VALID					=	-20
	_texts[TEXT_SERVICE_ACCESS_POINT_OUT_OF_RANGE],				//ERR_SERVICE_ACCESS_POINT_OUT_OF_RANGE				=	-21
	_texts[TEXT_SERVICE_MODE_UNKNOWN],										//ERR_SERVICE_MODE_UNKNOWN										=	-22
	_texts[TEXT_SERVER_NOT_CLOSED],												//ERR_SERVER_NOT_CLOSED												= -23
	_texts[TEXT_SERVER_NOT_OPEN],													//ERR_SERVER_NOT_OPEN													=	-24
	_texts[TEXT_SESSION_ALREADY_IN_USE],									//ERR_SESSION_ALREADY_IN_USE									=	-25
	_texts[TEXT_HOST_CONFIG_LOAD],												//ERR_HOST_CONFIG_LOAD												=	-26
	_texts[TEXT_GET_PROCESS_INFORMATION],									//ERR_GET_PROCESS_INFORMATION									=	-27
	_texts[TEXT_WRONG_ACCEPTOR_TYPE],											//ERR_WRONG_ACCEPTOR_TYPE											=	-28
	_texts[TEXT_LOAD_AP_NODES_FROM_CS],										//ERR_LOAD_AP_NODES_FROM_CS										=	-29
	_texts[TEXT_FIND_MY_AP_NODE_HOST_INFO],								//ERR_FIND_MY_AP_NODE_HOST_INFO								=	-30
	_texts[TEXT_AP_CONFIGURATION_MISMATCH_FROM_CS],				//ERR_AP_CONFIGURATION_MISMATCH_FROM_CS				=	-31
	_texts[TEXT_CONFIG_NODE_IP_CONFLICT],									//ERR_CONFIG_NODE_IP_CONFLICT									=	-32
	_texts[TEXT_REMOTE_SYSTEM_TYPE_UNKNOWN],							//ERR_REMOTE_SYSTEM_TYPE_UNKNOWN							=	-33
	_texts[TEXT_WRONG_STREAM_TYPE],												//ERR_WRONG_STREAM_TYPE												=	-34
	_texts[TEXT_ACCEPTOR_NOT_OPEN],												//ERR_ACCEPTOR_NOT_OPEN												=	-35
	_texts[TEXT_STREAM_ALREADY_IN_USE],										//ERR_STREAM_ALREADY_IN_USE										=	-36
	_texts[TEXT_NODE_NOT_FOUND],													//ERR_NODE_NOT_FOUND													=	-37
	_texts[TEXT_SERVICE_MODE_IS_PRIVATE],									//ERR_SERVICE_MODE_IS_PRIVATE									=	-38
	_texts[TEXT_PARAMETER_EMPTY_OR_NULL],									//ERR_PARAMETER_EMPTY_OR_NULL									=	-39
	_texts[TEXT_SERVICE_PUBLISH],													//ERR_SERVICE_PUBLISH													=	-40
	_texts[TEXT_NOT_ENOUGH_SPACE],												//ERR_NOT_ENOUGH_SPACE												=	-41
	_texts[TEXT_ACCEPTORS_ADDRESSES_NOT_FOUND],						//ERR_ACCEPTORS_ADDRESSES_NOT_FOUND						=	-42
	_texts[TEXT_PEER_CLOSED_CONNECTION],									//ERR_PEER_CLOSED_CONNECTION									=	-43
	_texts[TEXT_OBJECT_CORRUPTED],												//ERR_OBJECT_CORRUPTED												=	-44
	_texts[TEXT_SERVER_ALREADY_PUBLISHED],								//ERR_SERVER_ALREADY_PUBLISHED								=	-45
	_texts[TEXT_SESSION_DISCONNECTED],										//ERR_SESSION_DISCONNECTED										=	-46
	_texts[TEXT_BAD_PARAMETER_VALUE],											//ERR_BAD_PARAMETER_VALUE											=	-47
	_texts[TEXT_GET_NODE_STATE],													//ERR_GET_NODE_STATE													=	-48
	_texts[TEXT_CS_API_CALL_ERROR],												//ERR_CS_API_CALL_ERROR												=	-49
	_texts[TEXT_FIND_MY_AP_PARTNER_NODE_HOST_INFO],				//ERR_FIND_MY_AP_PARTNER_NODE_HOST_INFO				=	-50
	_texts[TEXT_SAF_IMM_OM_INIT],													//ERR_SAF_IMM_OM_INIT													=	-51
	_texts[TEXT_SAF_IMM_OM_GET],													//ERR_SAF_IMM_OM_GET													=	-52
	_texts[TEXT_SAF_IMM_NULL_ATTRIBUTE_VALUE],						//ERR_SAF_IMM_NULL_ATTRIBUTE_VALUE						=	-53
	_texts[TEXT_BAD_NODE_STATE],													//ERR_BAD_NODE_STATE													=	-54
	_texts[TEXT_BAD_SYSTEM_ID],														//ERR_BAD_SYSTEM_ID														=	-55
	_texts[TEXT_ADDRESS_EMPTY_OR_NULL],										//ERR_ADDRESS_EMPTY_OR_NULL										=	-56
	_texts[TEXT_OCP_SENDING_ACCEPT_INDICATION],						//ERR_OCP_SENDING_ACCEPT_INDICATION						=	-57
	_texts[TEXT_OCP_RECEIVING_ACCEPT_CONFIRMATION],				//ERR_OCP_RECEIVING_ACCEPT_CONFIRMATION				=	-58
	_texts[TEXT_OCP_ACCEPT_CONFIRM_BAD_RESPONSE],					//ERR_OCP_ACCEPT_CONFIRM_BAD_RESPONSE					=	-59
	_texts[TEXT_OCP_SEND_REQUEST_BAD_PRIM_ID_IN_HEADER], 	//ERR_OCP_SEND_REQUEST_BAD_PRIM_ID_IN_HEADER	=	-60
	_texts[TEXT_SAF_IMM_BAD_ATTRIBUTE_VALUE],							//ERR_SAF_IMM_BAD_ATTRIBUTE_VALUE							=	-61
	_texts[TEXT_NODE_NETWORK_CONFIGURATION],							//ERR_NODE_NETWORK_CONFIGURATION							=	-62
	_texts[TEXT_INDEX_OUT_OF_RANGE],											//ERR_INDEX_OUT_OF_RANGE											=	-63
	_texts[TEXT_SAF_IMM_EMPTY_ATTRIBUTE_VALUE],						//ERR_SAF_IMM_EMPTY_ATTRIBUTE_VALUE						=	-64

	_texts[TEXT_CS_CREATE_HWC_INSTANCE],									//ERR_CS_CREATE_HWC_INSTANCE									=	-65
	_texts[TEXT_CS_CREATE_BOARD_SEARCH_INSTANCE],					//ERR_CS_CREATE_BOARD_SEARCH_INSTANCE					=	-66
	_texts[TEXT_CS_GET_BOARD_IDS],												//ERR_CS_GET_BOARD_IDS												=	-67
	_texts[TEXT_CS_GET_SYSTEM_ID],												//ERR_CS_GET_SYSTEM_ID												=	-68
	_texts[TEXT_CS_GET_SYSTEM_NUMBER],										//ERR_CS_GET_SYSTEM_NUMBER										=	-69
	_texts[TEXT_CS_GET_SYSTEM_SEQUENCE],									//ERR_CS_GET_SYSTEM_SEQUENCE									=	-70
	_texts[TEXT_CS_GET_SIDE],															//ERR_CS_GET_SIDE															=	-71
	_texts[TEXT_CS_GET_IP_ETH_A],													//ERR_CS_GET_IP_ETH_A													=	-72
	_texts[TEXT_CS_GET_IP_ETH_B],													//ERR_CS_GET_IP_ETH_B													=	-73
	_texts[TEXT_CS_GET_ALIAS_ETH_A],											//ERR_CS_GET_ALIAS_ETH_A											=	-74
	_texts[TEXT_CS_GET_ALIAS_ETH_B],											//ERR_CS_GET_ALIAS_ETH_B											=	-75
	_texts[TEXT_CS_SET_ALARM_MASTER],											//ERR_CS_SET_ALARM_MASTER											=	-76
	_texts[TEXT_CS_SET_CLOCK_MASTER],											//ERR_CS_SET_CLOCK_MASTER											=	-77

	_texts[TEXT_PCP_PRIMITIVE_UNDEFINED],								//ERR_PCP_PRIMITIVE_UNDEFINED								=	-78
	_texts[TEXT_PCP_VERSION_NOT_SUPPORTED],							//ERR_PCP_VERSION_NOT_SUPPORTED							=	-79
	_texts[TEXT_PCP_DATA_BUFFER_OVERFLOW],							//ERR_PCP_DATA_BUFFER_OVERFLOW							=	-80
	_texts[TEXT_PCP_BAD_VERSION_IN_PRIMITIVE_DATA],			//ERR_PCP_BAD_VERSION_IN_PRIMITIVE_DATA			=	-81
	_texts[TEXT_PCP_BAD_PRIMITIVE_IN_PRIMITIVE_DATA],		//ERR_PCP_BAD_PRIMITIVE_IN_PRIMITIVE_DATA		=	-82
	_texts[TEXT_PCP_UNEXPECTED_PRIMITIVE_RECEIVED],			//ERR_PCP_UNEXPECTED_PRIMITIVE_RECEIVED			=	-83
	_texts[TEXT_PCP_PRIMITIVE_DATA_NOT_READY_TO_SEND],	//ERR_PCP_PRIMITIVE_DATA_NOT_READY_TO_SEND	=	-84

	_texts[TEXT_CP_STATE_NOTIFIER_NOT_DETACHED],				//ERR_CP_STATE_NOTIFIER_NOT_DETACHED				=	-85
	_texts[TEXT_SENDER_NOT_AVAILABLE],									//ERR_SENDER_NOT_AVAILABLE									=	-86
	_texts[TEXT_RECEIVER_NOT_AVAILABLE],								//ERR_RECEIVER_NOT_AVAILABLE								=	-87
	_texts[TEXT_CP_STATE_NOTIFIER_NOT_ATTACHED],				//ERR_CP_STATE_NOTIFIER_NOT_ATTACHED				=	-88
	_texts[TEXT_OCP_RECEIVED_HEADER_SIZES_MISMATCH],		//ERR_OCP_RECEIVED_HEADER_SIZES_MISMATCH		=	-89

	_texts[TEXT_PCP_UNEXPECTED_ERROR_CODE],												//ERR_PCP_UNEXPECTED_ERROR_CODE												=	-90
	_texts[TEXT_PCP_ERROR_CODE_NODE_NOT_VALID],										//ERR_PCP_ERROR_CODE_NODE_NOT_VALID										=	-91
	_texts[TEXT_PCP_ERROR_CODE_FILE_STORAGE_FAILURE],							//ERR_PCP_ERROR_CODE_FILE_STORAGE_FAILURE							=	-92
	_texts[TEXT_PCP_ERROR_CODE_ADDRESS_NOT_VALID],								//ERR_PCP_ERROR_CODE_ADDRESS_NOT_VALID								=	-93
	_texts[TEXT_PCP_ERROR_CODE_APPLICATION_NOT_VALID],						//ERR_PCP_ERROR_CODE_APPLICATION_NOT_VALID						=	-94
	_texts[TEXT_PCP_ERROR_CODE_NAME_NOT_UNIQUE],									//ERR_PCP_ERROR_CODE_NAME_NOT_UNIQUE									=	-95
	_texts[TEXT_PCP_ERROR_CODE_APPLICATION_NOT_REGISTERED],				//ERR_PCP_ERROR_CODE_APPLICATION_NOT_REGISTERED				=	-96
	_texts[TEXT_PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED],						//ERR_PCP_ERROR_CODE_ADDRESS_REQUEST_FAILED						=	-97
	_texts[TEXT_PCP_ERROR_CODE_APPLICATION_ALREADY_REGISTERED],		//ERR_PCP_ERROR_CODE_APPLICATION_ALREADY_REGISTERED		=	-98
	_texts[TEXT_PCP_ERROR_CODE_NO_IP_ADDRESS_AVAILABLE],					//ERR_PCP_ERROR_CODE_NO_IP_ADDRESS_AVAILABLE					=	-99
	_texts[TEXT_PCP_ERROR_CODE_NO_PORT_AVAILABLE],								//ERR_PCP_ERROR_CODE_NO_PORT_AVAILABLE								=	-100
	_texts[TEXT_PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE],					//ERR_PCP_ERROR_CODE_ILLEGAL_CONNECTION_TYPE					=	-101
	_texts[TEXT_PCP_ERROR_CODE_REGISTER_FAILURE],									//ERR_PCP_ERROR_CODE_REGISTER_FAILURE									=	-102
	_texts[TEXT_PCP_ERROR_CODE_UNREGISTER_FAILURE],								//ERR_PCP_ERROR_CODE_UNREGISTER_FAILURE								=	-103
	_texts[TEXT_PCP_ERROR_CODE_CONNECTION_ADDRESS_NUM_MISMATCH],	//ERR_PCP_ERROR_CODE_CONNECTION_ADDRESS_NUM_MISMATCH	=	-104
	_texts[TEXT_PCP_ERROR_CODE_BAD_IP_ADDRESSES_COUNT],						//ERR_PCP_ERROR_CODE_BAD_IP_ADDRESSES_COUNT						=	-105
	_texts[TEXT_PCP_ERROR_CODE_PROTOCOL_VERSION_NOT_SUPPORTED],		//ERR_PCP_ERROR_CODE_PROTOCOL_VERSION_NOT_SUPPORTED		=	-106
	_texts[TEXT_PCP_ERROR_CODE_ILLEGAL_ELEMENT_VALUE],						//ERR_PCP_ERROR_CODE_ILLEGAL_ELEMENT_VALUE						=	-107
	_texts[TEXT_PCP_ERROR_CODE_HOST_UNREACHABLE],									//ERR_PCP_ERROR_CODE_HOST_UNREACHABLE									=	-108
	_texts[TEXT_PCP_ERROR_CODE_OPERATION_NOT_ALLOWED],						//ERR_PCP_ERROR_CODE_OPERATION_NOT_ALLOWED						=	-109

	_not_defined_mesg,	//NOT DEFINED - 110
	_not_defined_mesg,	//NOT DEFINED - 111
	_not_defined_mesg,	//NOT DEFINED - 112
	_not_defined_mesg,	//NOT DEFINED - 113
	_not_defined_mesg,	//NOT DEFINED - 114

	_texts[TEXT_SYSTEM_POLL],																			//ERR_SYSTEM_POLL																			=	-115
	_texts[TEXT_OCP_BUFFER_SIZE_EXCEEDS_MAXIMUM_ALLOWED]					//ERR_OCP_BUFFER_SIZE_EXCEEDS_MAXIMUM_ALLOWED					=	-116
};
