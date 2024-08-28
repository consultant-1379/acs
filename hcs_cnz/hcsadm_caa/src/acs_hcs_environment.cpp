//======================================================================
//
// NAME
//      Environment.cpp
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-07-22 by XINDOBE
// CHANGES
//     
//======================================================================

#include "acs_hcs_cache.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_environment.h"
#include "acs_hcs_tracer.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_Util.H"
#include <ace/ACE.h>
#include "acs_hcs_progressReport_objectImpl.h"
#include "acs_hcs_progressReportHandler.h"
#include "acs_hcs_jobscheduler.h"
#include "acs_hcs_global_ObjectImpl.h"
#include "acs_hcs_jobbased_objectImpl.h"
//#include <Pdh.h>
//#include <clusapi.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
//#include <atlbase.h>

//#pragma comment(lib,"pdh.lib")

using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	queue<long> Environment::cpuUsageSamples;

	queue<long> Environment::memUsageSamples;

	// Constructor
	Environment::Environment()
	{
	}

	// Destructor
	Environment::~Environment()
	{
	}

	//This function retrives the CPU usage
	long Environment::getCPUUsage()
	{
		DEBUG("%s","Entering Environment::getCPUUsage() ");
		cout<<"Inside getCPUUsage()"<<endl;
		long double a[4]={0},b[4]={0},loadavg=0;
		FILE *fp;
			
	    	fp = fopen("/proc/stat","r");
		if (fp != NULL)
		{	
			fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
			fclose(fp);
		}	
		ACE_OS::sleep(1);
		fp = fopen("/proc/stat","r");
		if(fp != NULL)
		{	
			fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
			fclose(fp);
		}   
		loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
		printf("The current CPU utilization is : %Lf\n",loadavg);
		DEBUG("%s","Leaving  Environment::getCPUUsage()");								    		
		return round(loadavg);
	}
	
	//Gets the count of cpu usage samples in cpuUsageSamples queue
	int Environment::getCpuUsageCount() const
	{
		DEBUG("%s","In Environment::getCpuUsageCount() ");
		return cpuUsageSamples.size();
	}

	//Gets the count of memory usage samples in memUsageSamples queue
	int Environment::getMemUsageCount() const
	{
		DEBUG("%s","In  Environment::getMemUsageCount() ");
		return memUsageSamples.size();
	}
	

	
	//This function calculates the average of cpu load over 30 seconds
	long Environment::getAvrgCPUUsage()
	{	
		DEBUG("%s","Entering Environment::getAvrgCPUUsage() ");
		long totalCpuUsage = 0;
		size_t samplesCount = 6;
		size_t index = 0;
		int pushCpuRslt = 0;

		while(cpuUsageSamples.size() < samplesCount)
		{
			ACE_OS::sleep(5);
		}

		for(index = 0; index < samplesCount; index++ )
		{
			totalCpuUsage += cpuUsageSamples.front();
			pushCpuRslt = pushCpuUsageSample();
			if(pushCpuRslt == 0)
			{
				popCpuUsageSample();
			}		
		}
		DEBUG("%s","LEaving Environment::getAvrgCPUUsage() ");
		return (totalCpuUsage/samplesCount);
	}
	

	
	//This function retreives the memory load
	long Environment::getMemoryLoad()
	{
		DEBUG("%s","Entering Environment::getMemoryLoad() ");
		cout<<"Inside getMemoryLoad() "<<endl;


    		FILE *fp;
    		fp = fopen("/proc/meminfo","r");
		char buf[250];
    		long double memData[4];
		int j=0;
    		if(fp != NULL)
    		{
			do
			{
				string temp=" ";
				string str=" ";
                		fgets(buf, 250, fp);
                		temp = buf;
               			size_t found_first_space = temp.find_first_of(" ");
               			if(found_first_space  != std::string::npos)
               			{
					str = temp.substr(0,found_first_space -1);
					if((str.compare("MemTotal") == 0) || (str.compare("MemFree") == 0) || (str.compare("Buffers") == 0) || (str.compare("Cached") == 0))
               		                 {
                        			temp = temp.substr(found_first_space +1);
                        			size_t found_KB = temp.find_first_of("k");
                        			if(found_KB !=  std::string::npos)
                        			{
                                			temp =  temp.substr(0,found_KB-1);
                                			memData[j] = strtoull(temp.c_str(),NULL,10);
							j++;
       			              		}
					}
				}
			}while(j<=3);
		}
		fclose(fp);
		
		long double  memused = memData[0] - (memData[1] + memData[2]+ memData[3]);
    		long double  memusagePercent = (memused/memData[0])*100;
		long mem = (long)round(memusagePercent);
		printf("\n\nmem = %ld\n\n", mem);
		DEBUG("%s","Leaving Environment::getMemoryLoad() ");
		return mem;						    		
	}

	
	//This function calculates the average of memory load over 30 seconds
	long Environment::getAvrgMemoryLoad()
	{
		DEBUG("%s","Entering  Environment::getAvrgMemoryLoad() ");
		long totalMemUsage = 0;
		size_t index = 0;
		size_t samplesCount = 6;
		int pushMemRslt = 0;
		cout<<"before while"<<endl;
		cout<<"memUsageSamples is "<<memUsageSamples.size()<<endl;
		while(memUsageSamples.size() < samplesCount)
		{
			ACE_OS::sleep(5);
		}
		cout<<"before for"<<endl;
		for(index = 0; index < samplesCount; index++ )
		{
			totalMemUsage += memUsageSamples.front();
			pushMemRslt = pushMemUsageSample();
			if(pushMemRslt == 0)
			{
				popMemUsageSample();
			}
		}
		cout<<"after for"<<endl;
		DEBUG("%s","Leaving  Environment::getAvrgMemoryLoad() ");
		return (totalMemUsage/samplesCount);
	}
		

	
	//This function inserts a new CPU usage sample in the cpu usage queue
	int Environment::pushCpuUsageSample()
	{		
		DEBUG("%s","Entering Environment::pushCpuUsageSample() ");
		long cpuUsage = 0;

		cpuUsage = getCPUUsage();
		if(cpuUsage != -1)
		{
			cpuUsageSamples.push(cpuUsage);
			DEBUG("%s","Leaving Environment::pushCpuUsageSample() ");
			return 0;
		}
		else
		{	
			DEBUG("%s","Leaving Environment::pushCpuUsageSample() ");
			return -1;
		}
	}
	

	//This function inserts a new memory usage sample in the memory usage queue
	int Environment::pushMemUsageSample()
	{
		DEBUG("%s","Entering Environment::pushMemUsageSample() ");
		long memUsage = 0;

		memUsage = getMemoryLoad();
		if(memUsage != -1)
		{
			memUsageSamples.push(memUsage);
			DEBUG("%s","Leaving Environment::pushMemUsageSample() ");
			return 0;
		}
		else
		{
			DEBUG("%s","Leaving Environment::pushMemUsageSample() ");
			return -1;
		}
		DEBUG("%s","Leaving Environment::pushMemUsageSample()");
	}

		
	//This function removes the first CPU usage sample from the cpu usage queue
	void Environment::popCpuUsageSample()
	{		
		DEBUG("%s","Entering Environment::popCpuUsageSample() ");
		cpuUsageSamples.pop();
		DEBUG("%s","Leaving Environment::popCpuUsageSample() ");
	}

	//This function removes the first memory usage sample from the memory usage queue
	void Environment::popMemUsageSample()
	{
		DEBUG("%s","Entering Environment::popMemUsageSample() ");
		memUsageSamples.pop();
		DEBUG("%s","Leaving Environment::popMemUsageSample() ");
	}
	
	
	bool Environment::isBackUpOngoing()
	{
		DEBUG("%s","Entering  Environment::isBackUpOngoing() ");
		vector<ACS_APGCC_ImmAttribute *> attributes;

                ACS_CC_ImmParameter Backup_DN;
		ACS_CC_ImmParameter Backup_state;

	        Backup_DN.attrName = const_cast<char*>("asyncActionProgress");
		Backup_state.attrName = const_cast<char*>("state");

		ACS_CC_ReturnType returnCode;
		OmHandler immHandle;
		std::vector<std::string> pd_dnList;
		string rdn_Backup;

		char *classNamepd = const_cast<char*>("BrmBackupManager");
                Backup_DN.attrName = const_cast<char*>("asyncActionProgress");
                Backup_state.attrName = const_cast<char*>("state");

		returnCode=immHandle.Init();
                if(returnCode!=ACS_CC_SUCCESS)
                {
                        //ERROR("%s","ERROR: init FAILURE!!!");
                }
		returnCode = immHandle.getClassInstances(classNamepd, pd_dnList);
		if(returnCode == ACS_CC_SUCCESS)
		{
			int numDef  = pd_dnList.size();
			if(numDef == 1)
			{
				returnCode = immHandle.getAttribute(pd_dnList[0].c_str(), &Backup_DN);	
				if(returnCode != ACS_CC_SUCCESS)
				{
					immHandle.Finalize();
					DEBUG("%s","Leaving   Environment::isBackUpOngoing()");
					return true;
				}
				else
				{
					rdn_Backup = reinterpret_cast<char*>(Backup_DN.attrValues[0]);	
					returnCode = immHandle.getAttribute( rdn_Backup.c_str(), &Backup_state);
					if(returnCode != ACS_CC_SUCCESS)
					{
						if (*(reinterpret_cast<int*>(Backup_state.attrValues[0])) == 1 || *(reinterpret_cast<int*>(Backup_state.attrValues[0])) == 2)
						{		
							immHandle.Finalize();
							DEBUG("%s","Leaving  Environment::isBackUpOngoing() ");
							return false;
						}
						else
						{
							immHandle.Finalize();
							DEBUG("%s","Leaving  Environment::isBackUpOngoing() ");
							return true;
						}
					}
					else
					{	
						immHandle.Finalize();
						DEBUG("%s","Leaving  Environment::isBackUpOngoing() ");
						return true;
					}
				}
			}
			else
			{
				immHandle.Finalize();
				DEBUG("%s","Leaving  Environment::isBackUpOngoing() ");
				return true;
			}
		}
		else
		{
			immHandle.Finalize();
			DEBUG("%s","Leaving  Environment::isBackUpOngoing() ");
			return true;
		}
	}

	bool Environment::isRestoreOngoing()
	{
		DEBUG("%s"," Entering Environment::isRestoreOngoing()");
		vector<ACS_APGCC_ImmAttribute *> attributes;
		ACS_CC_ReturnType returnCode;
		OmHandler immHandle;
		std::vector<std::string> pd_dnList;
		char *classNamepd = const_cast<char*>("BrmBackup");
                ACS_CC_ImmParameter Backup_DN;
		ACS_CC_ImmParameter Backup_state;
		string rdn_Backup;
		bool restore = false;

	        Backup_DN.attrName = const_cast<char*>("asyncActionProgress");
                Backup_state.attrName = const_cast<char*>("state");

		returnCode=immHandle.Init();	
                if(returnCode!=ACS_CC_SUCCESS)
                {
                        //ERROR("%s","ERROR: init FAILURE!!!");
                }
		returnCode = immHandle.getClassInstances(classNamepd, pd_dnList);
		if(returnCode == ACS_CC_SUCCESS)
		{
			int numDef  = pd_dnList.size();
			for(int i = 0; i < numDef; i++)
			{
				returnCode = immHandle.getAttribute(pd_dnList[i].c_str(), &Backup_DN);
				if(returnCode != ACS_CC_SUCCESS)
				{
					continue;
				}
				rdn_Backup = reinterpret_cast<char*>(Backup_DN.attrValues[0]);
				returnCode = immHandle.getAttribute(rdn_Backup.c_str(), &Backup_state);
                                if(returnCode != ACS_CC_SUCCESS)
                                {
                                        continue;
                                }
				if ( *(reinterpret_cast<int*>(Backup_state.attrValues[0])) == 1 ||  *(reinterpret_cast<int*>(Backup_state.attrValues[0])) == 2)
				{
					restore = true;
				}				
			}
		}
		immHandle.Finalize();
		DEBUG("%s","Leaving Environment::isRestoreOngoing()  ");
		return restore;
	}

	bool Environment::isSwUpdateOngoing()
	{
		DEBUG("%s","Entering Environment::isSwUpdateOngoing ");
		OmHandler immHandle;
		vector<string> tmp_vector;
		ACS_CC_ReturnType result;
		bool swUpdateFlag = false;

		ACS_APGCC_ImmObject object;

		result = immHandle.Init();
		if ( result != ACS_CC_SUCCESS )
		{
			DEBUG("%s","Leaving Environment::isSwUpdateOngoing ");
			return swUpdateFlag;
		}
	
		result = immHandle.getClassInstances("UpgradePackage",tmp_vector);
		if ( result != ACS_CC_SUCCESS )
		{
			immHandle.Finalize();
			DEBUG("%s"," Leaving Environment::isSwUpdateOngoing");
			return swUpdateFlag;
		}

		if(tmp_vector.size() != 0)
		{
			int swUpdateState = SW_UP_UNKNOWN; 
			std::string swUpdateDN;
			std::string swUpReportPrgsDN;
	
			ACS_CC_ImmParameter immSwUpdateState;
			immSwUpdateState.attrName = const_cast<char*>("state");

			ACS_CC_ImmParameter immSwUpdateReportProgress;
			immSwUpdateReportProgress.attrName = const_cast<char*>("reportProgress");
			swUpdateDN = tmp_vector[0].c_str();
			result = immHandle.getAttribute(swUpdateDN.c_str(), &immSwUpdateState);
			if ( result == ACS_CC_SUCCESS )
			{
				swUpdateState =  *(reinterpret_cast<int*>(immSwUpdateState.attrValues[0]));
			}
			else
			{
			}

			switch(swUpdateState)
			{
				case SW_UP_UNKNOWN:
				case SW_UP_INITIALIZED:
				case SW_UP_PREPARE_IN_PROGRESS:
				case SW_UP_PREPARE_COMPLETED:
				case SW_UP_WAITING_FOR_COMMIT:
				case SW_UP_COMMIT_COMPLETED:
					swUpdateFlag = false;
					break;
				default:
					swUpdateFlag = true;
					break;
			}

		}
		immHandle.Finalize();
		DEBUG("%s","Leaving  Environment::isSwUpdateOngoing");
		return swUpdateFlag;

	}

	// Checks if all the resources are up and running
	bool Environment::isActiveNode()
	{
		DEBUG("%s","Entering Environment::isActiveNode() ");
                vector<ACS_APGCC_ImmAttribute *> attributes;
		const string nodeRunLevelAAttr = "nodeRunLevelA";
		const string nodeRunLevelBAttr = "nodeRunLevelB";
		int* runLevelA;
		int* runLevelB;
	        ACS_APGCC_ImmAttribute attribute_1;
	        ACS_APGCC_ImmAttribute attribute_2;
                ACS_CC_ReturnType returnCode;
                OmHandler immHandle;

                std::vector<std::string> pd_dnList;
                char *classNamepd = const_cast<char*>("ProcessControl");
                returnCode=immHandle.Init();
                if(returnCode!=ACS_CC_SUCCESS)
                {
                        //ERROR("%s","ERROR: init FAILURE!!!");
                }
                returnCode = immHandle.getClassInstances(classNamepd, pd_dnList);
                if(returnCode == ACS_CC_SUCCESS)
                {
                        int numDef  = pd_dnList.size();
                        if(numDef == 1)
                        {
				string node_id_path = "/etc/cluster/nodes/this/id";
				string nameObject;
				ifstream ifs(node_id_path.c_str());
                                char node_id[8] = {0};

				if ( ifs.good())
				{
					ifs.getline(node_id, 8);
				}
				ifs.close();
				
				nameObject = "apNodeInfoId=" + (string)node_id + "," + pd_dnList[0];
				attribute_1.attrName = const_cast<char*>("nodeRunLevelA");
				attribute_2.attrName = const_cast<char*>("nodeRunLevelB");

				attributes.push_back(&attribute_1);
			        attributes.push_back(&attribute_2);
			
				returnCode = immHandle.getAttribute(nameObject.c_str(), attributes);
                                if(returnCode != ACS_CC_SUCCESS)
                                {
					immHandle.Finalize();
					DEBUG("%s","Leaving Environment::isActiveNode() ");
                                        return false;
                                }
                                else
                                {
					runLevelA = reinterpret_cast<int*>(*(attribute_1.attrValues));
					runLevelB = reinterpret_cast<int*>(*(attribute_2.attrValues));
					if (*runLevelA == 5 && *runLevelB == 5)
					{
						immHandle.Finalize();
						DEBUG("%s","Leaving Environment::isActiveNode() ");
						return true;
					}
					else
					{
						immHandle.Finalize();
						DEBUG("%s","Leaving Environment::isActiveNode() ");
						return false;
					}
                                }
                        }
                        else
                        {
				immHandle.Finalize();
				DEBUG("%s","Leaving Environment::isActiveNode() ");
                                return false;
                        }
                }
                else
                {
			immHandle.Finalize();
			DEBUG("%s","Leaving Environment::isActiveNode() ");
                        return false;
                }
		DEBUG("%s","Leaving Environment::isActiveNode() ");
	}


	
	//This function checks if environment conditions to execute 'hcjdef' command are as expected.
        void Environment::assertEnvSetHcjdef(std::string executeArg)
        {
                DEBUG("%s","Entering Environment::assertEnvSetHcjdef(const string& executeArg) ");
                Configuration::Singleton::get()->read(); // Re-read the configuration file if needed.
#ifdef ACS_HCS_SIMULATION_USE
                return;
#endif

                //Commenting these part, later we ll need this as we ll consider SAMPLES
                //unsigned long memLoad = (unsigned long) getAvrgMemoryLoad();
                //unsigned long cpuUsage = (unsigned long) getAvrgCPUUsage();

                unsigned long memLoad = getMemoryLoad();
                unsigned long cpuUsage = getCPUUsage();

                cout<<"Maximun Memory usage is "<<(Configuration::Singleton::get()->getMemUsageMax())<<endl;
		cout<<"Maximun CPU usage is "<<(Configuration::Singleton::get()->getCpuUsageMax())<<endl;
         //       cout<<"Maximun CPU usage is "<<(Configuriation::Singleton::get()->getCpuUsageMax())<<endl;
                cout<<"executeArg in assertEnvSetHcjdef is: "<<executeArg<<endl;

                //Update the progress report incase of high memory load and  high CPU usage
                unsigned int j,index = 0;
                if( (memLoad > (Configuration::Singleton::get()->getMemUsageMax())) || (cpuUsage > (Configuration::Singleton::get()->getCpuUsageMax())) )
                {
                        for(j = 0; j < acs_hcs_progressReport_ObjectImpl::progressReport.size(); j++)
                        {
                        	if(executeArg == acs_hcs_progressReport_ObjectImpl::progressReport[j].DN)
                                {
                                	break;
                                }
                        }
                        index = j;
                        acs_hcs_progressReport_ObjectImpl::progressReport[index].state = acs_hcs_progressReport_ObjectImpl::CANCELLED;
                        acs_hcs_progressReport_ObjectImpl::progressReport[index].result = acs_hcs_progressReport_ObjectImpl::FAILURE;
                        {
                                time_t rawTime = time(0);
                                struct tm formattedTime;
                                char currentTime[20];
                                formattedTime = *localtime(&rawTime);
                                sprintf(currentTime, "%d-%d-%dT%d:%d:%d", formattedTime.tm_year+1900, formattedTime.tm_mon+1, formattedTime.tm_mday,formattedTime.tm_hour, formattedTime.tm_min, formattedTime.tm_sec);
                                acs_hcs_progressReport_ObjectImpl::progressReport[index].timeActionStarted = currentTime;
                                acs_hcs_progressReport_ObjectImpl::progressReport[index].timeOfLastStatusUpdate = currentTime;
                                acs_hcs_progressReport_ObjectImpl::progressReport[index].timeActionCompleted = currentTime;
                        }
                }


                if(memLoad > (Configuration::Singleton::get()->getMemUsageMax()))
                {
                        //ACS_HCS_TRACE("assertEnvSetHcjdef(): Memory usage is too high to execute a job: '" << memLoad << "'.");
                        acs_hcs_progressReport_ObjectImpl::progressReport[index].resultInfo = "Job Execution failed due to high Memory Laod";
                        ACS_HCS_THROW(Environment::ExceptionLimitMemory, "assertEnvSetHcjdef()", "Memory usage is too high to execute a job: '" << memLoad << "'.", "");
                }

                if(cpuUsage > (Configuration::Singleton::get()->getCpuUsageMax()))
                {
                        //ACS_HCS_TRACE("assertEnvSetHcjdef(): CPU usage is too high to execute a job: '" << cpuUsage << "'.");
                        acs_hcs_progressReport_ObjectImpl::progressReport[index].resultInfo = "Job Execution failed due to high CPU Usage";
                        ACS_HCS_THROW(Environment::ExceptionLimitCpu, "assertEnvSetHcjdef()", "CPU usage is too high to execute a job: '" << cpuUsage << "'.", "");
                }
                DEBUG("%s","Leaving  Environment::assertEnvSetHcjdef(const string& executeArg)");
        }


	//This function checks if environment conditions to execute HCS commands other than hcjdef are as expected.
	void Environment::assertEnvSetHCSCmd()
	{
	DEBUG("%s","Entering Environment::assertEnvSetHCSCmd() ");
#ifdef ACS_HCS_SIMULATION_USE
		return;
#endif
		if (!isActiveNode())
			ACS_HCS_THROW(Environment::ExceptionNodePassive, "assertEnvSetHCSCmd()", "This is a passive node.", "");

		if (isBackUpOngoing() || isRestoreOngoing() || isSwUpdateOngoing())
			ACS_HCS_THROW(Environment::ExceptionNodeReboot, "assertEnvSetHCSCmd()", "This node might go for a reboot.", "");
	DEBUG("%s","Leaving Environment::assertEnvSetHCSCmd() ");
	}
	

	bool Environment::getMode(bool& expertMode) const
	{
		DEBUG("%s","Entering Environment::getMode(bool& expertMode) ");
		const string clusterOpModeAttr = "clusterOpMode";
		char opMode[16] = {0};
		ACS_CC_ImmParameter paramToFind;
	        ACS_CC_ReturnType returnCode;
                OmHandler immHandle;
                std::vector<std::string> pd_dnList;
                char *classNamepd = const_cast<char*>("CpCluster");

                returnCode=immHandle.Init();

                if(returnCode!=ACS_CC_SUCCESS)
                {
                        //ERROR("%s","ERROR: init FAILURE!!!");
                }
                returnCode = immHandle.getClassInstances(classNamepd, pd_dnList);
		if(returnCode == ACS_CC_SUCCESS)
		{
			int numDef  = pd_dnList.size();	
			if(numDef == 1)
			{
				vector<string>::iterator it = pd_dnList.begin();
				string DN = (*it);
				/* Parse the above string here if needed to narrow down the DN to CpCluster=1*/
				strcpy(paramToFind.attrName, clusterOpModeAttr.c_str());
				returnCode = immHandle.getAttribute(DN.c_str(), &paramToFind);
				if(returnCode == ACS_CC_FAILURE)
				{
					DEBUG("%s","Leaving Environment::getMode(bool& expertMode) ");
					return false;
				}
				else
				{
					strcpy(opMode, reinterpret_cast<char*>(*(paramToFind.attrValues)));
					if(!strcmp(opMode, "EXPERT"))
					{
						expertMode = true;
					}
				}
			}
			else
			{	
				DEBUG("%s","Leaving Environment::getMode(bool& expertMode) ");
				return false;
			}
		}
		else
		{
			DEBUG("%s","Leaving Environment::getMode(bool& expertMode) ");
			return false;
		}
		DEBUG("%s","Leaving Environment::getMode(bool& expertMode) ");
		return true;
	}

	//This function returns the APT type of the node.
        bool Environment::getApt(APType& apt) const
        {
		DEBUG("%s","Entering Environment::getApt(APType& apt) ");
		bool flgMSC = false;
    bool flgHLR = false;
    bool flgBC = false;
		bool flgWLN = false;
		bool flgIPSTP = false; // Introduced to handle to new aptType called IPSTP
		//const string systemTypeAttr = "systemType";
		const string axeApplicationAttr = "axeApplication";
		int* systemType;
		int* APT;
		ACS_APGCC_ImmAttribute attr1, attr2;
	        ACS_CC_ReturnType returnCode;
	        OmHandler immHandle;
	        std::vector<std::string> pd_dnList;
        	char *classNamepd = const_cast<char*>(ACS_AXEFUNCTIONS_CLASS_NAME);
		
	        returnCode=immHandle.Init();

	        if(returnCode!=ACS_CC_SUCCESS)
        	{
                	//ERROR("%s","ERROR: init FAILURE!!!");
	        }
		returnCode = immHandle.getClassInstances(classNamepd, pd_dnList);
		if(returnCode == ACS_CC_SUCCESS)
		{
			int numDef  = pd_dnList.size();
			if(numDef == 1)
			{
				vector<string>::iterator it = pd_dnList.begin();
				string DN = (*it);
				/* Parse the above string here if needed to narrow down the DN to AxeFunctions=1*/
				std::vector<ACS_APGCC_ImmAttribute *> attributes;
				attr1.attrName = systemTypeAttr;
				attr2.attrName = axeApplicationAttr;
				attributes.push_back(&attr1);	 
				attributes.push_back(&attr2);			
				cout << "DN: " << DN << endl;
				returnCode = immHandle.getAttribute(DN.c_str(), attributes);
				if(returnCode == ACS_CC_FAILURE)
				{
					DEBUG("%s"," Leaving Environment::getApt(APType& apt");
					cout << "getAttribute failed" << immHandle.getInternalLastErrorText() << endl;
					return false;
				}
				else
				{
					cout << "getAttribute success" << endl;
					systemType = reinterpret_cast<int*>(*(attr1.attrValues));
					APT = reinterpret_cast<int*>(*(attr2.attrValues));
					cout << "systemType: " << *systemType << endl;
					cout << "APT: " << *APT << endl;	
					
					if(*systemType == 1)
					{
						flgBC = true;
					}
					if(*APT == 0)
					{
						flgMSC = true;
					}
					if(*APT == 1)
					{
						flgHLR = true;
					}
					if(*APT == 3)
					{
					 	flgWLN = true;
				  }
					if(*APT == 5)
					{
						flgIPSTP = true;
					}

					if(flgMSC == true && flgBC == true)
					{
									apt = MSCBC;
					}
					else if(flgMSC == true && flgBC == false)
					{
									apt = MSCS;
					}
					else if(flgHLR == true && flgBC == true)
					{
									apt = HLRBC;
					}
					else if(flgHLR == true && flgBC == false)
					{
									apt = HLR;
					}
					else if(flgWLN == true && flgBC == false)
          {
                  apt = WLN;
          }
					else if(flgIPSTP == true && flgBC == false)
					{
									apt = IPSTPS;
					}
//					else if(flgIPSTP == true && flgBC == true)
//					{
//									apt = IPSTPBC;
//					}
					else
					{
									apt = UnknownType;
					}
				}
			}
			else
			{	
				DEBUG("%s","Leaving Environment::getApt(APType& apt ");
				return false;
			}
		}
		else
		{
			DEBUG("%s","Leaving Environment::getApt(APType& apt ");
			return false;
		}
		DEBUG("%s","Leaving Environment::getApt(APType& apt ");
		return true;
        }

}
