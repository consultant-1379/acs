#include <ace/ACE.h>
#include "cute.h"
#include "cute_acs_emf_common.h"

void cute_acs_emf_common::checkForDVDStatus()
{
	ACE_TCHAR dev_info[10]; // device no
	int status = ACS_EMF_Common::checkForDVDStatus(dev_info);
	ASSERTM("\n DVD Not Found ",status==-1);
}

void cute_acs_emf_common::checkForMediumPresence()
{
	ACE_INT32 mediumPresence =  ACS_EMF_Common::checkForMediumPresence();
	ASSERTM("\n Medium Not Found ",mediumPresence == -1);
}

void cute_acs_emf_common::mountDVDData()
{
	ACE_INT32 mountStatus = ACS_EMF_Common::mountDVDData();
	ASSERTM("\n Mounting Failed ",mountStatus == -1);

}

void cute_acs_emf_common::unmountDVDData()
{
	ACE_INT32 mountStatus = ACS_EMF_Common::unmountDVDData();
	ASSERTM("\n UnMounting Failed ",mountStatus == -1);
}

void cute_acs_emf_common::copyDataToDestFolder ()
{
	const ACE_TCHAR* lpszArgs[100] = {"/root/AA"};
	ACE_INT32 nooffiles = 1;
	bool overwrite = true;
	ACE_TCHAR *errmsg= "sucess";
	ACE_INT32 copyStatus = ACS_EMF_Common::copyDataToDestFolder(lpszArgs,nooffiles,overwrite,errmsg);
	ASSERTM("\n copying Failed ",copyStatus == -1);
}

// Common methods used for EMF COPY TODVD

void cute_acs_emf_common::getMediumType()
{
	ACE_TCHAR mediumType [10];
	ACE_INT32 mediumPresence = 0;
	ACE_INT32 mediumStatus = ACS_EMF_Common::getMediumType(mediumType,mediumPresence);
	//	ASSERTM("\n getMediumType Failed ",mediumStatus == -1);
	mediumPresence = 1;
	mediumStatus = ACS_EMF_Common::getMediumType(mediumType,mediumPresence);
	mediumPresence = 3;
	mediumStatus = ACS_EMF_Common::getMediumType(mediumType,mediumPresence);
	ASSERTM("\n getMediumType Failed ",mediumStatus == -1);
}

void cute_acs_emf_common::createImage()
{
	const ACE_TCHAR *fileList[100] = {"/root/emfcopy"};
	ACE_INT32 nooffiles = 1;
	const ACE_TCHAR *imageName = "/tmp/cute_test.iso";
	ACE_INT32 flag = 0;
	ACE_INT32 status = ACS_EMF_Common::createImage(fileList,nooffiles,imageName,flag);
	ASSERTM("Image creation Failed = ",status == -1);
}

void cute_acs_emf_common::checkForRewritableCD ()
{
	ACE_TCHAR dev_info[10] = "1,0,0"; // device no
	ACE_INT32 status = ACS_EMF_Common::checkForRewritableCD(dev_info);
	ASSERTM("Checking Rewritable CD failed = ",status == -1);
}

void cute_acs_emf_common::formatMedium ()
{
	ACE_TCHAR  typeofmedium[20] = "DVD-RW";
	ACE_INT32 mediumType = 0;
	ACE_TCHAR  device[10] = "1,0,0"; // device no;
	ACE_INT32 status = ACS_EMF_Common::formatMedium (mediumType,device,typeofmedium);
//	ASSERTM("Formating DVD failed = ",status == -1);
	mediumType = 1;
	status = ACS_EMF_Common::formatMedium (mediumType,device,typeofmedium);
//	ASSERTM("Formating DVD failed = ",status == -1);
	mediumType = 2;
	status = ACS_EMF_Common::formatMedium (mediumType,device,typeofmedium);
//	ASSERTM("Formating DVD failed = ",status == -1);
}

void cute_acs_emf_common::writeImageOnMedium()
{
	const ACE_TCHAR* fileList[100] = {"/root/emfcopy"};
	ACE_INT32 nooffiles = 1;
	ACE_TCHAR dev_info[10] = "1,0,0";
	ACE_TCHAR mediumType [10] = "DVD-RW";
	bool bVerify = false;
	ACE_TCHAR imageName[50] = "/tmp/cute_test.iso";
	ACS_EMF_Common::writeImageOnMedium(fileList, nooffiles, dev_info, mediumType,(ACE_INT32)0,bVerify,imageName);
	// DVD+RW
	ACE_OS::strcpy(imageName,"DVD+RW");
	ACS_EMF_Common::writeImageOnMedium(fileList, nooffiles, dev_info, mediumType,(ACE_INT32)0,bVerify,imageName);
	// DVD+R
	ACE_OS::strcpy(imageName,"DVD+R");
	ACS_EMF_Common::writeImageOnMedium(fileList, nooffiles, dev_info, mediumType,(ACE_INT32)0,bVerify,imageName);
	// DVD-R
	ACE_OS::strcpy(imageName,"DVD-R");
	ACS_EMF_Common::writeImageOnMedium(fileList, nooffiles, dev_info, mediumType,(ACE_INT32)0,bVerify,imageName);
	// CD+RW
	ACE_OS::strcpy(imageName,"CD+RW");
	ACS_EMF_Common::writeImageOnMedium(fileList, nooffiles, dev_info, mediumType,(ACE_INT32)0,bVerify,imageName);
	// CD-RW
	ACE_OS::strcpy(imageName,"CD+RW");
	ACS_EMF_Common::writeImageOnMedium(fileList, nooffiles, dev_info, mediumType,(ACE_INT32)0,bVerify,imageName);


}

void cute_acs_emf_common::GenerateVolumeName()
{
	ACE_TCHAR imageName[128];
	ACE_OS::strcpy(imageName,ACS_EMF_Common::GenerateVolumeName());
}

void cute_acs_emf_common::calculateChecksum()
{
	const ACE_TCHAR* fileList[100] = {"/root/emfcopy"};
	ACE_INT32 nooffiles = 1;
	ACE_TCHAR chksumFile[50] = "/tmp/emfsrc_sum.sha1";
	ACS_EMF_Common::calculateChecksum(fileList,nooffiles,chksumFile);
}

void cute_acs_emf_common::verifychecksum()
{
	ACS_EMF_Common::verifychecksum();
}

void cute_acs_emf_common::getMediaUsedSpace()
{

}

void cute_acs_emf_common::removeTemporaryImage()
{
	ACE_TCHAR imageName[50] = "/tmp/cute_test.iso";
	ACE_INT32 status = ACS_EMF_Common::removeTemporaryImage(imageName);
	ASSERTM("Removing Temprary image failed",status == -1);
}

void cute_acs_emf_common::getDVDOwner()
{
	ACE_TCHAR hostname[64];
	sprintf(hostname,"%s",ACS_EMF_Common::GetHostName());
	int status = ACS_EMF_Common::getDVDOwner(hostname,false);
	ASSERTM("getDVDOwner is failed",status == -1);
	sprintf(hostname,"%s","SC-2-1");
	status = ACS_EMF_Common::getDVDOwner(hostname,true);
//	ASSERTM("getDVDOwner on remote node is failed",status == -1);
	sprintf(hostname,"%s","SC-2-2");
	status = ACS_EMF_Common::getDVDOwner(hostname,true);
//	ASSERTM("getDVDOwner on remote node is failed",status == -1);
	sprintf(hostname,"%s","SC-2-3");
	status = ACS_EMF_Common::getDVDOwner(hostname,true);
	ASSERTM("getDVDOwner on remote node is failed",status == -1);



}

void cute_acs_emf_common::getDeviceName()
{
	ACE_INT32 status = ACS_EMF_Common::setDeviceName();
	ASSERTM("Medium is not availble ",status == -1);
}

/*ROUTINE:make_suite_Cute_EMF_Common()
 */
cute::suite cute_acs_emf_common::make_suite_Cute_EMF_Common(){

	cute::suite s;
	s.push_back(CUTE(cute_acs_emf_common::checkForDVDStatus));
	s.push_back(CUTE(cute_acs_emf_common::checkForMediumPresence));
	s.push_back(CUTE(cute_acs_emf_common::mountDVDData));
	s.push_back(CUTE(cute_acs_emf_common::unmountDVDData));
	s.push_back(CUTE(cute_acs_emf_common::getMediumType));
	s.push_back(CUTE(cute_acs_emf_common::createImage));
	s.push_back(CUTE(cute_acs_emf_common::checkForRewritableCD));
	//s.push_back(CUTE(cute_acs_emf_common::writeImageOnMedium));
	s.push_back(CUTE(cute_acs_emf_common::GenerateVolumeName));
	s.push_back(CUTE(cute_acs_emf_common::removeTemporaryImage));
	s.push_back(CUTE(cute_acs_emf_common::getDeviceName));
	s.push_back(CUTE(cute_acs_emf_common::getDVDOwner));
	s.push_back(CUTE(cute_acs_emf_common::calculateChecksum));
	s.push_back(CUTE(cute_acs_emf_common::verifychecksum));
	return s;
}/*end make_suite_Cute_EMF_Common()*/
