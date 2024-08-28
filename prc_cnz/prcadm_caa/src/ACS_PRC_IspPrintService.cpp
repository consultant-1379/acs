#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
//#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <arpa/inet.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iomanip>

#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Acceptor.h>
#include "ace/Signal.h"
#include "ace/Sig_Handler.h"
#include "ace/Task.h"

#include "acs_apgcc_paramhandling.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_PRC_SignalHandler.h"
#include "ACS_PRC_ispprintBackupThread.h"
#include "ACS_PRC_IspLogEvent.h"
#include "ACS_PRC_ispapi.h"
#include "acs_prc_api.h"
#include "acs_prc_api_impl.h"
#include "ACS_TRA_Logging.h"
#include <ACS_APGCC_Util.H>
#include "ACS_PRC_IspAdminOperationImplementer.h"
#include "ACS_PRC_IspAdminOperationImplementerThread.h"
#include "ACS_PRC_IspNotificationThread.h"
#include "ACS_PRC_prcispThread.h"
#include "acs_apgcc_oihandler_V2.h"

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

const char pipeName[] = "/var/run/ap/AcsIspprintServer";
//Moved creation of /cluster/etc/ap/acs/prc/conf to service level
const char PRC_CONFIG_DIR_PRC_NAME[] = "/cluster/etc/ap/acs/prc/";
const char PRC_CONFIG_DIR_CONF_NAME[] = "/cluster/etc/ap/acs/prc/conf/";


static int siglist[] = {SIGKILL, SIGINT, SIGTERM, SIGABRT, SIGTSTP };
bool is_swm_2_0_isp;
ACE_UNIX_Addr addr(pipeName);
ACE_LSOCK_Acceptor Acceptor;
ACE_LSOCK_Stream stream;

using namespace std;

bool utcFlag      = false;
struct tm tmp1Time ={0,0,0,0,0,0,0,0,0,0,0};
struct tm tmp2Time={0,0,0,0,0,0,0,0,0,0,0};

#define FILE "/var/run/ap/prcispd.lck"
#define MAX_C 128
int stat(const char *path, struct stat *buf);

const char *state[10] = {"down                    ",
		"degraded (non redundant)",
		"degraded (redundant)    ",
		"up (non redundant)      ",
		"up (redundant)          ",
		"unknown                 "};

//option parameters
bool ispprint     = false;
bool analyse      = false;
bool runLevel     = false;
bool nodeState    = false;
bool serviceState = false;
bool reason       = false;
bool detailedInfo = false;

bool startTime    = false;
bool endTime      = false;
bool shortPrint   = false;
bool logSize 	  = false;
bool ownnode	  = false;

bool bApState = false;
bool bNodeStateA = false;
bool bNodeStateB = false;
bool bNodeRunLvlA = false;
bool bNodeRunLvlB = false;

vector<ACS_PRC_IspLogEvent> L;
vector<ACS_PRC_IspLogEvent> tmpEvent;
vector<ACS_PRC_IspLogEvent> tmpService;
vector<ACS_PRC_IspLogEvent> tmpServiceLast;
vector<ACS_PRC_IspLogEvent> tmpServiceFirst;
vector<ACS_PRC_IspLogEvent> tmpApStateEvent;
vector<ACS_PRC_IspLogEvent> tmpNodeStateEventA;
vector<ACS_PRC_IspLogEvent> tmpNodeStateEventB;
vector<ACS_PRC_IspLogEvent> tmpRunLevelStateEventA;
vector<ACS_PRC_IspLogEvent> tmpRunLevelStateEventB;
vector<ACS_PRC_IspLogEvent> tmpServiceStateEventA;
vector<ACS_PRC_IspLogEvent> tmpServiceStateEventB;

string begin_time;
string start_time;
string end_time;
string current_time;

time_t addCurrentTime;

char host_name[32] = {0};

bool sortByDate(ACS_PRC_IspLogEvent obj1 , ACS_PRC_IspLogEvent obj2){

	bool bRet = false;

	if((obj1.getDate().compare(obj2.getDate()) < 0))
		bRet = true;

	return bRet;
}

//****************************************************************************
//	eventAnalyserByNode()

//  used to calculate time in seconds between two event of same node
//****************************************************************************
double eventAnalyserByNode(const vector <ACS_PRC_IspLogEvent>& vett,int i,int pos)
{

	if (pos<i)
		pos=i;
	double dif=0;
	struct tm tmp1Time ={0,0,0,0,0,0,0,0,0,0,0};
	struct tm tmp2Time={0,0,0,0,0,0,0,0,0,0,0};

	//start time
	sscanf(vett[i].getDate().c_str(),"%d-%d-%d %d:%d:%d",&tmp1Time.tm_year,&tmp1Time.tm_mon,&tmp1Time.tm_mday,&tmp1Time.tm_hour,&tmp1Time.tm_min,&tmp1Time.tm_sec);

	tmp1Time.tm_year -= 1900;
	tmp1Time. tm_isdst = -1;
	tmp1Time.tm_mon -= 1;
	time_t start = mktime (&tmp1Time);
	//end time
	sscanf(vett[pos].getDate().c_str(),"%d-%d-%d %d:%d:%d",&tmp2Time.tm_year,&tmp2Time.tm_mon,&tmp2Time.tm_mday,&tmp2Time.tm_hour,&tmp2Time.tm_min,&tmp2Time.tm_sec);

	tmp2Time.tm_year -= 1900;
	tmp2Time. tm_isdst = -1;
	tmp2Time.tm_mon -= 1;
	time_t end = mktime (&tmp2Time);
	//update last position
	//pos = i;

	if(end > start){
		dif = difftime(end,start);
	}else{
		dif = difftime(start,end);
	}

	if(dif < 0){
		dif = -dif;
	}

	return dif;
}
//****************************************************************************************
//	eventAnalyser()

//  used to calculate time in seconds between two equals events without any other condition
//*****************************************************************************************
double eventAnalyser(const vector <ACS_PRC_IspLogEvent>& vett,unsigned int i,const char* event)
{

	double dif=0;
	struct tm tmp1Time ={0,0,0,0,0,0,0,0,0,0,0};
	struct tm tmp2Time={0,0,0,0,0,0,0,0,0,0,0};

	while( i < vett.size()-1 && strcmp(vett[i+1].getEvent().c_str(),event)!=0)
	{

		//start time
		sscanf(vett[i].getDate().c_str(),"%d-%d-%d %d:%d:%d",&tmp1Time.tm_year,&tmp1Time.tm_mon,&tmp1Time.tm_mday,&tmp1Time.tm_hour,&tmp1Time.tm_min,&tmp1Time.tm_sec);
		tmp1Time.tm_year -= 1900;
		tmp1Time. tm_isdst = -1;
		time_t start = mktime (&tmp1Time);
		//end time
		sscanf(vett[i+1].getDate().c_str(),"%d-%d-%d %d:%d:%d",&tmp2Time.tm_year,&tmp2Time.tm_mon,&tmp2Time.tm_mday,&tmp2Time.tm_hour,&tmp2Time.tm_min,&tmp2Time.tm_sec);
		tmp2Time.tm_year -= 1900;
		tmp2Time. tm_isdst = -1;
		time_t end = mktime (&tmp2Time);
		//update last position
		//pos = i;
		dif += difftime(end,start);
		i++;
	}

	return dif;
}
//****************************************************************************************
//	ConvertFromLocaltoUTC()

bool check_date( string data ){


	if ( ( data.length() == 19 ) && ( data[4] == '-' ) && ( data[7] == '-' ) && ( data[13] == ':' ) && ( data[16] == ':' ) )
		return true;
	else
		return false;
}

//  used to convert date in string format to UTC time format
//*****************************************************************************************
string ConvertFromLocaltoUTC( string data ){

	struct tm temp={0,0,0,0,0,0,0,0,0,0,0};

	char temp_data_str[128] = {0};

	strncpy (temp_data_str, data.c_str(), data.size());

	char* token = strtok (temp_data_str,"- :");

	temp.tm_year = atoi(token) - 1900;

	token = strtok (NULL,"- :");
	temp.tm_mon = atoi(token) - 1;

	token = strtok (NULL,"- :");
	temp.tm_mday = atoi(token);

	token = strtok (NULL,"- :");
	temp.tm_hour = atoi(token);

	token = strtok (NULL,"- :");
	temp.tm_min = atoi(token);

	token = strtok (NULL,"- :");
	temp.tm_sec = atoi(token);

	time_t temp_utc = mktime(&temp);
	tm* utc_time = gmtime ( &temp_utc );


	strftime (temp_data_str,20,"%Y-%m-%d %H:%M:%S",utc_time);

	data = string(temp_data_str);
	return data;
}
//****************************************************************************************
//	printTime()

//  used to print events between start time "t" and end time "x"
//*****************************************************************************************
void printTime( const vector<ACS_PRC_IspLogEvent>& List , string t, string x, bool short_print){

	string temp;
	char buffer[2048] = {0};

	if ( utcFlag )
		temp = "ISP log from " + t + " to " + x + " (UTC)\n";
	else
		temp = "ISP log from " + t + " to " + x + " (Local Time)\n";

	*reinterpret_cast<uint32_t *>(buffer) = htonl ( temp.size());
	strcpy( buffer +4, temp.c_str());
	stream.send_n(buffer, temp.size() + 4 );
	temp.clear();

	for (unsigned int i=0; i< List.size();i++){

		if( strcmp( List[i].getDate().c_str(), t.c_str()) >= 0 && strcmp( List[i].getDate().c_str(), x.c_str()) < 1){
			//cout << List[i].getEvent().c_str() << endl;
			if( strcmp( List[i].getEvent().c_str(), "Node Run Level Event" ) == 0 )	{
				temp.append(List[i].getDate());
				temp.append(" Node '");
				temp.append(List[i].getHostname());
				temp.append("' entered into runlevel '");
				temp.append(List[i].getRunLevel());
				temp.append("'. Reason was '");
				temp.append(List[i].getReasonInfo());
				if (strcmp( List[i].getMiscInfo().c_str(), " " ) != 0){
					temp.append("'. Info: ");
					temp.append(List[i].getMiscInfo());}else temp.append("'.");
				temp.append("\n");
				*reinterpret_cast<uint32_t *>(buffer) = htonl ( temp.size());
				strcpy( buffer +4, temp.c_str());
				stream.send_n(buffer, temp.size() + 4 );
				temp.clear();

			}else if( strcmp( List[i].getEvent().c_str(), "AP state event" ) == 0 ){
				//					temp.append(List[i].getDate());
				//					temp.append(" AP '");
				//					temp.append(List[i].getHostname());
				//					temp.append("' is in state '");
				//					temp.append(List[i].getRunLevel());
				//					temp.append("'. Reason was '");
				//					temp.append(List[i].getReasonInfo());
				//					if (strcmp( List[i].getMiscInfo().c_str(), " " ) != 0){Stand-by
				//					temp.append("'. Info: ");
				//					temp.append(List[i].getMiscInfo());}else temp.append("'.");
				//					temp.append("\n");
				//					*reinterpret_cast<uint32_t *>(buffer) = htonl ( temp.size());
				//					strcpy( buffer +4, temp.c_str());
				//					stream.send_n(buffer, temp.size() + 4 );
				temp.clear();
				continue;
			}
			else if ( ( strcmp( List[i].getEvent().c_str(), "Service Unit state event" ) == 0 ) && (!short_print)){
				temp.append(List[i].getDate());
				temp.append(" Resource '");
				temp.append(List[i].getResourceName());
				temp.append("' in node '");
				temp.append(List[i].getHostname());
				temp.append("' entered into state '");
				temp.append(List[i].getRunLevel());
				temp.append("'. Reason was '");
				temp.append(List[i].getReasonInfo());
				if (strcmp( List[i].getMiscInfo().c_str(), " " ) != 0){
					temp.append("'. Info: ");
					temp.append(List[i].getMiscInfo());}else temp.append("'.");
				temp.append("\n");
				*reinterpret_cast<uint32_t *>(buffer) = htonl ( temp.size());
				strcpy( buffer +4, temp.c_str());
				stream.send_n(buffer, temp.size() + 4 );
				temp.clear();
			}
			else if ( strcmp( List[i].getEvent().c_str(), "Node state event" ) == 0 ){
				temp.append(List[i].getDate());
				temp.append(" Node '");
				temp.append(List[i].getHostname());
				temp.append("' entered into state '");
				temp.append(List[i].getRunLevel());
				temp.append("'. Reason was '");
				temp.append(List[i].getReasonInfo());
				if (strcmp( List[i].getMiscInfo().c_str(), " " ) != 0){
					temp.append("'. Info: ");
					temp.append(List[i].getMiscInfo());}else temp.append("'.");
				temp.append("\n");
				*reinterpret_cast<uint32_t *>(buffer) = htonl ( temp.size());
				strcpy( buffer +4, temp.c_str());
				stream.send_n(buffer, temp.size() + 4 );
				temp.clear();
			}
			else if ( strcmp( List[i].getEvent().c_str(), "Sync event" ) == 0 ){

				temp.append(List[i].getDate());
				temp.append(" Synchronizing event in node ");
				temp.append(List[i].getHostname());
				temp.append("\n   ");

				temp.append("Node state is '");
				temp.append(List[i].getRunLevel().substr(0, List[i].getRunLevel().find("@@")));

				temp.append("'. Reason is '");
				temp.append(List[i].getRunLevel().substr(List[i].getRunLevel().find("@@") + 2));
				temp.append("'.\n   ");

				temp.append("Runlevel is '");
				temp.append(List[i].getReason().substr(0, List[i].getReason().find("@@")));

				temp.append("'. Reason is '");
				temp.append(List[i].getReason().substr(List[i].getReason().find("@@") + 2));
				temp.append("'.\n");

				*reinterpret_cast<uint32_t *>(buffer) = htonl ( temp.size());
				strcpy( buffer +4, temp.c_str());
				stream.send_n(buffer, temp.size() + 4 );
				temp.clear();
			}
		}
	}
}

int getNumenrOfEvent ( string node ){

	int numberOfEvent = 0;

	for (unsigned int i=0; i< L.size(); i++)
		if ( L[i].getHostname() == node )
			numberOfEvent++;

	return numberOfEvent;
}

void checkLastEvent(){

	vector<ACS_PRC_IspLogEvent>::reverse_iterator it;
	vector<ACS_PRC_IspLogEvent>::iterator itTemp;


	for ( it = tmpEvent.rbegin(); it != tmpEvent.rend(); it++ ){

		if((strcmp(it->getEvent().c_str(), "AP state event") == 0) && !bApState){

			it->setDate(start_time);
			L.push_back(*it);
			it->setDate(end_time);
			L.push_back(*it);
			bApState = true;
			continue;

		}else if((strcmp(it->getEvent().c_str(), "Node state event") == 0) &&
				(strcmp(it->getHostname().c_str(), "A") == 0 ) && !bNodeStateA){

			it->setDate(start_time);
			L.push_back(*it);
			it->setDate(end_time);
			L.push_back(*it);
			bNodeStateA = true;
			continue;

		}else if((strcmp(it->getEvent().c_str(), "Node state event") == 0) &&
				(strcmp(it->getHostname().c_str(), "B") == 0 ) && !bNodeStateB){

			it->setDate(start_time);
			L.push_back(*it);
			it->setDate(end_time);
			L.push_back(*it);
			bNodeStateB = true;
			continue;

		}else if((strcmp(it->getEvent().c_str(), "Node Run Level Event") == 0) &&
				(strcmp(it->getHostname().c_str(), "A") == 0 ) && !bNodeRunLvlA){

			it->setDate(start_time);
			L.push_back(*it);
			it->setDate(end_time);
			L.push_back(*it);
			bNodeRunLvlA = true;
			continue;

		}else if((strcmp(it->getEvent().c_str(), "Node Run Level Event") == 0) &&
				(strcmp(it->getHostname().c_str(), "B") == 0 ) && !bNodeRunLvlB){

			it->setDate(start_time);
			L.push_back(*it);
			it->setDate(end_time);
			L.push_back(*it);
			bNodeRunLvlB = true;
			continue;

		}

	}
}

void checkStateEvent(){

	vector<ACS_PRC_IspLogEvent>::reverse_iterator it, itTemp;
	bool bFirst = false;
	bool bLast = false;

	int sizeApstate = (int)tmpApStateEvent.size();

	for (int i = 0; i < sizeApstate; i++ ){

		if( strcmp(tmpApStateEvent[i].getDate().c_str(), start_time.c_str()) > -1 && strcmp(tmpApStateEvent[i].getDate().c_str(), end_time.c_str()) < 1){

			if((i-1 >= 0) && (!bFirst)){
				tmpApStateEvent[i-1].setDate(start_time);
				L.push_back(tmpApStateEvent[i-1]);
				bFirst = true;
			}

			if((i+1 < sizeApstate) && (!bLast)){
				tmpApStateEvent[i+1].setDate(end_time);
				L.push_back(tmpApStateEvent[i+1]);
				bLast = true;
			}
		}else if(strcmp(tmpApStateEvent[sizeApstate-1].getDate().c_str(), start_time.c_str()) < 1){

			tmpApStateEvent[sizeApstate-1].setDate(start_time);
			L.push_back(tmpApStateEvent[sizeApstate-1]);
			bFirst = true;//TR :HV63315 Change
		}
	}

	bFirst = false;
	bLast = false;

	int sizeNodeA = (int)tmpNodeStateEventA.size();
	for (int i = 0; i < sizeNodeA; i++ ){

		if( strcmp(tmpNodeStateEventA[i].getDate().c_str(), start_time.c_str()) > -1 && strcmp(tmpNodeStateEventA[i].getDate().c_str(), end_time.c_str()) < 1){

			if((i-1 >= 0) && (!bFirst)){
				tmpNodeStateEventA[i-1].setDate(start_time);
				L.push_back(tmpNodeStateEventA[i-1]);
				bFirst = true;
			}
			if((i+1 < sizeNodeA) && (!bLast)){
				tmpNodeStateEventA[i+1].setDate(end_time);
				L.push_back(tmpNodeStateEventA[i+1]);
				bLast = true;
			}
		}else if(strcmp(tmpNodeStateEventA[sizeNodeA-1].getDate().c_str(), start_time.c_str()) < 1){

			tmpNodeStateEventA[sizeNodeA-1].setDate(start_time);
			L.push_back(tmpNodeStateEventA[sizeNodeA-1]);
			bFirst = true;

		}
	}


	bFirst = false;
	bLast = false;

	int sizeNodeB = (int)tmpNodeStateEventB.size();
	for (int i = 0; i < sizeNodeB; i++ ){

		if( strcmp(tmpNodeStateEventB[i].getDate().c_str(), start_time.c_str()) > -1 && strcmp(tmpNodeStateEventB[i].getDate().c_str(), end_time.c_str()) < 1){

			if((i-1 >= 0) && (!bFirst)){
				tmpNodeStateEventB[i-1].setDate(start_time);
				L.push_back(tmpNodeStateEventB[i-1]);
				bFirst = true;
			}
			if((i+1 < sizeNodeB) && (!bLast)){
				tmpNodeStateEventB[i+1].setDate(end_time);
				L.push_back(tmpNodeStateEventB[i+1]);
				bLast = true;
			}
		}else if(strcmp(tmpNodeStateEventB[sizeNodeB-1].getDate().c_str(), start_time.c_str()) < 1){

			tmpNodeStateEventB[sizeNodeB-1].setDate(start_time);
			L.push_back(tmpNodeStateEventB[sizeNodeB-1]);
			bFirst = true;

		}
	}


	bFirst = false;
	bLast = false;

	int sizeRunLevA = (int)tmpRunLevelStateEventA.size();

	for (int i = 0; i < sizeRunLevA; i++ ){

		if( strcmp(tmpRunLevelStateEventA[i].getDate().c_str(), start_time.c_str()) > -1 && strcmp(tmpRunLevelStateEventA[i].getDate().c_str(), end_time.c_str()) < 1){

			if((i-1 >= 0) && (!bFirst)){
				tmpRunLevelStateEventA[i-1].setDate(start_time);
				L.push_back(tmpRunLevelStateEventA[i-1]);
				bFirst = true;
			}
			if((i+1 < sizeRunLevA) && (!bLast)){
				tmpRunLevelStateEventA[i+1].setDate(end_time);
				L.push_back(tmpRunLevelStateEventA[i+1]);
				bLast = true;
			}
		}else if(strcmp(tmpRunLevelStateEventA[sizeRunLevA-1].getDate().c_str(), start_time.c_str()) < 1){

			tmpRunLevelStateEventA[sizeRunLevA-1].setDate(start_time);
			L.push_back(tmpRunLevelStateEventA[sizeRunLevA-1]);
			bFirst = true;

		}
	}

	bFirst = false;
	bLast = false;

	int sizeRunLevB = (int)tmpRunLevelStateEventB.size();

	for (int i = 0; i < sizeRunLevB; i++ ){

		if( strcmp(tmpRunLevelStateEventB[i].getDate().c_str(), start_time.c_str()) > -1 && strcmp(tmpRunLevelStateEventB[i].getDate().c_str(), end_time.c_str()) < 1){

			if((i-1 >= 0) && (!bFirst)){
				tmpRunLevelStateEventB[i-1].setDate(start_time);
				L.push_back(tmpRunLevelStateEventB[i-1]);
				bFirst = true;
			}
			if((i+1 < sizeRunLevB) && (!bLast)){
				tmpRunLevelStateEventB[i+1].setDate(end_time);
				L.push_back(tmpRunLevelStateEventB[i+1]);
				bLast = true;
			}
		}else if(strcmp(tmpRunLevelStateEventB[sizeRunLevB-1].getDate().c_str(), start_time.c_str()) < 1){

			tmpRunLevelStateEventB[sizeRunLevB-1].setDate(start_time);
			L.push_back(tmpRunLevelStateEventB[sizeRunLevB-1]);
			bFirst = true;
		}
	}

	bFirst = false;
	bLast = false;

	int sizeServiceA = (int)tmpServiceStateEventA.size();

	for (int i = 0; i < sizeServiceA; i++ ){

		if( strcmp(tmpServiceStateEventA[i].getDate().c_str(), start_time.c_str()) > -1 && strcmp(tmpServiceStateEventA[i].getDate().c_str(), end_time.c_str()) < 1){

			if((i-1 >= 0) && (!bFirst)){
				tmpServiceStateEventA[i-1].setDate(start_time);
				L.push_back(tmpServiceStateEventA[i-1]);
				bFirst = true;
			}
			if((i+1 < sizeServiceA) && (!bLast)){
				tmpServiceStateEventA[i+1].setDate(end_time);
				L.push_back(tmpServiceStateEventA[i+1]);
				bLast = true;
			}
		}else if(strcmp(tmpServiceStateEventA[sizeServiceA-1].getDate().c_str(), start_time.c_str()) < 1){

			tmpServiceStateEventA[sizeServiceA-1].setDate(start_time);
			L.push_back(tmpServiceStateEventA[sizeServiceA-1]);
			bFirst = true;

		}

	}

	bFirst = false;
	bLast = false;

	int sizeServiceB = (int)tmpServiceStateEventB.size();

	for (int i = 0; i < sizeServiceB; i++ ){

		if( strcmp(tmpServiceStateEventB[i].getDate().c_str(), start_time.c_str()) > -1 && strcmp(tmpServiceStateEventB[i].getDate().c_str(), end_time.c_str()) < 1){

			if((i-1 >= 0) && (!bFirst)){
				tmpServiceStateEventB[i-1].setDate(start_time);
				L.push_back(tmpServiceStateEventB[i-1]);
				bFirst = true;
			}
			if((i+1 < sizeServiceB) && (!bLast)){
				tmpServiceStateEventB[i+1].setDate(end_time);
				L.push_back(tmpServiceStateEventB[i+1]);
				bLast = true;
			}
		}else if(strcmp(tmpServiceStateEventB[sizeServiceB-1].getDate().c_str(), start_time.c_str()) < 1){

			tmpServiceStateEventB[sizeServiceB-1].setDate(start_time);
			L.push_back(tmpServiceStateEventB[sizeServiceB-1]);
			bFirst = true;

		}
	}

}

void checkLastService(){

	bool bFound = false;
	bool bFirstFound = false;


	vector<ACS_PRC_IspLogEvent>::reverse_iterator it;
	vector<ACS_PRC_IspLogEvent>::iterator itServ,itFirst;

	for ( it = tmpService.rbegin(); it != tmpService.rend(); it++ ){
		bFound = false;

		for ( itServ = tmpServiceLast.begin(); itServ != tmpServiceLast.end() ; itServ++ ){

			if((strcmp(it->getResourceName().c_str(), itServ->getResourceName().c_str()) == 0) &&
					(strcmp(it->getHostname().c_str(), itServ->getHostname().c_str()) == 0)){

				bFound = true;
				break;
			}
		}

		if(!bFound){

			tmpServiceLast.push_back(*it);

		}

	}

	for(itServ = tmpServiceLast.begin();itServ != tmpServiceLast.end(); itServ++){
		bFirstFound = false;
		for ( itFirst = tmpServiceFirst.begin(); itFirst != tmpServiceFirst.end() ; itFirst++ ){

			if((strcmp(itFirst->getResourceName().c_str(), itServ->getResourceName().c_str()) == 0) &&
					(strcmp(itFirst->getHostname().c_str(), itServ->getHostname().c_str()) == 0)){

				bFirstFound = true;
				break;

			}
		}

		if(!bFirstFound ){

			itServ->setDate(start_time);
			L.push_back(*itServ);
			itServ->setDate(end_time);
			L.push_back(*itServ);
		}
	}

}

void createIspLogEvent ( string path_file ){

	string tmp;
	ACS_PRC_IspLogEvent E;

	vector<string> line;
	char* token;
	int tmp_size = 0;
	int tmp_size_row = 0;

	char buffer_log[4096] = {0};
	bool bOpen = false;

	ifstream ifs;
	ifs.open( path_file.c_str() );

	bApState = false;
	bNodeStateA = false;
	bNodeStateB = false;
	bNodeRunLvlA = false;
	bNodeRunLvlB = false;



	while ( ifs.good() ){

		bOpen = true;
		ifs.getline(buffer_log, 4096); // get every line...
		tmp_size_row = strlen(buffer_log);
		tmp.clear();

		if ( ifs.good() ){

			token = strtok (buffer_log,";");

			while (token != NULL) {
				line.push_back( (string) token);
				token = strtok (NULL, ";");
			}

			if ( ( line.size() >= 3 ) && (line.size() <= 9 )&& ( line[0].find("ISP Event") != string::npos ) ){

				int lSize = 0;
				lSize= 9 - line.size();
				while( lSize > 0){
					//if ( line.size() <= 8 ){
					line.push_back(" ");
					lSize--;
				}
				//IF EMPTY LIST - to solve not existing time problem -t -x


				if ( L.size() == 0 && line.size() == 0 )
				{
					E.setEvent(" ");
					E.setDate(" ");
					E.setHostname(" ");
					E.setRunLevel(" ");
					E.setReason(" ");
					E.setReasonInfo(" ");
					E.setMiscInfo(" ");
					L.push_back(E);
				}

				if ( check_date ( line[2] ) ){
					if ( utcFlag )
						tmp = ConvertFromLocaltoUTC ( line[2] );
					else
						tmp = line[2];
				}

				//ANALYSE OPTION LIST
				if (analyse)
				{
					if (strcmp(line[1].c_str(),"Service Unit state event")==0)
					{
						E.setEvent(line[1]);
						E.setDate(tmp);
						E.setResourceName(line[3]); //otherInfo used to store resource name
						E.setHostname(line[4]);
						E.setRunLevel(line[5]);
						E.setReason(line[6]);
						E.setReasonInfo(line[7]); // add 1 every field
						E.setMiscInfo(line[8]);
						if(strcmp(E.getHostname().c_str(), "A") == 0){
							tmpServiceStateEventA.push_back(E);
						}else{
							tmpServiceStateEventB.push_back(E);
						}

					}else{
						E.setEvent(line[1]);
						E.setDate(tmp);
						E.setHostname(line[3]);
						E.setRunLevel(line[4]);
						E.setReason(line[5]);
						E.setReasonInfo(line[6]);
						E.setMiscInfo(line[7]);
						if(strcmp(line[1].c_str(),"AP state event")==0){
							tmpApStateEvent.push_back(E);
						}else if((strcmp(line[1].c_str(),"Node state event")==0) && (strcmp(E.getHostname().c_str(), "A") == 0)){
							tmpNodeStateEventA.push_back(E);
						}else if((strcmp(line[1].c_str(),"Node state event")==0) && (strcmp(E.getHostname().c_str(), "B") == 0)){
							tmpNodeStateEventB.push_back(E);
						}else if((strcmp(line[1].c_str(),"Node Run Level Event")==0) && (strcmp(E.getHostname().c_str(), "A") == 0)){
							tmpRunLevelStateEventA.push_back(E);
						}else if((strcmp(line[1].c_str(),"Node Run Level Event")==0) && (strcmp(E.getHostname().c_str(), "B") == 0)){
							tmpRunLevelStateEventB.push_back(E);
						}
					}

					if( strcmp(tmp.c_str(), start_time.c_str()) > -1 && strcmp(tmp.c_str(), end_time.c_str()) < 1)
					{
						E.setEvent(line[1]);
						E.setDate(tmp);
						if (strcmp(line[1].c_str(),"Service Unit state event")==0)
						{
							E.setResourceName(line[3]); //otherInfo used to store resource name
							E.setHostname(line[4]);
							E.setRunLevel(line[5]);
							E.setReason(line[6]);
							E.setReasonInfo(line[7]); // add 1 every field
							E.setMiscInfo(line[8]);
							tmpServiceFirst.push_back(E);
							L.push_back(E);
							line.clear();
						}
						else
						{
							E.setHostname(line[3]);
							E.setRunLevel(line[4]);
							E.setReason(line[5]);
							E.setReasonInfo(line[6]);
							E.setMiscInfo(line[7]);

							if(strcmp(line[1].c_str(),"AP state event")==0){
								bApState = true;
							}

							if(strcmp(line[1].c_str(),"Node state event") == 0){
								if(strcmp(E.getHostname().c_str(), "A") == 0){
									bNodeStateA = true;
								}else{
									bNodeStateB = true;
								}

							}

							if(strcmp(line[1].c_str(),"Node Run Level Event") == 0){
								if(strcmp(E.getHostname().c_str(), "A") == 0){
									bNodeRunLvlA = true;
								}else{
									bNodeRunLvlB = true;
								}

							}
							L.push_back(E);
							line.clear();
						}
						//tmp_size += E.getEventSize();
						tmp_size += tmp_size_row;
					}else if(strcmp(tmp.c_str(), start_time.c_str()) < 1){
						E.setEvent(line[1]);
						E.setDate(tmp);
						if (strcmp(line[1].c_str(),"Service Unit state event")==0)
						{
							E.setResourceName(line[3]); //otherInfo used to store resource name
							E.setHostname(line[4]);
							E.setRunLevel(line[5]);
							E.setReason(line[6]);
							E.setReasonInfo(line[7]); // add 1 every field
							E.setMiscInfo(line[8]);
							tmpService.push_back(E);
							line.clear();
						}
						else
						{
							E.setHostname(line[3]);
							E.setRunLevel(line[4]);
							E.setReason(line[5]);
							E.setReasonInfo(line[6]);
							E.setMiscInfo(line[7]);
							tmpEvent.push_back(E);
							line.clear();
						}

					}

					////////////////

					tmp.clear();
					line.clear();
				} // end option a
				//OWNNODE OPTION LIST
				else if (ownnode){
					if( shortPrint ){
						if ( strcmp( line[1].c_str(), "Service Unit state event" ) != 0 ){
							if( (strcmp(tmp.c_str(), start_time.c_str())) > -1 && (strcmp(tmp.c_str(), end_time.c_str())) < 1){
								if ( strcmp(line[3].c_str(),host_name) == 0 ) {
									E.setEvent(line[1]);
									E.setDate(tmp);
									tmp.clear();
									E.setHostname(line[3]);
									E.setRunLevel(line[4]);
									E.setReason(line[5]);
									E.setReasonInfo(line[6]);
									E.setMiscInfo(line[7]);
									L.push_back(E);
									//tmp_size += E.getEventSize();
									tmp_size += tmp_size_row;
									line.clear();
								}
							}
						}
					}
					else {
						if( (strcmp(tmp.c_str(), start_time.c_str())) > -1 && (strcmp(tmp.c_str(), end_time.c_str())) < 1) {
							if ( strcmp(line[1].c_str(),"Service Unit state event") == 0) {
								if ( strcmp(line[4].c_str(),host_name) == 0 ) {
									E.setEvent(line[1]);
									E.setDate(tmp);
									tmp.clear();
									E.setResourceName(line[3]); //otherInfo used to store resource name
									E.setHostname(line[4]);
									E.setRunLevel(line[5]);
									E.setReason(line[6]);
									E.setReasonInfo(line[7]); // add 1 every field
									E.setMiscInfo(line[8]);
									L.push_back(E);
									//tmp_size += E.getEventSize();
									tmp_size += tmp_size_row;
									line.clear();
								}
							}
							else {
								if ( strcmp(line[3].c_str(),host_name) == 0 ) {
									E.setEvent(line[1]);
									E.setDate(tmp);
									tmp.clear();
									E.setHostname(line[3]);
									E.setRunLevel(line[4]);
									E.setReason(line[5]);
									E.setReasonInfo(line[6]);
									E.setMiscInfo(line[7]);
									L.push_back(E);
									//tmp_size += E.getEventSize();
									tmp_size += tmp_size_row;
									line.clear();
								}
							}
						}
					}
					tmp.clear();
					line.clear();
				}
				//ALL OTHER OPTION LIST
				else
				{
					if( shortPrint ){
						if ( strcmp( line[1].c_str(), "Service Unit state event" ) != 0 ){
							if( (strcmp(tmp.c_str(), start_time.c_str())) > -1 && (strcmp(tmp.c_str(), end_time.c_str())) < 1){
								//if ( strcmp(line[3].c_str(),host_name) == 0 ) {
								E.setEvent(line[1]);
								E.setDate(tmp);
								tmp.clear();
								E.setHostname(line[3]);
								E.setRunLevel(line[4]);
								E.setReason(line[5]);
								E.setReasonInfo(line[6]);
								E.setMiscInfo(line[7]);
								L.push_back(E);
								//tmp_size += E.getEventSize();
								tmp_size += tmp_size_row;
								line.clear();
								//}
							}
						}
					}
					else {
						if( (strcmp(tmp.c_str(), start_time.c_str())) > -1 && (strcmp(tmp.c_str(), end_time.c_str())) < 1) {
							E.setEvent(line[1]);
							E.setDate(tmp);

							if (strcmp(line[1].c_str(),"Service Unit state event")==0)
							{
								E.setResourceName(line[3]); //otherInfo used to store resource name
								E.setHostname(line[4]);
								E.setRunLevel(line[5]);
								E.setReason(line[6]);
								E.setReasonInfo(line[7]); // add 1 every field
								E.setMiscInfo(line[8]);
								L.push_back(E);
								line.clear();
							}
							else
							{
								E.setHostname(line[3]);
								E.setRunLevel(line[4]);
								E.setReason(line[5]);
								E.setReasonInfo(line[6]);
								E.setMiscInfo(line[7]);
								L.push_back(E);
								line.clear();
							}
							//tmp_size += E.getEventSize();
							tmp_size += tmp_size_row;
						}
					}
				}
			}

			tmp.clear();
			line.clear();
		}

		memset( buffer_log, 0, sizeof( buffer_log ));
	}

	if(bOpen && analyse){
		checkLastEvent();
		checkLastService();
		checkStateEvent();
	}

	ifs.close();



}

double addCurrentDate(const vector <ACS_PRC_IspLogEvent>& vett,int i)
{

	double dif=0;
	struct tm tmp1Time ={0,0,0,0,0,0,0,0,0,0,0};

	//start time
	sscanf(vett[i].getDate().c_str(),"%d-%d-%d %d:%d:%d",&tmp1Time.tm_year,&tmp1Time.tm_mon,&tmp1Time.tm_mday,&tmp1Time.tm_hour,&tmp1Time.tm_min,&tmp1Time.tm_sec);
	tmp1Time.tm_year -= 1900;
	tmp1Time.tm_isdst = -1;
	tmp1Time.tm_mon -= 1;
	time_t start = mktime (&tmp1Time);
	time_t rawtime;


	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	if(rawtime > start){
		dif = difftime(rawtime,start);
	}else{
		dif = difftime(start,rawtime);
	}

	return dif;
}

double setStartDate(const vector <ACS_PRC_IspLogEvent>& vett,int i)
{

	double dif=0;
	struct tm tmp1Time ={0,0,0,0,0,0,0,0,0,0,0};
	struct tm tmp2Time={0,0,0,0,0,0,0,0,0,0,0};

	//start time
	sscanf(start_time.c_str(),"%d-%d-%d %d:%d:%d",&tmp1Time.tm_year,&tmp1Time.tm_mon,&tmp1Time.tm_mday,&tmp1Time.tm_hour,&tmp1Time.tm_min,&tmp1Time.tm_sec);
	tmp1Time.tm_year -= 1900;
	tmp1Time. tm_isdst = -1;
	//tmp1Time.tm_mon -= 1;
	time_t start = mktime (&tmp1Time);
	//end time
	sscanf(vett[i].getDate().c_str(),"%d-%d-%d %d:%d:%d",&tmp2Time.tm_year,&tmp2Time.tm_mon,&tmp2Time.tm_mday,&tmp2Time.tm_hour,&tmp2Time.tm_min,&tmp2Time.tm_sec);
	tmp2Time.tm_year -= 1900;
	tmp2Time. tm_isdst = -1;
	//tmp1Time.tm_mon -= 1;
	time_t end = mktime (&tmp2Time);
	//update last position
	//pos = i;
	dif = difftime(end,start);

	return dif;

}

double setEndDate(const vector <ACS_PRC_IspLogEvent>& vett,int i)
{

	double dif=0;
	struct tm tmp1Time ={0,0,0,0,0,0,0,0,0,0,0};
	struct tm tmp2Time={0,0,0,0,0,0,0,0,0,0,0};

	//start time
	sscanf(vett[i].getDate().c_str(),"%d-%d-%d %d:%d:%d",&tmp1Time.tm_year,&tmp1Time.tm_mon,&tmp1Time.tm_mday,&tmp1Time.tm_hour,&tmp1Time.tm_min,&tmp1Time.tm_sec);
	tmp1Time.tm_year -= 1900;
	tmp1Time. tm_isdst = -1;
	//tmp1Time.tm_mon -= 1;
	time_t start = mktime (&tmp1Time);
	//end time
	sscanf(end_time.c_str(),"%d-%d-%d %d:%d:%d",&tmp2Time.tm_year,&tmp2Time.tm_mon,&tmp2Time.tm_mday,&tmp2Time.tm_hour,&tmp2Time.tm_min,&tmp2Time.tm_sec);
	tmp2Time.tm_year -= 1900;
	tmp2Time. tm_isdst = -1;
	//tmp1Time.tm_mon -= 1;
	time_t end = mktime (&tmp2Time);
	//update last position
	//pos = i;
	dif = difftime(end,start);

	return dif;

}

int main (int /*argc*/, char * /*argv*/[]) {
	int fdl, retry = 0;
	char strErr_no[1024] = {0};
	stringstream ss;//create a stringstream
	bool result = false;
	/* TR HV63313 change start
	ACS_PRC_prcispThread PrcispSocketTread;
	PrcispSocketTread.activate();
	TR HV63313 change stop */ //As the prcisp_syslog.log rotation is handled by /etc/logrot.d/acs_prc_isp_conf file
	// commenting this part to avoid duplication in /etc/logrot.d/default
	is_swm_2_0_isp = isSWM20();
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");
	//Added retry mechanism to create a lock file
	do {
		if ( (fdl = open(FILE, O_RDWR | O_CREAT | O_CLOEXEC,0664)) < 0) {
			snprintf(strErr_no,1024,"acs_prcispd %s - Unable to open lock file - retry - count = %i", PRCBIN_REVISION, retry );
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			++retry;
			sleep(60);
		} else {
			result = true;
		}
	}while((!result) && ( retry < 10 ));
	if(!result)
	{
		snprintf(strErr_no,1024,"acs_prcispd %s - Unable to open lock file - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		return 1;
	}

	memset(strErr_no, 0, sizeof(strErr_no));

	if (flock(fdl, LOCK_EX | LOCK_NB)) {
		if (errno == EWOULDBLOCK) {
			snprintf(strErr_no,1024,"acs_prcispd %s - Lock active. Another process instance is running - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
			Logging.Write(strErr_no, LOG_LEVEL_ERROR);
			Logging.Close();
			return 2;
		}
		snprintf(strErr_no,1024,"acs_prcispd %s - Lock failed - Return Code 3 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();
		return 3;
	}

        //creating /cluster/etc/ap/acs/prc/conf directory
        int status = ACE_OS::mkdir(PRC_CONFIG_DIR_PRC_NAME, S_IRWXU | S_IRWXG );
        if ( status != 0 )
        {
                ACE_UINT32  dwError = ACS_APGCC::getLastError();
                if (EEXIST != dwError)
                {
                        memset(strErr_no, 0, sizeof(strErr_no));
                        snprintf(strErr_no,1024,"acs_prcispd %s - Unable to create the first level directory for storing config parameters - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
                        Logging.Write(strErr_no, LOG_LEVEL_ERROR);
                        Logging.Close();
                        return 1;

                }
        }
        status = ACE_OS::mkdir(PRC_CONFIG_DIR_CONF_NAME, S_IRWXU | S_IRWXG );
        if ( status != 0 )
        {
                ACE_UINT32 dwError = ACS_APGCC::getLastError();
                if (EEXIST != dwError)
                {
                        memset(strErr_no, 0, sizeof(strErr_no));
                        snprintf(strErr_no,1024,"acs_prcispd %s - Unable to create the second level directory for storing config parameters - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
                        Logging.Write(strErr_no, LOG_LEVEL_ERROR);
                        Logging.Close();
                        return 1;
                }
        }

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcispd %s - Start", PRCBIN_REVISION);
	Logging.Write(strErr_no, LOG_LEVEL_WARN);

	ACS_PRC_API prc_node_state;

	ACS_PRC_IspNotificationThread PrcispNotificationThread;
	PrcispNotificationThread.start();
	PrcispNotificationThread.activate();

	ifstream ifs;
	string p_node_hostname_1 = "A";
	string p_node_hostname_2 = "B";

	ACS_PRC_ispprintBackupThread backupThread;
	backupThread.init_logrotd(true);
	backupThread.start();
	backupThread.activate();

	bool running      = true;

	//	vector APstate - Nodestate - Runlevelstate - Servicestate
	//  this container used to match with events attributes
	//  The use of vectors has been chosen because others
	//  elements could dynamically insert into lists

	vector <string>  Apstate;
	/*Apstate.push_back("Down");
	Apstate.push_back("DrNr");
	Apstate.push_back("Dr");
	Apstate.push_back("Nr");
	Apstate.push_back("Up");
	Apstate.push_back("unknown");*/

	Apstate.push_back("0"); //Down
	Apstate.push_back("1"); //Degraded Not redundant
	Apstate.push_back("2"); // Degraded
	Apstate.push_back("3"); // Up Not Redundant
	Apstate.push_back("4"); // Up
	Apstate.push_back("5"); //unknown

	vector<string> Nodestate;
	Nodestate.push_back("active");
	Nodestate.push_back("passive");
	Nodestate.push_back("unknown");

	vector<string> Runlevelstate;
	Runlevelstate.push_back("0");
	Runlevelstate.push_back("1");
	Runlevelstate.push_back("2");
	Runlevelstate.push_back("5");

	vector<string> Servicestate;
	Servicestate.push_back("Active");
	Servicestate.push_back("Stand-by");
	Servicestate.push_back("unknown");

	time_t ftime = 0;
	char now [80] = {0};

	gethostname(host_name,sizeof(host_name));
	string IsplogPathName = "/cluster/etc/ap/acs/prc/prcisp_syslog.log";
	string IsplogPathName_1 = "/cluster/etc/ap/acs/prc/prcisp_syslog.log.1";

	char buffer[4096] = {0};

	ACE_Sig_Handler ace_sigdispatcher;

	ACS_PRC_SignalHandler ispprint_Server_sighandler ( running );
	int numsigs = sizeof ( siglist ) / sizeof(int);
	for ( int i=0 ; i<numsigs ; ++i )
		ace_sigdispatcher.register_handler(siglist[i], & ispprint_Server_sighandler);

	::unlink(pipeName);

	if ( Acceptor.open(addr) != 0 ){

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcispd %s - ACE_LSOCK_Acceptor - Acceptor.open fails - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
		Logging.Write(strErr_no, LOG_LEVEL_ERROR);
		Logging.Close();

		Acceptor.close();
		Acceptor.remove();
		exit(1);
	}

	Ispapi p;
	p.open();

	while ( ispprint_Server_sighandler.getValue() ){

		begin_time.clear();
		start_time.clear();
		end_time.clear();
		current_time.clear();

		vector<string> line;

		char* cmd;

		if ( Acceptor.accept(stream,0,0,0) == -1 ){
			Acceptor.close();
			Acceptor.remove();
			if ( ACE_OS::last_error() != EINTR ){
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcispd %s - ACE_LSOCK_Acceptor - Acceptor.accept fails - Return Code 1 - errno = %i", PRCBIN_REVISION, ACE_OS::last_error() );
				Logging.Write(strErr_no, LOG_LEVEL_ERROR);
				Logging.Close();
				exit(1);
			}

		} else {

			stream.recv(buffer,4096);

			if ( strcmp(buffer,"SHUTDOWN") == 0) {
				memset(strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,1024,"acs_prcispd %s - Shutdown", PRCBIN_REVISION);
				Logging.Write(strErr_no, LOG_LEVEL_WARN);

				stream.close();
				break;
			}

			ftime = time(NULL);
			strftime (now,80,"%Y-%m-%d %X",localtime ( &ftime ));
			current_time = (string) now;

			addCurrentTime = time(NULL);
			localtime(&addCurrentTime);

			int result=0;

			if ((result = p.getFirstEventLogTime(begin_time)) <0)
				begin_time = current_time;

			//options analysis
			for (unsigned int i=0;i < strlen(buffer); i++){

				switch (buffer[i]) { // ispprint option check

				case 'a':
					analyse      = true;
					break;
				case 'l':
					runLevel     = true;
					break;
				case 'n':
					nodeState    = true;
					break;
				case 's':
					serviceState = true;
					break;
				case 'r':
					reason       = true;
					break;
				case 'd':
					detailedInfo = true;
					break;
				case 'g':
					utcFlag      = true;
					break;
				case 't':
					startTime    = true;
					break;
				case 'x':
					endTime      = true;
					break;
				case 'c':
					shortPrint   = true;
					break;
				case 'z':
					logSize = true;
					break;
				case 'm':
					ownnode = true;
					break;
				case 'N':
					ispprint   = true;
					break;
				}
			}

			if (startTime && !endTime)
			{
				//tokenizing buffer to take time value
				cmd = strtok (buffer,";@");
				while (cmd != NULL)
				{
					//cout<<cmd<<endl;
					line.push_back( (string) cmd);
					cmd = strtok (NULL, ";@");
				}
				for (unsigned int i=0; i < line.size(); i++)
					if (line[i] == "t")	{
						start_time = line[i+1];
						end_time = current_time;
					}
			}
			else if (endTime && !startTime)
			{
				//tokenizing buffer to take time value
				cmd = strtok (buffer,";@");
				while (cmd != NULL)
				{
					//cout<<cmd<<endl;
					line.push_back( (string) cmd);
					cmd = strtok (NULL, ";@");
				}
				for (unsigned int i=0;i<line.size();i++)
					if (line[i] == "x")
					{
						//end_time = strdup(line[i+1].c_str());
						end_time = line[i+1];
						start_time = begin_time;
					}
			}
			else if (startTime && endTime) {
				//tokenizing buffer to take time value
				cmd = strtok (buffer,";@");
				while (cmd != NULL)
				{
					//cout<<cmd<<endl;
					line.push_back( (string) cmd);
					cmd = strtok (NULL, ";@");
				}
				for (unsigned int i=0;i<line.size();i++)
				{
					if (line[i] == "t")
					{
						//start_time = strdup(line[i+1].c_str());
						start_time = line[i+1];
						//cout<<start_time<<endl;
					}
					else if (line[i] == "x")
					{
						//end_time = strdup(line[i+1].c_str());
						end_time = line[i+1];
						//cout<<end_time<<endl;
					}
				}
			}
			else if (!startTime && !endTime)
			{
				start_time = begin_time;
				end_time = current_time;
			}

			if (strcmp(begin_time.c_str(),start_time.c_str()) > 0)
				//Start time less than log file creation time, use the time
				start_time = begin_time;
			else if (strcmp(current_time.c_str(),end_time.c_str()) < 0)
				// End time greater than current time, use the current time
				end_time = current_time;

			line.clear();

			//HEADER TIME CONVERSION
			string tmp2;
			if ( utcFlag ) { // -g option
				//start time conversion
				tmp2 = ConvertFromLocaltoUTC (start_time);
				start_time.clear();
				start_time = tmp2;
				tmp2.clear();
				//end time conversion
				tmp2 = ConvertFromLocaltoUTC (end_time);
				end_time.clear();
				end_time = tmp2;
				tmp2.clear();
			}

			if ( strlen(buffer) != 0) {

				L.clear();
				tmpApStateEvent.clear();
				tmpNodeStateEventA.clear();
				tmpNodeStateEventB.clear();
				tmpRunLevelStateEventA.clear();
				tmpRunLevelStateEventB.clear();
				tmpServiceStateEventA.clear();
				tmpServiceStateEventB.clear();

				createIspLogEvent( IsplogPathName_1);

				createIspLogEvent( IsplogPathName );

				sort(L.begin(),L.end(),sortByDate);

				//***********************************************************************************************

				//				Command Request

				//cout<<"******SHORT PRINTOUT REQUEST*************************************"<<endl;
				if (!analyse && ( shortPrint && !logSize )) {
					//printShort(L,start_time,end_time);
					printTime(L,start_time,end_time,shortPrint);
					shortPrint=false;
					utcFlag=false;
				}

				//cout<<"******OTHER PARAMETER REQUEST************************************"<<endl;

				else if ((ispprint || utcFlag || ownnode || startTime || endTime) && !analyse && !logSize){
					printTime(L,start_time,end_time,shortPrint);
					ispprint=false;
					ownnode=false;
					utcFlag=false;
					startTime=false;
					endTime=false;
				}

				//cout<<"****PARAMETER -Z********************************************"<<endl;
				else if (logSize)
				{
					logSize = false;
					struct stat statbuf;
					struct stat statbuf_1;
					int total_size = 0;
					string log_size;

					if ( stat(IsplogPathName.c_str(), &statbuf) == 0 )
						total_size += statbuf.st_size;

					if ( stat(IsplogPathName_1.c_str(), &statbuf_1) == 0 )
						total_size += statbuf_1.st_size;

					int Event_Node_1 = getNumenrOfEvent ( "A" );
					int Event_Node_2 = getNumenrOfEvent ( "B" );

					sprintf(buffer+4,"ISP log size : %d\n",total_size);
					*reinterpret_cast<uint32_t *>(buffer) = htonl ( strlen(buffer+4) );
					stream.send_n(buffer, strlen(buffer+4) + 4);

					memset ( buffer, 0, sizeof(buffer) );

					sprintf(buffer+4,"Number of events for %s : %d", p_node_hostname_1.c_str(), Event_Node_1);
					*reinterpret_cast<uint32_t *>(buffer) = htonl ( strlen(buffer+4) );
					stream.send_n(buffer, strlen(buffer+4) + 4);

					memset ( buffer, 0, sizeof(buffer) );

					sprintf(buffer+4,"Number of events for %s : %d\n", p_node_hostname_2.c_str(), Event_Node_2);
					*reinterpret_cast<uint32_t *>(buffer) = htonl ( strlen(buffer+4) );
					stream.send_n(buffer, strlen(buffer+4) + 4);

					memset ( buffer, 0, sizeof(buffer) );

				}
				//cout<<"****ALL ANALYSE PARAMETERS***********************************"<<endl;

				else {

					memset ( buffer, 0, sizeof(buffer) );

					std::ostringstream oss;

					oss<<"ISP log analysis from " + start_time + " to " + end_time;
					if (utcFlag)
						oss<<" (UTC)       \n";
					else
						oss<<" (Local Time)\n";

					string outS(oss.str());

					*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
					strcpy( buffer +4, outS.c_str());
					stream.send_n(buffer, outS.size() + 4 );
					oss.str("");
					outS.clear();

					memset ( buffer, 0, sizeof(buffer) );

					if (analyse && L.size() > 0) {

						//LIST TO STORE POSITION WHERE FOUND ITEMS
						vector<int> pos_Ap,
						pos_NodeA,pos_NodeB,
						pos_RunLevelA,pos_RunLevelB,
						pos_Service,pos_ServiceB,
						pos_Detail_Ord,pos_Detail_Sp,pos_Detail_OrdB,pos_Detail_SpB;

						//COUNTER TO STORE TOTAL TIME
						double 	totalApTime=0,
								totalNodeATime=0,
								totalNodeBTime=0,
								totalRunLTimeA=0,
								totalRunLTimeB=0;

						double minutes=0,s=0,hours,m=0;

						//COUNTER TO STORE EVENTS TIME
						double counter_Ap[Apstate.size()],
						counter_NodeA[Nodestate.size()],
						counter_NodeB[Nodestate.size()],
						counter_RunLevelA[Runlevelstate.size()],
						counter_RunLevelB[Runlevelstate.size()],
						counter_Service[MAX_C]={0},
						counter_ServiceB[MAX_C]={0},
						counter_Reason_Ord[MAX_C]={0},
						counter_Reason_Sp[MAX_C]={0},
						counter_Reason_OrdB[MAX_C]={0},
						counter_Reason_SpB[MAX_C]={0},
						counter_Service_run[MAX_C]={0},
						counter_Service_stp[MAX_C]={0},
						counter_Service_runB[MAX_C]={0},
						counter_Service_stpB[MAX_C]={0};

						double counter_Detail_Ord[MAX_C][MAX_C],
						counter_Detail_Sp[MAX_C][MAX_C],
						counter_Detail_OrdB[MAX_C][MAX_C],
						counter_Detail_SpB[MAX_C][MAX_C];

						//MATRIX USED TO STORE SERVICES POSITION FOUND
						vector <vector <int> > detail_Ord,detail_Sp,detail_OrdB,detail_SpB;

						vector <int> tempA,tempB;
						int t=0;
						int lastAPState = 0;
						int lastAPStateEvent = 0;

						//other options work like this down description
						if(analyse){


							int foundFirstApEvent = 0;
							for (unsigned int k=0; k < Apstate.size();k++)counter_Ap[k]=0;

							//looking for in list Ap state events
							for (unsigned int i=0; i < L.size(); i++){

								if (strcmp(L[i].getEvent().c_str(),"AP state event")==0 ){
									tempA.push_back(i);
									(foundFirstApEvent) && (totalApTime += eventAnalyserByNode(L, lastAPStateEvent, i));
									// first cicle get every line of list
									for (unsigned int k=0; k < Apstate.size();k++){
										// second cicle check if is found another element =
										if(strcmp(L[i].getRunLevel().c_str(),Apstate[k].c_str())==0){
											//count time this is active
											(foundFirstApEvent) && (counter_Ap[lastAPState] += eventAnalyserByNode(L, lastAPStateEvent, i));
											lastAPState = k;
										}
									}
									lastAPStateEvent = i;
									foundFirstApEvent = 1;
								}

							}



							if(!endTime){
								(foundFirstApEvent) && (totalApTime += addCurrentDate(L,lastAPStateEvent));
								(foundFirstApEvent) && (counter_Ap[lastAPState]+=addCurrentDate(L,lastAPStateEvent));
							}else{

								if(tempA.size() == 1){
									(totalApTime += setStartDate(L,lastAPStateEvent));
									(counter_Ap[lastAPState]+=setStartDate(L,lastAPStateEvent));
								}
								(totalApTime += setEndDate(L,lastAPStateEvent));
								(counter_Ap[lastAPState]+=setEndDate(L,lastAPStateEvent));
							}

							if ( reason ) {
								int lastAPStateDetail = 0;
								lastAPState = 0;
								for (unsigned int j=0;j<Apstate.size();j++){
									//INIZIALIZE COUNTER
									counter_Detail_Ord[j][0]=0;
									counter_Detail_Sp[j][0]=0;
									counter_Reason_Ord[j]=0;
									counter_Reason_Sp[j]=0;
									for (unsigned int i=0; i< tempA.size();i++){
										if ( strcmp( Apstate[j].c_str(),L[tempA[i]].getRunLevel().c_str()) == 0 ){
											lastAPState = j;
											if ( strcmp(L[tempA[i]].getReason().c_str(),"ordered") == 0 ){
												counter_Reason_Ord[j] += 0; //eventAnalyser(L,tempA[i],"AP state event");
												if(detailedInfo) {
													for (unsigned int k=0; k < pos_Detail_Ord.size();k++) {
														if(strcmp(L[tempA[i]].getReasonInfo().c_str(), L[pos_Detail_Ord[k]].getReasonInfo().c_str())==0){
															//pos_Detail_Ord[k] = tempA[i];
															t++;
															counter_Detail_Ord[j][k]+=0; //eventAnalyser(L,tempA[i],"AP state event");
															lastAPStateDetail = k;

														}
													}
													if ( t==0 ) {
														pos_Detail_Ord.push_back(tempA[i]);
														counter_Detail_Ord[j][pos_Detail_Ord.size()-1]+=0; //eventAnalyser(L,tempA[i],"AP state event");
													}
													else
														t=0;
												}
											}
											else if (strcmp(L[tempA[i]].getReason().c_str(),"spontaneous")==0 ) {

												counter_Reason_Sp[j] += 0; //eventAnalyser(L,tempA[i],"AP state event");
												if( detailedInfo ) {
													for (unsigned int k=0; k < pos_Detail_Sp.size();k++) {
														if ( strcmp(L[tempA[i]].getReasonInfo().c_str(), L[pos_Detail_Sp[k]].getReasonInfo().c_str()) == 0 ) {
															//pos_Detail_Sp[k] = tempA[i];
															t++;
															counter_Detail_Sp[j][k]+=0; //eventAnalyser(L,tempA[i],"AP state event");
															lastAPStateDetail = k;
														}
													}
													if ( t == 0 ) {
														pos_Detail_Sp.push_back(tempA[i]);
														counter_Detail_Sp[j][pos_Detail_Sp.size()-1]+=0; //eventAnalyser(L,tempA[i],"AP state event");
													}
													else
														t=0;
												}
											}
										}
									}
									detail_Ord.push_back(pos_Detail_Ord);
									pos_Detail_Ord.clear();
									detail_Sp.push_back(pos_Detail_Sp);
									pos_Detail_Sp.clear();
								}
								//	                            for (unsigned int j=0;j<Apstate.size();j++)
								//	                              if ( strcmp( Apstate[j].c_str(),L[tempA[tempA.size()-1]].getRunLevel().c_str()) == 0 )
								//	                                lastAPState = j;
								//
								//	                            if (strcmp(L[tempA[tempA.size()-1]].getReason().c_str(),"ordered")==0 ) {
								//	                                counter_Reason_Ord[lastAPState]+=addCurrentDate(L,tempA[tempA.size()-1]);
								//	                                ((detailedInfo) && (counter_Detail_Ord[lastAPState][lastAPStateDetail]+=addCurrentDate(L,tempA[tempA.size()-1])));
								//
								//
								//	                            }else if (strcmp(L[tempA[tempA.size()-1]].getReason().c_str(),"spontaneous")==0 ) {
								//	                                counter_Reason_Sp[lastAPState]+=addCurrentDate(L,tempA[tempA.size()-1]);
								//	                                ((detailedInfo) && (counter_Detail_Sp[lastAPState][lastAPStateDetail]+=addCurrentDate(L,tempA[tempA.size()-1])));
								//	                            }
							}

							//---------------------------------PRINTOUT AP STATE---------------------------------------------------------

							/*AP State

							  State  [Reason] [Details]                        Time
							  -------------------------------------------------------------------
							  total time                                                  time
							  [[state                                            time] ]
							  |                                                        |
							  |       [[reason                                   time]]|
							  |       |        [  details                        time]||
							  |       |            .                                  ||
							  |       |            .                                  ||
							  [       [            .                                  ]]

							 */
							minutes = totalApTime / 60;
							s = (int)totalApTime % 60;
							hours = minutes / 60;
							m = (int)minutes % 60;

							//oss<<"\nISP log analysis from " + start_time + " to " + end_time;
							//if (utcFlag) oss<<" (UTC)       \n";
							//else oss<<" (Local Time)\n";
							oss<<"\nAP State\n"<<endl<<
									"           State   ";
							if (reason)oss<<"Reason";else oss<<"      ";
							if (detailedInfo)oss<<"  Details";else oss<<"          ";
							oss<<"                                      Time"<<endl;
							oss<<"-----------------------------------------------------------------------------"<<endl;
							cout.setf(ios::left, ios::adjustfield);
							cout.fill(' ');
							oss<<"total time"
									<< std::setfill(' ') << std::setw(61) << (int)hours << ":"
									<< std::setfill('0') << std::setw(2) << m << ":"
									<< std::setfill('0') << std::setw(2) << s;
							outS = oss.str();
							*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
							strcpy( buffer +4, outS.c_str());
							stream.send_n(buffer, outS.size() + 4 );
							tempA.clear();
							oss.str("");
							outS.clear();
							for (unsigned int i=0;i<Apstate.size();i++)
							{
								if (counter_Ap[i]!=0)
								{
									minutes = counter_Ap[i] / 60;
									s = (int)counter_Ap[i] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									cout.setf(ios::left, ios::adjustfield);
									cout.fill(' ');
									oss <<"           "<< state[i];
									cout.setf(ios::right, ios::adjustfield);
									oss << std::setfill(' ') << std::setw(36) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s;
									string outS(oss.str());
									*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
									strcpy( buffer +4, outS.c_str());
									stream.send_n(buffer, outS.size() + 4 );
									tempA.clear();
									oss.str("");
									outS.clear();
									analyse=false;
									if (reason && strcmp(state[i],"up (redundant)          ") != 0)
									{
										if (counter_Reason_Ord[i]!=0)
										{
											minutes=0,s=0,hours=0,m=0;
											minutes = counter_Reason_Ord[i] / 60;
											s = (int)counter_Reason_Ord[i] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( "ordered" );
											cout.width(60);
											oss <<"                   "<< "ordered"
													//cout.setf(ios::right, ios::adjustfield);
											<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s;
											string outS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
											strcpy( buffer +4, outS.c_str());
											stream.send_n(buffer, outS.size() + 4 );
											oss.str("");
											outS.clear();

											if (detailedInfo)
											{
												for (unsigned int j=0;j<detail_Ord[i].size();j++)
												{
													if ( counter_Detail_Ord[i][j]!=0)
													{
														minutes=0,s=0,hours=0,m=0;
														minutes = counter_Detail_Ord[i][j] / 60;
														s = (int)counter_Detail_Ord[i][j] % 60;
														hours = minutes / 60;
														m = (int)minutes % 60;
														int len = 30 - strlen( L[ detail_Ord[i][j] ].getReasonInfo().c_str() );
														cout.width(60);
														oss <<"                           "<< L[ detail_Ord[i][j] ].getReasonInfo().c_str()
											        		 //cout.setf(ios::right, ios::adjustfield);
														<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
																<< std::setfill('0') << std::setw(2) << m << ":"
																<< std::setfill('0') << std::setw(2) << s;
														string outS(oss.str());
														*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
														strcpy( buffer +4, outS.c_str());
														stream.send_n(buffer, outS.size() + 4 );
														oss.str("");
														outS.clear();
													}
												}
											}
										}
										if ( counter_Reason_Sp[i]!=0 )
										{
											minutes=0,s=0,hours=0,m=0;
											minutes = counter_Reason_Sp[i] / 60;
											s = (int)counter_Reason_Sp[i] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( "spontaneous" );
											cout.width(60);
											oss <<"                   "<< "spontaneous"
													<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
													<< std::setfill('0') << std::setw(2) << m << ":"
													<< std::setfill('0') << std::setw(2) << s;

											string outSS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outSS.size());
											strcpy( buffer +4, outSS.c_str());
											stream.send_n(buffer, outSS.size() + 4 );
											oss.str("");
											outSS.clear();
										}
										if (detailedInfo)
										{
											for (unsigned int j=0;j<detail_Sp[i].size();j++)
											{
												if ( counter_Detail_Sp[i][j]!=0)
												{
													minutes=0,s=0,hours=0,m=0;
													minutes = counter_Detail_Sp[i][j] / 60;
													s = (int)counter_Detail_Sp[i][j] % 60;
													hours = minutes / 60;
													m = (int)minutes % 60;
													int len = 30 - strlen( L[ detail_Sp[i][j] ].getReasonInfo().c_str() );
													cout.width(60);
													oss <<"                           "<< L[ detail_Sp[i][j] ].getReasonInfo().c_str()
											        		 //cout.setf(ios::right, ios::adjustfield);
													<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
															<< std::setfill('0') << std::setw(2) << m << ":"
															<< std::setfill('0') << std::setw(2) << s;
													string outS(oss.str());
													*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
													strcpy( buffer +4, outS.c_str());
													stream.send_n(buffer, outS.size() + 4 );
													oss.str("");
													outS.clear();
												}
											}
											for (unsigned int j=0;j<detail_Sp[i].size();j++)
												counter_Detail_Sp[i][j]=0;
											for (unsigned int j=0;j<detail_Ord[i].size();j++)
												counter_Detail_Ord[i][j]=0;
										}
									}
								}
							}
							tempA.clear();
							detail_Ord.clear();
							detail_Sp.clear();
							oss.str("");
							oss.flush();
							outS.clear();
							pos_Ap.clear();
							pos_Detail_Ord.clear();
							pos_Detail_Sp.clear();
						}

						//--------------------NODE STATE OPTION-------------------------------------------------------------------------

						if (nodeState)
						{
							int firstNodeStateEventA = 0;
							int firstNodeStateEventB = 0;
							for (unsigned int i=0; i < L.size(); i++)
							{
								if ( strcmp(L[i].getEvent().c_str(),"Node state event")==0 )
								{
									if (strcmp(L[i].getHostname().c_str(),p_node_hostname_1.c_str())==0 ){
										firstNodeStateEventA = 1;
										tempA.push_back(i);
									}
									else if (strcmp(L[i].getHostname().c_str(),p_node_hostname_2.c_str())==0 )
									{
										firstNodeStateEventB = 1;
										tempB.push_back(i);
									}
								}
							}
							t=0;
							if (tempA.size()==0)tempA.push_back(0);
							if (tempB.size()==0)tempB.push_back(0);

							for(unsigned int i=0;i<Nodestate.size();i++)counter_NodeA[i]=0;
							for(unsigned int i=0;i<Nodestate.size();i++)counter_NodeB[i]=0;

							//NODE SC-2-1

							int lastNodeState = 0;
							for(unsigned int i=0; i< tempA.size()-1;i++){ // use less one becouse don't know last event end
								// first cicle get every line of list
								totalNodeATime += eventAnalyserByNode(L,tempA[i],tempA[i+1]);
								for (unsigned int k=0; k < Nodestate.size();k++)
								{
									// second cicle check if is found another element =
									if(strcmp(L[tempA[i]].getRunLevel().c_str(),Nodestate[k].c_str())==0)
									{
										//count time this is active
										counter_NodeA[k]+=eventAnalyserByNode(L,tempA[i],tempA[i+1]);
									}

								}
							}

							for(unsigned int k=0; k < Nodestate.size(); k++)
								if(strcmp(L[tempA[tempA.size()-1]].getRunLevel().c_str(),Nodestate[k].c_str())==0)
									lastNodeState = k;

							if(!endTime){
								(firstNodeStateEventA) && (totalNodeATime+=addCurrentDate(L,tempA[tempA.size()-1]));
								(firstNodeStateEventA) && (counter_NodeA[lastNodeState]+=addCurrentDate(L,tempA[tempA.size()-1]));
							}else{

								if(tempA.size() == 1){
									(totalNodeATime += setStartDate(L,tempA[tempA.size()-1]));
									(counter_NodeA[lastNodeState]+=setStartDate(L,tempA[tempA.size()-1]));
								}
								(totalNodeATime += setEndDate(L,tempA[tempA.size()-1]));
								(counter_NodeA[lastNodeState]+=setEndDate(L,tempA[tempA.size()-1]));
							}

							//NODE SC-2-2

							lastNodeState = 0;
							for(unsigned int i=0; i< tempB.size()-1;i++){
								totalNodeBTime += eventAnalyserByNode(L,tempB[i],tempB[i+1]);
								for (unsigned int k=0; k < Nodestate.size();k++)
								{
									if(strcmp(L[tempB[i]].getRunLevel().c_str(),Nodestate[k].c_str())==0)
									{
										counter_NodeB[k]+=eventAnalyserByNode(L,tempB[i],tempB[i+1]);
										// CONTROLLO ULTIMO ELEMENTO
									}
								}
							}

							for(unsigned int k=0; k < Nodestate.size(); k++)
								if(strcmp(L[tempB[tempB.size()-1]].getRunLevel().c_str(),Nodestate[k].c_str())==0)
									lastNodeState = k;

							if(!endTime){
								(firstNodeStateEventB) && (totalNodeBTime+=addCurrentDate(L,tempB[tempB.size()-1]));
								(firstNodeStateEventB) && (counter_NodeB[lastNodeState]+=addCurrentDate(L,tempB[tempB.size()-1]));
							}else{

								if(tempB.size() == 1){
									(totalNodeBTime += setStartDate(L,tempB[tempB.size()-1]));
									(counter_NodeB[lastNodeState]+=setStartDate(L,tempB[tempB.size()-1]));
								}
								(totalNodeBTime += setEndDate(L,tempB[tempB.size()-1]));
								(counter_NodeB[lastNodeState]+=setEndDate(L,tempB[tempB.size()-1]));
							}

							if (reason)
							{
								counter_Reason_Ord[0]=0;
								counter_Reason_Sp[0]=0;
								counter_Reason_OrdB[0]=0;
								counter_Reason_SpB[0]=0;
								counter_Detail_Ord[0][0]=0;
								counter_Detail_Sp[0][0]=0;
								counter_Detail_OrdB[0][0]=0;
								counter_Detail_SpB[0][0]=0;

								unsigned int lastElement = tempA.size()-1;
								for (unsigned int i=0; i< tempA.size();i++)
								{
									if ((firstNodeStateEventA) && (strcmp( "unknown",L[tempA[i]].getRunLevel().c_str())==0 ))
									{
										if (strcmp(L[tempA[i]].getReason().c_str(),"ordered")==0 )
										{
											!(i == lastElement) && (counter_Reason_Ord[0] += eventAnalyserByNode(L,tempA[i],tempA[i+1]));
											(!endTime) && (i == lastElement) && (counter_Reason_Ord[0] += addCurrentDate(L,tempA[lastElement]));
											if (detailedInfo)
											{
												for (unsigned int k=0; k < pos_Detail_Ord.size();k++)
												{
													if(strcmp(L[tempA[i]].getReasonInfo().c_str(), L[pos_Detail_Ord[k]].getReasonInfo().c_str())==0)
													{
														t++;
														!(i == lastElement) && (counter_Detail_Ord[0][k] += eventAnalyserByNode(L,tempA[i],tempA[i+1]));
														(!endTime) && (i == lastElement) && (counter_Detail_Ord[0][k] += addCurrentDate(L,tempA[lastElement]));
													}
												}
												if (t==0)
												{
													pos_Detail_Ord.push_back(tempA[i]);
													!(i == lastElement) && (counter_Detail_Ord[0][pos_Detail_Ord.size()-1] += eventAnalyserByNode(L,tempA[i],tempA[i+1]));
													(!endTime) && (i == lastElement) && (counter_Detail_Ord[0][pos_Detail_Ord.size()-1] += addCurrentDate(L,tempA[lastElement]));
												}else t=0;
											}
										}
										else if (strcmp(L[tempA[i]].getReason().c_str(),"spontaneous")==0 )
										{
											!(i == lastElement) && (counter_Reason_Sp[0] += eventAnalyserByNode(L,tempA[i],tempA[i+1]));
											(!endTime) && (i == lastElement) && (counter_Reason_Sp[0] += addCurrentDate(L,tempA[lastElement]));
											if (detailedInfo)
											{
												for (unsigned int k=0; k < pos_Detail_Sp.size();k++)
												{
													if(strcmp(L[tempA[i]].getReasonInfo().c_str(), L[pos_Detail_Sp[k]].getReasonInfo().c_str())==0)
													{
														t++;
														!(i == lastElement) && (counter_Detail_Sp[0][k] += eventAnalyserByNode(L,tempA[i],tempA[i+1]));
														(!endTime) && (i == lastElement) && (counter_Detail_Sp[0][k] += addCurrentDate(L,tempA[lastElement]));
													}
												}
												if (t==0)
												{
													pos_Detail_Sp.push_back(tempA[i]);
													!(i == lastElement) && (counter_Detail_Sp[0][pos_Detail_Sp.size()-1]+=eventAnalyserByNode(L,tempA[i],tempA[i+1]));
													(!endTime) && (i == lastElement) && (counter_Detail_Sp[0][pos_Detail_Sp.size()-1] += addCurrentDate(L,tempA[lastElement]));
												}else t=0;
											}
										}
									}
								}
								if(detailedInfo){
									detail_Ord.push_back(pos_Detail_Ord);
									detail_Sp.push_back(pos_Detail_Sp);
									pos_Detail_Ord.clear();
									pos_Detail_Sp.clear();
								}

								unsigned int lastElementB = tempB.size()-1;
								for (unsigned int i=0; i< tempB.size();i++)
								{
									if ((firstNodeStateEventB) && (strcmp("unknown",L[tempB[i]].getRunLevel().c_str())==0 ))
									{
										if (strcmp(L[tempB[i]].getReason().c_str(),"ordered")==0 )
										{
											!(i == lastElementB) && (counter_Reason_OrdB[0] += eventAnalyserByNode(L,tempB[i],tempB[i+1]));
											(!endTime) && (i == lastElementB) && (counter_Reason_OrdB[0] += addCurrentDate(L,tempB[lastElementB]));

											if (detailedInfo)
											{
												for (unsigned int k=0; k < pos_Detail_OrdB.size();k++)
												{
													if(strcmp(L[tempB[i]].getReasonInfo().c_str(), L[pos_Detail_OrdB[k]].getReasonInfo().c_str())==0)
													{
														t++;
														!(i == lastElementB) && (counter_Detail_OrdB[0][k] += eventAnalyserByNode(L,tempB[i],tempB[i+1]));
														(!endTime) && (i == lastElementB) && (counter_Detail_OrdB[0][k] += addCurrentDate(L,tempB[lastElementB]));
													}
												}
												if (t==0)
												{
													pos_Detail_OrdB.push_back(tempB[i]);
													!(i == lastElementB) && (counter_Detail_OrdB[0][pos_Detail_OrdB.size()-1] += eventAnalyserByNode(L,tempB[i],tempB[i+1]));
													(!endTime) && (i == lastElementB) && (counter_Detail_OrdB[0][pos_Detail_OrdB.size()-1] += addCurrentDate(L,tempB[lastElementB]));
												}else t=0;
											}
										}
										else if (strcmp(L[tempB[i]].getReason().c_str(),"spontaneous")==0 )
										{
											!(i == lastElementB) && (counter_Reason_SpB[0] += eventAnalyserByNode(L,tempB[i],tempB[i+1]));
											(!endTime) && (i == lastElementB) && (counter_Reason_SpB[0] += addCurrentDate(L,tempB[lastElementB]));
											if (detailedInfo)
											{
												for (unsigned int k=0; k < pos_Detail_SpB.size();k++)
												{
													if(strcmp(L[tempB[i]].getReasonInfo().c_str(), L[pos_Detail_SpB[k]].getReasonInfo().c_str())==0)
													{
														t++;
														!(i == lastElementB) && (counter_Detail_SpB[0][k] += eventAnalyserByNode(L,tempB[i],tempB[i+1]));
														(!endTime) && (i == lastElementB) && (counter_Detail_SpB[0][k] += addCurrentDate(L,tempB[lastElementB]));
													}
												}
												if (t==0)
												{
													pos_Detail_SpB.push_back(tempB[i]);
													!(i == lastElementB) && (counter_Detail_SpB[0][pos_Detail_SpB.size()-1] += eventAnalyserByNode(L,tempB[i],tempB[i+1]));
													(!endTime) && (i == lastElementB) && (counter_Detail_SpB[0][pos_Detail_SpB.size()-1] += addCurrentDate(L,tempB[lastElementB]));
												}else t=0;
											}
										}
									}

								}

								if(detailedInfo){
									detail_OrdB.push_back(pos_Detail_OrdB);
									detail_SpB.push_back(pos_Detail_SpB);
								}
								pos_Detail_OrdB.clear();
								pos_Detail_SpB.clear();

							}


							//---------------------------------------------------PPRINTOUT NODE STATE----------------------------------------------

							/*[Node State                                                         ]
						|                                                                   |
						|Node       State  [Reason] [Details]                        Time   |
						|-------------------------------------------------------------------|
						|node                                                        time   |
						|         [[state                                            time] ]|
						|         |                                                        ||
						|         |       [[reason                                   time]]||
						|         |       |        [details                          time]|||
						|         |       |            .                                  |||
						|         |       |            .                                  |||
						|         [       [            .                                  ]]|
						|                                                                   |    */

							//NODE SC-2-1
							minutes = totalNodeATime / 60;
							s = (int) totalNodeATime % 60;
							hours = minutes / 60;
							m = (int)minutes % 60;

							oss<<"\n\nNode State\n"<<endl<<
									"Node       State   ";
							if (reason)oss<<"Reason";else oss<<"      ";
							if (detailedInfo)oss<<"  Details";else oss<<"          ";
							oss<<"                                      Time"<<endl;
							oss<<"-----------------------------------------------------------------------------"<<endl;
							cout.setf(ios::left, ios::adjustfield);
							cout.fill(' ');
							int len_node_1 = 10 - strlen(p_node_hostname_1.c_str());
							oss << p_node_hostname_1
									<< std::setfill(' ') << std::setw(61+len_node_1) << (int)hours << ":"
									<< std::setfill('0') << std::setw(2) << m << ":"
									<< std::setfill('0') << std::setw(2) << s;
							string outS(oss.str());
							*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
							strcpy( buffer +4, outS.c_str());
							stream.send_n(buffer, outS.size() + 4 );
							oss.str("");
							outS.clear();


							for (unsigned int i=0;i<Nodestate.size();i++)
							{
								if ( counter_NodeA[i]!=0)
								{
									minutes = counter_NodeA[i] / 60;
									s = (int) counter_NodeA[i] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									int len = 30 - strlen( Nodestate[i].c_str() );
									cout.width(60);
									cout.setf(ios::left, ios::adjustfield);
									cout.fill(' ');
									oss <<"           "<< Nodestate[i].c_str()
							    		 << std::setfill(' ') << std::setw(30+len) << (int)hours << ":"
										 << std::setfill('0') << std::setw(2) << m << ":"
										 << std::setfill('0') << std::setw(2) << s;
									string outS(oss.str());
									*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
									strcpy( buffer +4, outS.c_str());
									stream.send_n(buffer, outS.size() + 4 );
									oss.str("");
									outS.clear();
								}
							}
							if (reason)
							{
								if (counter_Reason_Ord[0]!=0 )
								{
									minutes = counter_Reason_Ord[0] / 60;
									s = (int)counter_Reason_Ord[0] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									int len = 30 - strlen( "ordered" );
									cout.width(60);
									oss <<"                   "<< "ordered"
											<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s;
									string outS(oss.str());
									*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
									strcpy( buffer +4, outS.c_str());
									stream.send_n(buffer, outS.size() + 4 );
									oss.str("");
									outS.clear();

									if (detailedInfo)
									{
										for (unsigned int j=0;j<detail_Ord[0].size();j++)
										{
											if (counter_Detail_Ord[0][j]!=0)
											{
												minutes=0,s=0,hours=0,m=0;
												minutes = counter_Detail_Ord[0][j] / 60;
												s = (int)counter_Detail_Ord[0][j] % 60;
												hours = minutes / 60;
												m = (int)minutes % 60;
												int len = 30 - strlen( L[ detail_Ord[0][j] ].getReasonInfo().c_str() );
												cout.width(60);
												oss <<"                           "<< L[ detail_Ord[0][j] ].getReasonInfo().c_str()
										    		 << std::setfill(' ') << setw(14+len) << (int)hours << ":"
													 << std::setfill('0') << std::setw(2) << m << ":"
													 << std::setfill('0') << std::setw(2) << s;
												string outS(oss.str());
												*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
												strcpy( buffer +4, outS.c_str());
												stream.send_n(buffer, outS.size() + 4 );
												oss.str("");
												outS.clear();
											}
										}
									}
								}
								if( counter_Reason_Sp[0]!=0)
								{
									minutes = counter_Reason_Sp[0] / 60;
									s = (int)counter_Reason_Sp[0] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									int len = 30 - strlen( "spontaneous" );
									cout.width(60);
									oss <<"                   "<< "spontaneous"
											<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s;
									string outSS(oss.str());
									*reinterpret_cast<uint32_t *>(buffer) = htonl ( outSS.size());
									strcpy( buffer +4, outSS.c_str());
									stream.send_n(buffer, outSS.size() + 4 );
									oss.str("");
									outSS.clear();
								}
								if (detailedInfo)
								{
									for (unsigned int j=0;j<detail_Sp[0].size();j++)
									{
										if ( counter_Detail_Sp[0][j]!=0 )
										{
											minutes=0,s=0,hours=0,m=0;
											minutes = counter_Detail_Sp[0][j] / 60;
											s = (int)counter_Detail_Sp[0][j] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( L[ detail_Sp[0][j] ].getReasonInfo().c_str() );
											cout.width(60);
											oss <<"                           "<< L[ detail_Sp[0][j] ].getReasonInfo().c_str()
										    		 //cout.setf(ios::right, ios::adjustfield);
											<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
													<< std::setfill('0') << std::setw(2) << m << ":"
													<< std::setfill('0') << std::setw(2) << s;
											string outS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
											strcpy( buffer +4, outS.c_str());
											stream.send_n(buffer, outS.size() + 4 );
											oss.str("");
											outS.clear();
										}
									}
								}

							}



							//NODE SC-2-2

							minutes = totalNodeBTime / 60;
							s = (int) totalNodeBTime % 60;
							hours = minutes / 60;
							m = (int)minutes % 60;
							int len_node_2 = 10 - strlen(p_node_hostname_2.c_str());
							oss << p_node_hostname_2
									<< std::setfill(' ') << std::setw(61+len_node_2) << (int)hours << ":"
									<< std::setfill('0') << std::setw(2) << m << ":"
									<< std::setfill('0') << std::setw(2) << s;
							string outSS(oss.str());
							*reinterpret_cast<uint32_t *>(buffer) = htonl ( outSS.size());
							strcpy( buffer +4, outSS.c_str());
							stream.send_n(buffer, outSS.size() + 4 );
							oss.str("");
							outSS.clear();
							for (unsigned int i=0;i<Nodestate.size();i++)
							{
								if ( counter_NodeB[i]!=0 )
								{
									minutes = counter_NodeB[i] / 60;
									s = (int) counter_NodeB[i] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									int len = 30 - strlen( Nodestate[i].c_str() );
									cout.width(60);
									cout.setf(ios::left, ios::adjustfield);
									cout.fill(' ');
									oss <<"           "<< Nodestate[i].c_str()
							    		 << std::setfill(' ') << std::setw(30+len) << (int)hours << ":"
										 << std::setfill('0') << std::setw(2) << m << ":"
										 << std::setfill('0') << std::setw(2) << s;
									string outS(oss.str());
									*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
									strcpy( buffer +4, outS.c_str());
									stream.send_n(buffer, outS.size() + 4 );
									oss.str("");
								}
							}
							if (reason)
							{
								if ( counter_Reason_OrdB[0]!=0 )
								{
									minutes = counter_Reason_OrdB[0] / 60;
									s = (int)counter_Reason_OrdB[0] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									int len = 30 - strlen( "ordered" );
									cout.width(60);
									oss <<"                   "<< "ordered"
											//cout.setf(ios::right, ios::adjustfield);
									<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
									<< std::setfill('0') << std::setw(2) << m << ":"
									<< std::setfill('0') << std::setw(2) << s;
									string outS(oss.str());
									*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
									strcpy( buffer +4, outS.c_str());
									stream.send_n(buffer, outS.size() + 4 );
									oss.str("");
									outS.clear();

									if (detailedInfo)
									{
										for (unsigned int j=0;j<detail_OrdB[0].size();j++)
										{
											minutes=0,s=0,hours=0,m=0;
											minutes = counter_Detail_OrdB[0][j] / 60;
											s = (int)counter_Detail_OrdB[0][j] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( L[ detail_OrdB[0][j] ].getReasonInfo().c_str() );
											cout.width(60);
											oss <<"                           "<< L[ detail_OrdB[0][j] ].getReasonInfo().c_str()
										            		 << std::setfill(' ') << setw(14+len) << (int)hours << ":"
															 << std::setfill('0') << std::setw(2) << m << ":"
															 << std::setfill('0') << std::setw(2) << s;
											string outS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
											strcpy( buffer +4, outS.c_str());
											stream.send_n(buffer, outS.size() + 4 );
											oss.str("");
											outS.clear();
										}
									}
								}
								if ( counter_Reason_SpB[0]!=0 )
								{
									minutes = counter_Reason_SpB[0] / 60;
									s = (int)counter_Reason_SpB[0] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									int len = 30 - strlen( "spontaneous" );
									cout.width(60);
									oss <<"                   "<< "spontaneous"
											<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s;
									string outSS(oss.str());
									*reinterpret_cast<uint32_t *>(buffer) = htonl ( outSS.size());
									strcpy( buffer +4, outSS.c_str());
									stream.send_n(buffer, outSS.size() + 4 );
									oss.str("");
									outSS.clear();
								}
								if (detailedInfo)
								{
									for (unsigned int j=0;j<detail_SpB[0].size();j++)
									{
										if ( counter_Detail_SpB[0][j]!=0 )
										{
											minutes=0,s=0,hours=0,m=0;
											minutes = counter_Detail_SpB[0][j] / 60;
											s = (int)counter_Detail_SpB[0][j] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( L[ detail_SpB[0][j] ].getReasonInfo().c_str() );
											cout.width(60);
											oss <<"                           "<< L[ detail_SpB[0][j] ].getReasonInfo().c_str()
										        		 //cout.setf(ios::right, ios::adjustfield);
											<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
													<< std::setfill('0') << std::setw(2) << m << ":"
													<< std::setfill('0') << std::setw(2) << s;
											string outS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
											strcpy( buffer +4, outS.c_str());
											stream.send_n(buffer, outS.size() + 4 );
											oss.str("");
											outS.clear();
										}
									}

									for(unsigned int k=0;k<detail_Ord[0].size();k++)
										counter_Detail_Ord[0][k]=0;
									for(unsigned int k=0;k<detail_Sp[0].size();k++)
										counter_Detail_Sp[0][k]=0;
									for(unsigned int k=0;k<detail_OrdB[0].size();k++)
										counter_Detail_OrdB[0][k]=0;
									for(unsigned int k=0;k<detail_Sp[0].size();k++)
										counter_Detail_SpB[0][k]=0;
									detail_Ord.clear();
									detail_OrdB.clear();
									detail_SpB.clear();
									detail_Sp.clear();
								}
							}
							tempA.clear();
							tempB.clear();
							nodeState=false;
							oss.str("");
							oss.flush();
							outS.clear();
						}
						//-------------------NODE RUN LEVEL------------------------------------------------------------------

						if(runLevel)
						{
							int lastRunLevel = 0;
							for (unsigned int i=0; i < L.size(); i++)
							{
								if ( strcmp(L[i].getEvent().c_str(),"Node Run Level Event")==0 )
								{
									if (strcmp(L[i].getHostname().c_str(),p_node_hostname_1.c_str())==0 )
										tempA.push_back(i);
									else if (strcmp(L[i].getHostname().c_str(),p_node_hostname_2.c_str())==0 )
										tempB.push_back(i);
								}
							}
							if (tempA.size()==0)tempA.push_back(0);
							if (tempB.size()==0)tempB.push_back(0);

							for (unsigned int k=0; k < Runlevelstate.size();k++)counter_RunLevelA[k]=0;
							for (unsigned int k=0; k < Runlevelstate.size();k++)counter_RunLevelB[k]=0;

							lastRunLevel=0;
							for(unsigned int i=0; i< tempA.size()-1;i++){

								// first cicle get every line of list
								totalRunLTimeA += eventAnalyserByNode(L,tempA[i],tempA[i+1]);
								for (unsigned int k=0; k < Runlevelstate.size();k++)
								{
									// second cicle check if is found another element
									if(strcmp(L[tempA[i]].getRunLevel().c_str(),Runlevelstate[k].c_str())==0)
									{
										//count time this is active
										counter_RunLevelA[k]+=eventAnalyserByNode(L,tempA[i],tempA[i+1]);
									}
								}
							}

							for(unsigned int k=0; k < Runlevelstate.size(); k++)
								if(strcmp(L[tempA[tempA.size()-1]].getRunLevel().c_str(),Runlevelstate[k].c_str())==0)
									lastRunLevel = k;

							if(!endTime){
								totalRunLTimeA += addCurrentDate(L,tempA[tempA.size()-1]);
								counter_RunLevelA[lastRunLevel]+=addCurrentDate(L,tempA[tempA.size()-1]);
							}else{
								//totalRunLTimeA += setEndDate();
								//counter_RunLevelA[lastRunLevel] += setEndDate();
							}

							lastRunLevel = 0;
							for(unsigned int i=0; i< tempB.size()-1;i++){
								totalRunLTimeB += eventAnalyserByNode(L,tempB[i],tempB[i+1]);
								for (unsigned int k=0; k < Runlevelstate.size();k++)
								{
									if(strcmp(L[tempB[i]].getRunLevel().c_str(),Runlevelstate[k].c_str())==0)
									{
										counter_RunLevelB[k]+=eventAnalyserByNode(L,tempB[i],tempB[i+1]);
									}
								}
							}

							for(unsigned int k=0; k < Runlevelstate.size(); k++)
								if(strcmp(L[tempB[tempB.size()-1]].getRunLevel().c_str(),Runlevelstate[k].c_str())==0)
									lastRunLevel = k;

							if(!endTime){
								totalRunLTimeB += addCurrentDate(L,tempB[tempB.size()-1]);
								counter_RunLevelB[lastRunLevel]+=addCurrentDate(L,tempB[tempB.size()-1]);
							}else{
								//totalRunLTimeB += setEndDate();
								//counter_RunLevelB[lastRunLevel] += setEndDate();
							}

							if (reason)
							{
								int lastRunLevel = 0;
								int lastRunLevelDetail = 0;
								int lastRunLevelDetailB = 0;
								unsigned int lastElement = tempA.size()-1;

								for (unsigned int j=0;j<Runlevelstate.size();j++){
									counter_Detail_Ord[j][0]=0;
									counter_Detail_Sp[j][0]=0;
									counter_Reason_Ord[j]=0;
									counter_Reason_Sp[j]=0;
									counter_Detail_OrdB[j][0]=0;
									counter_Detail_SpB[j][0]=0;
									counter_Reason_OrdB[j]=0;
									counter_Reason_SpB[j]=0;

									for (unsigned int i=0; i< tempA.size();i++)
									{
										if (strcmp( Runlevelstate[j].c_str(),L[tempA[i]].getRunLevel().c_str())==0 )
										{
											if (strcmp(L[tempA[i]].getReason().c_str(),"ordered")==0 )
											{
												!(i == lastElement) && (counter_Reason_Ord[j] += eventAnalyserByNode(L,tempA[i],tempA[i+1]));
												(!endTime) && (i == lastElement) && (counter_Reason_Ord[j] += addCurrentDate(L,tempA[lastElement]));
												//(endTime) && (counter_Reason_Ord[j] += setEndDate());

												if(detailedInfo)
												{
													for (unsigned int k=0; k < pos_Detail_Ord.size();k++)
													{
														if(strcmp(L[tempA[i]].getReasonInfo().c_str(), L[pos_Detail_Ord[k]].getReasonInfo().c_str())==0)
														{
															t++;
															!(i == lastElement) && (counter_Detail_Ord[j][k]+=eventAnalyserByNode(L,tempA[i],tempA[i+1]));
															(!endTime) && (i == lastElement) && (counter_Detail_Ord[j][k] += addCurrentDate(L,tempA[lastElement]));
															//(endTime) && (counter_Detail_Ord[j][k] += setEndDate());
															lastRunLevelDetail = k;
														}
													}
													if (t==0)
													{
														pos_Detail_Ord.push_back(tempA[i]);
														!(i == lastElement) && (counter_Detail_Ord[j][pos_Detail_Ord.size()-1]+=eventAnalyserByNode(L,tempA[i],tempA[i+1]));
														(!endTime) && (i == lastElement) && (counter_Detail_Ord[j][pos_Detail_Ord.size()-1] += addCurrentDate(L,tempA[lastElement]));
														//(endTime) && (counter_Detail_Ord[j][pos_Detail_Ord.size()-1] += setEndDate());
													}else t=0;
												}
											}
											else if (strcmp(L[tempA[i]].getReason().c_str(),"spontaneous")==0 )
											{
												!(i == lastElement) && (counter_Reason_Sp[j] += eventAnalyserByNode(L,tempA[i],tempA[i+1]));
												(!endTime) && (i == lastElement) && (counter_Reason_Sp[j] += addCurrentDate(L,tempA[lastElement]));
												//(endTime) && (counter_Reason_Sp[j] += setEndDate());
												if(detailedInfo)
												{
													for (unsigned int k=0; k < pos_Detail_Sp.size();k++)
													{
														if(strcmp(L[tempA[i]].getReasonInfo().c_str(), L[pos_Detail_Sp[k]].getReasonInfo().c_str())==0)
														{
															t++;
															!(i == lastElement) && (counter_Detail_Sp[j][k]+=eventAnalyserByNode(L,tempA[i],tempA[i+1]));
															(!endTime) && (i == lastElement) && (counter_Detail_Sp[j][k] += addCurrentDate(L,tempA[lastElement]));
															//(endTime) && (counter_Detail_Sp[j][k] += setEndDate());
															lastRunLevelDetail = k;
														}
													}
													if (t==0)
													{
														pos_Detail_Sp.push_back(tempA[i]);
														!(i == lastElement) && (counter_Detail_Sp[j][pos_Detail_Sp.size()-1]+=eventAnalyserByNode(L,tempA[i],tempA[i+1]));
														(!endTime) && (i == lastElement) && (counter_Detail_Sp[j][pos_Detail_Sp.size()-1] += addCurrentDate(L,tempA[lastElement]));
														//(endTime) && (counter_Detail_Sp[j][pos_Detail_Sp.size()-1] += setEndDate());
													}else t=0;
												}
											}
										}
									}

									detail_Ord.push_back(pos_Detail_Ord);
									pos_Detail_Ord.clear();
									detail_Sp.push_back(pos_Detail_Sp);
									pos_Detail_Sp.clear();


									unsigned int lastElementB = tempB.size()-1;
									for (unsigned int i=0; i< tempB.size();i++)
									{
										if (strcmp( Runlevelstate[j].c_str(),L[tempB[i]].getRunLevel().c_str())==0 )
										{
											lastRunLevel = i;
											if (strcmp(L[tempB[i]].getReason().c_str(),"ordered")==0 )
											{
												!(i == lastElementB) && (counter_Reason_OrdB[j] += eventAnalyserByNode(L,tempB[i],tempB[i+1]));
												(!endTime) && (i == lastElementB) && (counter_Reason_OrdB[j] += addCurrentDate(L,tempB[lastElementB]));
												//(endTime) && (counter_Reason_OrdB[j] += setEndDate());
												if(detailedInfo)
												{
													for (unsigned int k=0; k < pos_Detail_OrdB.size();k++)
													{
														if(strcmp(L[tempB[i]].getReasonInfo().c_str(), L[pos_Detail_OrdB[k]].getReasonInfo().c_str())==0)
														{
															t++;
															!(i == lastElementB) && (counter_Detail_OrdB[j][k]+=eventAnalyserByNode(L,tempB[i],tempB[i+1]));
															(!endTime) && (i == lastElementB) && (counter_Detail_OrdB[j][k] += addCurrentDate(L,tempB[lastElementB]));
															//(endTime) && (counter_Detail_OrdB[j][k] += setEndDate());
														}
													}
													if (t==0)
													{
														pos_Detail_OrdB.push_back(tempB[i]);
														!(i == lastElementB) && (counter_Detail_OrdB[j][pos_Detail_OrdB.size()-1]+=eventAnalyserByNode(L,tempB[i],tempB[i+1]));
														(!endTime) && (i == lastElementB) && (counter_Detail_OrdB[j][pos_Detail_OrdB.size()-1] += addCurrentDate(L,tempB[lastElementB]));
														//(endTime) && (counter_Detail_OrdB[j][pos_Detail_OrdB.size()-1] += setEndDate());
													}else t=0;
												}
											}
											else if (strcmp(L[tempB[i]].getReason().c_str(),"spontaneous")==0 )
											{
												!(i == lastElementB) && (counter_Reason_SpB[j] += eventAnalyserByNode(L,tempB[i],tempB[i+1]));
												(!endTime) && (i == lastElementB) && (counter_Reason_SpB[j] += addCurrentDate(L,tempB[lastElementB]));
												//(endTime) && (counter_Reason_SpB[j] += setEndDate());
												if(detailedInfo)
												{
													for (unsigned int k=0; k < pos_Detail_SpB.size();k++)
													{
														if(strcmp(L[tempB[i]].getReasonInfo().c_str(), L[pos_Detail_SpB[k]].getReasonInfo().c_str())==0)
														{
															t++;
															!(i == lastElementB) && (counter_Detail_SpB[j][k]+=eventAnalyserByNode(L,tempB[i],tempB[i+1]));
															(!endTime) && (i == lastElementB) && (counter_Detail_SpB[j][k] += addCurrentDate(L,tempB[lastElementB]));
															//(endTime) && (counter_Detail_SpB[j][k] += setEndDate());
															lastRunLevelDetailB = k;
														}
													}
													if (t==0)
													{
														pos_Detail_SpB.push_back(tempB[i]);
														!(i == lastElementB) && (counter_Detail_SpB[j][pos_Detail_SpB.size()-1]+=eventAnalyserByNode(L,tempB[i],tempB[i+1]));
														(!endTime) && (i == lastElementB) && (counter_Detail_SpB[j][pos_Detail_SpB.size()-1] += addCurrentDate(L,tempB[lastElementB]));
														//(endTime) && (counter_Detail_SpB[j][pos_Detail_SpB.size()-1] += setEndDate());
													}else t=0;
												}
											}
										}
									}
									detail_OrdB.push_back(pos_Detail_OrdB);
									pos_Detail_OrdB.clear();
									detail_SpB.push_back(pos_Detail_SpB);
									pos_Detail_SpB.clear();

								}
							}


							//---------------------------------------------------PRINTOUT RUNLEVEL EVENT----------------------------------------------

							/*[Run Level                                                          ]
								|                                                                   |
								|Node       Level  [Reason] [Details]                        Time   |
								|-------------------------------------------------------------------|
								|node                                                        time   |
								|         [[level                                            time] ]|
								|         |                                                        ||
								|         |       [[reason                                   time]]||
								|         |       |        [details                          time]|||
								|         |       |            .                                  |||
								|         |       |            .                                  |||
								|         [       [            .                                  ]]|
								|                                                                   |
								|                                 .                                 |
								|                                 .                                 |
								|                                 .                                 |
								|                                                                   |
							 * */
							//SC-2-1

							minutes = totalRunLTimeA / 60;
							s = (int) totalRunLTimeA % 60;
							hours = minutes / 60;
							m = (int)minutes % 60;

							oss<<"\n\nRun Level\n"<<endl<<
									"Node       Level   ";
							if (reason)oss<<"Reason";else oss<<"      ";
							if (detailedInfo)oss<<"  Details";else oss<<"          ";
							oss<<"                                      Time"<<endl;
							oss<<"-----------------------------------------------------------------------------"<<endl;
							cout.setf(ios::left, ios::adjustfield);
							cout.fill(' ');
							int len_node_1 = 10 - strlen(p_node_hostname_1.c_str());
							oss << p_node_hostname_1
									<< std::setfill(' ') << std::setw(61+len_node_1) << (int)hours << ":"
									<< std::setfill('0') << std::setw(2) << m << ":"
									<< std::setfill('0') << std::setw(2) << s;
							string outS(oss.str());
							*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
							strcpy( buffer +4, outS.c_str());
							stream.send_n(buffer, outS.size() + 4 );
							oss.str("");
							outS.clear();

							for (unsigned int i=0;i<Runlevelstate.size();i++)
							{
								if ( counter_RunLevelA[i]!=0 )
								{
									minutes = counter_RunLevelA[i] / 60;
									s = (int) counter_RunLevelA[i] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									int len = 30 - strlen( Runlevelstate[i].c_str() );
									cout.width(60);
									cout.setf(ios::left, ios::adjustfield);
									cout.fill(' ');
									oss <<"           "<< Runlevelstate[i].c_str()
									    		 << std::setfill(' ') << std::setw(30+len) << (int)hours << ":"
												 << std::setfill('0') << std::setw(2) << m << ":"
												 << std::setfill('0') << std::setw(2) << s;
									string outS(oss.str());
									*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
									strcpy( buffer +4, outS.c_str());
									stream.send_n(buffer, outS.size() + 4 );
									oss.str("");
									outS.clear();

									if (reason && (strcmp(Runlevelstate[i].c_str(),"5") != 0) )
									{
										if ( counter_Reason_Ord[i]!=0 )
										{
											minutes = counter_Reason_Ord[i] / 60;
											s = (int)counter_Reason_Ord[i] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( "ordered" );
											cout.width(60);
											oss <<"                   "<< "ordered"
													<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
													<< std::setfill('0') << std::setw(2) << m << ":"
													<< std::setfill('0') << std::setw(2) << s;
											string outS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
											strcpy( buffer +4, outS.c_str());
											stream.send_n(buffer, outS.size() + 4 );
											oss.str("");
											outS.clear();

											if (detailedInfo)
											{
												for (unsigned int j=0;j<detail_Ord[i].size();j++)
												{
													if( counter_Detail_Ord[i][j]!=0 )
													{
														minutes=0,s=0,hours=0,m=0;
														minutes = counter_Detail_Ord[i][j] / 60;
														s = (int)counter_Detail_Ord[i][j] % 60;
														hours = minutes / 60;
														m = (int)minutes % 60;
														int len = 30 - strlen( L[ detail_Ord[i][j] ].getReasonInfo().c_str() );
														cout.width(60);
														oss <<"                           "<< L[ detail_Ord[i][j] ].getReasonInfo().c_str()
												    		 //cout.setf(ios::right, ios::adjustfield);
														<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
																<< std::setfill('0') << std::setw(2) << m << ":"
																<< std::setfill('0') << std::setw(2) << s;
														string outS(oss.str());
														*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
														strcpy( buffer +4, outS.c_str());
														stream.send_n(buffer, outS.size() + 4 );
														oss.str("");
														outS.clear();
													}
												}
											}
											oss.str("");
											outS.clear();
										}
										if ( counter_Reason_Sp[i]!=0)
										{
											minutes = counter_Reason_Sp[i] / 60;
											s = (int)counter_Reason_Sp[i] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( "spontaneous" );
											cout.width(60);
											oss <<"                   "<< "spontaneous"
													<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
													<< std::setfill('0') << std::setw(2) << m << ":"
													<< std::setfill('0') << std::setw(2) << s;
											string outSS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outSS.size());
											strcpy( buffer +4, outSS.c_str());
											stream.send_n(buffer, outSS.size() + 4 );
											oss.str("");
											outSS.clear();
										}
										if (detailedInfo)
										{
											for (unsigned int j=0;j<detail_Sp[i].size();j++)
											{
												if ( counter_Detail_Sp[i][j]!=0 )
												{
													minutes=0,s=0,hours=0,m=0;
													minutes = counter_Detail_Sp[i][j] / 60;
													s = (int)counter_Detail_Sp[i][j] % 60;
													hours = minutes / 60;
													m = (int)minutes % 60;
													int len = 30 - strlen( L[ detail_Sp[i][j] ].getReasonInfo().c_str() );
													cout.width(60);
													oss <<"                           "<< L[ detail_Sp[i][j] ].getReasonInfo().c_str()
												    		 //cout.setf(ios::right, ios::adjustfield);
													<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
															<< std::setfill('0') << std::setw(2) << m << ":"
															<< std::setfill('0') << std::setw(2) << s;
													string outS(oss.str());
													*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
													strcpy( buffer +4, outS.c_str());
													stream.send_n(buffer, outS.size() + 4 );
													oss.str("");
													outS.clear();
												}
											}
										}
									}
								}

							}

							//NODE SC-2-2

							minutes = totalRunLTimeB / 60;
							s = (int) totalRunLTimeB % 60;
							hours = minutes / 60;
							m = (int)minutes % 60;
							int len_node_2 = 10 - strlen(p_node_hostname_2.c_str());
							oss << p_node_hostname_2
									<< std::setfill(' ') << std::setw(61+len_node_2) << (int)hours << ":"
									<< std::setfill('0') << std::setw(2) << m << ":"
									<< std::setfill('0') << std::setw(2) << s<< endl;
							for (unsigned int i=0;i<Runlevelstate.size();i++)
							{
								if ( counter_RunLevelB[i]!=0 )
								{
									minutes = counter_RunLevelB[i] / 60;
									s = (int) counter_RunLevelB[i] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									int len = 30 - strlen( Runlevelstate[i].c_str() );
									cout.width(60);
									cout.setf(ios::left, ios::adjustfield);
									cout.fill(' ');
									oss <<"           "<< Runlevelstate[i].c_str()
									    		 << std::setfill(' ') << std::setw(30+len) << (int)hours << ":"
												 << std::setfill('0') << std::setw(2) << m << ":"
												 << std::setfill('0') << std::setw(2) << s;
									string outS(oss.str());
									*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
									strcpy( buffer +4, outS.c_str());
									stream.send_n(buffer, outS.size() + 4 );
									oss.str("");
									outS.clear();

									if (reason && (strcmp(Runlevelstate[i].c_str(),"5")!=0))
									{
										if ( counter_Reason_OrdB[i]!=0)
										{
											minutes = counter_Reason_OrdB[i] / 60;
											s = (int)counter_Reason_OrdB[i] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( "ordered" );
											cout.width(60);
											oss <<"                   "<< "ordered"
													<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
													<< std::setfill('0') << std::setw(2) << m << ":"
													<< std::setfill('0') << std::setw(2) << s;
											string outS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
											strcpy( buffer +4, outS.c_str());
											stream.send_n(buffer, outS.size() + 4 );
											oss.str("");
											outS.clear();

											if (detailedInfo)
											{
												for (unsigned int j=0;j<detail_OrdB[i].size();j++)
												{
													if (counter_Detail_OrdB[i][j]!=0 )
													{
														minutes=0,s=0,hours=0,m=0;
														minutes = counter_Detail_OrdB[i][j] / 60;
														s = (int)counter_Detail_OrdB[i][j] % 60;
														hours = minutes / 60;
														m = (int)minutes % 60;
														int len = 30 - strlen( L[ detail_OrdB[i][j] ].getReasonInfo().c_str() );
														cout.width(60);
														oss <<"                           "<< L[ detail_OrdB[i][j] ].getReasonInfo().c_str()
												    		 //cout.setf(ios::right, ios::adjustfield);
														<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
																<< std::setfill('0') << std::setw(2) << m << ":"
																<< std::setfill('0') << std::setw(2) << s;
														string outS(oss.str());
														*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
														strcpy( buffer +4, outS.c_str());
														stream.send_n(buffer, outS.size() + 4 );
														oss.str("");
														outS.clear();
													}
												}
											}
										}
										if ( counter_Reason_SpB[i]!=0)
										{
											minutes = counter_Reason_SpB[i] / 60;
											s = (int)counter_Reason_SpB[i] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( "spontaneous" );
											cout.width(60);
											oss <<"                   "<< "spontaneous"
													<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
													<< std::setfill('0') << std::setw(2) << m << ":"
													<< std::setfill('0') << std::setw(2) << s;
											string outSS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outSS.size());
											strcpy( buffer +4, outSS.c_str());
											stream.send_n(buffer, outSS.size() + 4 );
											oss.str("");
											outSS.clear();
										}
										if (detailedInfo)
										{
											for (unsigned int j=0;j<detail_SpB[i].size();j++)
											{
												if (  counter_Detail_SpB[i][j]!=0  )
												{
													minutes=0,s=0,hours=0,m=0;
													minutes = counter_Detail_SpB[i][j] / 60;
													s = (int)counter_Detail_SpB[i][j] % 60;
													hours = minutes / 60;
													m = (int)minutes % 60;
													int len = 30 - strlen( L[ detail_SpB[i][j] ].getReasonInfo().c_str() );
													cout.width(60);
													oss <<"                           "<< L[ detail_SpB[i][j] ].getReasonInfo().c_str()
												    		 //cout.setf(ios::right, ios::adjustfield);
													<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
															<< std::setfill('0') << std::setw(2) << m << ":"
															<< std::setfill('0') << std::setw(2) << s;
													string outS(oss.str());
													*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
													strcpy( buffer +4, outS.c_str());
													stream.send_n(buffer, outS.size() + 4 );
													oss.str("");
													outS.clear();
												}
											}

											counter_RunLevelA[i]=0;
											counter_RunLevelB[i]=0;
											for(unsigned int k=0;k< detail_Ord[i].size();k++)
												counter_Detail_Ord[i][k]=0;
											for(unsigned int k=0;k< detail_Sp[i].size();k++)
												counter_Detail_Sp[i][k]=0;
											for(unsigned int k=0;k< detail_OrdB[i].size();k++)
												counter_Detail_Ord[i][k]=0;
											for(unsigned int k=0;k< detail_SpB[i].size();k++)
												counter_Detail_Sp[i][k]=0;

										}
									}
								}
							}
							detail_Ord.clear();
							detail_OrdB.clear();
							detail_SpB.clear();
							detail_Sp.clear();
							tempA.clear();
							tempB.clear();
							runLevel=false;
						}
						//--------------------SERVICE STATE------------------------------------------------

						if(serviceState){

							for (unsigned int i=0; i < L.size(); i++)
							{
								if ( strcmp(L[i].getEvent().c_str(),"Service Unit state event")==0 )
								{
									if (strcmp(L[i].getHostname().c_str(),p_node_hostname_1.c_str())==0 )
										tempA.push_back(i);
									else if (strcmp(L[i].getHostname().c_str(),p_node_hostname_2.c_str())==0 )
										tempB.push_back(i);
								}
							}

							if (tempA.size()==0)tempA.push_back(0);
							if (tempB.size()==0)tempB.push_back(0);

							t=0;
							counter_Service_run[0]=0;
							counter_Service_runB[0]=0;
							counter_Service_stp[0]=0;
							counter_Service_stpB[0]=0;
							counter_Reason_Ord[0]=0;
							counter_Reason_Sp[0]=0;
							counter_Detail_Ord[0][0]=0;
							counter_Detail_Sp[0][0]=0;
							int firstStateEvent = 0;
							int firstStateEventB = 0;

							for (unsigned int i=0; i < tempA.size(); i++)
							{
								// first cicle get every line of list
								for (unsigned int k=0; k < pos_Service.size();k++)
								{
									firstStateEvent = 1;
									// second cicle check if is found another element =
											if(strcmp( L[tempA[i]].getResourceName().c_str(),L[pos_Service[k]].getResourceName().c_str())==0)
											{

												t++;
												counter_Service[k]+=eventAnalyserByNode(L,pos_Service[k],tempA[i]);
												//count time this is active
												if(strcmp( L[pos_Service[k]].getRunLevel().c_str(),"Active")==0)
													counter_Service_run[k]+=eventAnalyserByNode(L,pos_Service[k],tempA[i]);

												else if(strcmp( L[pos_Service[k]].getRunLevel().c_str(),"Stand-by")==0){
													counter_Service_stp[k] += eventAnalyserByNode(L,pos_Service[k],tempA[i]);
													counter_Reason_Sp[k] = counter_Detail_Sp[k][0] = counter_Service_stp[k];
												}
												//	                                    printf("SERVICE:%s - k=%d\n"
												//	                                        "last_pos_Service=%d - tempA=%d\n"
												//	                                        "counter_Reason_Sp[k]=%f --- counter_Detail_Sp[k][0]=%f --- counter_Service_stp[k]=%f;\n"
												//	                                        "-----------------------------------------------------------------------------------\n",
												//	                                        L[tempA[i]].getResourceName().c_str(),k,pos_Service[k], tempA[i],
												//	                                        counter_Reason_Sp[k], counter_Detail_Sp[k][0], counter_Service_stp[k]);
												pos_Service[k] = tempA[i];
											}
								}
								if (t==0)
								{
									counter_Reason_Sp[pos_Service.size()]=0;
									counter_Detail_Sp[pos_Service.size()][0]=0;
									pos_Service.push_back(tempA[i]);
								}else t=0;
							}

							for (unsigned int k=0; k < pos_Service.size();k++)
							{

								(!endTime) && (firstStateEvent) && (counter_Service[k]+=addCurrentDate(L,pos_Service[k]));

								//(endTime) && (counter_Service[k]+=setEndDate());

								if(strcmp( L[pos_Service[k]].getRunLevel().c_str(),"Active")==0){

									(!endTime) && (firstStateEvent) && (counter_Service_run[k]+=addCurrentDate(L,pos_Service[k]));

									//(endTime) && (counter_Service_run[k]+=setEndDate());

								}else if(strcmp( L[pos_Service[k]].getRunLevel().c_str(),"Stand-by")==0){

									(!endTime) && (firstStateEvent) && (counter_Service_stp[k]+=addCurrentDate(L,pos_Service[k]));

									//(endTime) && (counter_Service_stp[k]+=setEndDate());

									counter_Reason_Sp[k] = counter_Detail_Sp[k][0] = counter_Service_stp[k];

								}
							}/////////////////////////////////////////////////////////////


							for (unsigned int i=0; i < tempB.size(); i++)
							{
								firstStateEventB = 1;
								// first cicle get every line of list
								for (unsigned int k=0; k < pos_ServiceB.size();k++)
								{
									// second cicle check if is found another element =
									if(strcmp( L[tempB[i]].getResourceName().c_str(),L[pos_ServiceB[k]].getResourceName().c_str())==0)
									{
										t++;
										counter_ServiceB[k]+=eventAnalyserByNode(L,pos_ServiceB[k],tempB[i]);
										//count time this is active
										if(strcmp( L[pos_ServiceB[k]].getRunLevel().c_str(),"Active")==0)
											counter_Service_runB[k]+=eventAnalyserByNode(L,pos_ServiceB[k],tempB[i]);

										else if(strcmp( L[pos_ServiceB[k]].getRunLevel().c_str(),"Stand-by")==0){
											counter_Service_stpB[k]+=eventAnalyserByNode(L,pos_ServiceB[k],tempB[i]);
											counter_Reason_SpB[k] = counter_Detail_SpB[k][0] = counter_Service_stpB[k];
										}


										pos_ServiceB[k] = tempB[i];

									}
								}
								if (t==0)
								{
									counter_Reason_SpB[pos_ServiceB.size()] = 0;
									counter_Detail_SpB[pos_ServiceB.size()][0] = 0;
									pos_ServiceB.push_back(tempB[i]);
								}else t=0;
							}

							for (unsigned int k=0; k < pos_ServiceB.size();k++){

								(!endTime) && (firstStateEventB) && (counter_ServiceB[k]+=addCurrentDate(L,pos_ServiceB[k]));

								//(endTime) && (counter_ServiceB[k]+=setEndDate());

								if(strcmp( L[pos_ServiceB[k]].getRunLevel().c_str(),"Active")==0){

									(!endTime) && (firstStateEventB) && (counter_Service_runB[k]+=addCurrentDate(L,pos_ServiceB[k]));

									//(endTime) && (counter_Service_runB[k]+=setEndDate());

								}else if(strcmp( L[pos_ServiceB[k]].getRunLevel().c_str(),"Stand-by")==0){

									(!endTime) && (firstStateEventB) && (counter_Service_stpB[k]+=addCurrentDate(L,pos_ServiceB[k]));

									//(endTime) && (counter_Service_stpB[k]+=setEndDate());

									counter_Reason_SpB[k] = counter_Detail_SpB[k][0] = counter_Service_stpB[k];

								}
							}

							if (reason)
							{
								//	                            int posEvent = 0;
								//	                            int pos_cont = 0;
								//	                            for (unsigned int j=0;j<pos_Service.size();j++){
								//	                                counter_Reason_Ord[j]=0;
								//	                                counter_Reason_Sp[j]=0;
								//	                                counter_Detail_Ord[j][0]=0;
								//	                                counter_Detail_Sp[j][0]=0;
								//	                                int lastElement = (pos_Service[j]);
								//	                                int foundFirstSp = 0;
								//	                                int foundFirstOrd = 0;
								//	                                int orderedState = 0;
								//	                                int spontaneousState = 0;
								//	                                int firstActiveEvent = 0;
								//	                                for (unsigned int i=0; i< tempA.size();i++)
								//	                                  {
								//	                                    if(strcmp( L[pos_Service[j]].getResourceName().c_str(),L[tempA[i]].getResourceName().c_str())==0)
								//	                                      {
								//
								//	                                        if (strcmp( "Stand-by",L[tempA[i]].getRunLevel().c_str())==0 )
								//	                                          {
								//	                                            if (strcmp(L[tempA[i]].getReason().c_str(),"ordered")==0 )
								//	                                              {
								//	                                                orderedState = 1;
								//	                                                (foundFirstOrd || firstActiveEvent) && !(spontaneousState) && (counter_Reason_Ord[j] += eventAnalyserByNode(L,pos_Service[j], tempA[i]));
								//	                                                (spontaneousState) && (counter_Reason_Sp[j] += eventAnalyserByNode(L,pos_Service[j], tempA[i]));
								//	                                                (tempA[i] == lastElement) && (counter_Reason_Ord[j] += addCurrentDate(L, lastElement));
								//
								//	                                                if(detailedInfo)
								//	                                                  {
								//	                                                    for (unsigned int k=0; k < pos_Detail_Ord.size();k++)
								//	                                                      {
								//
								//	                                                        if(strcmp(L[tempA[i]].getReasonInfo().c_str(), L[pos_Detail_Ord[k]].getReasonInfo().c_str())==0)
								//	                                                          {
								//	                                                            t++;
								//	                                                            !(spontaneousState) && (counter_Detail_Ord[j][pos_cont] += eventAnalyserByNode(L,posEvent,tempA[i]));
								//	                                                            (spontaneousState) && (counter_Detail_Sp[j][pos_cont] += eventAnalyserByNode(L,posEvent,tempA[i]));
								//	                                                            (tempA[i] == lastElement) && (counter_Detail_Ord[j][k] += addCurrentDate(L, lastElement));
								//	                                                            pos_cont = k;
								//	                                                            //posEvent = tempA[i];
								//	                                                          }
								//	                                                      }
								//	                                                    if (t==0)
								//	                                                      {
								//	                                                        pos_Detail_Ord.push_back(tempA[i]);
								//	                                                        (tempA[i] == lastElement) && (counter_Detail_Ord[j][pos_Detail_Ord.size()-1] += addCurrentDate(L, lastElement));
								//	                                                        (foundFirstOrd || firstActiveEvent) && !(spontaneousState) && (counter_Detail_Ord[j][pos_cont] += eventAnalyserByNode(L, posEvent,tempA[i]));
								//	                                                        (spontaneousState) && (counter_Detail_Sp[j][pos_cont] += eventAnalyserByNode(L, posEvent,tempA[i]));
								//	                                                        pos_cont = pos_Detail_Ord.size() - 1;
								//	                                                        //posEvent = tempA[i];
								//
								//	                                                      }else t=0;
								//	                                                  }
								//	                                                foundFirstOrd = 1;
								//	                                                spontaneousState = 0;
								//	                                              }
								//	                                            else if (strcmp(L[tempA[i]].getReason().c_str(),"spontaneous")==0 )
								//	                                              {
								//	                                                spontaneousState = 1;
								//	                                                (foundFirstSp || firstActiveEvent) && !(orderedState) && (counter_Reason_Sp[j] += eventAnalyserByNode(L,pos_Service[j],tempA[i]));
								//	                                                (orderedState) && (counter_Reason_Ord[j] += eventAnalyserByNode(L,pos_Service[j],tempA[i]));
								//	                                                (tempA[i] == lastElement) && (counter_Reason_Sp[j] += addCurrentDate(L, lastElement));
								//	                                                printf("L[pos_Service[j]].getResourceName().c_str()=%s \n"
								//	                                                    "foundFirstSp=%d || firstActiveEvent=%d) && !(orderedState=%d) && (counter_Reason_Sp[j]=%f\n",L[pos_Service[j]].getResourceName().c_str(),
								//	                                                    foundFirstSp,firstActiveEvent,orderedState , counter_Reason_Sp[j]);
								//	                                                if(detailedInfo)
								//	                                                  {
								//	                                                    for (unsigned int k=0; k < pos_Detail_Sp.size();k++)
								//	                                                      {
								//
								//	                                                        if(strcmp(L[tempA[i]].getReasonInfo().c_str(), L[pos_Detail_Sp[k]].getReasonInfo().c_str())==0)
								//	                                                          {
								//	                                                            t++;
								//	                                                            !(orderedState) && (counter_Detail_Sp[j][pos_cont] += eventAnalyserByNode(L,posEvent,tempA[i]));
								//	                                                            (orderedState) && (counter_Detail_Ord[j][pos_cont] += eventAnalyserByNode(L,posEvent,tempA[i]));
								//	                                                            (tempA[i] == lastElement) && (counter_Detail_Sp[j][k] += addCurrentDate(L, lastElement));	                                            				pos_cont = k;
								//	                                                            //posEvent = tempA[i];
								//	                                                          }
								//	                                                      }
								//	                                                    if (t==0)
								//	                                                      {
								//	                                                        pos_Detail_Sp.push_back(tempA[i]);
								//	                                                        (tempA[i] == lastElement) && (counter_Detail_Sp[j][pos_Detail_Sp.size()-1] += addCurrentDate(L, lastElement));
								//	                                                        (foundFirstSp || firstActiveEvent) && !(orderedState) && (counter_Detail_Sp[j][pos_cont] += eventAnalyserByNode(L, posEvent,tempA[i]));
								//	                                                        (orderedState) && (counter_Detail_Ord[j][pos_cont] += eventAnalyserByNode(L, posEvent,tempA[i]));
								//	                                                        pos_cont = pos_Detail_Sp.size() - 1;
								//	                                                        //posEvent = tempA[i];
								//
								//	                                                      }else t=0;
								//	                                                  }
								//	                                                foundFirstSp = 1;
								//	                                                orderedState = 0;
								//	                                              }
								//	                                          }else{
								//	                                              firstActiveEvent = 1;
								//	                                          }
								//	                                        //update second last
								//	                                        pos_Service[j] = tempA[i];
								//	                                        posEvent = tempA[i];
								//	                                      }
								//	                                    if(tempA[i] == lastElement) break;
								//	                                  }
								//
								//	                                if(detailedInfo){
								//	                                    detail_Ord.push_back(pos_Detail_Ord);
								//	                                    pos_Detail_Ord.clear();
								//	                                    detail_Sp.push_back(pos_Detail_Sp);
								//	                                    pos_Detail_Sp.clear();
								//	                                }
								//	                            }///////////////////////////////////////
								//
								//	                            posEvent = 0;
								//	                            pos_cont = 0;
								//	                            for (unsigned int j=0;j<pos_ServiceB.size();j++){
								//	                                counter_Reason_OrdB[j]=0;
								//	                                counter_Reason_SpB[j]=0;
								//	                                counter_Detail_OrdB[j][0]=0;
								//	                                counter_Detail_SpB[j][0]=0;
								//	                                int lastElementB = (pos_ServiceB[j]);
								//	                                int foundFirstSp = 0;
								//	                                int foundFirstOrd = 0;
								//	                                int orderedState = 0;
								//	                                int spontaneousState = 0;
								//	                                int firstActiveEvent = 0;
								//
								//	                                for (unsigned int i=0; i< tempB.size();i++)
								//	                                  {
								//	                                    if(strcmp( L[pos_ServiceB[j]].getResourceName().c_str(),L[tempB[i]].getResourceName().c_str())==0)
								//	                                      {
								//	                                        //	                                        printf("Service == %s \n", L[tempB[i]].getResourceName().c_str());
								//	                                        if (strcmp( "Stand-by",L[tempB[i]].getRunLevel().c_str())==0 ){
								//	                                            if (strcmp(L[tempB[i]].getReason().c_str(),"ordered")==0 )
								//	                                              {
								//	                                                orderedState = 1;
								//	                                                (foundFirstOrd || firstActiveEvent) && !(spontaneousState) && (counter_Reason_OrdB[j] += eventAnalyserByNode(L,pos_ServiceB[j], tempB[i]));
								//	                                                (spontaneousState) && (counter_Reason_SpB[j] += eventAnalyserByNode(L,pos_ServiceB[j], tempB[i]));
								//	                                                (tempB[i] == lastElementB) && (counter_Reason_OrdB[j] += addCurrentDate(L, lastElementB));
								//	                                                //pos_ServiceB[j] = tempB[i];
								//
								//	                                                if(detailedInfo)
								//	                                                  {
								//	                                                    for (unsigned int k=0; k < pos_Detail_OrdB.size();k++)
								//	                                                      {
								//
								//	                                                        if(strcmp(L[tempB[i]].getReasonInfo().c_str(), L[pos_Detail_OrdB[k]].getReasonInfo().c_str())==0)
								//	                                                          {
								//	                                                            t++;
								//	                                                            !(spontaneousState) && (counter_Detail_OrdB[j][pos_cont] += eventAnalyserByNode(L,posEvent,tempB[i]));
								//	                                                            (spontaneousState) && (counter_Detail_SpB[j][pos_cont] += eventAnalyserByNode(L,posEvent,tempB[i]));
								//	                                                            (tempB[i] == lastElementB) && (counter_Detail_OrdB[j][k] += addCurrentDate(L, lastElementB));	                                							pos_cont = k;
								//	                                                            //posEvent = tempB[i];
								//	                                                          }
								//	                                                      }
								//	                                                    if (t==0)
								//	                                                      {
								//	                                                        pos_Detail_OrdB.push_back(tempB[i]);
								//	                                                        (tempB[i] == lastElementB) && (counter_Detail_OrdB[j][pos_Detail_OrdB.size()-1] += addCurrentDate(L, lastElementB));
								//	                                                        (foundFirstOrd || firstActiveEvent) && !(spontaneousState) && (counter_Detail_OrdB[j][pos_cont] += eventAnalyserByNode(L, posEvent,tempB[i]));
								//	                                                        (spontaneousState) && (counter_Detail_SpB[j][pos_cont] += eventAnalyserByNode(L, posEvent,tempB[i]));
								//	                                                        pos_cont = pos_Detail_OrdB.size() - 1;
								//	                                                        //posEvent = tempB[i];
								//
								//	                                                      }else t=0;
								//	                                                  }
								//	                                                foundFirstOrd = 1;
								//	                                                spontaneousState = 0;
								//	                                              }
								//	                                            else if (strcmp(L[tempB[i]].getReason().c_str(),"spontaneous")==0 )
								//	                                              {
								//	                                                spontaneousState = 1;
								//	                                                (foundFirstSp || firstActiveEvent) && !(orderedState) && (counter_Reason_SpB[j] += eventAnalyserByNode(L,pos_ServiceB[j],tempB[i]));
								//	                                                (orderedState) && (counter_Reason_OrdB[j] += eventAnalyserByNode(L,pos_ServiceB[j],tempB[i]));
								//	                                                (tempB[i] == lastElementB) && (counter_Reason_SpB[j] += addCurrentDate(L, lastElementB));
								//	                                                //	                                                printf("SPO foundFirstSp == %d -- firstActiveEvent == %d -- counter_Reason_SpB == %f\n",
								//	                                                //	                                                    foundFirstSp,firstActiveEvent,counter_Reason_SpB[j]);
								//	                                                //pos_ServiceB[j] = tempB[i];
								//	                                                if(detailedInfo)
								//	                                                  {
								//	                                                    for (unsigned int k=0; k < pos_Detail_SpB.size();k++)
								//	                                                      {
								//	                                                        if(strcmp(L[tempB[i]].getReasonInfo().c_str(), L[pos_Detail_SpB[k]].getReasonInfo().c_str())==0)
								//	                                                          {
								//	                                                            t++;
								//	                                                            !(orderedState) && (counter_Detail_SpB[j][pos_cont] += eventAnalyserByNode(L,posEvent,tempB[i]));
								//	                                                            (orderedState) && (counter_Detail_OrdB[j][pos_cont] += eventAnalyserByNode(L,posEvent,tempB[i]));
								//	                                                            (tempB[i] == lastElementB) && (counter_Detail_SpB[j][k] += addCurrentDate(L, lastElementB));
								//	                                                            pos_cont = k;
								//	                                                            //	                                                            printf("SPO CHCK foundFirstSp == %d -- pos_cont == %d -- counter_Reason_SpB == %f\n",
								//	                                                            //	                                                                foundFirstSp,pos_cont,counter_Detail_SpB[j][pos_cont]);
								//	                                                            //posEvent = tempB[i];
								//	                                                          }
								//	                                                      }
								//	                                                    if (t==0)
								//	                                                      {
								//	                                                        pos_Detail_SpB.push_back(tempB[i]);
								//	                                                        (tempB[i] == lastElementB) && (counter_Detail_SpB[j][pos_Detail_SpB.size()-1] += addCurrentDate(L, lastElementB));
								//	                                                        (foundFirstSp || firstActiveEvent) && !(orderedState) && (counter_Detail_SpB[j][pos_cont] += eventAnalyserByNode(L, posEvent,tempB[i]));
								//	                                                        (orderedState) && (counter_Detail_OrdB[j][pos_cont] += eventAnalyserByNode(L, posEvent,tempB[i]));
								//	                                                        pos_cont = pos_Detail_SpB.size() - 1;
								//	                                                        //	                                                        printf("SPO PUSH foundFirstSp == %d -- pos_cont == %d -- counter_Reason_SpB == %f\n",
								//	                                                        //	                                                            foundFirstSp,pos_cont,counter_Detail_SpB[j][pos_cont]);
								//	                                                        //posEvent = tempB[i];
								//
								//	                                                      }else t=0;
								//	                                                  }
								//	                                                foundFirstSp = 1;
								//	                                                orderedState = 0;
								//	                                              }
								//	                                        }else{
								//	                                            firstActiveEvent = 1;
								//	                                        }
								//	                                        //update second last
								//	                                        pos_ServiceB[j] = tempB[i];
								//	                                        posEvent = tempB[i];
								//	                                      }
								//	                                    if(tempB[i] == lastElementB) break;
								//	                                  }
								//
								//	                                if(detailedInfo){
								//	                                    detail_OrdB.push_back(pos_Detail_OrdB);
								//	                                    pos_Detail_OrdB.clear();
								//	                                    detail_SpB.push_back(pos_Detail_SpB);
								//	                                    pos_Detail_SpB.clear();
								//	                                }
								//	                            }
							}


							//---------------------------------------------------PRINTOUT SERVICE STATE----------------------------------------------

							/*[Service State node SC-2-n                                             ]
								|                                                                    |
								| Service  State  [Reason] [Details]                         Time    |
								|------------------------------------------------------------------- |
								|[                                                                  ]|
								||[service                                                   time]  ||
								||                                                                  ||
								||          [[state                                          time] ]||
								||          |                                                      |||
								||          |       [[reason                                 time]]|||
								||          |       |        [details                        time]||||
								||          |       |            .                                ||||
								||          |       |            .                                ||||
								||          |       [            .                                ]|||
								||          [                                                      ]||
								|[                                                                  ]|
								|                                    .                               |
							 * */


							oss<<"\n\nService State node "<< p_node_hostname_1<< "\n" << endl <<
									"Service    State   ";
							if (reason)oss<<"Reason";else oss<<"      ";
							if (detailedInfo)oss<<"  Details";else oss<<"          ";
							oss<<"                                      Time"<<endl;
							oss<<"-----------------------------------------------------------------------------"<<endl;



							for (unsigned int i=0;i<pos_Service.size();i++)
							{
								cout.setf(ios::left, ios::adjustfield);
								cout.fill(' ');
								minutes = counter_Service[i] / 60;
								s = (int) counter_Service[i] % 60;
								hours = minutes / 60;
								m = (int)minutes % 60;
								int len = 30 - strlen( L[pos_Service[i]].getResourceName().c_str() );
								cout.width(60);
								cout.setf(ios::left, ios::adjustfield);
								cout.fill(' ');
								oss << L[pos_Service[i]].getResourceName()
										        		 << std::setfill(' ') << std::setw(41+len) << (int)hours << ":"
														 << std::setfill('0') << std::setw(2) << m << ":"
														 << std::setfill('0') << std::setw(2) << s<<endl;

								if( counter_Service_run[i]!=0 )
								{
									minutes = counter_Service_run[i] / 60;
									s = (int) counter_Service_run[i] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									len = 30 - strlen( " running" );
									cout.width(60);
									cout.setf(ios::left, ios::adjustfield);
									cout.fill(' ');
									oss <<"           "<< " running"
											<< std::setfill(' ') << std::setw(30+len) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s<<endl;
								}
								if ( counter_Service_stp[i]!=0 )
								{
									minutes = counter_Service_stp[i] / 60;
									s = (int) counter_Service_stp[i] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									len = 30 - strlen( " stopped" );
									cout.width(60);
									cout.setf(ios::left, ios::adjustfield);
									cout.fill(' ');
									oss <<"           "<< " stopped"
											<< std::setfill(' ') << std::setw(30+len) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s<<endl;
								}
								string outS(oss.str());
								*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
								strcpy( buffer +4, outS.c_str());
								stream.send_n(buffer, outS.size() + 4 );
								oss.str("");
								if (reason)
								{
									if ( counter_Reason_Ord[i]!=0 )
									{
										minutes = counter_Reason_Ord[i] / 60;
										s = (int)counter_Reason_Ord[i] % 60;
										hours = minutes / 60;
										m = (int)minutes % 60;
										int len = 30 - strlen( "ordered" );
										cout.width(60);
										oss <<"                   "<< "ordered"
												//cout.setf(ios::right, ios::adjustfield);
												<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
												<< std::setfill('0') << std::setw(2) << m << ":"
												<< std::setfill('0') << std::setw(2) << s;
										string outS(oss.str());
										*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
										strcpy( buffer +4, outS.c_str());
										stream.send_n(buffer, outS.size() + 4 );
										oss.str("");
										outS.clear();

										if (detailedInfo)
										{
											for (unsigned int j=0;j<detail_Ord[i].size();j++)
											{
												if ( counter_Detail_Ord[i][j]!=0 )
												{
													minutes=0,s=0,hours=0,m=0;
													minutes = counter_Detail_Ord[i][j] / 60;
													s = (int)counter_Detail_Ord[i][j] % 60;
													hours = minutes / 60;
													m = (int)minutes % 60;
													int len = 30 - strlen( L[ detail_Ord[i][j] ].getReasonInfo().c_str() );
													cout.width(60);
													oss <<"                           "<< L[ detail_Ord[i][j] ].getReasonInfo().c_str()
													            		 //cout.setf(ios::right, ios::adjustfield);
													<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
															<< std::setfill('0') << std::setw(2) << m << ":"
															<< std::setfill('0') << std::setw(2) << s;
													string outS(oss.str());
													*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
													strcpy( buffer +4, outS.c_str());
													stream.send_n(buffer, outS.size() + 4 );
													oss.str("");
													outS.clear();
												}
											}
										}
									}
									//	                                    printf("----------------\n"
									//	                                        "PRINTING: k=%d - counter_Reason_Sp=%f\n",i, counter_Reason_Sp[i]);
									if ( counter_Reason_Sp[i]!=0 )
									{
										minutes = counter_Reason_Sp[i] / 60;
										s = (int)counter_Reason_Sp[i] % 60;
										hours = minutes / 60;
										m = (int)minutes % 60;
										int len = 30 - strlen( "spontaneous" );
										cout.width(60);
										oss <<"                   "<< "spontaneous"
												//cout.setf(ios::right, ios::adjustfield);
												<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
												<< std::setfill('0') << std::setw(2) << m << ":"
												<< std::setfill('0') << std::setw(2) << s;
										string outSS(oss.str());
										*reinterpret_cast<uint32_t *>(buffer) = htonl ( outSS.size());
										strcpy( buffer +4, outSS.c_str());
										stream.send_n(buffer, outSS.size() + 4 );
										oss.str("");
										outSS.clear();
									}
									if (detailedInfo)
									{
										//	                                        for (unsigned int j=0;j<detail_Sp[i].size();j++)
										//	                                          {
										if ( counter_Detail_Sp[i][0]!=0 )
										{
											minutes=0,s=0,hours=0,m=0;
											minutes = counter_Detail_Sp[i][0] / 60;
											s = (int)counter_Detail_Sp[i][0] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( "fault" );
											cout.width(60);
											oss <<"                           "<< "fault"
													<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
													<< std::setfill('0') << std::setw(2) << m << ":"
													<< std::setfill('0') << std::setw(2) << s;
											string outS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
											strcpy( buffer +4, outS.c_str());
											stream.send_n(buffer, outS.size() + 4 );
											oss.str("");
											outS.clear();
										}
										//	                                          }
									}
								}
							}

							oss<<"\n\nService State node " << p_node_hostname_2 << "\n" << endl <<
									"Service    State   ";
							if (reason)oss<<"Reason";else oss<<"      ";
							if (detailedInfo)oss<<"  Details";else oss<<"          ";
							oss<<"                                      Time"<<endl;
							oss<<"-----------------------------------------------------------------------------"<<endl;
							cout.setf(ios::left, ios::adjustfield);
							cout.fill(' ');
							string outS(oss.str());
							*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
							strcpy( buffer +4, outS.c_str());
							stream.send_n(buffer, outS.size() + 4 );
							oss.str("");

							for (unsigned int i=0;i<pos_ServiceB.size();i++)
							{
								minutes = counter_ServiceB[i] / 60;
								s = (int) counter_ServiceB[i] % 60;
								hours = minutes / 60;
								m = (int)minutes % 60;
								int len = 30 - strlen( L[pos_ServiceB[i]].getResourceName().c_str() );
								cout.width(60);
								cout.setf(ios::left, ios::adjustfield);
								cout.fill(' ');
								oss << L[pos_ServiceB[i]].getResourceName()
                                    		<< std::setfill(' ') << std::setw(41+len) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s<<endl;
								minutes = counter_ServiceB[i] / 60;

								if (counter_Service_runB[i]!=0)
								{
									minutes = counter_Service_runB[i] / 60;
									s = (int) counter_Service_runB[i] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									len = 30 - strlen( " running" );
									cout.width(60);
									cout.setf(ios::left, ios::adjustfield);
									cout.fill(' ');
									oss <<"           "<< " running"
											<< std::setfill(' ') << std::setw(30+len) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s<<endl;
								}
								if (counter_Service_stpB[i]!=0)
								{
									minutes = counter_Service_stpB[i] / 60;
									s = (int) counter_Service_stpB[i] % 60;
									hours = minutes / 60;
									m = (int)minutes % 60;
									len = 30 - strlen( " stopped" );
									cout.width(60);
									cout.setf(ios::left, ios::adjustfield);
									cout.fill(' ');
									oss <<"           "<< " stopped"
											<< std::setfill(' ') << std::setw(30+len) << (int)hours << ":"
											<< std::setfill('0') << std::setw(2) << m << ":"
											<< std::setfill('0') << std::setw(2) << s<<endl;
								}
								string outS(oss.str());
								*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
								strcpy( buffer +4, outS.c_str());
								stream.send_n(buffer, outS.size() + 4 );
								oss.str("");
								outS.clear();

								if (reason)
								{
									if ( counter_Reason_OrdB[i]!=0 )
									{
										minutes = counter_Reason_OrdB[i] / 60;
										s = (int)counter_Reason_OrdB[i] % 60;
										hours = minutes / 60;
										m = (int)minutes % 60;
										int len = 30 - strlen( "ordered" );
										cout.width(60);
										oss <<"                   "<< "ordered"
												//cout.setf(ios::right, ios::adjustfield);
										<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
										<< std::setfill('0') << std::setw(2) << m << ":"
										<< std::setfill('0') << std::setw(2) << s;
										string outS(oss.str());
										*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
										strcpy( buffer +4, outS.c_str());
										stream.send_n(buffer, outS.size() + 4 );
										oss.str("");
										outS.clear();

										if (detailedInfo)
										{
											for (unsigned int j=0;j<detail_OrdB[i].size();j++)
											{
												if ( counter_Detail_OrdB[i][j]!=0 )
												{
													minutes=0,s=0,hours=0,m=0;
													minutes = counter_Detail_OrdB[i][j] / 60;
													s = (int)counter_Detail_OrdB[i][j] % 60;
													hours = minutes / 60;
													m = (int)minutes % 60;
													int len = 30 - strlen( L[ detail_OrdB[i][j] ].getReasonInfo().c_str() );
													cout.width(60);
													oss <<"                           "<< L[ detail_OrdB[i][j] ].getReasonInfo().c_str()
														        		 << std::setfill(' ') << setw(14+len) << (int)hours << ":"
																		 << std::setfill('0') << std::setw(2) << m << ":"
																		 << std::setfill('0') << std::setw(2) << s;
													string outS(oss.str());
													*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
													strcpy( buffer +4, outS.c_str());
													stream.send_n(buffer, outS.size() + 4 );
													oss.str("");
													outS.clear();
												}
											}
										}
									}
									if ( counter_Reason_SpB[i]!=0 )
									{
										minutes = counter_Reason_SpB[i] / 60;
										s = (int)counter_Reason_SpB[i] % 60;
										hours = minutes / 60;
										m = (int)minutes % 60;
										int len = 30 - strlen( "spontaneous" );
										cout.width(60);
										oss <<"                   "<< "spontaneous"
												<< std::setfill(' ') << setw(22+len) << (int)hours << ":"
												<< std::setfill('0') << std::setw(2) << m << ":"
												<< std::setfill('0') << std::setw(2) << s;
										string outSS(oss.str());
										*reinterpret_cast<uint32_t *>(buffer) = htonl ( outSS.size());
										strcpy( buffer +4, outSS.c_str());
										stream.send_n(buffer, outSS.size() + 4 );
										oss.str("");
										outSS.clear();
									}
									if (detailedInfo)
									{
										//	                                    for (unsigned int j=0; j<detail_SpB[i].size();j++)
										//	                                      {
										if ( counter_Detail_SpB[i][0]!=0 )
										{
											minutes=0,s=0,hours=0,m=0;
											minutes = counter_Detail_SpB[i][0] / 60;
											s = (int)counter_Detail_SpB[i][0] % 60;
											hours = minutes / 60;
											m = (int)minutes % 60;
											int len = 30 - strlen( "fault"/*L[ detail_SpB[i][j] ].getReasonInfo().c_str() */);
											cout.width(60);
											oss <<"                           "<< "fault"/*L[ detail_SpB[i][j] ].getReasonInfo().c_str()*/
													<< std::setfill(' ') << setw(14+len) << (int)hours << ":"
													<< std::setfill('0') << std::setw(2) << m << ":"
													<< std::setfill('0') << std::setw(2) << s;
											string outS(oss.str());
											*reinterpret_cast<uint32_t *>(buffer) = htonl ( outS.size());
											strcpy( buffer +4, outS.c_str());
											stream.send_n(buffer, outS.size() + 4 );
											oss.str("");
											outS.clear();
										}
										//	                                      }
									}

								}
							}

							//Clean all
							for(unsigned int i=0;i<MAX_C;i++)
							{
								for(unsigned int k=0;k<MAX_C;k++){
									counter_Detail_Ord[i][k]=0;
									counter_Detail_Sp[i][k]=0;
									counter_Detail_OrdB[i][k]=0;
									counter_Detail_SpB[i][k]=0;
								}
							}

							detail_Ord.clear();
							detail_OrdB.clear();
							detail_Sp.clear();
							detail_SpB.clear();
							tempB.clear();
							tempA.clear();


							//clear pos
							serviceState=false;
							pos_ServiceB.clear();
							pos_Service.clear();

						}

						oss.str("");
						oss.flush();
						detailedInfo=false;
						analyse=false;
						serviceState=false;
						runLevel=false;
						nodeState=false;
						reason=false;
						startTime=false;
						endTime=false;
						utcFlag=false;
					}
				}

				//***********************************************************************************************
				sprintf(buffer+4,"END");
				*reinterpret_cast<uint32_t *>(buffer) = htonl ( 4 );
				stream.send_n(buffer, 8);

				//List Cleaning
				start_time.clear();
				end_time.clear();

				L.clear();
				tmpEvent.clear();
				tmpService.clear();
				tmpServiceLast.clear();
				tmpServiceFirst.clear();
				tmpApStateEvent.clear();
				tmpNodeStateEventA.clear();
				tmpNodeStateEventB.clear();
				tmpRunLevelStateEventA.clear();
				tmpRunLevelStateEventB.clear();
				tmpServiceStateEventA.clear();
				tmpServiceStateEventB.clear();

				//ifs.close();

				analyse      = false;
				runLevel     = false;
				nodeState    = false;
				serviceState = false;
				reason       = false;
				detailedInfo = false;

				startTime    = false;
				endTime      = false;
				shortPrint   = false;
				logSize 	 = false;
				ownnode	  	 = false;

			}

			else {
				sprintf(buffer+4,"NOT_FOUND");
				*reinterpret_cast<uint32_t *>(buffer) = htonl ( 10 );
				stream.send_n(buffer, 14);
			}

			stream.close();
		}

	}

	if ( ACE_OS::last_error() == EINTR ){
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcispd %s - Signal %s received ==> Shutdown", PRCBIN_REVISION, ispprint_Server_sighandler.getStrSignal().c_str() );
		Logging.Write(strErr_no, LOG_LEVEL_WARN);
	}

	Acceptor.close();
	Acceptor.remove();

	acs_apgcc_oihandler_V2* oiHandler = PrcispNotificationThread.getOI_Handler();
	ACS_PRC_IspAdminOperationImplementer* IspAdminOperationImplementer = PrcispNotificationThread.getOI();

	memset(strErr_no, 0, sizeof(strErr_no));

	PrcispNotificationThread.stop();	//Fix for TR HY21238
	PrcispNotificationThread.wait();

	if ( oiHandler->removeObjectImpl(IspAdminOperationImplementer) != ACS_CC_FAILURE)
		snprintf(strErr_no,1024,"acs_prcispd %s - Success to remove PRCISP as object implementer for ispConfigId", PRCBIN_REVISION );
	else
		snprintf(strErr_no,1024,"acs_prcispd %s - Fail to remove PRCISP as object implementer for ispConfigId", PRCBIN_REVISION );

	Logging.Write ( strErr_no, LOG_LEVEL_WARN );

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcispd %s - Stop", PRCBIN_REVISION );
	Logging.Write( strErr_no,LOG_LEVEL_WARN);

	p.close();
	Logging.Close();
	return 0;
}
