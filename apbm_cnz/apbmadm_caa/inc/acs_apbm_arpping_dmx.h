#ifndef ACS_APBM_ARPPING_DMX_H_
#define ACS_APBM_ARPPING_DMX_H_


#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netdb.h>

#define ETHER_TYPE_FOR_ARP 0x0806
#define HW_TYPE_FOR_ETHER 0x0001
#define OP_CODE_FOR_ARP_REQ 0x0001
#define OP_CODE_FOR_ARP_REP 0x0002
#define HW_LEN_FOR_ETHER 0x06
#define HW_LEN_FOR_IP 0x04
#define PROTO_TYPE_FOR_IP 0x0800


typedef unsigned char byte1;
typedef unsigned short int byte2;
typedef unsigned int byte4;

// For Proper memory allocation in the structure
#pragma pack(1)
typedef struct arp_packet
{
 // ETH Header
byte1 dest_mac[6];
 byte1 src_mac[6];
 byte2 ether_type;
 // ARP Header
 byte2 hw_type;
 byte2 proto_type;
 byte1 hw_size;
 byte1 proto_size;
 byte2 arp_opcode;
 byte1 sender_mac[6];
 byte4 sender_ip;
 byte1 target_mac[6];
 byte4 target_ip;
 // Paddign
 char padding[18];
}ARP_PKT;

class acs_apbm_arpping_dmx
{
private:
	unsigned long int ipAddr;
	struct in_addr dst;
	struct ifreq ifrMac, ifrIndex, ifr;
    char *interface;
    char * destIpAddr;


public:
    acs_apbm_arpping_dmx(const char *inf, const char * dmxIpAddress);
    ~acs_apbm_arpping_dmx();
    unsigned int setSrcIpAddressFromInf();
    unsigned int getSrcIpAddressFromDest();
    void createARPRequest(ARP_PKT *msg);
    int sendARPPing(ARP_PKT *pkt,unsigned timeout_ms);
    int recv_pack(unsigned char *buf, int len, struct sockaddr_ll *FROM, unsigned char *pkt);
    void print_hex(unsigned char *p, int len);
};
#pragma pack() //Always rest pragma

#endif
