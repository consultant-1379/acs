#include "acs_emf_common.h"
#include<iostream>
#include "cute.h"
using namespace std;


class cute_acs_emf_common
{
public:
	static void checkForDVDStatus();
	static void checkForMediumPresence();
	static void mountDVDData();
	static void unmountDVDData();
	static void copyDataToDestFolder ();
	// Common methods used for EMF COPY TODVD
	static void getMediumType();
	static void createImage();
	static void checkForRewritableCD ();
	static void formatMedium ();
	static void writeImageOnMedium();

	static void GenerateVolumeName();
	static void calculateChecksum();
	static void verifychecksum();
	static void getMediaUsedSpace();
	static void removeTemporaryImage();
	static void getDVDOwner();
	static void getDeviceName();
	static cute::suite make_suite_Cute_EMF_Common();

	//	private:
	//		static ACS_EMF_DVDHandler* acs_emf_dvdhandler;
};
