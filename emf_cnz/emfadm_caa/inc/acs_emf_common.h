/*=================================================================== */
/**
   @file   acs_emf_common.h

   @brief Header file for EMF module.

   @version 1.0.0


   HISTORY
   This section contains eeference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A        DD/MM/YYYY   XRAMMAT   Initial Release
 */
/*==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_EMF_COMMON_H
#define ACS_EMF_COMMON_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_emf_defs.h"
#include <acs_prc_api.h>
#include <ACS_CC_Types.h>
#include <string>
#include <list>
#include <ACS_DSD_MacrosConstants.h>
#include <dirent.h>
#include <sys/statvfs.h>

//masks used by checkForDVDStatus
#define CHECKDVD_MASK_NOTPRESENT	0x00  // DVD not physically present
#define CHECKDVD_MASK_PRESENT		0x01  // DVD present and configured
#define CHECKDVD_MASK_BUSY			0x02  // DVD already in use
#define CHECKDVD_MASK_NOTCONFIG		0x04  // DVD present but not configured
#define CHECKDVD_MASK_UNKNOWN		0x10  // DVD state unknown


#define ACS_EMF_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
// APGCC DEFINES
#define ACS_EMF_APGCC_HWVERSION		64
#define ACS_EMF_APGCC_APG43				3
#define ACS_EMF_APGCC_GEP1				1
#define ACS_EMF_APGCC_GEP2				2
#define ACS_EMF_APGCC_GEP4				3
#define ACS_EMF_APGCC_GEP5				3
#define ACS_EMF_APGCC_GEP7				4

// contants for EMF copy

/*====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
enum NODE_NAME
{
        NODE_A=1,
        NODE_B=2,
        UNDEFINED=3
};

namespace acs_emf_common {
enum dvd_media_presence {
	DVDMEDIA_ERROR = -1,
	DVDMEDIA_CD_MEDIATYPE = 1,
	DVDMEDIA_NOT_PRESENT = 1,
	DVDMEDIA_PRESENT = 0
};

enum dvd_media_state {
	DVDMEDIA_STATE_ERROR = -1,
	DVDMEDIA_STATE_EMPTY = 0,
	DVDMEDIA_STATE_NOT_EMPTY = 1,
};

enum dvd_media_writeType {
	WRITETYPE_NEWSESSION = 0,
	WRITETYPE_APPENDING = 1,
	WRITETYPE_FORMAT_WRITE = 2,
};
typedef dvd_media_writeType EMF_DVD_WRITETYPE  ;
}
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief  The ACS_EMF_Common class handle the common methods used for dvd operations.
*/
/*=================================================================== */
class ACS_EMF_Common
{
	/*=====================================================================
						 PRIVATE DECLARATION SECTION
	 ==================================================================== */
private:
	/*===================================================================
						 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
						 PRIVATE METHOD
	=================================================================== */
	/*=================================================================== */

	/*=================================================================== */
	/**
			@brief       Default constructor for ACS_EMF_Common

			@par         None

			@pre         None

			@post        None

			@exception   None
	 */
	/*=================================================================== */
   ACS_EMF_Common() { };

   /*=====================================================================
   	                      PUBLIC DECLARATION SECTION
   	 ==================================================================== */
public:
   	/*===================================================================
   						  PUBLIC ATTRIBUTE
   	=================================================================== */

   	/*===================================================================
   							   PUBLIC METHOD
   	=================================================================== */
   	/*=================================================================== */

	/*=================================================================== */
	/**
			 @brief    	Check whether the data disk is reachable, i.e. the disk is mapped as fixed
						disk. Can be handy to determine if this is the passive or active node when
						the cluster is in a funny mode or not available

			 @par         None

			 @pre         None

			 @post        None

			 @return      0 on success
                                                 Fail otherwise
			 @exception   None
	*/
	/*=================================================================== */
   //static bool CheckActiveNode( );

   /*=================================================================== */
   	/**
   			 @brief    	Check a file or directory for existence and permissions
						Also checks if it is a file or device (1) or directory (2)
						-1 in case of failure

   			 @par         None

   			 @pre         None

   			 @post        None

   			 @param			lpszFile
									   contains the file or directory

   			  @return      1 for file 2 for directory
                                                 Fail otherwise

   			 @exception   None
   	*/
   	/*=================================================================== */
   static int  CheckFile(const ACE_TCHAR* lpszFile);

	/*=================================================================== */
	/**
			 @brief    	Method used to check directory for write access

			 @par         None

			 @pre         None

			 @post        None

			 @param			lpszFile
								   contains the file or directory

			 @return      0 on success
                                                 Fail otherwise
			 @exception   None
	*/
	/*=================================================================== */
   static int CheckDirForWriteAccess(const ACE_TCHAR*);
   /*=================================================================== */
   	/**
   			 @brief    	Method used to checks given directory is empty or not

   			 @par         None

   			 @pre         None

   			 @post        None

   			 @param			dname
   								   contains the directory name

			 @return      0 on success
                                                 Fail otherwise

   			 @exception   None
   	*/
   	/*=================================================================== */
   static ACE_INT32 isEmptyDir(const ACE_TCHAR*);
	/*=================================================================== */
	/**
			 @brief    	Method used to get the current host name

			 @par         None

			 @pre         None

			 @post        None

			 @return      host name

			 @exception   None
	*/
	/*=================================================================== */
   static const ACE_TCHAR* GetHostName();

	/*=================================================================== */
	/**
			 @brief    	getNode

			 @par         None

			 @pre         None

			 @post        None

			 @return      host name

			 @exception   None
	*/
	/*=================================================================== */
   static NODE_NAME getNode();
	/*=================================================================== */
	/**
			 @brief    	GetNodeName

			 @par         None

			 @pre         None

			 @post        None

			 @param		  node : ACE_INT32

			 @param       nodeName : ACE_TCHAR*

			 @return      ACE_INT32

			 @exception   None
	*/
	/*=================================================================== */
   static ACE_INT32 GetNodeName(ACE_INT32 node,ACE_TCHAR *nodeName);
	/*=================================================================== */
	/**
			 @brief    	GetUserName

			 @par         None

			 @pre         None

			 @post        None

			 @return      ACE_TCHAR*

			 @exception   None
	*/
	/*=================================================================== */
   static const ACE_TCHAR* GetUserName();
	/*=================================================================== */
	/**
			 @brief    	killAllOutstandingPIDs

			 @par         None

			 @pre         None

			 @post        None

			 @return      ACE_INT32

			 @exception   None
	*/
	/*=================================================================== */
   static ACE_INT32 killAllOutstandingPIDs();


	/*=================================================================== */
	/**
			 @brief    	  Method used to Get the physical path from the logical name

			 @par         None

			 @pre         None

			 @post        None

			 @param		  pszLogicalName
							Contains the logical name

			 @param		  szPath
							 Contains the path

			 @return      true on success
							   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static bool GetDataDiskPath( std::string& pszLogicalName, std::string&  szPath);


	/*=================================================================== */
	/**
			 @brief    	Method used to Get the current date and time as a character string.

			 @par         None

			 @pre         None

			 @post        None

			 @return     0 on success
												   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static const ACE_TCHAR* GetDateTimeString( );
   /*=================================================================== */
   /**
   			 @brief    	Method used to Get the current date and time as a character string without offset.

   			 @par         None

   			 @pre         None

   			 @post        None

   			 @return     0 on success
   												   Fail otherwise

   			 @exception   None
    */
   /*=================================================================== */
   static const ACE_TCHAR* GetDateTimeWithoutOffset();

	/*=================================================================== */
	/**
			 @brief    	Method used to Get the date and time as a character string from a FILETIME struct
						expressed in local time.

			 @par         None

			 @pre         None

			 @post        None

			 @param       lpFileTime

			 @return     datetime

			 @exception   None
	*/
	/*=================================================================== */
   static const ACE_TCHAR* GetDateTimeString(time_t lpFileTime);

	/*=================================================================== */
	/**
			 @brief    	Method used to Get the date and time as a character string.

			 @par         None

			 @pre         None

			 @post        None

			 @param       nTime

			 @param       bConvToLocalTime

			 @return     datetime

			 @exception   None
	*/
	/*=================================================================== */
   static const ACE_TCHAR* GetDateTimeString(const long nTime,const bool bConvToLocalTime);

	/*=================================================================== */
	/**
			 @brief    	Method used to Compose the EMF CD/DVD image file path

			 @par         None

			 @pre         None

			 @post        None

			 @param      lpszPath

			 @exception   None
	*/
	/*=================================================================== */
//   static void GetEMFImageFilePath(ACE_TCHAR* lpszPath);
	/*=================================================================== */
	/**
			 @brief    	Method used to Compose the EMF Local log file path

			 @par         None

			 @pre         None

			 @post        None

			 @param      lpszPath


			 @exception   None
	*/
	/*=================================================================== */
//   static void GetEMFLocalLogFilePath(ACE_TCHAR* lpszPath);
	/*=================================================================== */
	/**
			 @brief    	Method used to Compose the EMF log file path

			 @par         None

			 @pre         None

			 @post        None

			 @param      lpszPath

			 @exception   None
	*/
	/*=================================================================== */
//   static void GetEMFLogFilePath(ACE_TCHAR* lpszPath);
	/*=================================================================== */
	/**
			 @brief    	Method used to Get the APG hardware version: APG40C/2, APG40C/4 or APG43

			 @par         None

			 @pre         None

			 @post        None

 		     @return     0 on success
												   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static int GetHWVersion();

	/*=================================================================== */
	/**
			 @brief    	Method used to Get the APG hardware variant: 1 for GEP1, 2 for GEP2

			 @par         None

			 @pre         None

			 @post        None

			 @return     0 on success
												   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static int GetHWVariant();
	/*=================================================================== */
	/**
			 @brief    	RemoveSATADeviceRequest

			 @par       None

			 @pre       None

			 @post      None

			 @return    bool

			 @exception None
	*/
	/*=================================================================== */
   static bool RemoveSATADeviceRequest();
	/*=================================================================== */
	/**
			 @brief    	Move the DVD SATA device ownership to this node

			 @par       None

			 @pre       None

			 @post      None

			 @return    Return codes:
					    0 : Port select successfully executed
					    1 : SATA Driver not found
					    2 : Failed to open SATA Driver
						3 : Port select failed
						4 : Event object was signaled (operation cancelled by calling thread)

			 @exception None
	*/
	/*=================================================================== */
   static ACE_INT32 MoveSATADevice( );

   // Common Methods for EMF Copy FROMDVD
   /*=================================================================== */
   	/**
   			 @brief    	Method used to checks DVD status of the present node

   			 @par         None

   			 @pre         None

   			 @post        None

   			 @param       device

   			 @return      0 on success
   												   Fail otherwise

   			 @exception   None
   	*/
   	/*=================================================================== */
   static ACE_INT16 checkForDVDStatus(std::string & deviceFileName);
   static ACE_INT16 checkForUSBStatus();
   static ACE_INT32 checkForPassiveDVDStatus(ACE_TCHAR * device);
   static bool isDVDMounted();
   static bool isDVDConfigured(ACE_INT16 systemId);

	/*=================================================================== */
	/**
			 @brief    	Method used to checks DVD/CD Medium Type present in the current node

			 @par         None

			 @pre         None

			 @post        None

			 @return     0 on success
												   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static ACE_INT32 checkForMediumPresence();

   /*=================================================================== */
   	/**
   			 @brief    	Method used to mounts attached DVD/CD data to /mnt Folder

   			 @par        None

   			 @pre        None

   			 @post       None

   			 @return     0 on success
   						 Fail otherwise

   			 @exception   None
   	*/
   	/*=================================================================== */
   static ACE_INT32 mountDVDData();
   static ACE_INT32 mountDVDOnPassive();
   static ACE_INT32 mountPassiveDVDOnActive(int32_t sysId, acs_dsd::NodeSideConstants nodeSide);
   static ACE_INT32 getNodePeerID(ACE_TCHAR * peer_id);
   static ACE_INT32 getNodeClusterIp(char(&NodeIp)[EMF_IPADDRESS_MAXLEN], int32_t sysId, acs_dsd::NodeSideConstants nodeSide);
	/*=================================================================== */
	/**
			 @brief    	Method used to unmounts DVD/CD

			 @par         None

			 @pre         None

			 @post        None

			 @return     0 on success
												   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static ACE_INT32 unmountDVDData();
   static ACE_INT32 unmountDVDOnPassive();
   static ACE_INT32 unmountUSBOnPassive();
   /*=================================================================== */
   	/**
   			 @brief    	Method used to Copies mounted DVD/CD data to given location

   			 @par         None

   			 @pre         None

   			 @post        None

   			 @param		  filenames
							 Contains the destination folder path

   			 @param       nooffiles

   			 @param       overwrite

   			 @param       errorText

   			 @return      0 on copying successful
   						  Fail otherwise

   			 @exception   None
   	*/
   	/*=================================================================== */
   static ACE_INT32 copyDataToDestFolder (const ACE_TCHAR* filenames[100], ACE_INT32 nooffiles,bool overwrite,ACE_TCHAR * errorText);
   static ACE_INT32 removeFilesOrFolders(const ACE_TCHAR* filenames[100], ACE_INT32 nooffiles);
        /*=================================================================== */
        /**
                         @brief         Gives exact medium type i.e DVD+RW, DVD-RW ... from given medium.

                         @par         None

                         @pre         None

                         @post        None

                         @param                 mediumType
                                                        Contains the medium type.mediumType contain result on success

                         @param                 medium
                                                        Contains the medium. medium could be 0 or 1 (0 - DVD, 1 - CD).

                         @return     0 on success
                                                                                                   Fail otherwise

                         @exception   None
        */

	/*=================================================================== */
   // Common methods used for EMF COPY TODVD
   static ACE_INT32 getMediumType(ACE_TCHAR * mediumType);

   /*=================================================================== */
   	/**
   			 @brief    	  Method used to creates image file from given file list.

   			 @par         None

   			 @pre         None

   			 @post        None

   			 @param		  filenames
										 Contains the file names

   			 @param		  nooffiles
										 Contains the number of files

			 @param		  imageName
										 Contains the image name

			 @param		  flag
						  flag 0 indicates DVD
					      flag 1 indicates CD

   			 @return      0 on createImage successful
   							Fail otherwise

   			 @exception   None
   	*/
   	/*=================================================================== */
   static ACE_INT32 createImage(const ACE_TCHAR* filenames[100],ACE_INT32 nooffiles,const ACE_TCHAR* imageName,ACE_INT32 flag);

	/*=================================================================== */
	/**
			 @brief    	Method used to checks CD is rewritable or not.

			 @par         None

			 @pre         None

			 @post        None

			 @param			device
								 Contains the device name

			 @return     0 on success
												   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static ACE_INT32 checkForRewritableCD (ACE_TCHAR * device);

	/*=================================================================== */
	/**
			 @brief    	Method used to formats the Medium based on given medium type.

			 @par       None

			 @pre       None

			 @post      None

			 @param		mediumType
						Contains the medium type.
						mediumType = 0 means medium is DVD
						mediumType = 1 means medium is CD.

   			 @param		device
						Contains the device name

			 @param		typeofmedium
						typeofmedium is like DVD+RW

			 @return    0 - formatMedium successful
						-1 - Failed

			 @exception   None
	*/
	/*=================================================================== */
   static ACE_INT32 formatMedium (ACE_INT32 mediumType= NULL,ACE_TCHAR * device = NULL ,ACE_TCHAR * typeofmedium = NULL);
	/*=================================================================== */
	/**
			 @brief    	Method used to Writes Image on given medium when copy to dvd operation performs.

			 @par         None

			 @pre         None

			 @post        None

			 @param		  fileList
							Contains the file names

   			 @param		  nooffiles
							Contains the number of files

			 @param		  device
							Contains the device name

			 @param		   mediumType
							 Contains the medium type
							 mediumType = 0 means medium is DVD
							 mediumType = 1 means medium is CD.

   			 @param		   writeType
							 Contains the write type for medium.
							 writeType = 0 means medium is new
							 writeType = 1 means medium is appending.

			 @param		   verifyFlag
							 Contains the flag

			 @param		   imageName
							 Contains the image name

			 @return       0 on success
						     Fail otherwise

			 @exception    None
	*/
	/*=================================================================== */
   static ACE_INT32 writeImageOnMedium(const ACE_TCHAR* fileList[100], ACE_INT32 nooffiles, ACE_TCHAR* device, ACE_TCHAR* mediumType,acs_emf_common::EMF_DVD_WRITETYPE writeType,bool verifyFlag, ACE_TCHAR* imageName);
   static ACE_INT32 writeDataOnUSBMedium(const ACE_TCHAR* fileList[100], ACE_INT32 nooffiles,ACE_INT32 writeType,bool verifyFlag);

 	/*=================================================================== */
	/**
			 @brief    	Method used to Generate a CD/DVD volume name for a new session.

			 @par       None

			 @pre       None

			 @post      None

			 @return    0 on success
												   Fail otherwise
			 @exception   None
	*/
	/*=================================================================== */
   static const ACE_TCHAR* GenerateVolumeName();

	/*=================================================================== */
	/**
			 @brief    	Method used to calculates checksum for given files and stores in /tmp/emf_sum.sha1.

			 @par         None

			 @pre         None

			 @post        None

			 @param		  fileList
										 Contains the file names

   			 @param		  nooffiles
										 Contains the number of files

			 @param		  fname
										 Contains the file name

			 @return      0 on success
												   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
//   static ACE_INT32 calculateChecksum(const ACE_TCHAR *fileList[100], ACE_INT32 nooffiles, const ACE_TCHAR *fname);

	/*=================================================================== */
	/**
			 @brief    	  Method used to verifies checksum for STP files and DVD files.

			 @par         None

			 @pre         None

			 @post        None


			 @return      0 on success
								Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
      static ACE_INT32 calculateChecksumOfUSB(const ACE_TCHAR* FilePath,const ACE_TCHAR* fname);

        /*=================================================================== */
        /**
                         @brief           Method used to verifies checksum for STP files and DVD files.

                         @par         None

                         @pre         None

                         @post        None


                         @return      0 on success
                                                                Fail otherwise

                         @exception   None
        */

	/*=================================================================== */
//   static ACE_INT32 verifychecksum();

   /*=================================================================== */
   	/**
   			 @brief    	  Method used to return the medium used spaces.

   			 @par         None

   			 @pre         None

   			 @post        None


   			 @param		  usedspace
							 Contains used space

   			 @return      0 on success
   								 Fail otherwise

   			 @exception   None
   	*/

   	/*=================================================================== */
   static ACE_INT32 getMediaUsedSpace(ACE_UINT64 *iUsedSpace);
   static ACE_INT32 getMediaUsedSpaceOnPassive(ACE_TCHAR* usedspace);
      /*=================================================================== */
   	/**
   			 @brief    	 Method used to return the medium used spaces.

   			 @par         None

   			 @pre         None

   			 @post        None

			 @param		  fileList
							 Contains the file names
             @param		  nooffiles
							 Contains the number of files
   			 @param		 usedspace
							 Contains used space

   			 @return      0 on success
   												   Fail otherwise

   			 @exception   None
   	*/
   	/*=================================================================== */
   static ACE_INT32 getGivenFilesConsumedSpace(const ACE_TCHAR *fileList[100], ACE_INT32 nooffiles, ACE_TCHAR *usedspace);
	/*=================================================================== */
	/**
			 @brief    	 Method used to removes temp image i.e /tmp/image.iso.

			 @par        None

			 @pre        None

			 @post       None

			 @param		 imageName
							 Contains image name

			 @return     0 on success
						   Fail otherwise

			 @exception  None
	*/
	/*=================================================================== */
   static ACE_INT32 removeTemporaryImage(const ACE_TCHAR*);
	/*=================================================================== */
	/**
			 @brief    	 Method used to get the Owner of DVD.

			 @par        None

			 @pre        None

			 @post       None

			 @param		 hostname
							 Contains host name

			 @param		 remote
							contains the remote flag

			 @return     0 on success
						   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static ACE_INT32 getDVDOwner(const ACE_TCHAR*, bool);

	/*=================================================================== */
	/**
			 @brief    	 Method used to gets device name  and copies to global variable names g_devicefile.

			 @par         None

			 @pre         None

			 @post        None

			 @return      0 on success
						  Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static void setDeviceName(const std::string &strDevName);
   static const char * getDeviceName();

	/*=================================================================== */
	/**
			 @brief    	  Method used to gets the History log attributes information from IMM.

			 @par         None

			 @pre         None

			 @post        None

			 @param		  aObjectDN
							 Contains Object Name

			 @param		  attrName
							 contains the attribute name

			 @param		  pszConfigType
							 contains the value

			 @return      0 on success
							  Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static ACE_INT32 fetchEMFHistoryAttribute(std::string& aObjectDN , char* attrName,char* pszConfigType);
   /*=================================================================== */
   /**
   			 @brief    	  Method used to gets the History log attributes information from IMM.

   			 @par         None

   			 @pre         None

   			 @post        None

   			 @param		  aObjectDN
   							 Contains Object Name

   			 @param		  attrName
   							 contains the attribute name

   			 @param		  operType
   							 contains the value

   			 @return      0 on success
   							  Fail otherwise

   			 @exception   None
    */
   /*=================================================================== */
   static ACE_INT32 fetchEMFHistoryIntegerAttribute(std::string& aObjectDN , char* attrName,int & operType);

	/*=================================================================== */
	/**
			 @brief    	  Method used for creating the thread

			 @par         None

			 @pre         None

			 @post        None

			 @return      0 on success
							   Fail otherwise

			 @exception   None
	*/
	/*=================================================================== */
   static ACE_INT32 s32Spawn(ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),void * Arglist,const
							 ACE_TCHAR* lpszThreadName,ACE_INT32 s32GrpId);

   /*=================================================================== */
   	/**
   			 @brief    	 Method used to start the thread

   			 @par        None

   			 @pre        None

   			 @post       None

   			 @return     0 on success
   												   Fail otherwise

   			 @exception  None
   	*/
   	/*=================================================================== */
   static ACE_INT32 StartThread( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
								 void* Arglist,
								 const ACE_TCHAR* lpszThreadName,
								 ACE_INT32 s32GrpId = -1);
   /*=================================================================== */
   /**
         			 @brief    	 Method used to set the ccbId, errorId and errorText in Runtime ErrorHandler Object

         			 @param      ccbid contains the ccbId of the transaction

         			 @param      errorId contains the errorId of the transaction

					 @param      errorText contains the errorText of the transaction

         			 @pre        None

         			 @post       None

         			 @return     0 on success
         						-1 on Failure case

         			 @exception   None
    */
   /*=================================================================== */
   static ACS_CC_ReturnType setErrorText(unsigned long long ccbid, int errorId, std::string errorText);
   /*=================================================================== */
   /**
               			 @brief    	 Method used to validate the given filename.

               			 @pre         None

               			 @post        None

               			 @return     0 on success
               						-1 on Failure case

               			 @exception   None
    */
   /*=================================================================== */
   static bool ValidateFilename(const ACE_TCHAR* ,ACE_INT32);
   /*=================================================================== */
   /**
                 			 @brief    	 Method used to validate the given filename for FROMDVD operation.

                 			 @pre        None

                 			 @post       None

                 			 @return     0 on success
                 						-1 on Failure case

                 			 @exception   None
    */
   /*=================================================================== */
   static ACE_INT32 ValidateFileArgumentFromMedia( const ACE_TCHAR*);
   /*=================================================================== */
   /**
                    			 @brief    	 Method used to validate the given filename for TODVD operation.

                    			 @pre         None

                    			 @post        None

                    			 @return     0 on success
                    						-1 on Failure case

                    			 @exception   None
    */
   /*=================================================================== */
   static ACE_INT32 ValidateFileArgumentToMedia( const ACE_TCHAR*);

   /*=================================================================== */
   /**
                    			 @brief    	 Checks given argument(s) is valid file or not for specific file copy in FROMDVD option

                    			 @pre        None

                    			 @post       None

                    			 @param		 lpszFilePath

                    			 @return     0 on success
                    						-1 on Failure case

                    			 @exception   None
    */
   /*=================================================================== */
   static ACE_INT32 ValidateForSpecificFileCopy(const ACE_TCHAR* lpszFilePath);
   /*=================================================================== */
   /**
                    			 @brief    	 Method used to validate the given label in TODVD operation.

                    			 @pre         None

                    			 @post        None

                    			 @return     0 on success
                    						-1 on Failure case

                    			 @exception   None
    */
   /*=================================================================== */
   static ACE_INT32 ValidateLabel(const ACE_TCHAR*);
   /*=================================================================== */
   /**
                    			 @brief    	 Method used to expand the given relative path to absolute path.

                    			 @pre         None

                    			 @post        None

                    			 @param		  lpszPath
									           Contains Object Name

                    			 @return     None

                    			 @exception   None
    */
   /*=================================================================== */
  // static void ExpandPath(char* lpszPath);
   static void ExpandPath(std::string & lpszPath);
   /*=================================================================== */
   /**
                    			 @brief    	 Method used to validate the given node name in emfinfo operation.

                    			 @pre         None

                    			 @post        None

                    			 @return     true on success
                    						 false on Failure case

                    			 @exception   None
    */
   /*=================================================================== */
   static bool CheckValidNodename(const ACE_TCHAR* );
   /*=================================================================== */
   /**
                    			 @brief    	 Method used to set DVD operation state to IDLE.

                    			 @pre         None

                    			 @post        None

                    			 @return     None

                    			 @exception   None
    */
   /*=================================================================== */
   static void setMediaOperationStateToIDLE();
   /*=================================================================== */
   /**
                    			 @brief    	 Method used to set DVD operation state to PROGRESS.

                    			 @pre         None

                    			 @post        None

                    			 @return     None

                    			 @exception   None
    */
   /*=================================================================== */
   static void setMediaOperationStateToProgress(acs_emf_operation_status_t);
   /*=================================================================== */
     /**
                      			 @brief    	 Method used to fecth DVD operation state.

                      			 @pre         None

                      			 @post        None

                      			 @return     acs_emf_operation_status_t state.

                      			 @exception   None
      */
     /*=================================================================== */
   static acs_emf_operation_status_t  getMediaOperationState();
   /*=================================================================== */
   /**
                    			 @brief    	 Method used to create a directory.

                    			 @pre         None

                    			 @post        None

                    			 @return     None

                    			 @exception   None
    */
   /*=================================================================== */
   static void createDirectory(const ACE_TCHAR* lpszDirname);
   /*=================================================================== */
   /**
                       			 @brief    	 Method used to fetch emf root dn from IMM.

                       			 @pre         None

                       			 @post        None

                       			 @return     None

                       			 @exception   None
    */
   /*=================================================================== */
   static int fetchDnOfRootObjFromIMM ();
   /*=================================================================== */
   /**
                          			 @brief    	 Method used to fetch current node state.

                          			 @pre         None

                          			 @post        None

                          			 @return      1 - active
												  2 - passive
												 -1 - Undefined

                          			 @exception   None
    */
   /*=================================================================== */
   static ACE_INT32 getNodeState();

 /*=================================================================== */
   /**
                                     @brief      Method used to eject media.

                                     @pre         None

                                     @post        None

                                     @return      0 - success
												  -1 - failed

                                     @exception   None
    */
   /*=================================================================== */
	static ACE_INT32 ejectDVDMedia();

	/*=================================================================== */
   /**
                                     @brief      Method used to setup nfs on active node.

                                     @pre         None

                                     @post        None

                                     @return      0 - success
                                                  -1 - failed

                                     @exception   None
    */
   /*=================================================================== */

	static ACE_INT32 setupNFSOnActive();
	static ACE_INT32 setupNFSOnPassive();
	 /*=================================================================== */
   /**
                                     @brief      Method used to mount fileM on passive node.

                                     @pre         None

                                     @post        None

                                     @return      0 - success
                                                  -1 - failed

                                     @exception   None
    */
   /*=================================================================== */
	static ACE_INT32 moutFileMOnPassive();
	static ACE_INT32 moutFileMMediaOnPassive();
	static ACE_INT32 mountMediaOnOtherNode();


	static ACE_INT32 moutFileMOnPassive(int32_t sysId, acs_dsd::NodeSideConstants nodeSide);
	/*=================================================================== */
   /**
                                     @brief      Method used to remove mount on active node.

                                     @pre         None

                                     @post        None

                                     @return      0 - success
                                                  -1 - failed

                                     @exception   None
    */
   /*=================================================================== */
static ACE_INT32 removeNFSOnActive();
 	/*=================================================================== */
   /**
                                     @brief      Method used to remove mount on active node.

                                     @pre         None

                                     @post        None

                                     @return      0 - success
                                                  -1 - failed

                                     @exception   None
    */
   /*=================================================================== */
	static ACE_INT32 unmountFileM();
	/*=================================================================== */
   /**
                                     @brief      Method used to check the fileM mount on peer node.

                                     @pre         None

                                     @post        None

                                     @return      true - fileM mounted
                                                  false - fileM not mounted

                                     @exception   None
    */
   /*=================================================================== */
	static bool isFileMmounted();

	 /*=================================================================== */
   /**
                                     @brief      Method used to copy files of active node on passive node.

                                     @pre         None

                                     @post        None

                                     @return      0 - success
                                                  -1 - failed

                                     @exception   None
    */
   /*=================================================================== */
	static ACE_INT32 copyFileListOnPassive(const ACE_TCHAR *fileList[], ACE_INT32 nooffiles);
	 /*=================================================================== */
   /**
                                     @brief      Method used to remove all files of active node on passive node.

                                     @pre         None

                                     @post        None

                                     @return      0 - success
                                                  -1 - failed

                                     @exception   None
    */
   /*=================================================================== */
	static ACE_INT32 removeFilesOnPassive();
	/*=================================================================== */
   /**
                                     @brief      Method used to unmount passive /media on active node.

                                     @pre         None

                                     @post        None

                                     @return      0 - success
                                                  -1 - failed

                                     @exception   None
    */
   /*=================================================================== */
	static ACE_INT32 unmountPassiveDVDOnActive();
	//static ACE_INT32 unmountPassiveBackupOnActive();
	static ACE_INT32 createEMFFolder();
	static ACE_INT32 unmountPassiveDVDData(bool bisShutdown, bool bUnmountMedia=true);
	static ACE_INT32 isDVDEmpty();
	static ACE_INT32 isDvdCorrectlyMounted(bool bIsLocal);
	static ACE_INT32 isMediaAlreadyMountedOnActive();
	static ACE_INT32 isMediaAlreadyMountedOnPassive();


	static ACE_INT32 mountMediaOnNode();
	static ACE_INT32 umountUSBData();
	static ACE_INT32 mountThumbDrive();
	static ACE_INT32 CheckFilesystem();
	static ACE_INT32 mountMedia();
	static ACE_INT32 mountUSBMedia();
	static ACE_INT32 unMountUSBMedia();
	static ACE_INT32 formatAndMountMedia();

	//static ACE_INT32 mountcheckForMediainPassive();
	//static ACE_INT32 formatAndMountPassiveMedia();
	static ACE_INT32 copyFromMasterUsbMedia();
	static ACE_INT32 copyToMediaFolder();
	static ACE_INT32 appendVolumeName(const ACE_TCHAR *);
	static ACE_INT32 getMasterUsb();
	static ACE_INT32 setMasterUsb(ACE_INT16 slotnum);
	static void getLocalNode(int32_t *piSystemId, acs_dsd::NodeStateConstants *penmNodeState);
	static ACE_INT32 setMediaFlag(ACE_INT16 pos,ACE_INT16 flag);
	static ACE_INT32 getMediaFlag(ACE_INT16 pos);
	static ACE_INT32 removeMediaData(ACE_INT16 node);
	static ACE_INT32 removeLFFromMedia();
	static ACE_INT32 _execlp(const ACE_TCHAR *cmdStr);
	static string getMediaMount();
	static ACE_INT32 removeaDataFromFileM(string dname);
	static ACE_INT32 getMediaSpaceInfo(ACE_INT32 nodeState,ACE_TCHAR* availablespace,ACE_TCHAR* usedspace,ACE_TCHAR* totalspace);
 	static bool isFileObjectDirectory(string& aPath);
 	static bool isMountd();
	static bool isThumbDriveHealthy();
	static ACE_TCHAR* match(ACE_TCHAR* Str1, ACE_TCHAR* Str2);
	static ACE_INT32 getPeerNodeState(int32_t &nodeState);
public:
	/*===================================================================
						 PUBLIC ATTRIBUTE
	=================================================================== */
	/**
			  @brief     parentDNofEMF

	 */
   static std::string parentDNofEMF;

private:
   static ACE_INT32 getMediaUsedSpaceMediaInfo(ACE_UINT64 *iUsedSpace);
   static ACE_INT32 getMediaUsedSpaceIsoInfo(ACE_UINT64 *iUsedSpace);
};

/*
 @TODO: AS ErrorHandling is not yet ready.... it is commented for time being
class EMFErrorObjectHandler : public ACS_APGCC_RuntimeOwner {
public:
		EMFErrorObjectHandler();
        //virtual ~MyObjectHandler();
        // Implementation of virtual method
        ACS_CC_ReturnType updateCallback(const char* p_objName, const char* p_attrName);
};
*/
#endif
