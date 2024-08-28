#include "acs_nclib_session.h"
#include "acs_nclib_message.h"
#include "acs_nclib_udp.h"
#include "acs_nclib_udp_reply.h"
#include "acs_nclib_factory.h"
#include "acs_nclib_library.h"
#include "stdio.h"
#include "stdlib.h"

int main (int argc, char * argv []) {


	const char * addr;
	const char * user="";
	int iterations = 1;
	std::string data;
	int cmd[12]={0x23,0x07,0x01,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char cmdascii[12]={0};
	unsigned char temp[12]={0};
	cmdascii[0]=(unsigned char)cmd[0];
	cmdascii[1]=(unsigned char)cmd[1];
	cmdascii[2]=(unsigned char)cmd[2];
	cmdascii[3]=(unsigned char)cmd[3];
	cmdascii[4]=(unsigned char)cmd[4];
	cmdascii[5]=(unsigned char)cmd[5];
	cmdascii[6]=(unsigned char)cmd[6];
	cmdascii[7]=(unsigned char)cmd[7];
	cmdascii[8]=(unsigned char)cmd[8];
	cmdascii[9]=(unsigned char)cmd[9];
	cmdascii[10]=(unsigned char)cmd[10];
	cmdascii[11]=(unsigned char)cmd[11];
	int it = 0;
	int errfl=-1;
//	std::string mac_cmd("SHLF--------");
//	std::string mac_cmd("BLD:0003MAC-");
	std::string mac_cmd(reinterpret_cast<const char *> (cmdascii),
                     sizeof (cmdascii) / sizeof (cmdascii[0]));

	int length = int(mac_cmd.length());
	std::cout<<"length =" << length;
	unsigned char *buff = new unsigned char[length+1];
	memcpy(buff, mac_cmd.c_str(), length+1);
	
//	strcpy(temp,reinterpret_cast<const char *> mac_cmd);
	if(cmdascii[0] != buff[0])
		std::cout << "copm failed for 0\n";
	else if(cmdascii[1] != buff[1])
		std::cout << "copm failed for 0\n";
	else if(cmdascii[2] != buff[2])
		std::cout << "copm failed for 0\n";
	else if(cmdascii[3] != buff[3])
		std::cout << "copm failed for 0\n";
	else if(cmdascii[4] != buff[4])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[5] != buff[5])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[6] != buff[6])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[7] != buff[7])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[8] != buff[8])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[9] != buff[9])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[10] != buff[10])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[11] != buff[11])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[12] != buff[12])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[13] != buff[13])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[14] != buff[14])
		std::cout << "copm correct for 0\n";
	else if(cmdascii[15] != buff[15])
		std::cout << "copm correct for 0\n";
	else
		std::cout << "correct.......\n";

	if (argc == 2)
	{
		addr = argv[1];
	}
	else
	{
		std::cout << "Usage udp_client <ip_address>" << std::endl;
		return 1;
	}


	do
	{
		acs_nclib_session session(acs_nclib::UDP);

		if (session.open(addr,831,acs_nclib::USER_AUTH_NONE,user) != acs_nclib::ERR_NO_ERRORS)
		{
			std::cout << "Open Failed......." << std::endl;
			return 1;
		}

		acs_nclib_udp* udp = acs_nclib_factory::create_udp(acs_nclib::OP_UDP_GET);

		udp->set_cmd(mac_cmd);

		//acs_nclib_rpc_reply* answer = 0;
		int result = 0;
	
//		while(true)
//		{

			if ((result = session.send(udp)) == 0)
			{
				std::cout << "\n\nUDP Get Message sent:" << udp << std::endl;
				//sleep(1);

				acs_nclib_message* answer = 0;
				acs_nclib_udp_reply * reply=0;
				if (session.receive(answer, 2000) == acs_nclib::ERR_NO_ERRORS)
				{
					reply=dynamic_cast<acs_nclib_udp_reply*>(answer);
					if((reply!=0) &&  (reply->get_message_id() == udp->get_message_id()))
	                       		{
						reply->get_data(data);
						errfl=0;
						std::cout << "\n\nAnswer Received:\n" << data.c_str() << std::endl;
					}
					else
					{
                        			std::cout << "DBG: Receive message have different id or reply error" << std::endl;
					}
                        	}
				else
				{
                        		std::cout << "DBG: Receive Failed, error" << std::endl;
                		}
				
				if (answer)
					acs_nclib_factory::dereference(answer);

			}	
			else
			{	
				std::cout << "Send Failed. RC = " << session.last_error_code() << " " << session.last_error_text() << std::endl;
			}


		if (udp)
			acs_nclib_factory::dereference(udp);

//		if (msg)
//			acs_nclib_factory::dereference(msg);

		if (session.close() == 0)
			std::cout << "Session Closed" << std::endl;

		it++;
	} while (it != iterations);

	return 0;
}
