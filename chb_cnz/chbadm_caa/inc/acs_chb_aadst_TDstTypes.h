#pragma once

#include <string>
#include <list>

////////////////////////////// Int define //////////////////////////////
#define INT_AADST_DATE_SIZE						2
#define INT_AADST_CASTC_YEARBASE			2000

////////////////////////////// String define //////////////////////////////
#define STR_AADST_CASTC_DATBEG				"DATBEG="
#define STR_AADST_CASTC_TIMBEG				"TIMBEG="
#define STR_AADST_CASTC_DATEND				"DATEND="
#define STR_AADST_CASTC_TIMEND				"TIMEND="
#define STR_AADST_CASTC_CLKADJ				"CLKADJ="
#define STR_AADST_CASTC_TIMEZONE			"TIMEZONE="
#define STR_AADST_CASTC_SEPARATOR			","

////////////////////////////// Char define //////////////////////////////
#define CHR_AADST_NUMBER_FILL					'0'

////////////////////////////// Class TDstTime: contain time information //////////////////////////////
class TDstTime
{
	public:
	////////////////////////////////////////////// Costructor/Distructor
		TDstTime();
		virtual ~TDstTime();
	////////////////////////////////////////////// Public method
		// Set the time
		void setTime(int iHour, int iMin, int iSec);
		// Get the time
		void getTime(int *piHour, int *piMin, int *piSec) const;
		// Set the date
		void setDate(int iDay, int iMonth, int iYear);
		// Get the date
		void getDate(int *piDay, int *piMonth, int *piYear) const;
	////////////////////////////////////////////// Private Attributes
	private:
		int m_iSec;
		int m_iMin;
		int m_iHour;
		int m_iYear;
		int m_iMonth;
		int m_iDay;
};

////////////////////////////// Class TZdumpItem: Contains timezone information //////////////////////////////
class TZdumpItem
{
	public:
	////////////////////////////////////////////// Costructor/Distructor
	TZdumpItem();
	virtual ~TZdumpItem();
	////////////////////////////////////////////// Public method
		// Set the UT time
		void setUtTime(int iHour, int iMin, int iSec);
		// Get the UT time
		void getUtTime(int *piHour, int *piMin, int *piSec) const;
		// Set the UT date
		void setUtDate(int iDay, int iMonth, int iYear);
		// Get the UT date
		void getUtDate(int *piDay, int *piMonth, int *piYear) const;
		// Set the TZ time
		void setTzTime(int iHour, int iMin, int iSec);
		// Get the TZ time
		void getTzTime(int *piHour, int *piMin, int *piSec) const;
		// Set the TZ date
		void setTzDate(int iDay, int iMonth, int iYear);
		// Get the TZ date
		void getTzDate(int *piDay, int *piMonth, int *piYear) const;
		// Set GmtOffset
		inline void setGmtOffset(long int iGmtOff) {m_iGmtOff = iGmtOff;};
		// Get GmtOffset
		inline long int getGmtOffset() const {return (m_iGmtOff);};
		// Set DST flag
		void setDst(int iIsDst);
		// Get DST flag
		bool getDst() const {return(m_bIsDst);};
	////////////////////////////////////////////// Private Attributes
	private:
		// Time info in UTC
		TDstTime m_UtDate;
		// Time info relative to timezone
		TDstTime m_TzDate;
		// Offset from UTZ and timezone
		long int m_iGmtOff;
		// flag: true if the isdst is 1. False otherwise
		bool m_bIsDst;
};

////////////////////////////// Class TZdumpData: Contains timezone data template //////////////////////////////
class TZdumpData
{
	public:
	////////////////////////////////////////////// Costructor/Distructor
		TZdumpData();
		virtual ~TZdumpData();
	////////////////////////////////////////////// Public method
		// Set timezone name
		void setZoneName(const std::string &strName);
		// Get timezone name
		void getZoneName(std::string *pstrName) const;
		// Get internal list reference
		std::list<TZdumpItem*>* getZdumpLst() {return(&m_lstItem);};
		// Clone from object
		void clone(const TZdumpData& objFrom);
		// Release the list resource
		void Clear();
	////////////////////////////////////////////// Private Attributes
	private:
		std::string m_strTzName;
		std::list<TZdumpItem*> m_lstItem;
};

////////////////////////////// Class TCastcItem: Contains CASTC arguments data //////////////////////////////
class TCastcItem
{
	public:
	////////////////////////////////////////////// Costructor/Distructor
		TCastcItem();
		virtual ~TCastcItem();
	////////////////////////////////////////////// Public method
		// Set the TIMBEG time
		void setTimBeg(int iHour, int iMin);
		// Get the TIMBEG time
		void getTimBeg(int *piHour, int *piMin) const;
		// Set the DATBEG date
		void setDatBeg(int iDay, int iMonth, int iYear);
		// Get the DATBEG date
		void getDatBeg(int *piDay, int *piMonth, int *piYear) const;
		// Set the TIMEND time
		void setTimEnd(int iHour, int iMin);
		// Get the TIMEND time
		void getTimEnd(int *piHour, int *piMin) const;
		// Set the DATEND date
		void setDatEnd(int iDay, int iMonth, int iYear);
		// Get the DATEND date
		void getDatEnd(int *piDay, int *piMonth, int *piYear) const;
		// Set the CLKADJ
		inline void setClkAdj(int iClkAdj) {m_iClkAdj	= iClkAdj;};
		// Get the CLKADJ
		int getClkAdj() const {return(m_iClkAdj);};
		// Set the TimeZone id
		inline void setTimeZoneId(int iId) {m_iIdZone = iId;};
		// Get the TimeZone id
		int getTimeZoneId() const {return(m_iIdZone);};
		// Build the date in CASTC format. If bIsDst is false the castc format 1 argument
		// are returned. Otherwise if bIsDst is true the CASTC format 2 argument are returned.
		// View method code comment for more details.
		bool buildCastcParameter(std::string *pstrArgs, bool bIsDst);
	////////////////////////////////////////////// Private method
	private:
		// Check the date
		bool CheckDate(bool bEndTimeOnly);

	////////////////////////////////////////////// Private Attributes
	private:
		// Time info "DATBEG" and "TIMBEG" for CASTC command
		TDstTime m_TimeBeg;
		// Time info "DATEND" and "TIMEND" for CASTC command
		TDstTime m_TimeEnd;
		// Time info "CLKADJ" for CASTC command
		int m_iClkAdj;
		// Time zone identifier (0 to 23)
		int m_iIdZone;
};
