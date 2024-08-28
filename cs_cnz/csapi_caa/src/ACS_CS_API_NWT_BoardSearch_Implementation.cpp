#include<algorithm>
#include "ACS_CS_API_NWT_BoardSearch_Implementation.h"

using namespace std;


ACS_CS_API_NWT_BoardSearch_Implementation::ACS_CS_API_NWT_BoardSearch_Implementation()
{
	reset();
}

ACS_CS_API_NWT_BoardSearch_Implementation::~ACS_CS_API_NWT_BoardSearch_Implementation()
{}

void ACS_CS_API_NWT_BoardSearch_Implementation::setNtwName (string networkName)
{
	netwName = networkName;
}

void ACS_CS_API_NWT_BoardSearch_Implementation::setAdminState (string admnStat)
{
	(void)transform(admnStat.begin(), admnStat.end(), admnStat.begin(), ::toupper);
	adminState = admnStat;
}

void ACS_CS_API_NWT_BoardSearch_Implementation::setExtNetwID (string externalNetwId)
{
	extNetwId = externalNetwId;
}

void ACS_CS_API_NWT_BoardSearch_Implementation::setIntNetwID (string internalNetwId)
{
	intNetwId = internalNetwId;
}

void ACS_CS_API_NWT_BoardSearch_Implementation::setMAC (string macAddr)
{
	mac = macAddr;
}

void ACS_CS_API_NWT_BoardSearch_Implementation::setInterfaceID (string intfaceId)
{
	interfaceId = intfaceId;
}

void ACS_CS_API_NWT_BoardSearch_Implementation::setBoardID (BoardID brdId)
{
	boardId =  brdId;
}

void ACS_CS_API_NWT_BoardSearch_Implementation::reset ()
{
	boardId = (BoardID)0;
	extNetwId = "";
	intNetwId = "";
	interfaceId = "";
	netwName = "";
	mac = "";
	adminState = "";
}

bool ACS_CS_API_NWT_BoardSearch_Implementation::match(const ACS_CS_API_NetworkTable_R1 &tableRow)
{
    bool matched = true;
    //Following conditions check for given search criteria
    if(!extNetwId.empty() && extNetwId != tableRow.extNetwId)
    	matched = false;
    if(!intNetwId.empty() && intNetwId != tableRow.intNetwId)
    	matched = false;
    if(boardId != (BoardID)0 && boardId != tableRow.boardId)
    	matched = false;
    if(!adminState.empty() && adminState.compare(tableRow.adminState)!=0)
    	matched = false;
    if(!interfaceId.empty() && interfaceId.compare(tableRow.interfaceId)!=0)
    	matched = false;
    if(!netwName.empty() && netwName.compare(tableRow.netwName)!=0)
    	matched = false;
    if(!mac.empty() && mac.compare(tableRow.mac)!=0)
    	matched = false;
    return matched;
 }




