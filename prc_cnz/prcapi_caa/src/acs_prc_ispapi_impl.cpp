#include "ACS_PRC_ispapi.h"
#include "acs_prc_ispapi_impl.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/file.h>
//#include <fcntl.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <errno.h>
//#include <pthread.h>

namespace
{
	const int MAX_RETRY_ON_ACQUIRE_FAIL = 50U;
	const int MAX_RETRY_ON_RELEASE_FAIL = 50U;
	const int RETRY_DELAY_IN_MICROSEC = 100000U;	// 100 ms

	const char LOCK_FOLDER_NAME[] = "/cluster/etc/ap/acs/prc/.ISPFILElock";
	const mode_t LOCK_FOLDER_PERMISSION = S_IRWXU | S_IRWXG | S_IROTH ; // 774
};

namespace
{
	const int WRITE_FAILURE = -1;
};

using namespace std;

const int Ispapi_Implementation::BUFFER_SIZE_(8192);
const time_t Ispapi::TIME_NULL_ = 0;

//const char LOCAL_ISP_FILE[] = "/var/log/prcisp_syslog.log";

string Ispapi_Implementation::ownNode="";

char Ispapi_Implementation::ownNode_ = ' ';

string getReasonCategory ( reasonType reason ){

	switch (reason)
	{
	// State change ordered by operator
		case functionChange:
		case referenceFC:
		case softFunctionChange:
		case manualRebootInitiated:
		case hwmResetInitiated:
		case hwmBlkInitiated:
		case hwmDblkInitiated:
		case manualShutdown:
		case fccReset:
			return "ordered";
			break;

	// Spontaneous state change
		case causedByFailover:
		case causedByError:
		case userInitiated:
		case causedByEvent:
		case systemInitiated:
		case nsfInitiated:
		case resourceFailed:
		case bugcheckInitiated:
		case amBoardTemperature:
		case amBoardVoltage:
		case amBoardPMC:
		case amBoardCPCI:
		case amBoardWatchdog:
		case unknownReason:
			return "spontaneous";
			break;

		default :
			return "";
	}

}

//========================================================================================
//	Constructor     
//========================================================================================

Ispapi_Implementation::Ispapi_Implementation() :
data_(),
bufptr_(data_)
{
	syslog_path = strdup("/cluster/etc/ap/acs/prc/prcisp_syslog.log");

	p_local_node_id_path = "/etc/cluster/nodes/this/id";
	p_remote_node_id_path = "/etc/cluster/nodes/peer/id";
	p_local_node_hostname_path = "/etc/cluster/nodes/this/hostname";
	p_remote_node_hostname_path = "/etc/cluster/nodes/peer/hostname";
	syslog_fifo = 0;

}

//========================================================================================
//	Destructor     
//========================================================================================

Ispapi_Implementation::~Ispapi_Implementation(){
	free ( syslog_path );
}

//========================================================================================
//	Open the log file     
//========================================================================================

bool 
Ispapi_Implementation::open()
{
	ifstream ifs;

	ifs.open(p_local_node_id_path.c_str());
	if ( ifs.good())
		getline(ifs,p_local_node_id);
		//ifs.getline(node_id, 8);
	ifs.close();

	ifs.open(p_remote_node_id_path.c_str());
	if ( ifs.good())
		getline(ifs,p_remote_node_id);
		//ifs.getline(node_id, 8);
	ifs.close();

	ifs.open(p_local_node_hostname_path.c_str());
	if ( ifs.good())
		getline(ifs,p_local_node_hostname);
		//ifs.getline(node_id, 8);
	ifs.close();

	ifs.open(p_remote_node_hostname_path.c_str());
	if ( ifs.good())
		getline(ifs,p_remote_node_hostname);
		//ifs.getline(node_id, 8);
	ifs.close();

	// Get own node letter
	//string ownNodeName = isplogptr_->getOwnNodeName();
	string ownNodeName = "A";
	string::reverse_iterator iter = ownNodeName.rbegin();
	ownNode_ = toupper(*iter);

	//char buff_host[32];
	//gethostname(buff_host,sizeof(buff_host));

	Ispapi_Implementation::ownNode = p_local_node_hostname;

	// Open log
	IspFile_lock::cleanLockFolder();

	//syslog_fifo = ::open(syslog_path, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR );

	return true;
}


//========================================================================================
//	Close the log file     
//========================================================================================

bool 
Ispapi_Implementation::close(){
    return true; //::close(syslog_fifo);
}


//========================================================================================
//	Report a node runlevel event     
//========================================================================================

bool Ispapi_Implementation::runLevelEvent(string node, runLevelType level, reasonType reason, string info, time_t ftime)
{
	// Check parameters

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	if (node.size() == 0 || (node != p_local_node_hostname && node != p_remote_node_hostname))
	{
		Logging.Write("Ispapi_Implementation::runLevelEvent - Illegal node in runlevel event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	if (level < level_0 || level > unknownLevel)
	{
		Logging.Write("Ispapi_Implementation::runLevelEvent - Illegal runlevel in runlevel event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	if (reason < causedByFailover || reason > amBoardCPCI)
	{
		Logging.Write("Ispapi_Implementation::runLevelEvent - Invalid reason",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	Logging.Close();

	if ( ftime == 0) {
		ftime = time(NULL);
	}

	string temp("");
	char buffer [80] = {0};

	strftime (buffer,80,"%Y-%m-%d %X",localtime ( &ftime ));

	// String data
	string tstrings[] =
	{
		"ISP Event;Node Run Level Event;",
		buffer + string(";"),
		toString(node).data() + string(";"),
		toString(level).data() + string(";"),
		getReasonCategory(reason) + string(";"),
		toString(reason).data() + string(";"),
		info.data() + string(";\n")
	};

	int numstrings = 7;

	size_t size = 0;

	for (int i = 0; i < numstrings; i++)
	{
		size += tstrings[i].length();
		temp.append(tstrings[i]);
	}

	return safeLogWrite(temp.c_str(), size);
}

//========================================================================================
//	Report a node state event     
//========================================================================================

bool 
Ispapi_Implementation::nodeStateEvent(string node, nodeStateType state, reasonType reason, string info,
					   time_t ftime)
{
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	// Check parameters
	if (node.size() == 0 || (node != p_local_node_hostname && node != p_remote_node_hostname))
	{
		Logging.Write("Ispapi_Implementation::nodeStateEvent - Illegal node in nodestate event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	if (state < active || state > unknownNodeState)
	{
		Logging.Write("Ispapi_Implementation::nodeStateEvent - Illegal node state in nodestate event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	if ( node != Ispapi_Implementation::ownNode )
	{
		Logging.Write("Ispapi_Implementation::nodeStateEvent - Nodestate event did not originate from own node",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	if (reason < causedByFailover || reason > amBoardCPCI)
	{
		Logging.Write("Ispapi_Implementation::nodeStateEvent- Invalid reason in Nodestate event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	Logging.Close();

	if ( ftime == 0) {
		ftime = time(NULL);
	}

	string temp("");
	char buffer [80] = {0};

	strftime (buffer,80,"%Y-%m-%d %X",localtime ( &ftime ));

	// String data
	string tstrings[] =
	{
		"ISP Event;Node state event;",
		buffer + string(";"),
		toString(node).data() + string(";"),
		toString(state).data() + string(";"),
		getReasonCategory(reason) + string(";"),
		toString(reason).data() + string(";"),
		info.data() + string(";\n")
	};

	int numstrings = 7;

	size_t size = 0;

	for (int i = 0; i < numstrings; i++)
	{
		size += tstrings[i].length();
		temp.append(tstrings[i]);
	}

	return safeLogWrite(temp.c_str(), size);
}

//========================================================================================
//	Report a resource state event    
//========================================================================================

bool 
Ispapi_Implementation::SuStateEvent(string resourceName, string node, resourceStateType state,
						   reasonType reason, string info, time_t ftime)
{
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	if (node.size() == 0 || (node != p_local_node_hostname && node != p_remote_node_hostname))
	{
		Logging.Write("Ispapi_Implementation::SuStateEvent - Illegal node in SuStateEvent event",LOG_LEVEL_ERROR);
		//fprintf(stdout,"Illegal node in SuStateEvent event.\n");
		return false;
	}

	if (state < started || state > unknownResourceState)
	{
		Logging.Write("Ispapi_Implementation::SuStateEvent - Illegal Service Unit state in SuStateEvent event",LOG_LEVEL_ERROR);
		//fprintf(stdout,"Illegal Service Unit state ( %i ) in SuStateEvent event.\n",state);
		return false;
	}

	if ( node != Ispapi_Implementation::ownNode )
	{
		Logging.Write("Ispapi_Implementation::SuStateEvent - uStateEvent event did not originate from own node",LOG_LEVEL_ERROR);
		//fprintf(stdout,"SuStateEvent event did not originate from own node.\n");
		return false;
	}

	if (reason < causedByFailover || reason > amBoardCPCI)
	{
		Logging.Write("Ispapi_Implementation::SuStateEvent - Invalid reason in SuStateEvent event",LOG_LEVEL_ERROR);
		//fprintf(stdout,"Invalid reason in SuStateEvent event.\n");
		return false;
	}

	Logging.Close();

	if ( ftime == 0) {
		ftime = time(NULL);
	}

	string temp("");
	char buffer [80];

	strftime (buffer,80,"%Y-%m-%d %X",localtime ( &ftime ));

	// String data
	string tstrings[] =
	{
		"ISP Event;Service Unit state event;",
		buffer + string(";"),
		toString(resourceName).data() + string(";"),
		toString(node).data() + string(";"),
		toString(state).data() + string(";"),
		getReasonCategory(reason) + string(";"),
		toString(reason).data() + string(";"),
		info.data() + string(";\n")
	};

	int numstrings = 8;

	size_t size = 0;

	for (int i = 0; i < numstrings; i++)
	{
		size += tstrings[i].length();
		temp.append(tstrings[i]);
	}

	return safeLogWrite(temp.c_str(), size);
}

//========================================================================================
//	Reports that the other node has shut down    
//========================================================================================

bool 
Ispapi_Implementation::otherNodeShutDown(std::string node, reasonType reason,
						  std::string info, time_t ftime)
{
	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	if (node.size() == 0 || (node != p_local_node_hostname && node != p_remote_node_hostname))
	{
		Logging.Write("Ispapi_Implementation::otherNodeShutDown - Illegal node in node shutdown event",LOG_LEVEL_ERROR);
		//fprintf(stdout,"Illegal node in node shutdown event.\n");
		return false;
	}

	if ( node == Ispapi_Implementation::ownNode )
	{
		Logging.Write("Ispapi_Implementation::otherNodeShutDown - Node shutdown event did not originate from remote node",LOG_LEVEL_ERROR);
		//fprintf(stdout,"Node shutdown event did not originate from remote node.\n");
		return false;
	}

	if (reason < causedByFailover || reason > amBoardCPCI)
	{
		Logging.Write("Ispapi_Implementation::otherNodeShutDown - Invalid reason Node shutdown event",LOG_LEVEL_ERROR);
		//fprintf(stdout,"Invalid reason Node shutdown event.\n");
		return false;
	}

	Logging.Close();

	if ( ftime == 0) {
		ftime = time(NULL);
	}

	string temp("");
	char buffer [80];

	strftime (buffer,80,"%Y-%m-%d %X",localtime ( &ftime ));

	// String data
	string tstrings[] =
	{
		"ISP Event;Shutdown event;",
		buffer + string(";"),
		toString(node).data() + string(";"),
		getReasonCategory(reason) + string(";"),
		toString(reason).data() + string(";"),
		info.data() + string("; ;\n")
	};

	const int numstrings(6);

	size_t size = 0;

	for (int i = 0; i < numstrings; i++)
	{
		size += tstrings[i].length();
		temp.append(tstrings[i]);
	}

	return safeLogWrite(temp.c_str(), size);;
  }


//========================================================================================
//	Runlevel outstream operator
//========================================================================================

std::ostream&
operator<<(std::ostream& s, runLevelType level)
{
	switch (level){
		case level_0:		s << "0";		break;
		case level_1:		s << "1";		break;
		case level_2:		s << "2";		break;
		case level_3:		s << "3";		break;
		case level_4:		s << "4";		break;
		case level_5:		s << "5";		break;
		case unknownLevel:	s << "unknown";	break;
		default:;
	}
	return s;
}


//========================================================================================
//	Node state outstream operator
//========================================================================================

std::ostream&
operator<<(std::ostream& s, nodeStateType state)
{
	switch (state){
		case active:			s << "active";	break;
		case passive:			s << "passive";	break;
		case unknownNodeState:	s << "unknown";	break;
		default:;
		}
	return s;
}


//========================================================================================
//	Resource state outstream operator
//========================================================================================

std::ostream&
operator<<(std::ostream& s, resourceStateType state)
{
	switch (state){
		case started:				s << "Active";		break;
		case stopped:				s << "Stand-by";	break;
		case failed:				s << "failed";		break;
		case inserted:				s << "inserted";	break;
		case removed:				s << "removed";		break;
		case unknownResourceState:	s << "unknown";		break;
		default:;
	}
	return s;
}

//========================================================================================
//	Reason type outstream operator
//========================================================================================

std::ostream&
operator<<(std::ostream& s, reasonType reason)
{
	switch (reason){
		case causedByFailover:      s << "failover";                    break;
		case causedByError:         s << "fault";                       break;
		case functionChange:        s << "function change (FCH)";       break;
		case softFunctionChange:    s << "soft function change (SFC)";  break;
		case userInitiated:         s << "anyone";                      break;
		case causedByEvent:         s << "PRC_Eva";                     break;
		case manualRebootInitiated: s << "manually reboot";             break;
		case systemInitiated:       s << "Internal use only";           break;
		case unknownReason:         s << "unknown";                     break;
		case referenceFC:           s << "function change (FCR)";       break;
		case nsfInitiated:          s << "NSF Initiated";               break;
		case resourceFailed:        s << "PRC: Resource failed";        break;
		case hwmResetInitiated:     s << "HWM reset";                   break;
		case hwmBlkInitiated:       s << "HWM blk";                     break;
		case hwmDblkInitiated:      s << "HWM dblk";                    break;
		case manualShutdown:        s << "Manual shut down";            break;
		case bugcheckInitiated:     s << "BugCheck";                    break;
		case fccReset:              s << "fcc_reset";                   break;
		case amBoardWatchdog:       s << "Frozen Node";                 break;
		case amBoardTemperature:    s << "High Temperature";            break;
		case amBoardVoltage:        s << "Bad Voltage";                 break;
		case amBoardPMC:            s << "PMC Power Signal";            break;
		case amBoardCPCI:           s << "CPCI Bus Signal";             break;
		default:;
	}
	return s;
}

//========================================================================================
//	Reports that the other node has shut down
//========================================================================================

int
Ispapi_Implementation::getFirstEventLogTime( std::string& time )
{
	ifstream ifs;
	ifstream ifs_1;

	char buffer[1024] = {0};
	int result = -1;

	string ispLog = syslog_path;
	string ispLog_1 = syslog_path + string(".1");

	ifs_1.open(ispLog_1.c_str());

	if ( ifs_1.good() ){
		ifs_1.getline(buffer, 1024);

		if ( !strlen( buffer ) )
			result = -1;
		else {

			char* token = strtok (buffer,";");

			token = strtok (NULL, ";");
			token = strtok (NULL, ";");

			if ( strlen( token ) ) {
				time.append(token);
				result = 0;
			}
			else
				result = -1;
		}
	}

	if ( result == -1 ) {
		ifs.open(ispLog.c_str());

		if ( ifs.good() ){
			ifs.getline(buffer, 1024);

			if ( !strlen( buffer ) )
				result = -1;
			else {

				char* token = strtok (buffer,";");

				token = strtok (NULL, ";");
				token = strtok (NULL, ";");

				if ( strlen( token ) ) {
					time.append(token);
					result = 0;
				}
				else
					result = -1;
			}
		}
		else
			result = -1;
	}

	ifs.close();
	ifs_1.close();

	return result;
}


bool
Ispapi_Implementation::APStateEvent( int APstate, time_t ftime)
{
	ACS_TRA_Logging Logging;

	Logging.Open("PRC");

	if ( APstate < 0 || APstate > 4 )
	{
		Logging.Write("Ispapi_Implementation::APStateEvent - Illegal AP state in APStateEvent event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	Logging.Close();

	if ( ftime == 0) {
		ftime = time(NULL);
	}

	string temp("");
	char buffer [80] = {0};

	strftime (buffer,80,"%Y-%m-%d %X",localtime ( &ftime ));

	// String data
	string tstrings[] =
	{
		"ISP Event;AP state event;",
		buffer + string(";"),
		string(" ;"),
		toString(APstate).data() + string(";"),
		string(";"),
		string(";"),
		string(";\n")
	};

	int numstrings = 7;

	size_t size = 0;

	for (int i = 0; i < numstrings; i++){
		size += tstrings[i].length();
		temp.append(tstrings[i]);
	}

	return safeLogWrite(temp.c_str(), size);

}

void Ispapi_Implementation::setremoteNode( const char* remoteNode ){
	this->p_remote_node_hostname = remoteNode;
}

void Ispapi_Implementation::setlocalNode( const char* localNode ){
	this->p_local_node_hostname = localNode;
}

bool Ispapi_Implementation::node_State_Runlevel_Sync ( std::string node, nodeStateType state, reasonType node_reason, runLevelType RunLEvel, reasonType runlevel_reason){

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	// Check parameters
	if (node.size() == 0 || (node != p_local_node_hostname && node != p_remote_node_hostname))
	{
		Logging.Write("Ispapi_Implementation::node_State_Runlevel_Sync - Illegal node in Sync event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	if (state < active || state > unknownNodeState)
	{
		Logging.Write("Ispapi_Implementation::node_State_Runlevel_Sync - Illegal node state in Sync event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	if (node_reason < causedByFailover || node_reason > amBoardCPCI)
	{
		Logging.Write("Ispapi_Implementation::node_State_Runlevel_Sync - Invalid reason in Sync event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}


	if (RunLEvel < level_0 || RunLEvel > unknownLevel)
	{
		Logging.Write("Ispapi_Implementation::node_State_Runlevel_Sync - Illegal node state in Sync event",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	if (runlevel_reason < causedByFailover || runlevel_reason > amBoardCPCI)
	{
		Logging.Write("Ispapi_Implementation::node_State_Runlevel_Sync - Invalid reason",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	if ( node != Ispapi_Implementation::ownNode )
	{
		Logging.Write("Ispapi_Implementation::node_State_Runlevel_Sync - Sync event did not originate from own node",LOG_LEVEL_ERROR);
		Logging.Close();
		return false;
	}

	Logging.Close();

	time_t ftime = time(NULL);

	string temp("");
	char buffer [80] = {0};

	strftime (buffer,80,"%Y-%m-%d %X",localtime ( &ftime ));

	// String data
	string tstrings[] =
	{
		"ISP Event;Sync event;",
		buffer + string(";"),
		toString(node).data() + string(";"),

		toString(state).data() + string("@@") + toString(node_reason).data() + string(";"),

		toString(RunLEvel).data() + string("@@") + toString(runlevel_reason).data() + string(";"),

		string(" ;\n")
	};

	int numstrings = 6;

	size_t size = 0;

	for (int i = 0; i < numstrings; i++)
	{
		size += tstrings[i].length();
		temp.append(tstrings[i]);
	}

	return safeLogWrite(temp.c_str(), size);

}

bool Ispapi_Implementation::safeLogWrite(const void* bufferToWrite, const size_t& bufferSize)
{
	// get exclusive access to the ISP file
	IspFile_lock lock;

	syslog_fifo = ::open(syslog_path, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR );
	//int syslog_local_file = ::open(LOCAL_ISP_FILE, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR );

	bool writeResult = ( WRITE_FAILURE != ::write(syslog_fifo, bufferToWrite, bufferSize) );
	//bool writeResult_local_file = ( WRITE_FAILURE != ::write(syslog_local_file, bufferToWrite, bufferSize) );

	::close(syslog_fifo);
	//::close(syslog_local_file);

	return writeResult;
}

Ispapi_Implementation::IspFile_lock::IspFile_lock()
: m_LockAcquired(false)
{
	int numberOfFail = 0U;

	do
	{
		// create lock folder
		m_LockAcquired = ( ::mkdir(LOCK_FOLDER_NAME, LOCK_FOLDER_PERMISSION) == 0U );

		if(!m_LockAcquired)
		{
			// lock not acquired
			++numberOfFail;
			// wait before retry
			usleep(RETRY_DELAY_IN_MICROSEC);
		}

	}while(!m_LockAcquired && (numberOfFail < MAX_RETRY_ON_ACQUIRE_FAIL));

}

Ispapi_Implementation::IspFile_lock::~IspFile_lock()
{
	int numberOfFail = 0U;

	while(m_LockAcquired && (numberOfFail < MAX_RETRY_ON_RELEASE_FAIL) )
	{
		// remove the lock folder
		m_LockAcquired = ( ( ::rmdir(LOCK_FOLDER_NAME) != 0U ) && ( ENOENT != errno ) );

		if(m_LockAcquired)
		{
			// failed lock release
			++numberOfFail;
			usleep(RETRY_DELAY_IN_MICROSEC);
		}
	}
}

void Ispapi_Implementation::IspFile_lock::cleanLockFolder()
{
	::rmdir(LOCK_FOLDER_NAME);
}
