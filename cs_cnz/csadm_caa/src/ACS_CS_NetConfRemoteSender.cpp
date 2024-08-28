/* 
 * File:   ACS_CS_NetConfRemoteSender.cpp
 * Author: renato
 * 
 * Created on 9 ottobre 2012, 16.43
 */

#include "ACS_CS_NetConfRemoteSender.h"
#include "ACS_CS_NetConfBuilderCommand.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "acs_nclib_session.h"
#include "acs_nclib_message.h"
#include "acs_nclib_hello.h"
#include "acs_nclib_rpc.h"
#include "acs_nclib_udp_reply.h"
#include "acs_nclib_udp.h"
#include "acs_nclib_factory.h"
#include "acs_nclib_library.h"
#include "stdio.h"
#include "stdlib.h"

#include "ACS_CS_Registry.h"

#define DMXC_PORT 831

ACS_CS_NetConfRemoteSender::ACS_CS_NetConfRemoteSender():
data(""),udp_data(""),message_error(),message_error_type(0),message_error_severity(0),message_error_info(),ipaddress("0.0.0.0"),port(120)
{

}

ACS_CS_NetConfRemoteSender::~ACS_CS_NetConfRemoteSender()
{
    
}
int ACS_CS_NetConfRemoteSender::sendUdpMessage(std::string query)
{
	udp_data=std::string("");

	uint32_t bspAddress[2] = {0};

        ACS_CS_Registry::getDmxcAddress(bspAddress[0], bspAddress[1]);

        acs_nclib_session session(acs_nclib::UDP);

	bool bsp1=true;
        int errfl=0;
	for(int i=0; i<2;i++)
	{
        if (session.open( bsp1?bspAddress[0]:bspAddress[1],DMXC_PORT,acs_nclib::USER_AUTH_NONE,"") != acs_nclib::ERR_NO_ERRORS)
        {

                std::cout << "ACS_CS_NetConfRemoteSender::sendUdpMessage : session.open 1"<<bsp1<<std::endl;

               /* if (session.open(bspAddress[1],DMXC_PORT,acs_nclib::USER_AUTH_NONE,"") != acs_nclib::ERR_NO_ERRORS)
                {
                        std::cout << "ACS_CS_NetConfRemoteSender::sendUdpMessage : session.open 2"<<std::endl;
                   //     errfl=-1;
                     //   message_error_type=session.last_error_code();
                       // message_error_info=session.last_error_text();
                        //message_error_severity=0;
                }*/
        }
                acs_nclib_udp* udp = acs_nclib_factory::create_udp(acs_nclib::OP_UDP_GET);

                udp->set_cmd(query);
		acs_nclib_message* answer = 0;
		acs_nclib_udp_reply * reply=0;
                int result = 0;
                if ((result = session.send(udp)) == 0)
                {
                        std::cout << "\n\nUDC Get Message sent:" << udp << std::endl;

                        if (session.receive(answer, 2000) == acs_nclib::ERR_NO_ERRORS)
                        {
				reply=dynamic_cast<acs_nclib_udp_reply*>(answer);
				if((reply!=0) &&  (reply->get_message_id() == udp->get_message_id()))
                                        {
                                                reply->get_data(udp_data);
                                                errfl=0;
                                                std::cout << "\n\nAnswer Received:" << data.c_str() << std::endl;
						break;
                                        }
				else
                                        {
                                                std::cout << "DBG: Receive message have different id or reply error" << std::endl;
						break;
                                        }

                        }
                        else
                        {
                                std::cout << "DBG: Receive Failed, error" << std::endl;
				errfl=-1;
				bsp1=false;
                        }

		if (answer)
                                acs_nclib_factory::dereference(answer);
			}	
                else
                {
                        std::cout << "Send Failed. RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
			errfl=-1;
			bsp1=false;
                }

		
                if (udp)
                        acs_nclib_factory::dereference(udp);


                if (session.close() == 0)
                        std::cout << "Session Closed" << std::endl;
}
	return errfl;

}

int ACS_CS_NetConfRemoteSender::sendNetconfMessage(std::string query)
{

    std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage : Entering Method"<<std::endl;
    this->xml_filter=query;
    
    data=std::string("");

//	char pwd[20];
//	char user[20];
//	char addr1[20];
//	char addr2[20];
//
//	sprintf(addr1,"%s","192.254.15.1");
//	sprintf(addr2,"%s","192.254.16.1");
//	sprintf(pwd,"%s","tre,14");
//	sprintf(user,"%s","root");

    uint32_t bspAddress[2] = {0};

    ACS_CS_Registry::getDmxcAddress(bspAddress[0], bspAddress[1]);

	acs_nclib_session session(acs_nclib::TCP);


	int errfl=0;

	if (session.open(bspAddress[0],DMXC_PORT,acs_nclib::USER_AUTH_NONE,"") != acs_nclib::ERR_NO_ERRORS)
	{

		std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage : session.open 1"<<std::endl;

		if (session.open(bspAddress[1],DMXC_PORT,acs_nclib::USER_AUTH_NONE,"") != acs_nclib::ERR_NO_ERRORS)
		{
			std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage : session.open 2"<<std::endl;
			errfl=-1;
			message_error_type=session.last_error_code();
			message_error_info=session.last_error_text();
			message_error_severity=0;
		}
	}

	if(errfl!=0)
        {
                 std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage Error! errfl "<<errfl<<std::endl;
		return errfl;
        }

	acs_nclib_rpc* rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_GET);
	acs_nclib_filter* filter = acs_nclib_factory::create_filter(acs_nclib::FILTER_SUBTREE);

	if(!filter)
        {
		return -1;
        }

	filter->set_filter_content(query.c_str());

	if(!rpc)
		return -1;
	rpc->add_filter(filter);

	errfl=-1;

	if (session.send(rpc) == acs_nclib::ERR_NO_ERRORS)
	{

                std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage::session.send(rpc)="<<rpc<<std::endl;
		acs_nclib_message* answer = 0;
		acs_nclib_rpc_reply * reply=0;

		if (session.receive(answer, 2000) == acs_nclib::ERR_NO_ERRORS)
		{

			reply=dynamic_cast<acs_nclib_rpc_reply*>(answer);

			if(reply!=0)
			{

				if(reply->get_reply_type()==acs_nclib::REPLY_DATA)
				{
					data=reply->get_data();
					errfl=0;
				}
			}
		} else
		{
			std::cout << "DBG: Netconf Receive Failed, error" << session.last_error_code() << ", " << session.last_error_text() << std::endl;
		}


		if(errfl != 0)
		{
                         std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage::Error ! errfl != 0"<<std::endl;

			if(reply && (reply->get_reply_type() == acs_nclib::REPLY_DATA_WITH_ERRORS || reply->get_reply_type() == acs_nclib::REPLY_ERROR))
			{
				std::vector<acs_nclib_rpc_error*> errors;
				reply->get_error_list(errors);

				if (errors.size() > 0)
				{
                                        std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage:: errors.size() > 0 >>>>>"<<std::endl;
					message_error_type=errors[0]->get_error_type();
					message_error_info=errors[0]->get_error_message();
					message_error_severity=0;
                                        std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage:: message_error_type >>>>>"<<message_error_type<<std::endl;
                                        std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage:: message_error_info >>>>>"<<message_error_info<<std::endl;
				}

				acs_nclib_factory::dereference(errors);

			}
			else
			{
				message_error_type=session.last_error_code();
				message_error_info=session.last_error_text();
				message_error_severity=0;
                                std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage::message_error_type >>>>>"<<message_error_type<<std::endl;
                                std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage::message_error_info >>>>>"<<message_error_info<<std::endl;

			}
		}

		if (answer)
			acs_nclib_factory::dereference(answer);
	}
	else
	{
		errfl=-1;

		message_error_type=session.last_error_code();
		message_error_info=session.last_error_text();
		message_error_severity=0;
                std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage::session.send failed "<<std::endl;
                std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage::message_error_type >>>>>"<<message_error_type<<std::endl;
                std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage::message_error_info >>>>>"<<message_error_info<<std::endl;

	}

	if (rpc)
		acs_nclib_factory::dereference(rpc);

	if (filter)
		acs_nclib_factory::dereference(filter);


	if (session.close() == 0)
	{
		std::cout << "Session Closed" << std::endl;
	}


        std::cout << "ACS_CS_NetConfRemoteSender::sendNetconfMessage::Leaving function with errfl ="<<errfl<<std::endl;
	return errfl;
}

const char* ACS_CS_NetConfRemoteSender::getErrorMessage()
{
        return NULL;
}

int ACS_CS_NetConfRemoteSender::getErrorType()
{
    return message_error_type;
}

std::string ACS_CS_NetConfRemoteSender::getData()
{
    return data;
}
std::string ACS_CS_NetConfRemoteSender::getUdpData()
{
    return udp_data;
}
int ACS_CS_NetConfRemoteSender::getErrorSeverity()
{
    return message_error_severity;
}

const char* ACS_CS_NetConfRemoteSender::getErrorMessageInfo()
{
    return message_error_info;
}

