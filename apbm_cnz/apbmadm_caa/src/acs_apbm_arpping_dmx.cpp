#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_macrosconstants.h"
#include "acs_apbm_arpping_dmx.h"

acs_apbm_arpping_dmx::acs_apbm_arpping_dmx(const char *inf, const char * dmxIpAddress):
interface(NULL),destIpAddr(NULL)
{

	int interface_len = strlen(inf) + 1;
	interface = (char *)malloc(interface_len);
	memset(interface,0,interface_len);
	memcpy(interface,inf,strlen(inf));

	int destIpAddr_len = strlen(dmxIpAddress)+1;
	destIpAddr = (char *)malloc(destIpAddr_len);
	memset(destIpAddr,0,destIpAddr_len);
	memcpy(destIpAddr, dmxIpAddress,strlen(dmxIpAddress));


	ACS_APBM_LOG(LOG_LEVEL_INFO, "getting the src IP from Interface ..%s .%s = %s", interface, destIpAddr,dmxIpAddress);
	dst.s_addr = 0;
	ipAddr=0;
}

acs_apbm_arpping_dmx::~acs_apbm_arpping_dmx()
{
	if(destIpAddr)
	{
		free(destIpAddr);
		destIpAddr = NULL;
	}

	if (interface)
	{
		free(interface);
		interface = NULL;
	}

}


unsigned int acs_apbm_arpping_dmx::setSrcIpAddressFromInf()
{

	int if_fd;
	int retVal;
	if_fd = socket(AF_INET, SOCK_STREAM, 0);

	if( if_fd < 0 )
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Error creating socket");
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}
	memcpy(ifr.ifr_name, interface, IF_NAMESIZE);
	memcpy(ifrMac.ifr_name, interface, IF_NAMESIZE);
	memcpy(ifrIndex.ifr_name, interface, IF_NAMESIZE);

	/* IOCTL to get IP address */
	retVal = ioctl(if_fd, SIOCGIFADDR, &ifr, sizeof(ifr));
	if( retVal < 0 )
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ioctls error \r\n");
		close(if_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}

	/* Simple typecasting for easy access to ip address */
	struct sockaddr_in *sin;
	sin = (struct sockaddr_in *)&ifr.ifr_addr;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IF Name: %s IP Address: %s ",interface, inet_ntoa(sin->sin_addr));

	/* IOCTL to get Src MAC address */
	retVal = ioctl(if_fd, SIOCGIFHWADDR, &ifrMac, sizeof(ifrMac));
	if( retVal < 0 )
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IOCTL: getting Mac Address failed");
		close(if_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR ;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MAC address: %02x:%02x:%02x:%02x:%02x:%02x \n",
			ifrMac.ifr_hwaddr.sa_data[0]&0xFF,
			ifrMac.ifr_hwaddr.sa_data[1]&0xFF,
			ifrMac.ifr_hwaddr.sa_data[2]&0xFF,
			ifrMac.ifr_hwaddr.sa_data[3]&0xFF,
			ifrMac.ifr_hwaddr.sa_data[4]&0xFF,
			ifrMac.ifr_hwaddr.sa_data[5]&0xFF);



	retVal = ioctl(if_fd, SIOCGIFINDEX, &ifrIndex, sizeof(ifrIndex));
	if( retVal < 0 )
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "IOCTL : getting the index failed");
		close(if_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}


	close(if_fd);
	return acs_apbm::ERR_NO_ERRORS;
}

unsigned int acs_apbm_arpping_dmx::getSrcIpAddressFromDest()
{
	int on = 1;
	int probe_fd;
	char *target = destIpAddr;
	struct sockaddr_in saddr;
	socklen_t alen = sizeof(saddr);
	probe_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if(probe_fd < 0)
		return acs_apbm::ERR_DMX_ARPING_ERR;

	if (inet_aton(target, &dst) != 1)
	{
		struct hostent *hp;
		hp = gethostbyname2(target, AF_INET);
		if (!hp)
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "arping: unknown host %s\n", destIpAddr);
			close(probe_fd);
			return acs_apbm::ERR_DMX_ARPING_ERR;
		}
		memcpy(&dst, hp->h_addr, 4);
		free(hp);
	        hp=NULL;

	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;


	saddr.sin_port = htons(1025);
	saddr.sin_addr = dst;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Dest IP Address: %s \r\n", inet_ntoa(dst));
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Dest IPAddress2: %s \r\n", inet_ntoa(saddr.sin_addr));

	if (setsockopt(probe_fd, SOL_SOCKET, SO_BINDTODEVICE, interface, strlen(interface)+1) == -1)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "WARNING: interface is ignored");
		close(probe_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}

	if (setsockopt(probe_fd, SOL_SOCKET, SO_DONTROUTE, (char*)&on, sizeof(on)) == -1)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "WARNING: setsockopt(SO_DONTROUTE)");
		close(probe_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}


	if (connect(probe_fd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "connect");
		close(probe_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}
	if (getsockname(probe_fd, (struct sockaddr*)&saddr, &alen) == -1)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "getsockname");
		close(probe_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "src sin address 2 %s == \r\n ", inet_ntoa(saddr.sin_addr));
	close(probe_fd);
	ipAddr = ntohl(saddr.sin_addr.s_addr);
	return acs_apbm::ERR_NO_ERRORS;
}



void acs_apbm_arpping_dmx::createARPRequest(ARP_PKT *msg)
{
	ARP_PKT pkt ;
	// setting the destination mac to FF FF FF FF FF FF FF
	memset(pkt.dest_mac, 0xFF, (6 * sizeof(byte1)));

	// setting the src mac
	memset(pkt.src_mac,   (ifrMac.ifr_hwaddr.sa_data[0]&0xFF), sizeof(byte1));
	memset(pkt.src_mac+1, (ifrMac.ifr_hwaddr.sa_data[1]&0xFF), sizeof(byte1));
	memset(pkt.src_mac+2, (ifrMac.ifr_hwaddr.sa_data[2]&0xFF), sizeof(byte1));
	memset(pkt.src_mac+3, (ifrMac.ifr_hwaddr.sa_data[3]&0xFF), sizeof(byte1));
	memset(pkt.src_mac+4, (ifrMac.ifr_hwaddr.sa_data[4]&0xFF), sizeof(byte1));
	memset(pkt.src_mac+5, (ifrMac.ifr_hwaddr.sa_data[5]&0xFF), sizeof(byte1));

	pkt.ether_type = htons(ETHER_TYPE_FOR_ARP);

	// ARP Header
	pkt.hw_type = htons(HW_TYPE_FOR_ETHER);
	pkt.proto_type = htons(PROTO_TYPE_FOR_IP);
	pkt.hw_size = HW_LEN_FOR_ETHER;
	pkt.proto_size = HW_LEN_FOR_IP;
	pkt.arp_opcode = htons(OP_CODE_FOR_ARP_REQ);

	memcpy(pkt.sender_mac, pkt.src_mac, (6 * sizeof(byte1)));

	pkt.sender_ip = htonl(ipAddr);

	memset(pkt.target_mac,0, (6 * sizeof(byte1)));

	pkt.target_ip = dst.s_addr;

	// Padding
	memset(pkt.padding, 0 , 18 * sizeof(byte1));

	memcpy(msg,&pkt,sizeof(pkt));


	return;

}

int acs_apbm_arpping_dmx::sendARPPing(ARP_PKT *pkt,unsigned timeout_ms )
{
	// Socket to send ARP packet
	int arp_fd;
	int retVal=0;
	bool flag=0;
	ARP_PKT buf;
	int on = 1;

	fd_set readfds, masterfds;
	struct timeval timeout;
	timeout.tv_sec = 0;                    /*set the timeout to 10 seconds*/
	timeout.tv_usec = timeout_ms*1000;//Increasing the timeout value from 500 microsec to 500 millisec --HY29993

	timeval t1, t2;



	memcpy(&buf,pkt,sizeof(ARP_PKT));

	arp_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if( arp_fd == -1 )
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ARP Socket");
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}

	struct sockaddr_ll sa;
	sa.sll_family = AF_PACKET;
	sa.sll_ifindex = ifrIndex.ifr_ifindex;
	sa.sll_protocol = htons(ETH_P_ARP);
	sa.sll_halen=0;
	memset(sa.sll_addr,0,8);
	sa.sll_pkttype = 0;
	sa.sll_hatype=0;

	int rc = 0;
	rc = ioctl(arp_fd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ioctl() failed");
		close(arp_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}


	if (bind(arp_fd, (struct sockaddr*)&sa, sizeof(sa)) == -1)
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "bind : failed");
		close(arp_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}

	retVal = sendto(arp_fd, &buf, sizeof(buf), 0,(struct sockaddr *)&sa, sizeof(sa));
	if( retVal < 0 )
	{
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "sendto : failed");
		close(arp_fd);
		return acs_apbm::ERR_DMX_ARPING_ERR;
	}

	retVal=0;
	int k=1;
	struct sockaddr_ll sout;
	int fromlen = sizeof(sout);

	FD_ZERO(&masterfds);
	FD_SET(arp_fd, &masterfds);
	double elapsedTime;
	memcpy(&readfds, &masterfds, sizeof(fd_set));
	gettimeofday(&t1, 0);
	while(retVal>=0)
	{

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Receiving the reply %d \r\n", k++);

		if (select(arp_fd+1,&readfds, NULL, NULL, &timeout) < 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "on select");
			close(arp_fd);
			return acs_apbm::ERR_DMX_ARPING_ERR;
		}

		if (FD_ISSET(arp_fd, &readfds))
		{
			retVal = recvfrom(arp_fd,&buf,sizeof(buf),0,(struct sockaddr *)&sout, (socklen_t *)&fromlen);
			if(retVal < 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "recvfrom");
				close(arp_fd);
				return acs_apbm::ERR_DMX_ARPING_ERR;
			}

			if(recv_pack((unsigned char *)&buf, retVal, &sout, (unsigned char *)pkt) == acs_apbm::ERR_NO_ERRORS)
			{
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Message received is the ARP reply");
				flag=1;
				break;
			}

			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Message received is not the ARP reply");
			gettimeofday(&t2, 0);
			elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
			elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
			if(elapsedTime > timeout_ms)
			{
				//set_error_info(acs_nclib::ERR_TIMEOUT_RECEIVE, "Timeout occurred while receiving");
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "ERROR: Receive message failed! Timeout occurred while receiving");
				flag=0;
				break;
			}

		}
		else
		{
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "timeout");
			flag = 0;
			break;
		}

	}

	if(flag == 1)
	{
		// print_hex((unsigned char *)&buf, sizeof(buf));
		close(arp_fd);
		return acs_apbm::ERR_NO_ERRORS;
	}
	close(arp_fd);
	return acs_apbm::ERR_DMX_ARPING_TIMEOUT;

}


int acs_apbm_arpping_dmx::recv_pack(unsigned char *buf, int len, struct sockaddr_ll *FROM, unsigned char *pkt)
{
	//      struct timeval tv;
	ARP_PKT *ah = (ARP_PKT*)buf;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "length == %d" , len);
	//unsigned char *p = (unsigned char *)(ah+1);
	//    byte4 src_ip, dst_ip;

	if (FROM->sll_pkttype != PACKET_HOST &&
			FROM->sll_pkttype != PACKET_BROADCAST &&
			FROM->sll_pkttype != PACKET_MULTICAST)
		return acs_apbm::ERR_DMX_ARPING_ERR;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "checking the opcode  %d \r\n", ntohs(ah->arp_opcode));

	/* Only these types are recognised */
	if(ah->arp_opcode != htons(OP_CODE_FOR_ARP_REP))
		return acs_apbm::ERR_DMX_ARPING_ERR;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "checking the hw_type ah->hw_type %d \r\n", ntohs(ah->hw_type));

	/* ARPHRD check and this darned FDDI hack here :-( */
	if (ah->hw_type != htons(FROM->sll_hatype) &&
			(ah->hw_type != htons(HW_TYPE_FOR_ETHER)))
		return acs_apbm::ERR_DMX_ARPING_ERR;


	/* Protocol must be IP. */
	if (ah->proto_type != htons(PROTO_TYPE_FOR_IP))
		return acs_apbm::ERR_DMX_ARPING_ERR;
	if (ah->proto_size != 4)
		return 0;
	if (ah->hw_size != HW_LEN_FOR_ETHER)
		return acs_apbm::ERR_DMX_ARPING_ERR;


	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "checking the sender ip %d tartget ip == %d  \r\n", ntohl(ah->sender_ip) , ntohl(((ARP_PKT *)pkt)->target_ip));
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "checking the sender ip %d tartget ip == %d  \r\n", ntohl(ah->target_ip) , ntohl(((ARP_PKT *)pkt)->sender_ip));


	if(ah->sender_ip != ((ARP_PKT *)pkt)->target_ip )
		return acs_apbm::ERR_DMX_ARPING_ERR;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "checking the targetip \r\n");
	if(((ARP_PKT *)pkt)->sender_ip != ah->target_ip )
		return acs_apbm::ERR_DMX_ARPING_ERR;

	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Destination MAC = ");
	// print_hex(ah->sender_mac,6);
	return acs_apbm::ERR_NO_ERRORS;


}

void acs_apbm_arpping_dmx::print_hex(unsigned char *p, int len)
{
	/*  int i;
      for (i=0; i<len; i++) {
            ACS_APBM_LOG(LOG_LEVEL_DEBUG, "%02X", p[i]);
            if (i != len-1)
                  ACS_APBM_LOG(LOG_LEVEL_DEBUG, ":");
      }*/


	int j = 0;
	for(j = 0; j < len; j++ )
	{
		if((j%16) == 0 && j != 0 )
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "\n");
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "%02x ",*(p+j)& 0xFF );
	}

	return;
}

