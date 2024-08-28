//	Copyright Ericsson AB 2007. All rights reserved.

#include "ACS_CS_HostFile.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Registry.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_HostFile_TRACE);


#include <vector>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace ACS_CS_NS;
using std::string;
using std::vector;
using std::istringstream;
using std::ostringstream;
using std::endl;
using std::fstream;
using std::ios_base;

static const uint32_t AP_1_NODE_A_ETH_A_IP	= 0xC0A8A901;	// 192.168.169.1
static const uint32_t AP_1_NODE_A_ETH_B_IP	= 0xC0A8AA01;	// 192.168.170.1
static const uint32_t AP_1_NODE_B_ETH_A_IP	= 0xC0A8A902;	// 192.168.169.2
static const uint32_t AP_1_NODE_B_ETH_B_IP	= 0xC0A8AA02;	// 192.168.170.2

const char ACS_CS_DEFAULT_DATABASE_PATH[] = "/etc/hosts";

// initialize values
uint32_t ACS_CS_HostFile::ipA1 = 0;
uint32_t ACS_CS_HostFile::ipA2 = 0;
uint32_t ACS_CS_HostFile::ipB1 = 0;
uint32_t ACS_CS_HostFile::ipB2 = 0;
uint32_t ACS_CS_HostFile::scb_rp_for_test = 0;



ACS_CS_HostFile::ACS_CS_HostFile()
      : isTestEnvironment(false)
{
    hostFilePath = getHostFilePath();
    isTestEnvironment = ACS_CS_Registry::isTestEnvironment();
}


ACS_CS_HostFile::~ACS_CS_HostFile()
{
}


uint32_t ACS_CS_HostFile::getIPAddress (std::string hostName)
{
    ACS_CS_TRACE((ACS_CS_HostFile_TRACE,
                    "(%t) ACS_CS_HostFile::getIPAddress()\n"
                    "Searching for hostname %s\n", hostName.c_str()));

    uint32_t ipAddress = 0;

    if (hostName == AP_1_NODE_A_ETH_A_HOSTNAME)
        ipAddress = AP_1_NODE_A_ETH_A_IP;
    else if (hostName == AP_1_NODE_A_ETH_B_HOSTNAME)
        ipAddress = AP_1_NODE_A_ETH_B_IP;
    else if (hostName == AP_1_NODE_B_ETH_A_HOSTNAME)
        ipAddress = AP_1_NODE_B_ETH_A_IP;
    else if (hostName == AP_1_NODE_B_ETH_B_HOSTNAME)
        ipAddress = AP_1_NODE_B_ETH_B_IP;

    ACS_CS_HostFile * hostFileInstance = ACS_CS_HostFile::getInstance();

    if (hostFileInstance) {
        uint32_t ipFromHostFile = hostFileInstance->readFromHostFile(hostName);

        if (ipFromHostFile)
            ipAddress = ipFromHostFile;
    }

    ACS_CS_TRACE((ACS_CS_HostFile_TRACE,
                    "(%t) ACS_CS_HostFile::getIPAddress()\n"
                    "Hostname %s, ip = %d.%d.%d.%d\n",
                    hostName.c_str(),
                    ( (ipAddress >> 24) & 0xFF),
                    ( (ipAddress >> 16) & 0xFF),
                    ( (ipAddress >> 8) & 0xFF),
                    ( ipAddress & 0xFF) ) );

    return ipAddress;
}


 ACS_CS_HostFile * ACS_CS_HostFile::getInstance ()
{
     return instance::instance();
}


uint32_t ACS_CS_HostFile::readFromHostFile (std::string hostName)
{
   ACS_CS_TRACE((ACS_CS_HostFile_TRACE,
           "(%t) ACS_CS_HostFile::readFromHostFile()\n"
           "Entering function\n"));

    if (!isTestEnvironment)
        return 0;

    // If the host file has been checked before
    // the address is stored in a static variable
    if (hostName == AP_1_NODE_A_ETH_A_HOSTNAME) {
        if (ipA1)
            return ipA1;
    } else if (hostName == AP_1_NODE_A_ETH_B_HOSTNAME) {
        if (ipA2)
            return ipA2;
    } else if (hostName == AP_1_NODE_B_ETH_A_HOSTNAME) {
        if (ipB1)
            return ipB1;
    } else if (hostName == AP_1_NODE_B_ETH_B_HOSTNAME) {
        if (ipB2)
            return ipB2;
    } else if (hostName == SCBRP_FOR_TEST) {
        if (scb_rp_for_test)
            return scb_rp_for_test;
    }

    uint32_t ipAddress = 0;

    // Make new IP into a string
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_addr.s_addr = htonl(ipAddress);

    // Open file for reading
    fstream inFile;
    inFile.open(hostFilePath.c_str(), ios_base::in);

    if (inFile.is_open()) {
        string line;
        string ip, column;
        bool entryFound = false;
        (void) inFile.seekg(0);

        // Loop through file, line by line
        while (std::getline(inFile, line)) {
            // Break up line into tokens
            istringstream tokenizer(line);

            if (tokenizer >> ip) {
                // Check that the first character isn't a comment (begins with "#")
                if (line.substr(0, 1) != "#") {
                    // Loop through the rest of the columns
                    while (tokenizer >> column) {
                        // Compare the value in each column with the given hostname
                        if (column == hostName) {
                            ipAddress = ntohl(inet_addr(ip.c_str()));
                            entryFound = true;

                            // Save the ip address in a static variable for future use
                            if (hostName == AP_1_NODE_A_ETH_A_HOSTNAME)
                                ipA1 = ipAddress;
                            else if (hostName == AP_1_NODE_A_ETH_B_HOSTNAME)
                                ipA2 = ipAddress;
                            else if (hostName == AP_1_NODE_B_ETH_A_HOSTNAME)
                                ipB1 = ipAddress;
                            else if (hostName == AP_1_NODE_B_ETH_B_HOSTNAME)
                                ipB2 = ipAddress;
                            else if (hostName == SCBRP_FOR_TEST)
                            	scb_rp_for_test = ipAddress;

                            break;
                        }
                    }

                    if (entryFound)
                        break;
                }
            }
        }

        inFile.close();
    } else {
        ACS_CS_TRACE((ACS_CS_HostFile_TRACE,
                        "(%t) ACS_CS_HostFile::readFromHostFile()\n"
                        "Error: Cannot open file %s for reading\n",
                        hostFilePath.c_str()));
    }

   return ipAddress;
}


string ACS_CS_HostFile::getHostFilePath ()
{
    string str = ACS_CS_DEFAULT_DATABASE_PATH;

    ACS_CS_TRACE((ACS_CS_HostFile_TRACE,
            "(%t) ACS_CS_HostFile::getHostFilePath()\n"
            "Entering function\n"));

    return str;
}

