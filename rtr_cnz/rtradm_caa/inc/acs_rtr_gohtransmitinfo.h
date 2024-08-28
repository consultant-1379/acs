/*=================================================================== */
/**
   @file  acs_rtr_gohtransmitinfo.h 

   @brief Header file for rtr module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       24/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_RTR_GOHTRANSMITINFO_H
#define ACS_RTR_GOHTRANSMITINFO_H
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_rtr_filebuild.h"
#include "acs_rtr_blockbuild.h"
#include "acs_rtr_statistics.h"

#include <string>

using namespace std;
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief     ACS_RTR_GohTransmitInfo
 */
/*=================================================================== */
class ACS_RTR_GohTransmitInfo
{
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief     Constructor for ACS_RTR_GohTransmitInfo class.
 */
/*=================================================================== */
	ACS_RTR_GohTransmitInfo();
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_RTR_GohTransmitInfo();
/*=================================================================== */
/**
   @brief     getRTRFile

   @return    RTRfile pointer
 */
/*=================================================================== */
	//File transfer methods
	inline RTRfile* getRTRFile() { return _file; }
/*=================================================================== */
/**
   @brief     getRTRBlock

   @return    RTRfile pointer
 */
/*=================================================================== */
	inline RTRblock* getRTRBlock() { return _block; }
/*=================================================================== */
/**
   @brief     getStatistics

   @return    RTRfile pointer
 */
/*=================================================================== */
	inline RTR_statistics* getStatistics() { return _stat; }
/*=================================================================== */
/**
   @brief     getCpName

   @return    string
 */
/*=================================================================== */
	inline string getCpName() { return *_cpname; }
/*=================================================================== */
/**
   @brief     getCpID

   @return    short
 */
/*=================================================================== */
	inline short getCpID() { return _cpId; }
/*=================================================================== */
/**
   @brief     setRTRFile

   @param     file

   @return    void
 */
/*=================================================================== */

	void setRTRFile(RTRfile* file);
/*=================================================================== */
/**
   @brief     setRTRBlock

   @param     block

   @return    void
 */
/*=================================================================== */
	void setRTRBlock(RTRblock* block);
/*=================================================================== */
/**
   @brief     setStatistics

   @param     stat

   @return    void
 */
/*=================================================================== */
	void setStatistics(RTR_statistics* stat);
/*=================================================================== */
/**
   @brief     setCpName

   @param     cpname

   @return    void
 */
/*=================================================================== */
	void setCpName(string& cpname);
/*=================================================================== */
/**
   @brief     setCpId

   @param     cpId

   @return    void
 */
/*=================================================================== */
	void setCpId(short cpId);

private:
	//File transfer informations
	RTRfile* _file;
	RTRblock* _block;
	RTR_statistics* _stat;
	string* _cpname;
	short _cpId;
};
#endif
