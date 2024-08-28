/*
 * ACS_CS_TFTP_Configurator.cpp
 *
 *  Created on: Jun 4, 2015
 *      Author: eanform
 */

#include "ACS_CS_TFTP_Configurator.h"
#include "ACS_CS_ImBlade.h"
#include "ACS_CS_ImInterface.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_Trace.h"
#include "ACS_APGCC_Util.H"

#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>

#include <sys/stat.h>

using namespace ACS_CS_INTERNAL_API;
namespace io = boost::iostreams;
namespace fs = boost::filesystem;

ACS_CS_Trace_TDEF(ACS_CS_TFTP_Configurator_TRACE);

ACS_CS_TFTP_Configurator::ACS_CS_TFTP_Configurator() {
	// TODO Auto-generated constructor stub
	s_cphwInfo="DEFAULT";
}

ACS_CS_TFTP_Configurator::~ACS_CS_TFTP_Configurator() {
	// TODO Auto-generated destructor stub
}

ACS_CS_TFTP_Configurator * ACS_CS_TFTP_Configurator::instance()
{
	return instance_::instance();
}

void ACS_CS_TFTP_Configurator::finalize()
{
	instance_::close();
}

bool ACS_CS_TFTP_Configurator::createTftpInfo(ACS_CS_TFTP_Info& outInfo, const ACS_CS_ImBlade* blade)
{
	bool result = false;

	switch (blade->functionalBoardName)
	{
	case CPUB:
		result = createCpTftpInfo(outInfo, blade);
		break;
	case IPLB:
		result = createIplbTftpInfo(outInfo, blade);
		break;
	default:
		result = true;
		break;
	}

	return result;
}

int  ACS_CS_TFTP_Configurator::getSystemIdentifier(const ACS_CS_ImBlade * blade)
{
	int sysId = -1;

	if (blade)
	{
		if (blade->type == APBLADE_T || blade->type == CPBLADE_T)
		{
			switch(blade->systemType)
			{
			case SINGLE_SIDED_CP:
				sysId = ACS_CS_Protocol::SysType_BC + blade->sequenceNumber;
				break;

			case DOUBLE_SIDED_CP:
				sysId = ACS_CS_Protocol::SysType_CP + blade->systemNumber;
				break;

			case AP:
				sysId = ACS_CS_Protocol::SysType_AP + blade->systemNumber;
				break;

			default:
				break;
			}
		}
		else if (blade->type == OTHERBLADE_T)
		{
			switch (blade->functionalBoardName)
			{
			case IPLB:
				sysId = ACS_CS_Protocol::SysType_IPLB + blade->systemNumber;
				break;
			default:
				break;
			}
		}
	}

	return sysId;
}


/*
 *  1 - Create uuid folder
 *  2 - Create mac symbolic link to uuid folder
 *  3 - Create boot.ipxe file in uuid folder
 *  4 - Create chainboot.ipxe file in uuid folder
 */
bool ACS_CS_TFTP_Configurator::addTftpEntry(const ACS_CS_TFTP_Info& bladeInfo, const ACS_CS_ImBlade* blade)
{
	bool result = true;
	ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "addTftpEntry() TFTP area configuration for UUID:<%s>",bladeInfo.uuid.c_str()));	
	std::string uuidFolder = ACS_CS_NS::COMPUTE_TFTP_PATH + "/" + bladeInfo.uuid;
	fs::path uuidBasedFolder(uuidFolder);

	std::string macHyp = bladeInfo.mac[0];
	ACS_APGCC::replace(macHyp,":","-");

	fs::path uuidFolderName(bladeInfo.uuid);

	std::string macFile = ACS_CS_NS::COMPUTE_TFTP_PATH + "/" + macHyp;
	fs::path macBasedLink(macFile);
        

	try
	{
		fs::create_directories(uuidBasedFolder);
	}
	catch(fs::filesystem_error const & e)
	{
		ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "addTftpEntry() failed to create UUID folder:<%s>, error:<%s>", uuidFolder.c_str(), e.what() ));
		result = false;
	}

	if (result)
	{
		try
		{
			//create symbolic link
			if(!fs::exists(macBasedLink))
			{
				fs::create_symlink(uuidFolderName, macBasedLink);
			}

			if(blade->functionalBoardName == IPLB)
			{
				std::string sourceBootFile = ACS_CS_NS::TFTP_PATH + ACS_CS_NS::TFTP_IPLB_BOOT_IPXE_PATH;
				std::string destBootFile = uuidFolder + "/boot.ipxe";
				//copy boot.ipxe from IPLB default path to uuid pat
				try
				{	
					//copy file
					ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "addTftpEntry() copying file from <%s> to <%s>",sourceBootFile.c_str(),destBootFile.c_str()  ));
					fs::copy_file(sourceBootFile, destBootFile);                                       
					result=true;
				}
				catch (const fs::filesystem_error& e)
				{
					ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "addTftpEntry() copying exception raised %s ", e.what()));
					result=false;
				}
					return result;
			}
			//handle boot.ipxe
			if (s_cphwInfo.compare("DEFAULT") == 0)
			{
				s_cphwInfo = ACS_CS_ImUtils::getToggle(ACS_CS_ImmMapper::AXEINFO_CP_OBJECT);
			}
			std::string ipxeFileName = uuidFolder + "/boot.ipxe";

			io::stream<io::file_sink> ipxeFile(ipxeFileName/*, std::ios_base::out | std::ios_base::app*/);

			ipxeFile << "#!ipxe \n\n";
			ipxeFile << "#Setting PXE path prefix option \n";
			ipxeFile << "set 210:string tftp://" <<  bladeInfo.tftpServer <<  bladeInfo.bootFolder << "\n\n";
			ipxeFile << "chain " <<  bladeInfo.bootFileName << " ||\n";
			ipxeFile.close();

			//assign execution permissions
			chmod(ipxeFileName.c_str(), ACCESSPERMS);


			//handle chainboot.ipxe
			std::string chainFileName = uuidFolder + "/chainboot.ipxe";
			io::stream<io::file_sink> chainFile(chainFileName);

			chainFile << "#!ipxe \n\n";
			chainFile << "#Setting PXE path prefix option \n";
			chainFile << "set 210:string tftp://" <<  bladeInfo.tftpServer <<  bladeInfo.bootFolder << "\n\n";

			chainFile << "#Set CP specific option \n";
			chainFile << "set 229:string env="<<s_cphwInfo<<" \n";

			interfaceSet interfaces = getInterfaces(bladeInfo);

				for (interfaceSet::iterator it = interfaces.begin(); interfaces.end() != it; ++it)
				{
					chainFile << "set apznic_"<< it->first << ":string " << it->second <<"\n";
				}

			chainFile << "set filename "<< bladeInfo.bootFileName << "\n";
			chainFile << "set next-server "<< bladeInfo.tftpServer << "\n";
			chainFile << "set serverpath "<< bladeInfo.bootFolder << "\n\n";

			chainFile << "#Setting PXE config file option \n";
			chainFile << "set 209:string "<< bladeInfo.bootConfigFile << "\n";
			chainFile << "chain "<< bladeInfo.bootFolder << bladeInfo.bootConfigFile << " ||\n";
			chainFile.close();

			//assign execution permissions
			chmod(chainFileName.c_str(), ACCESSPERMS);


		}
		catch(fs::filesystem_error const & e)
		{
			ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "addTftpEntry() failed. Error:<%s>", e.what() ));
			result = false;
		}

	}

	return result;
}

/*
 *  1 - Delete uuid folder and his content
 *  2 - Delete mac symbolic link
 */
bool ACS_CS_TFTP_Configurator::removeTftpEntry(const ACS_CS_TFTP_Info& bladeInfo)
{
	bool result = true;

	std::string uuidFolder = ACS_CS_NS::COMPUTE_TFTP_PATH + "/" + bladeInfo.uuid;
	fs::path uuidBasedFolder(uuidFolder);

	std::string macHyp = bladeInfo.mac[0];
	ACS_APGCC::replace(macHyp,":","-");

	std::string macFile = ACS_CS_NS::COMPUTE_TFTP_PATH + "/" + macHyp;
	fs::path macBasedLink(macFile);

	if (!bladeInfo.uuid.empty())
	{
		//remove uuid folder
		try
		{
			//if(fs::exists(uuidBasedFolder))
			fs::remove_all(uuidBasedFolder);
		}
		catch(fs::filesystem_error const & e)
		{
			ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "removeTftpEntry() failed to remove UUID folder:<%s>, error:<%s>", uuidFolder.c_str(), e.what() ));
			result = false;
		}
	}

	if (!bladeInfo.mac[0].empty())
	{
		//remove symbolic link
		try
		{
			fs::remove(macBasedLink);
		}
		catch(fs::filesystem_error const & e)
		{
			ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "removeTftpEntry() failed to remove MAC symbolic link:<%s>, error:<%s>", macFile.c_str(), e.what() ));
			result = false;
		}
	}

	return result;
}

void ACS_CS_TFTP_Configurator::getTftpContent(std::map<std::string, ACS_CS_TFTP_Info>& outList)
{
	fs::path compute_path(ACS_CS_NS::COMPUTE_TFTP_PATH);

	if(!fs::exists(compute_path))
		return;

	fs::directory_iterator end_itr;

	for (fs::directory_iterator itr(compute_path); itr != end_itr; itr++)
	{
		try {
			fs::path itemPath = itr->path();


			if (fs::is_symlink(itemPath))
			{

				fs::path uuidPath = fs::read_symlink(itemPath);

				ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "getTftpContent() TFTP item: %s is a symbolic link to %s",
						itemPath.string().c_str(),
						uuidPath.string().c_str() ));

				std::string uuid = uuidPath.string();
				std::string mac = ACS_APGCC::afterLast(itemPath.string(), std::string("/"));
				ACS_APGCC::replace(mac,"-",":");

				std::map<std::string, ACS_CS_TFTP_Info>::iterator info_itr = outList.find(uuid);

				if (info_itr == outList.end())
				{
					//Add item
					ACS_CS_TFTP_Info info;
					info.uuid = uuid;
					info.mac[0] = mac;

					ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "getTftpContent() Checking TFTP item: %s",
							info.uuid.c_str()));

					outList.insert(std::pair<std::string, ACS_CS_TFTP_Info>(info.uuid, info));
				}
				else
				{
					//Update existing item
					info_itr->second.mac[0] = mac;
				}
			}
			else if (fs::is_directory(itemPath))
			{
				ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "getTftpContent() TFTP item: %s is a directory",
						itemPath.string().c_str()));

				std::string uuid = ACS_APGCC::afterLast(itemPath.string(), std::string("/"));

				std::map<std::string, ACS_CS_TFTP_Info>::iterator info_itr = outList.find(uuid);

				if (info_itr == outList.end())
				{
					//Add item
					ACS_CS_TFTP_Info info;
					info.uuid = uuid;

					ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "getTftpContent() Checking TFTP item: %s",
							info.uuid.c_str()));

					outList.insert(std::pair<std::string, ACS_CS_TFTP_Info>(info.uuid, info));
				}
			}
		}
		catch (fs::filesystem_error const & e)
		{
			ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "getTftpContent() failed, error:<%s>", e.what() ));
		}

	}

}

bool ACS_CS_TFTP_Configurator::createCpTftpInfo(ACS_CS_TFTP_Info& outInfo, const ACS_CS_ImBlade* blade)
{
	bool result = false;

	int sysId = getSystemIdentifier(blade);  //TODO: Shall we move it somewhere else?

	if (sysId >= 0)
	{
		outInfo.uuid = blade->uuid;
		ACS_APGCC::toLower(outInfo.uuid);

		outInfo.mac[0] = blade->macAddressEthA;
		outInfo.mac[1] = blade->macAddressEthB;
		ACS_APGCC::toLower(outInfo.mac[0]);
		ACS_APGCC::toLower(outInfo.mac[1]);

		if (!(ACS_CS_Registry::isMultipleCPSystem()))
		{
			switch (blade->side)
			{
			case ACS_CS_Protocol::Side_B:
				outInfo.bootFolder = ACS_CS_NS::TFTP_CPUBB_BOOT_FOLDER;
				break;
			default:
				//According to DS "A side" value is default one.
				outInfo.bootFolder = ACS_CS_NS::TFTP_CPUBA_BOOT_FOLDER;
				break;
			}
		}
		else
		{
			std::string defCpName;
			ACS_CS_API_Util::getDefaultCPName(sysId, defCpName);

			std::string cpNameFolder = "/" + defCpName;

			switch(blade->side)
			{
			case ACS_CS_Protocol::Side_B:
				outInfo.bootFolder = cpNameFolder + ACS_CS_NS::TFTP_BCB_BOOT_FOLDER;
				break;
			default:
				//According to DS "A side" value is default one.
				outInfo.bootFolder = cpNameFolder + ACS_CS_NS::TFTP_BCA_BOOT_FOLDER;
				break;
			}
		}

		outInfo.bootFileName = outInfo.bootFolder + std::string(ACS_CS_NS::TFTP_CPUB_VIRT_BOOT_FILENAME);
		outInfo.bootConfigFile = ACS_CS_NS::TFTP_CPUB_BOOT_CONFIG_FILENAME;

		char tftp_server_str[INET_ADDRSTRLEN];
		uint32_t tftp_server_ip = htonl(ACS_CS_NS::CLUSTER_IP_ETHA);
		inet_ntop(AF_INET, &(tftp_server_ip), tftp_server_str, INET_ADDRSTRLEN);

		outInfo.tftpServer = tftp_server_str;

		result = true;
	}

	return result;
}

bool ACS_CS_TFTP_Configurator::createIplbTftpInfo(ACS_CS_TFTP_Info& outInfo, const ACS_CS_ImBlade* blade)
{

	outInfo.uuid = blade->uuid;
	ACS_APGCC::toLower(outInfo.uuid);
	outInfo.mac[0] = blade->macAddressEthA;
	outInfo.mac[1] = blade->macAddressEthB;
	ACS_APGCC::toLower(outInfo.mac[0]);
	ACS_APGCC::toLower(outInfo.mac[1]);

	outInfo.bootFolder = ACS_CS_NS::TFTP_IPLB_BOOT_FOLDER;

	outInfo.bootFileName = outInfo.bootFolder + std::string(ACS_CS_NS::TFTP_CPUB_NATIVE_BOOT_FILENAME);
	outInfo.bootConfigFile = ACS_CS_NS::TFTP_CPUB_BOOT_CONFIG_FILENAME;

	char tftp_server_str[INET_ADDRSTRLEN];
	uint32_t tftp_server_ip = htonl(ACS_CS_NS::CLUSTER_IP_ETHA);
	inet_ntop(AF_INET, &(tftp_server_ip), tftp_server_str, INET_ADDRSTRLEN);

	outInfo.tftpServer = tftp_server_str;
	
	return true;
}

void ACS_CS_TFTP_Configurator::checkSmartImageStructure()
{
	std::string smartImageFolder = "si";
	std::string smartImageDataDiskFolder = "smartImage";
	std::string smartImageConfigFile = "apz_smart_image_parameter.cfg";
	std::string sourceFolder = ACS_CS_NS::ACS_CONF_PATH + "/" + smartImageFolder;
 	std::string destFolderParent = ACS_CS_NS::TFTP_PATH + "/" + smartImageDataDiskFolder;
	fs::path destBasedFolderParent(destFolderParent);

	std::string destFolder = destFolderParent + "/" + smartImageFolder;
	fs::path destBasedFolder(destFolder);

	std::string configFile = destFolderParent + "/" + smartImageConfigFile;
 	string DHCP_ON_OFF(ACS_CS_ImUtils::readDHCPToggle());

 	// check if folder exists
 	if(!fs::exists(destBasedFolderParent) )
 	{
 		//folder not exists, create it
 		try
 		{
 			fs::create_directory(destBasedFolderParent);
 			try
 			{
 				fs::perms perms = (fs::perms)ACS_CS_NS::PERM_ALL;
 				fs::permissions(destBasedFolderParent,perms);
 			}
 			catch (const std::exception & ex)
 			{
 				ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "checkSmartImageStructure() failed to modify permission on directory <%s> , error:<%s>",
 						destBasedFolderParent.c_str(), ex.what()));
 			}
 		}
 		catch (fs::filesystem_error const & e)
 		{
 			ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "checkSmartImageStructure() failed to create directory <%s> , error:<%s>",
 					destBasedFolderParent.c_str(), e.what()));
 		}
 	}

  // Create smart image config file
	if(!fs::exists(configFile))
	{
		try 
			{
			io::stream<io::file_sink> smartImageFile(configFile);

			smartImageFile << "vapz_dhcp="<<DHCP_ON_OFF<< "\n";
			smartImageFile.close();
		}
		catch (fs::filesystem_error const & e)
		{
			ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "checkSmartImageStructure() failed to create <%s>, error:<%s>", smartImageConfigFile.c_str(), e.what()));
		}
	}

	if(!fs::exists(destBasedFolder))
	{
		// folder doesn't exist
		try
		{
			fs::create_symlink(sourceFolder, destBasedFolder);
		}
		catch (fs::filesystem_error const & e)
		{
			ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_ERROR, "checkSmartImageStructure() failed to createSymLink fromm:<%s> to:<%s>, error:<%s>",
					sourceFolder.c_str(), destBasedFolder.c_str(), e.what()));
		}
	}
}

interfaceSet ACS_CS_TFTP_Configurator::getInterfaces(const ACS_CS_TFTP_Info & info)
{
	const ACS_CS_ImModel * model = ACS_CS_ImRepository::instance()->getModelCopy();

	interfaceSet interfaces;

	if (model)
	{
		const ACS_CS_ImHost* imHost = model->getNetworkHost(info.uuid);

		if (imHost)
		{
			std::set<const ACS_CS_ImBase *> imInterfaceBase;
			model->getChildren(imHost, imInterfaceBase, TRM_INTERFACE_T);

			for (std::set<const ACS_CS_ImBase *>::iterator it = imInterfaceBase.begin(); imInterfaceBase.end() != it; ++it)
			{
				const ACS_CS_ImInterface * imInterface = dynamic_cast<const ACS_CS_ImInterface *>(*it);

				if (imInterface)
				{
					interfaces.insert(std::pair<std::string, std::string>(ACS_CS_ImUtils::getIdValueFromRdn(imInterface->interfaceId), imInterface->mac));
				}
			}
		}

		delete model;
	}

	return interfaces;
}

bool ACS_CS_TFTP_Configurator::checkDefaultSwPackage(const int crType)
{
	ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "checkDefaultSwPackage Entering"));
	bool result=false;

	switch (crType)
	{      
		case IPLB_TYPE:
			result = checkBootfile();
			break;
		default:
			result = false;
			break;
	}

	return result;

}

bool ACS_CS_TFTP_Configurator::checkBootfile()
{
	bool result = false;
	ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "checkbootFile Entering"));

	std::string bootFile = ACS_CS_NS::TFTP_PATH + "/" + ACS_CS_NS::TFTP_IPLB_BOOT_IPXE_PATH;

	if(fs::exists(bootFile))
	{
		ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "checkbootFile() boot file exists:"));
		result = true;
	}
	ACS_CS_FTRACE((ACS_CS_TFTP_Configurator_TRACE, LOG_LEVEL_DEBUG, "checkbootFile exiting"));

	return result;
}
