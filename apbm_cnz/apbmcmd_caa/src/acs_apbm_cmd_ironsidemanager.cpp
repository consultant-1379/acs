#include "acs_apbm_cmd_ironsidemanager.h"
#include "bios_set_handler.h"

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLChar.hpp>
#include <sstream>
#include <acs_nclib_factory.h>
#include <acs_nclib_session.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ACS_CS_API.h"
#include "acs_nclib_udp_reply.h"
#include "acs_nclib_udp.h"
#include "acs_nclib_hello.h"
#include "acs_nclib_rpc.h"
#include "acs_nclib_message.h"
#include <stdlib.h>
#include <algorithm>
#include <fstream>
//#include "ACS_TRA_trace.h"

/************************************************************/
/*					DMX STATIC VALUES						*/
/************************************************************/
//#define DMX_ADDRESS_A 	"192.254.15.1"
//#define DMX_ADDRESS_B 	"192.254.16.1"
#define DMX_PORT 		831
//#define DMX_USER 		"root"
//#define DMX_PWD 		"tre,14"
#define DMX_ACTION_NS "urn:com:ericsson:ecim:1.0"


#define DMX_DEFAULT_TIMEOUT_MS	5000
#define ACS_APBM_NOT_READY "NOTRDY"
#define ACS_APBM_UNSPEC "UNSPEC"

acs_apbm_cmd_ironsidemanager::acs_apbm_cmd_ironsidemanager():
m_ncSession(acs_nclib::TCP)
{
	m_ncSessionOpen = false;

    acs_nclib_library::init();

    ACS_CS_API_NetworkElement::getDmxcAddress(_dmxc_addresses[0],_dmxc_addresses[1]);

}

acs_apbm_cmd_ironsidemanager::~acs_apbm_cmd_ironsidemanager()
{
        acs_nclib_library::exit();
 
}
								
int acs_apbm_cmd_ironsidemanager::executeUdp(std::string query,std::string &udp_data)
{
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
                std::cout << "acs_apbm_nc_RemoteSender::sendUdpMessage : session.open failed"<<_dmxc_addresses[i]<<std::endl;
		errfl= bs_ironside_ns::IRONSIDE_FAILURE;
		continue;
        }
                udp->set_cmd(query);
                acs_nclib_message* answer = 0;
                acs_nclib_udp_reply * reply=0;
                int result = 0;
                if ((result = session.send(udp)) == 0)
                {
			BIOSSET_LOG("DEBUG -acs_apbm_cmd_ironsidemanager::executeUDP,Message sent %s \n", query.c_str());
                        if (session.receive(answer, 3000) == acs_nclib::ERR_NO_ERRORS)
                        {
				reply=dynamic_cast<acs_nclib_udp_reply*>(answer);
                                if(reply!=0 )
                                        {
                                                reply->get_data(udp_data);
                                                errfl=0;
                                               	BIOSSET_LOG("DEBUG -reply received %s",udp_data.c_str());
                                        }
                                else
                                        {
                                                std::cout << "DBG: Receive message have different id or reply error" << std::endl;
						errfl= bs_ironside_ns::IRONSIDE_FAILURE;
                                        }
				query_sent =true;
                        }
                        else
                        {

                                BIOSSET_LOG("ERROR -Receive failed",0);
                                errfl= bs_ironside_ns::IRONSIDE_FAILURE;
                                
                        }

                       if (answer)
                                acs_nclib_factory::dereference(answer);
                }
                else
                {
                        BIOSSET_LOG("ERROR- Sent failed",0);
                        errfl= bs_ironside_ns::IRONSIDE_FAILURE;
                }
                
		if (session.close() == 0)
                {
                        BIOSSET_LOG("DEBUG -Session Closed",0);

                }

	}
	if (udp)
                {
                        acs_nclib_factory::dereference(udp);
                }

        return errfl;

}

int acs_apbm_cmd_ironsidemanager::getShelfID(std::string physical_address,std::string &ShelfID)
{
	int result = bs_ironside_ns::ERR_NO_ERROR;
	std::map<std::string, std::string>::iterator it;
        char filter[16] = {0};
        snprintf(filter, 13, "SHLF--------");
        std::string data;

        if ((result = executeUdp(filter, data)) == bs_ironside_ns::ERR_NO_ERROR)
        {
		if((strcmp((data.c_str()),"NOTRDY") == 0) || (strcmp((data.c_str()),"ACCESS") == 0) || (strcmp((data.c_str()),"UNSPEC") == 0))
		{
			return bios_set_handler_ns::ERR_BOARD_NOT_FOUND;	
		}
		std::vector<std::string> tokens;
	        std::string buf;

        	std::stringstream ss(data);

                while (ss >> buf)
		{
                	tokens.push_back(buf);
                }

               	for(uint32_t i = 0; i < tokens.size(); i++)
                {
                        std::string shelfid, physadd;
                        std::string::size_type pos_start = 0, pos_end;
                        pos_end = tokens[i] .find(":",pos_start);

                        //ACS_APBM_LOG(LOG_LEVEL_INFO, "Tokens[%d] = %s", i, tokens[i].c_str());
	                if (pos_end != std::string::npos)
                        {

                        	shelfid = tokens[i].substr(0, pos_end);
				physadd = tokens[i].substr(pos_end+1);
                        }

                        _shelf_addr_id_map.insert(std::pair<std::string, std::string>(physadd, shelfid));

                }

                        it = _shelf_addr_id_map.find(physical_address);

                        if (it != _shelf_addr_id_map.end())
                        {
                                ShelfID = it->second;
                        }
			else
			{
				return bs_ironside_ns::ERR_IN_EXECUTION;		
			}

	}
	else
	{
		return result;
        }

        return result;
}

int acs_apbm_cmd_ironsidemanager::getBiosDefaultImage(std::string shelfId,int slot,int &bios_image)
{
	
        int result = bs_ironside_ns::ERR_NO_ERROR;
	char filter[16] = {0};
        snprintf(filter, 13, "BIOS%02d%02dDIGT",atoi(shelfId.c_str()),slot);

        std::string data;

        if ((result = executeUdp(filter, data)) == bs_ironside_ns::ERR_NO_ERROR)
        {
                data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if(strcmp((data.c_str()),"BIOS1") == 0)
		{
			bios_image = 0;
		}
		else if (strcmp ((data.c_str()),"BIOS2") == 0 )
		{	
			bios_image =1;
		}
		else
		{
		return bios_set_handler_ns::ERR_BOARD_NOT_FOUND;
		}
	}
	else
	{
		return result;
	}
	
	
        return result;

} 
int acs_apbm_cmd_ironsidemanager::getBiosPointer(std::string shelfId,int slot,int &bios_pointer)
{
	int result = bs_ironside_ns::ERR_NO_ERROR;
        char filter[16] = {0};
        snprintf(filter, 13, "BIOS%02d%02dPTGT",atoi(shelfId.c_str()),slot);

        std::string data;

        if ((result = executeUdp(filter, data)) == bs_ironside_ns::ERR_NO_ERROR)
        {
                data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		if(strcmp(data.c_str(),"BIOS1") == 0)
		{
                	bios_pointer = 0;
		}
                else if (strcmp (data.c_str(),"BIOS2") == 0)
		{
                	bios_pointer = 1;
		}
                else
		{
                	return bios_set_handler_ns::ERR_BOARD_NOT_FOUND;
		}

        }
        else
        {
                	return result;
        }

        return result;

}
int acs_apbm_cmd_ironsidemanager::setBiosDefaultImage(std::string shelfId,int slot,int bios_image)
{
        int result = bs_ironside_ns::ERR_NO_ERROR;     
	char filter[16] = {0};
        if(bios_image == 0 )
        snprintf(filter, 13, "BIOS%02d%02dDIB1",atoi(shelfId.c_str()),slot);
        else if (bios_image == 1)
        snprintf(filter, 13, "BIOS%02d%02dDIB2",atoi(shelfId.c_str()),slot);
        else
          return bs_ironside_ns::ERR_IN_EXECUTION;

        std::string data;

        if ((result = executeUdp(filter, data)) == bs_ironside_ns::ERR_NO_ERROR)
        {
 		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
                if (data.compare("OK") != 0)
                {
			return bios_set_handler_ns::ERR_BOARD_NOT_FOUND;
                }
        }
        else
        {
                	return result;
        }
	
        return bs_ironside_ns::ERR_NO_ERROR;

}
int acs_apbm_cmd_ironsidemanager::setBiosPointer(std::string shelfId,int slot,int bios_pointer)
{
	int result = bs_ironside_ns::ERR_NO_ERROR;
        char filter[16] = {0};
	if(bios_pointer == 0 )
        snprintf(filter, 13, "BIOS%02d%02dPTB1",atoi(shelfId.c_str()),slot);
        else if (bios_pointer == 1)
        snprintf(filter, 13, "BIOS%02d%02dPTB2",atoi(shelfId.c_str()),slot);
        else
          return bs_ironside_ns::ERR_IN_EXECUTION;

        std::string data;

        if ((result = executeUdp(filter, data)) == bs_ironside_ns::ERR_NO_ERROR)
        {
                data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
                if (data.compare("OK") != 0)
                {
                        return bios_set_handler_ns::ERR_BOARD_NOT_FOUND;
                }
        }
        else
        {
                        return result;
        }
	return bs_ironside_ns::ERR_NO_ERROR;
}

int acs_apbm_cmd_ironsidemanager::MagAddrToPhysicalAddr(const char* shelfaddr)
{

	std::stringstream ss(shelfaddr);
	std::string item;
	char delim = '.';

	std::vector<std::string> elems;
	while(std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	char tmp[32] = {0};
	if (elems.size() == 4)
	{
		int plug0 = atoi(elems.at(0).c_str());
		int plug1 = atoi(elems.at(1).c_str());
		int plug3 = atoi(elems.at(3).c_str());

		sprintf(tmp,"%x%x%x",plug3,plug1,plug0);
	}
	else
		return -1;

	char* endPtr;
	int mag = strtol(tmp, &endPtr,16);

	return mag;

}
