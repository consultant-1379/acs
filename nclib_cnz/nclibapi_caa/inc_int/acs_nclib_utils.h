/** @file acs_nclib_utils.h
 *	@brief
 *	@author estevol (Stefano Volpe)
 *	@date 2012-10-06
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
 *	| R-001 | 2012-10-06 | estevol      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#ifndef ACS_NCLIB_UTILS_H_
#define ACS_NCLIB_UTILS_H_

//message
namespace ACS_NCLIB_XML_TAG {
	static const char XML_HEADER[] 		= "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	static const char XML_MSG_SEP_1_0[] = "]]>]]>";
	static const char XML_MSG_SEP_1_1[]	= "\n##\n";
}

namespace ACS_NCLIB_NS_TAG {
	static const char NAMESPACE_1_0[] = "xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">\n";
	static const char NAMESPACE_1_1[] = "xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.1\">\n";
}

//rpc
namespace ACS_NCLIB_RPC_TAG {
	static const char HEADER[] 					= "<rpc ";
    static const char MESSAGEID_HEADER[] 		= "<rpc message-id=";
    static const char TRAILER[] 				= "\n</rpc>";
}

//rpc_reply
namespace ACS_NCLIB_RPC_REPLY_TAG {
	static const char HEADER[] 					= "<rpc-reply ";
    static const char MESSAGEID_HEADER[] 		= "<rpc-reply message-id=";
    static const char TRAILER[] 				= "\n</rpc-reply>";
    static const char OK[] 						= "<ok/>";
    static const char DATA_HEADER[] 			= "<data>\n";
    static const char DATA_TRAILER[] 			= "</data>";
}

namespace ACS_NCLIB_HELLO_TAG {
	static const char HEADER[] 						= "<hello ";
	static const char CAPABILITIES_HEADER[]			= "<capabilities>";
	static const char CAPABILITIES_TRAILER[]		= "</capabilities>";
	static const char CAPABILITY_HEADER[]			= "<capability>";
	static const char CAPABILITY_TRAILER[]			= "</capability>";
	static const char SESSIONID_HEADER[]			= "<session-id>";
	static const char SESSIONID_TRAILER[]			= "</session-id>";
	static const char TRAILER[] 					= "</hello>";

}


//filter
namespace ACS_NCLIB_FILTER_TAG {
    static const char SUBTREE_HEADER[]   		= "<filter type=\"subtree\">\n";
    static const char TRAILER[]          		= "</filter>";
}

namespace ACS_NCLIB_GETCONFIG_TAG {
    static const char HEADER[]    				= "<get-config>\n";
    static const char TRAILER[]   				= "\n</get-config>";
}

namespace ACS_NCLIB_EDITCONFIG_TAG {
    static const char HEADER[]    				= "<edit-config>\n";
    static const char TRAILER[]   				= "\n</edit-config>";
}

namespace ACS_NCLIB_LOCK_TAG {
    static const char HEADER[]    				= "<lock>\n";
    static const char TRAILER[]   				= "\n</lock>";
}

namespace ACS_NCLIB_UNLOCK_TAG {
    static const char HEADER[]    				= "<unlock>\n";
    static const char TRAILER[]   				= "\n</unlock>";
}

namespace ACS_NCLIB_GET_TAG {
    static const char HEADER[]    = "<get>\n";
    static const char TRAILER[]   = "\n</get>";
}

namespace ACS_NCLIB_ACTION_TAG {
    static const char HEADER[]    				= "<action>\n";
    static const char NS_HEADER[]    			= "<action xmlns=\"";
    static const char NS_HEADER_CLOSE[]			= "\">";
    static const char TRAILER[]   				= "\n</action>";
    static const char DATA_HEADER[] 			= "<data>\n";
    static const char DATA_TRAILER[] 			= "</data>";
}

namespace ACS_NCLIB_CLOSE_SESSION_TAG {
    static const char OPERATION[]    = "<close-session/>\n";
}

namespace ACS_NCLIB_COMMIT_TAG {
    static const char OPERATION[]    = "<commit/>\n";
}

namespace ACS_NCLIB_DATASTORE_TAG {
    static const char SOURCE_HEADER[]    = "<source>\n";
    static const char TARGET_HEADER[]    = "<target>\n";
    static const char RUNNING[]          = "<running/>";
    static const char CANDIDATE[]        = "<candidate/>";
    static const char SOURCE_TRAILER[]   = "\n</source>";
    static const char TARGET_TRAILER[]   = "\n</target>";
}

namespace ACS_NCLIB_CONFIG_TAG {
    static const char HEADER_1_0[]  = "<config xmlns:xc=\"urn:ietf:params:xml:ns:netconf:base:1.0\">\n";
    static const char HEADER_1_1[]  = "<config xmlns:xc=\"urn:ietf:params:xml:ns:netconf:base:1.1\">\n";
    static const char TRAILER[]   	= "\n</config>";
}

namespace ACS_NCLIB_DEFAULT_OPERATION_TAG {
    static const char MERGE[]   = "<default-operation>merge</default-operation>\n";
    static const char REPLACE[] = "<default-operation>replace</default-operation>\n";
    static const char NONE[]    = "<default-operation>none</default-operation>\n";
}

namespace ACS_NCLIB_ERROR_OPTION_TAG {
    static const char STOP[]       = "<error-option>stop-on-error</error-option>\n";
    static const char CONTINUE[]   = "<error-option>continue-on-error</error-option>\n";
    static const char ROLLBACK[]   = "<error-option>rollback-on-error</error-option>\n";
}

namespace ACS_NCLIB_TEST_OPTION_TAG {
    static const char TEST_THEN_SET[]       = "<test-option>test-then-set</test-option>\n";
    static const char SET[]                 = "<test-option>set</test-option>\n";
    static const char TEST_ONLY[]       	= "<test-option>test-only</test-option>\n";
}


namespace ACS_NCLIB_ERROR_TAG {
	static const char HEADER[] 				= "<rpc-error>";
	static const char TRAILER[] 			= "\n</rpc-error>";
	static const char ERR_TYPE_HAEDER[] 	= "<error-type>";
	static const char ERR_TYPE_TRAILER[] 	= "</error-tag>";
	static const char ERR_TAG_HAEDER[] 		= "<error-tag>";
	static const char ERR_TAG_TRAILER[] 	= "</error-tag>";
	static const char ERR_SEV_HAEDER[] 		= "<error-severity>";
	static const char ERR_SEV_TRAILER[] 	= "</error-severity>";
	static const char ERR_INFO_HAEDER[] 	= "<error-info>";
	static const char ERR_INFO_TRAILER[] 	= "</error-info>";
	static const char ERR_MSG_HAEDER[] 		= "<error-message>";
	static const char ERR_MSG_TRAILER[] 	= "</error-message>";
	static const char ERR_APP_TAG_HAEDER[] 	= "<error-app-tag>";
	static const char ERR_APP_TAG_TRAILER[] = "</error-app-tag>";
	static const char ERR_PATH_HAEDER[] 	= "<error-path>";
	static const char ERR_PATH_TRAILER[] 	= "</error-path>";

}

#endif
