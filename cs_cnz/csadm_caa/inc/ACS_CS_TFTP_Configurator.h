/*
 * ACS_CS_TFTP_Configurator.h
 *
 *  Created on: Jun 4, 2015
 *      Author: eanform
 */

#ifndef CSADM_CAA_INC_ACS_CS_TFTP_CONFIGURATOR_H_
#define CSADM_CAA_INC_ACS_CS_TFTP_CONFIGURATOR_H_

#define BOOST_FILESYSTEM_VERSION 3

#include "ACS_CS_ImRepository.h"
#include <ace/Singleton.h>
#include <string>


class ACS_CS_ImBlade;

struct ACS_CS_TFTP_Info
{
	std::string uuid;
	std::string mac[2];
	std::string bootFileName;
	std::string bootFolder;
	std::string bootConfigFile;
	std::string tftpServer;
};

typedef std::set<std::pair<std::string, std::string> > interfaceSet;

class ACS_CS_TFTP_Configurator {

public:
	static ACS_CS_TFTP_Configurator * instance();
	static void finalize();

	bool createTftpInfo(ACS_CS_TFTP_Info& , const ACS_CS_ImBlade*);

	bool addTftpEntry(const ACS_CS_TFTP_Info& , const ACS_CS_ImBlade*);
	bool removeTftpEntry(const ACS_CS_TFTP_Info& );

	void getTftpContent(std::map<std::string, ACS_CS_TFTP_Info>& );

	// Check template file in TFTP area for Smart Image
	void checkSmartImageStructure();
	bool checkDefaultSwPackage(const int crType);
	bool checkBootfile();


private:
	ACS_CS_TFTP_Configurator();
	virtual ~ACS_CS_TFTP_Configurator();

	bool createCpTftpInfo(ACS_CS_TFTP_Info& , const ACS_CS_ImBlade*);
	bool createIplbTftpInfo(ACS_CS_TFTP_Info& , const ACS_CS_ImBlade*);

	int getSystemIdentifier (const ACS_CS_ImBlade*);

	interfaceSet getInterfaces(const ACS_CS_TFTP_Info & info);

	std::string s_cphwInfo;

	typedef ACE_Singleton<ACS_CS_TFTP_Configurator, ACE_Recursive_Thread_Mutex> instance_;
	friend class ACE_Singleton<ACS_CS_TFTP_Configurator, ACE_Recursive_Thread_Mutex>;
};

#endif /* CSADM_CAA_INC_ACS_CS_TFTP_CONFIGURATOR_H_ */
