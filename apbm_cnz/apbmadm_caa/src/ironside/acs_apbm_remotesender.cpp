/* 
 * File:   acs_apbm_remotesender.cpp
 * Author: renato
 * 
 * Created on 9 ottobre 2012, 16.43
 */

#include "acs_apbm_remotesender.h"
#include "acs_apbm_commandbuilder.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "acs_apbm_logger.h"
#include "acs_nclib_session.h"
#include "acs_nclib_message.h"
#include "acs_nclib_hello.h"
#include "acs_nclib_rpc.h"
#include "acs_nclib_factory.h"
#include "acs_nclib_library.h"
#include "acs_apbm_macros.h"
#include "stdio.h"
#include "stdlib.h"
#include "acs_nclib_udp_reply.h"
#include "acs_nclib_udp.h"
#define DMX_PORT 	        831 	
//#define DMX_ACTION_NS	"http://tail-f.com/ns/netconf/actions/1.0"
#define DMX_ACTION_NS "urn:com:ericsson:ecim:1.0"

#define DMX_DEFAULT_TIMEOUT_MS	5000


acs_apbm_remotesender::acs_apbm_remotesender(char target_a[16], char target_b[16], unsigned q_timeout)
{
	_timeout_ms = (q_timeout > 0)? q_timeout: DMX_DEFAULT_TIMEOUT_MS;

	::strncpy(_dmxc_addresses[0], target_a, ACS_APBM_ARRAY_SIZE(_dmxc_addresses[0]));
	::strncpy(_dmxc_addresses[1], target_b, ACS_APBM_ARRAY_SIZE(_dmxc_addresses[1]));

}

acs_apbm_remotesender::~acs_apbm_remotesender()
{
    
}

const char* acs_apbm_remotesender::getErrorMessage()
{
        return NULL;
}

int acs_apbm_remotesender::getErrorType()
{
    return message_error_type;
}

int acs_apbm_remotesender::getErrorSeverity()
{
    return message_error_severity;
}

const char* acs_apbm_remotesender::getErrorMessageInfo()
{
    return message_error_info;
}

int acs_apbm_remotesender::sendUdpMessage(std::string query)
{
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s with with%s", __FUNCTION__,query.c_str());
	std::string xml_filter = query;
        bool query_sent = false;
        udp_data = std::string("");

        int errfl=0;
        acs_nclib_session session(acs_nclib::UDP);
        acs_nclib_udp* udp = acs_nclib_factory::create_udp(acs_nclib::OP_UDP_GET);

        for(int i=0; i<2 && !query_sent;i++)
        {
        if (session.open(_dmxc_addresses[i],DMX_PORT,acs_nclib::USER_AUTH_NONE,"") != acs_nclib::ERR_NO_ERRORS)
        {
                std::cout << "acs_apbm_RemoteSender::sendUdpMessage : session.open failed"<<_dmxc_addresses[i]<<std::endl;
                errfl=-1;
                continue;
        }
                udp->set_cmd(query);
                acs_nclib_message* answer = 0;
                acs_nclib_udp_reply * reply=0;
                int result = 0;
                if ((result = session.send(udp)) == 0)
                {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Message sent ");
                        if (session.receive(answer, 3000) == acs_nclib::ERR_NO_ERRORS)
                        {
                                reply=dynamic_cast<acs_nclib_udp_reply*>(answer);
                                if(reply!=0 )
                                        {
                                                reply->get_data(udp_data);
                                                errfl=0;
						ACS_APBM_LOG(LOG_LEVEL_DEBUG, "reply received %s",udp_data.c_str());
                                        }
					else
                                        {
                                                std::cout << "DBG: Receive message have different id or reply error" << std::endl;
                                                errfl=-1;
                                        }
                                query_sent =true;
                        }
                        else
                        {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Receive failed ");
                                errfl=-1;

                        }

                       if (answer)
                                acs_nclib_factory::dereference(answer);
                }
                else
                {
		       ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Sent failed");
                       errfl=-1;
                }

                if (session.close() == 0)
                {
		       ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Session Closed");	
                }

        }
	if (udp)
                {
                        acs_nclib_factory::dereference(udp);
                }

        return errfl;
	
}
std::string acs_apbm_remotesender::getUdpData()
{
    return udp_data;
}
