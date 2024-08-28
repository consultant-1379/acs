#include <sstream>
#include <iomanip>
#include <stdio.h>
#include "acs_chb_aadst_TDstTypes.h"

////////////////////////////// Class TDstTime: contain time information //////////////////////////////
TDstTime::TDstTime() :
m_iSec(-1),
m_iMin(-1),
m_iHour(-1),
m_iYear(-1),
m_iMonth(-1),
m_iDay(-1)
{

}

TDstTime::~TDstTime()
{

}

void TDstTime::setTime(int iHour, int iMin, int iSec)
{
	// Set Hour in internal member
	m_iHour = iHour;
	// Set Minutes in internal member
	m_iMin = iMin;
	// Set seconds in internal member
	m_iSec = iSec;
}

void TDstTime::getTime(int *piHour, int *piMin, int *piSec) const
{
	// Get hour if the relative pointer is not null
	if(piHour != NULL){
		// Store internal hour in piHour
		*piHour = m_iHour;
	}
	// Get minuts if the relative pointer is not null
	if(piHour != NULL){
		// Store internal minuts in piMin
		*piMin = m_iMin;
	}
	// Get seconds if the relative pointer is not null
	if(piSec != NULL){
		// Store internal seconds in piSec
		*piSec = m_iSec;
	}
}

void TDstTime::setDate(int iDay, int iMonth, int iYear)
{
	// Set Day in internal member
	m_iDay = iDay;
	// Set Month in internal member
	m_iMonth = iMonth;
	// Set Year in internal member
	m_iYear = iYear;
}

void TDstTime::getDate(int *piDay, int *piMonth, int *piYear) const
{
	// Get day if the relative pointer is not null
	if(piDay != NULL){
		// Store internal day in piDay
		*piDay = m_iDay;
	}
	// Get month if the relative pointer is not null
	if(piMonth != NULL){
		// Store internal month in piMonth
		*piMonth = m_iMonth;
	}
	// Get Year if the relative pointer is not null
	if(piYear != NULL){
		// Store internal seconds in piSec
		*piYear = m_iYear;
	}
}

////////////////////////////// Class TZdumpItem: Contains timezone information //////////////////////////////
TZdumpItem::TZdumpItem() :
m_iGmtOff(0),
m_bIsDst(false)
{
}

TZdumpItem::~TZdumpItem()
{
}

void TZdumpItem::setUtTime(int iHour, int iMin, int iSec)
{
	m_UtDate.setTime(iHour, iMin, iSec);
}

void TZdumpItem::getUtTime(int *piHour, int *piMin, int *piSec) const
{
	m_UtDate.getTime(piHour, piMin, piSec);
}

void TZdumpItem::setUtDate(int iDay, int iMonth, int iYear)
{
	m_UtDate.setDate(iDay, iMonth, iYear);
}

void TZdumpItem::getUtDate(int *piDay, int *piMonth, int *piYear) const
{
	m_UtDate.getDate(piDay, piMonth, piYear);
}

void TZdumpItem::setTzTime(int iHour, int iMin, int iSec)
{
	m_TzDate.setTime(iHour, iMin, iSec);
}

void TZdumpItem::getTzTime(int *piHour, int *piMin, int *piSec) const
{
	m_TzDate.getTime(piHour, piMin, piSec);
}

void TZdumpItem::setTzDate(int iDay, int iMonth, int iYear)
{
	m_TzDate.setDate(iDay, iMonth, iYear);
}

void TZdumpItem::getTzDate(int *piDay, int *piMonth, int *piYear) const
{
	m_TzDate.getDate(piDay, piMonth, piYear);
}

void TZdumpItem::setDst(int iIsDst)
{
	// Set m_bIsDst to false if iIsDst==0; true otherwise.
	if(iIsDst == 0){
		m_bIsDst = false;
	}else{
		m_bIsDst = true;
	}
}

////////////////////////////// Class TZdumpData: Contains timezone data template //////////////////////////////
TZdumpData::TZdumpData()
{
	// Clear the list (prevention)
	Clear();
}

TZdumpData::~TZdumpData()
{
	// Release the list resource
	Clear();
}

void TZdumpData::setZoneName(const std::string &strName)
{
	// Set Zone name
	m_strTzName = strName;
}

void TZdumpData::getZoneName(std::string *pstrName) const
{
	// Retrive zone name
	if(pstrName!=NULL){
		*pstrName = m_strTzName;
	}
}

void TZdumpData::clone(const TZdumpData& objFrom)
{
	int iHour;
	int iMin;
	int iSec;
	int iDay;
	int iMonth;
	int iYear;
	long int iGmtOff;
	int iIsDst;
	bool bIsDst;
	TZdumpItem* pObjTo;
	std::list<TZdumpItem*>::const_iterator it;
	// Initialization
	iHour = -1;
	iMin = -1;
	iSec = -1;
	iDay = -1;
	iMonth = -1;
	iYear = -1;
	iGmtOff = -1;
	iIsDst = -1;
	bIsDst = false;
	pObjTo = NULL;
	// Copy the zone name
	m_strTzName = objFrom.m_strTzName;
	// Initialize the iterator
	it = objFrom.m_lstItem.begin();
	// For each element in the list
	while (it != objFrom.m_lstItem.end()){
		// Create the object
		pObjTo = new TZdumpItem();
		// Insert in the destination list
		m_lstItem.push_back(pObjTo);
		// Copy the UT time
		(*it)->getUtTime(&iHour, &iMin, &iSec);
		pObjTo->setUtTime(iHour, iMin, iSec);
		// Copy the UT date
		(*it)->getUtDate(&iDay, &iMonth, &iYear);
		pObjTo->setUtDate(iDay, iMonth, iYear);
		// Copy the TZ time
		(*it)->getTzTime(&iHour, &iMin, &iSec);
		pObjTo->setTzTime(iHour, iMin, iSec);
		// Copy the TZ date
		(*it)->getTzDate(&iDay, &iMonth, &iYear);
		pObjTo->setTzDate(iDay, iMonth, iYear);
		// Copy GmtOffset
		iGmtOff = (*it)->getGmtOffset();
		pObjTo->setGmtOffset(iGmtOff);
		// Copy DST flag
		bIsDst = (*it)->getDst();
		// Convert isdst
		if(bIsDst == false){
			iIsDst = 0;
		}else{
			iIsDst = 1;
		}
		pObjTo->setDst(iIsDst);
		// Next item
		++it;
	}
}

void TZdumpData::Clear()
{
	std::list<TZdumpItem*>::iterator it;
	// Set iterator to begin of the internal list
	it = m_lstItem.begin();
	// For all element in m_lstItem
	while(it != m_lstItem.end()){
		// Release the memori
		delete(*it);
		// Next element
		++it;
	}
	// Clear the list
	m_lstItem.clear();
}

////////////////////////////// Class TCastcItem: Contains CASTC arguments data //////////////////////////////
TCastcItem::TCastcItem() :
m_iClkAdj (0),
m_iIdZone (-1)
{
}

TCastcItem::~TCastcItem()
{
}

void TCastcItem::setTimBeg(int iHour, int iMin)
{
	// Set begin time with 0 seconds (CASTC not use the seconds)
	m_TimeBeg.setTime(iHour, iMin, 0);
}

void TCastcItem::getTimBeg(int *piHour, int *piMin) const
{
	// Get begin time (CASTC not use the seconds)
	m_TimeBeg.getTime(piHour, piMin, NULL);
}

void TCastcItem::setDatBeg(int iDay, int iMonth, int iYear)
{
	// Set the begin date
	m_TimeBeg.setDate(iDay, iMonth, iYear);
}

void TCastcItem::getDatBeg(int *piDay, int *piMonth, int *piYear) const
{
	// Get the begin date
	m_TimeBeg.getDate(piDay, piMonth, piYear);
}

void TCastcItem::setTimEnd(int iHour, int iMin)
{
	// Set End time with 0 seconds (CASTC not use the seconds)
	m_TimeEnd.setTime(iHour, iMin, 0);
}

void TCastcItem::getTimEnd(int *piHour, int *piMin) const
{
	// Get End time (CASTC not use the seconds)
	m_TimeEnd.getTime(piHour, piMin, NULL);
}

void TCastcItem::setDatEnd(int iDay, int iMonth, int iYear)
{
	// Set the End date
	m_TimeEnd.setDate(iDay, iMonth, iYear);
}

void TCastcItem::getDatEnd(int *piDay, int *piMonth, int *piYear) const
{
	// Get the End date
	m_TimeEnd.getDate(piDay, piMonth, piYear);
}

bool TCastcItem::buildCastcParameter(std::string *pstrArgs, bool bIsDst)
{
	int iMin;
	int iHour;
	int iYear;
	int iMonth;
	int iDay;
	int iClkAdj;
	int iIdTz;
	bool bRet;
	std::stringstream strTmp;
	// Initialization
	iMin = -1;
	iHour = -1;
	iYear = -1;
	iMonth = -1;
	iDay = -1;
	iClkAdj = -1;
	iIdTz = -1;
	bRet = false;
	// Check for error
	if(CheckDate(bIsDst) == true){
		// Check if is necessaary first part: CASTC use two syntax:
		// 1) We not are in the summer time: specify data begin and data end
		// 2) We are in the summer time: only the end summer time must be specified.
		if(bIsDst == false){
			// Get Begin parameter
			getDatBeg(&iDay, &iMonth, &iYear);
			getTimBeg(&iHour, &iMin);
			// Build: DATBEG
			strTmp << STR_AADST_CASTC_DATBEG;
			strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << (iYear - INT_AADST_CASTC_YEARBASE);
			strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << iMonth;
			strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << iDay;
			strTmp << STR_AADST_CASTC_SEPARATOR;
			// Build TIMBEG
			strTmp << STR_AADST_CASTC_TIMBEG;
			strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << iHour;
			strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << iMin;
			strTmp << STR_AADST_CASTC_SEPARATOR;
		}
		// Get time and date end
		getDatEnd(&iDay, &iMonth, &iYear);
		getTimEnd(&iHour, &iMin);
		// Build "DATEND"
		strTmp << STR_AADST_CASTC_DATEND;
		strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << (iYear - INT_AADST_CASTC_YEARBASE);
		strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << iMonth;
		strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << iDay;
		strTmp << STR_AADST_CASTC_SEPARATOR;
		// Build TIMEND
		strTmp << STR_AADST_CASTC_TIMEND;
		strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << iHour;
		strTmp << std::setfill(CHR_AADST_NUMBER_FILL) << std::setw(INT_AADST_DATE_SIZE) << iMin;
		strTmp << STR_AADST_CASTC_SEPARATOR;
		// Build CLKADJ
		iClkAdj = getClkAdj();
		strTmp << STR_AADST_CASTC_CLKADJ;
		strTmp << iClkAdj;
		strTmp << STR_AADST_CASTC_SEPARATOR;
		// Build TIMEZONE
		iIdTz = getTimeZoneId();
		strTmp << STR_AADST_CASTC_TIMEZONE;
		strTmp << iIdTz;
		// Assign to output args
		pstrArgs->assign(strTmp.str());
		// Set ok
		bRet = true;
	}
	// Exit from method
	return(bRet);
}

////////////////////////////////////////////// Private method
bool TCastcItem::CheckDate(bool bEndTimeOnly)
{
	//int iSec;
	int iMin;
	int iHour;
	int iYear;
	int iMonth;
	int iDay;
	bool bRet;
	// Initialization
	//iSec = -1;
	iMin = -1;
	iHour = -1;
	iYear = -1;
	iMonth = -1;
	iDay = -1;
	bRet = false;
	// Get begin date
	getDatBeg(&iDay, &iMonth, &iYear);
	// Get begin time
	getTimBeg(&iHour, &iMin);
	// Check start date only for CASTC format 2 (only end time is valid)
	if((bEndTimeOnly == true) || ((iDay <= 31) && (iDay > 0) && (iMonth <= 12) && (iMonth > 0) &&
		 (iHour >= 0) && (iHour < 24) && (iMin >= 0) && (iMin < 60) &&
		 (iYear - INT_AADST_CASTC_YEARBASE >= 0) && (iYear - INT_AADST_CASTC_YEARBASE < 100))){
		// Get end date
		getDatEnd(&iDay, &iMonth, &iYear);
		// Get end time
		getTimEnd(&iHour, &iMin);
		// Check date
		if((iDay <= 31) && (iDay > 0) && (iMonth <= 12) && (iMonth > 0) &&
		   (iHour >= 0) && (iHour < 24) && (iMin >= 0) && (iMin < 60) &&
			 (iYear - INT_AADST_CASTC_YEARBASE >= 0) && (iYear - INT_AADST_CASTC_YEARBASE < 100)){
			// Begin date is ok
			bRet = true;
		}
	}
	// Exit from method
	return (bRet);
}
