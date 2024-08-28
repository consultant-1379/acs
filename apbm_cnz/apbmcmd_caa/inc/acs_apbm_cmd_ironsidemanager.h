#ifndef ACS_APBM_IRONSIDEMANAGER_H
#define ACS_APBM_IRONSIDEMANAGER_H

#include "acs_nclib_library.h"
#include "acs_nclib_session.h"
#include "acs_nclib_factory.h"
#include <string.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

namespace bs_ironside_ns{

        enum error_code  {
                        ERR_NO_ERROR = 0,
                        ERR_IN_EXECUTION = - 1,
			IRONSIDE_FAILURE =-29
                };
}

class acs_nclib_session;

class acs_apbm_cmd_ironsidemanager{

public:
        acs_apbm_cmd_ironsidemanager();
        ~acs_apbm_cmd_ironsidemanager();
               
        int getShelfID(std::string physical_address,std::string &ShelfID);
        int getBiosDefaultImage(std::string shelfid, int slot, int &bios_image);
        int getBiosPointer(std::string shelfid, int slot, int &bios_pointer);
        int setBiosDefaultImage(std::string shelfid, int slot,int bios_image);
        int setBiosPointer(std::string shelfid, int slot,int bios_pointer);
        int MagAddrToPhysicalAddr(const char* shelfaddr);
 
private:
        bool m_ncSessionOpen;
        acs_nclib_session m_ncSession;
	int executeUdp(std::string query,std::string &udp_data); 
	std::map<std::string,std::string> _shelf_addr_id_map;
        std::string m_physicalAddress;
        std::string data;
        uint32_t _dmxc_addresses[2];

};
#endif
                                                                                                                                                             

