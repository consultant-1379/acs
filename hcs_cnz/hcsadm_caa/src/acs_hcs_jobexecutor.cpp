//========================================================================================
// COPYRIGHT (C) Ericsson AB 2011 - All rights reserved
//
// The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written permission from Ericsson
// AB or in accordance with the terms and conditions stipulated in the agreement/contract
// under which the program(s) have been supplied.
//========================================================================================

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
//#include <strstream>
#include <fstream>
#include <string>

#include <ace/ACE.h>
//#include <zip>
#include <ace/Synch.h>

#include "acs_hcs_cache.h"
#include "acs_hcs_commandclient.h"
#include "acs_hcs_configuration.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_exception.h"
#include "acs_hcs_environment.h"
#include "acs_hcs_formattertxt.h"
#include "acs_hcs_printoutparser.h"
#include "acs_hcs_reportgenerator.h"
#include "acs_hcs_rulechecker.h"
#include "acs_hcs_synchron.h"
#include "acs_hcs_jobexecutor.h"
#include "acs_hcs_timeutils.h"
#include "acs_hcs_tracer.h"
#include "acs_hcs_transferqueue.h"
#include "acs_hcs_global_ObjectImpl.h"
#include <syslog.h>
#include <ace/Signal.h>
#include "acs_hcs_exitcodes.h"
#include "acs_hcs_jobbased_objectImpl.h"
#include "acs_hcs_global_ObjectImpl.h"
#include "acs_hcs_jobscheduler.h"
#include "acs_hcs_healthcheckservice.h"

#include "acs_hcs_progressReport_objectImpl.h"
#include "acs_hcs_progressReportHandler.h"
#include "acs_hcs_directory.h"


using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//================================================================================
	// Friends
	//================================================================================

	ostream& operator<<(ostream& os, const JobExecutor::Job& job)
	{
		os        << job.name.size()         << " " << job.name;
		os << " " << job.ruleSetIds.size()   << " " << job.ruleSetIds;
		os << " " << job.ruleIds.size()      << " " << job.ruleIds;
		os << " " << job.ruleCats.size()     << " " << job.ruleCats;
		os << " " << job.cpName.size()       << " " << job.cpName;
		os << " " << job.timeStamp.size()    << " " << job.timeStamp;
		os << " " << job.iterations.size()   << " " << job.iterations;
		os << " " << job.transferQueue.size()   << " " << job.transferQueue;
		os << " " << sizeof job.expertUser   << " " << job.expertUser;
		stringstream s;
		s << job.iterExecuted;
		string iterExecuted = s.str();
		os << " " << iterExecuted.size() << " " << iterExecuted;
		os << endl;

		return os;
	}

	istream& operator>>(istream& is, JobExecutor::Job& job)
	{
		char buf[512];

		if (is.good())
		{
			is.getline(buf, 512);

			string line = buf;

			if (line.empty())
				return is;

			string str;
			size_t pos = 0;

			job.setName      (JobExecutor::Job::readNextItem(str, pos, line));
			job.setRuleSetIds(JobExecutor::Job::readNextItem(str, pos, line));
			job.setRuleIds   (JobExecutor::Job::readNextItem(str, pos, line));
			job.setRuleCats  (JobExecutor::Job::readNextItem(str, pos, line));
			job.setCpName    (JobExecutor::Job::readNextItem(str, pos, line));
			job.setTimeStamp (JobExecutor::Job::readNextItem(str, pos, line));
			job.setIterations(JobExecutor::Job::readNextItem(str, pos, line));
			job.setTransferQueue(JobExecutor::Job::readNextItem(str, pos, line));
			job.setExpertUser(JobExecutor::Job::readNextItem(str, pos, line) == "1");
			job.iterExecuted = ACE_OS::atoi(JobExecutor::Job::readNextItem(str, pos, line).c_str());
		}

		return is;
	}

	//================================================================================
	// Class JobExecutor
	//================================================================================

	JobExecutor::JobExecutor() : busy(false), cancelled(false), nRulesTotal(0), nRulesCompleted(0)
	{
	}

	JobExecutor::~JobExecutor()
	{
	}

	void JobExecutor::cancel(const char* reason)
	{
		(void)reason;
		this->currentJob.invalidate();
		this->cancelled = true;
	}

	bool JobExecutor::hasBeenCancelled() const
	{
		return this->cancelled == true;
	}

	const JobExecutor::Job& JobExecutor::getCurrentJob() const
	{
		return this->currentJob;
	}

        const int& JobExecutor::Job::getSuccessLogs() const
	{
		return(this->successLogs);
	}

	string JobExecutor::getJobId(const string& jobName, string dateTime)
	{
		DEBUG("%s","Entering JobExecutor::getJobId");

		size_t pos = 0;

		while ((pos = dateTime.find_first_of("-:", pos)) != string::npos)
			dateTime.erase(pos++, 1);

		
		pos = dateTime.find_first_of(" ");
		if(pos != string::npos)
			dateTime[pos] = 'T';

		while ((pos = dateTime.find_first_of(" ", pos)) != string::npos)
			dateTime.erase(pos++, 1);

		stringstream s;
		s << jobName << "_" << dateTime;
		DEBUG("%s","Leaving JobExecutor::getJobId");
		return s.str();
	}

	void JobExecutor::initContext(const string& jobId, RuleFilter::Context& context, string time_Stamp)
	{
		cout << "Init context" << endl;
		//string ruleSetFile = Configuration::Singleton::get()->getRulesDirName() + "/initial_rules/rules_msc_context.xml";
		DEBUG("%s","Entering JobExecutor::initContext");
		string ruleSetFile = Configuration::Singleton::get()->getRulesInitialDirName() + "/rules_msc_context.xml";
		string initalRulePath=Configuration::Singleton::get()->getRulesInitialDirName();
		bool initialRuleExist = false;
		string initialFile;
		Directory dirReportJob(initalRulePath);
		Directory::FileNames fileNames;
		dirReportJob.ls(fileNames,"rules_msc_context.xml");

		for(Directory::FileNames::const_iterator fileName = fileNames.begin(); fileName != fileNames.end(); ++fileName)
		{
			initialFile = (string)fileName->c_str();
			cout<<"filename is: "<<initialFile<<endl;
			if(strcmp(ruleSetFile.c_str(),initialFile.c_str()) == 0  )
			{
				initialRuleExist = true;
			}
		}
			
		if(initialRuleExist == false)
		{		

				 cout << "rule set file not present in correct path exception" << endl;
	                         ACS_HCS_THROW(JobExecutor::ExceptionInitialRuleFile, "initContext()", "rule set file not present in correct path exception.", "Rule set '" << ruleSetFile << "' not present.");
		}


			
		DocumentXml doc(ruleSetFile);
		DocumentXml::Nodes rules;
		DocumentXml::Nodes::Iterator rule;
		doc.getNodesList(rules,"/ruleset/rule", NULL );
//		doc.getNodes(rules, "/ruleset/rule");
		RuleChecker ruleChecker;
		cout << "Before for" << endl;
		try 
		{
			for (rule = rules.begin(); rule != rules.end(); ++rule)
			{
			//const string& ruleSetId = doc.getNode("/ruleset").getAttribute("id").getValue();
				string ruleSetId = doc.getRulesetIdValue();
				const string& ruleId = (*rule).getAttribute("id").getValue();
				cout << "ruleId: " << ruleId << endl;
				CmdResultFiles cmdResultFiles (jobId, ruleSetId, ruleId,time_Stamp);
				ruleChecker.setCmdResultFiles(cmdResultFiles.getFileNames());
				DocumentXml::Node result = doc.getSpecifiedNode("./check",(*rule),false);
				if (!ruleChecker.isPassed(result, context.cpName))
				{
					cout << "Init context exception" << endl;
					ACS_HCS_THROW(JobExecutor::ExceptionJobExecution, "initContext()", "Could not initialize execution context.", "Rule set '" << ruleSetFile << "' not passed.");
				}
			}
		}
		catch(const PrintoutParser::ExceptionParseError& ex)
                {
			ACS_HCS_THROW(JobExecutor::ExceptionJobExecution, "initContext()", "Could not initialize execution context.",  "Rule set '" << ruleSetFile << "' not passed.");
                }
		const DocumentPrintout::Filter& resSwLevel = ruleChecker.getResultFilter("mml-pcorp-ci");
		set<string> cis;
		resSwLevel.getValues(cis, "CI");
		

		/*
		  find the AC-A/CP-A MISSRA mark in the string set.
		  This is according to the follwowing documents 
		  "Software Level Mark in Block MISSRA" from MSC PLM
		  "BSC use of the SW level mark in block MISSRA" from BSC PLM
		*/

		//ACS_HCS_TRACE("initContext(): find the mark in a set of " << cis.size() << "elements");
		set<string>::iterator mark_it = cis.begin();
		bool found = false;

		while( mark_it != cis.end() && !found )
		{
			//ACS_HCS_TRACE("initContext(): mark = " << *mark_it);
			if( ((*mark_it)[0] == 'H') && (isdigit( (*mark_it)[1]) )  )
			{
				//ACS_HCS_TRACE("initContext(): context.nodeType = HLR");
				context.swLevel = (*mark_it).substr(4, 2) + "." + (*mark_it).substr(6, 1);
				context.nodeType = "HLR";
				found = true;
			}
			else
			if( ((*mark_it)[0] == 'M') && (isdigit( (*mark_it)[1]) )  )
			{
				//ACS_HCS_TRACE("initContext(): context.nodeType = MSC-S");
				context.swLevel = (*mark_it).substr(4, 2) + "." + (*mark_it).substr(6, 1);
				context.nodeType = "MSC-S";
				found = true;
			} 
			else 
			if( ((*mark_it)[0] == 'B') && (isdigit( (*mark_it)[1]) )  )
			{
				//ACS_HCS_TRACE("initContext(): context.nodeType = BSC");
				context.swLevel = (*mark_it).substr(4, 2) + "." + (*mark_it).substr(6, 1);
				context.nodeType = "BSC";
				found = true;
			}
			else
                        if( ((*mark_it)[0] == 'W') && (isdigit( (*mark_it)[1]) )  )
                        {
                                //ACS_HCS_TRACE("initContext(): context.nodeType = WLN");
                                context.swLevel = (*mark_it).substr(4, 2) + "." + (*mark_it).substr(6, 1);
                                context.nodeType = "WLN";
                                found = true;
                        }

			mark_it++;

		}

		if(!found)
		{
			//ACS_HCS_TRACE("initContext(): mark not found, exception: Could not initialize execution context.");
			//if a significant mark is not found throws an exception since Node Type is not possible to determine
			syslog(LOG_INFO, "Init context failed");	
			ACS_HCS_THROW(JobExecutor::ExceptionJobExecution, "initContext()", "Could not initialize execution context.", "Could not determine node type.");
		}
//		context.swLevel = (*cis.begin()).substr(4, 2) + "." + (*cis.begin()).substr(6, 1);

		const DocumentPrintout::Filter& resApzType    = ruleChecker.getResultFilter("mml-saosp-apz-type");
		const DocumentPrintout::Filter& resApzVersion = ruleChecker.getResultFilter("mml-saosp-apz-version");
		set<string> apzTypes;
		set<string> apzVersions;
		resApzType.getValues(apzTypes, "VALUE");
		resApzVersion.getValues(apzVersions, "VALUE");
		stringstream s;
		s << (*apzTypes.begin()) << setfill('0') << setw(2) << (*apzVersions.begin()); // APZ-version should have 2 digits.
		context.apzType = s.str();

		Environment env;
		Environment::APT apt;
		bool expertMode;
		env.getMode(expertMode);
		env.getApt(apt);

		switch (apt)
		{
		case Environment::/*APT::*/MSCS :
			context.nodeType = "MSC-S";
			break;

		case Environment::/*APT::*/MSCBC :
			context.nodeType = "MSC-SBC";
			context.opMode   = (expertMode ? "expert" : "normal");
			break;

		case Environment::/*APT::*/HLR :
			context.nodeType = "HLR";
			break;

		case Environment::/*APT::*/HLRBC :
			context.nodeType = "HLR-BC";
			context.opMode   = (expertMode ? "expert" : "normal");
			break;

		case Environment::/*APT::*/WLN :
			context.nodeType = "WLN";
			break;

		case Environment::IPSTPS :
			context.nodeType = "IPSTPS";
			break;

	//	case Environment::IPSTPBC :
	//		context.nodeType = "IPSTPBC";
  //		context.opMode   = (expertMode ? "expert" : "normal");
	//		break;

		default:
                { } 
		}
	}

	bool JobExecutor::setupDirectories()
	{
		bool res = true;
		DEBUG("%s","Entering JobExecutor::setupDirectories()");
		Directory dirData(Configuration::Singleton::get()->getDataDir());
		dirData.create();

		Cache::Singleton::get()->create();
		try
		{
			Cache::Singleton::get()->clear();
			Directory dirReports(Configuration::Singleton::get()->getReportsDirName());
			dirReports.create();
		}
		catch (const Directory::ExceptionIo& ex)
		{
			res = false;
		}
		catch (...)
		{
			res = false;
		}
		DEBUG("%s","Leaving JobExecutor::setupDirectories()");
		return res;
	}

        bool JobExecutor::setupDirectories(string jobName)
        {
		bool res = true;
                DEBUG("%s","Entering JobExecutor::setupDirectories(jobname)");
		cout<<"Entering JobExecutor::setupDirectories(jobname)\n";
		try
		{
			Directory dirReportJobname(Configuration::Singleton::get()->getReportJobName(jobName));
			dirReportJobname.create();
			Directory dirReportTmp(Configuration::Singleton::get()->getReportsTmpDirName(jobName));
	                dirReportTmp.create();
        	        dirReportTmp.clear(true);
		}
		catch(const Directory::ExceptionIo& ex)
		{
			res = false;
		}

		DEBUG("%s","Leaving JobExecutor::setupDirectories(jobname)");
		cout<<"Leaving JobExecutor::setupDirectories(jobname)\n";
		return res;
	}


	void JobExecutor::start(const Job& job, HealthCheckCmd* const cmd, string dn)
	{
		DEBUG("%s","Entering JobExecutor::start()");
		/* Structure that holds the execution status of the ruleId along with the recommended action if it fails */

		ruleStat res;

		cout << "Job execution start" << endl;
		Synchron::Guard<ACE_Thread_Mutex> guard(this->mutexStart, false);

		if (!guard.tryAcquire())
		{
			//jobScheduler::jobExecution = false;			
			DEBUG("%s","Inside guard.tryAcquire()");
			syslog(LOG_INFO, "Inside guard.tryAcquire");
			
			AcsHcs::HealthCheckService::sameJob = this->getCurrentJob().getName();

			ACS_HCS_THROW(JobExecutor::ExceptionJobInProgress, "start()", "Cannot execute job '" << job.getName() << "'.", "Job '" << this->getCurrentJob().getName() << "' is in progress.");
		}

		int successLogStatus = job.getSuccessLogs();
		
		this->currentJob = job;
		this->cancelled  = false;

		Configuration::Singleton::get()->read(); // Re-read the configuration file if needed.
		if(!JobExecutor::setupDirectories())
		{
			HealthCheckService::diskFull = true;
			return;
		}
		
		if(!JobExecutor::setupDirectories(job.getName()))
		{
			HealthCheckService::diskFull = true;
			return;
		}

		string dateTimeStart = TimeUtils::timeToStr(/*ACE_OS::*/time(0), "%Y-%m-%d %H:%M:%S");
		string jobId         = JobExecutor::getJobId(job.getName(), dateTimeStart);
		string jobId1=job.getName();
		string reportDN =dn;
		string time_Stamp =  TimeUtils::timeToStr(/*ACE_OS::*/time(0), "%Y%m%dT%H%M%S");
		cout<<"jod id is: "<<jobId<<endl;
		 cout<<"jod id is: "<<jobId1<<endl;
		Reporter reporter(jobId1,time_Stamp,cmd);
		string jobName = "jobId=" +job.getName()+",HealthCheckhealthCheckMId=1";

		struct Cleanup
		{
			 string filePath;
			 //FILE *fp1 = NULL;
			 string  cmdString,cmdString1,cmdString2;
			 unsigned int j,index;
			Cleanup(JobExecutor& jobExecutor, Reporter& reporter, const string& jobId1, const string& transferQueue, const int compression, const string& jobName, string time_Stamp,  const string& jobId, string reportDN)
				: jobExecutor(jobExecutor), reporter(reporter), jobId1(jobId1), transferQueue(transferQueue), compression(compression), jobName(jobName), time_Stamp(time_Stamp), jobId(jobId), reportDN(reportDN)
			{			
				index=0; j=0;	
			}

			~Cleanup()
			{
				this->jobExecutor.currentJob.invalidate();
				this->reporter.close();
				acs_hcs_global_ObjectImpl obj;
				string manualRepName=jobId;
				string zipname=jobId1+"_"+time_Stamp;
				char* value = const_cast<char*>(manualRepName.c_str());
	                        void* value1[1]={reinterpret_cast<void*>(value)};
        	                obj.updateImmAttribute(reportDN,LASTREPORTNAME,ATTR_STRINGT,value1);
				Directory dirReportTmp(Configuration::Singleton::get()->getReportsTmpDirName(jobId1));
				if(!dirReportTmp.exists())
				{
					cout<<".tmp doesn't exists"<<endl;
					acs_hcs_global_ObjectImpl obj;
                                        index = obj.getProgressReportIndex(jobName);
					HealthCheckService::folderDeleted = true;
					 // TR HS37486.. checking whether progress report exists or not before updating it
					if(AcsHcs::HealthCheckService::progressDeleted == false)
					{
                                        	//acs_hcs_progressReport_ObjectImpl::progressReport[index].state = 4;
	                                 	//acs_hcs_progressReport_ObjectImpl::progressReport[index].result = 2i;	
						acs_hcs_progressReport_ObjectImpl::progressReport[index].state = acs_hcs_progressReport_ObjectImpl::CANCELLED;
						acs_hcs_progressReport_ObjectImpl::progressReport[index].result = acs_hcs_progressReport_ObjectImpl::FAILURE;
						acs_hcs_progressReport_ObjectImpl::progressReport[index].resultInfo +="";
					}
				}
				else
				{
					dirReportTmp.rename(dirReportTmp.getName(), Configuration::Singleton::get()->getReportJobName(jobId1) + "/" + this->time_Stamp);				
					if( this->compression == 0)
					{
						cout<<"Compression is enabled"<<endl;
						DEBUG("%s","Compression is enabled.");
						//string filePath = Configuration::Singleton::get()->getReportsDirName() + "/" + this->jobId;		 
						string filePath = Configuration::Singleton::get()->getReportJobName(jobId1)+ "/" +this->time_Stamp;
						cout<<"filepath for compression is: "<<filePath<<endl;
						FILE *fp1 = NULL;
						//cmdString = "zip -r -j " + filePath + " " + filePath+ " " +"2>/dev/null";
						string compressionResult = Configuration::Singleton::get()->getReportJobName(jobId1) + "/" + zipname;
						cout<<"compressionResult: "<<compressionResult<<endl;
						cmdString = "zip -r -j " + compressionResult + " " + filePath;
						cout<<"cmdString is: "<<cmdString<<endl;
						fp1  = popen (cmdString.c_str(), "r");
						if(!fp1)
						{
							perror("open failed");
							//exit(errno);
						}

						const int BUFSIZE = 1000;
						char buf[ BUFSIZE ];
						while( fgets( buf, BUFSIZE,  fp1 ) != NULL) {
							fprintf( stdout, "%s", buf  );
						}
						if(pclose(fp1)!=0)
						{
							acs_hcs_global_ObjectImpl obj;
		                                	index = obj.getProgressReportIndex(jobName);
							 // TR HS37486.. checking whether progress report exists or not before updating it
							if(AcsHcs::HealthCheckService::progressDeleted == false)
								acs_hcs_progressReport_ObjectImpl::progressReport[index].additionalInfo +="Compression Failed;\n"; 
						}

						cout<<"After Zipping..."<<endl;
	                                	cmdString1 = "rm " + filePath+ "/*" + " " +"2>/dev/null";
                                        	cout<<"cmdString1 is: "<<cmdString1<<endl;
					 	system(cmdString1.c_str());
						cout<<"Deleted the source directory"<<endl;
						cmdString2 = "mv " +compressionResult+".zip"+ " " + filePath+" 2>/dev/null";	
						cout<<"cmdString2 is: "<<cmdString2<<endl;
						system(cmdString2.c_str());
						cout<<"Moved the compressed directory"<<endl;
	
					}	

					if (transferQueue != "")
					{
						DEBUG("%s","The report files and the printout files shall be sent out to the remote destination using the Destination URI");	
						TransferQueue tq;
						if( this->compression == 0 )
						{
						//tq.sendFilesToTq(jobId, transferQueue, jobName, this->compression);	
							tq.sendFilesToTq(jobId1, transferQueue, jobName, time_Stamp, this->compression);
						}
						else
						{
							//tq.sendFilesToTq(jobId, transferQueue, jobName);
							//tq.sendFilesToTq(jobId1, transferQueue, jobName, jobId);
							tq.sendFilesToTq(jobId1, transferQueue, jobName,time_Stamp);
					
						}
					}
				}	

			}
				
			JobExecutor&  jobExecutor;
			Reporter&     reporter;
			//const string& jobId;
			const string& jobId1;
			const string  transferQueue;
			const int compression;
			const string& jobName;
			string time_Stamp;
			const string& jobId;
			string reportDN;
		} cleanup(*this, reporter, jobId1, job.getTransferQueue(),job.getCompression(), jobName, time_Stamp, jobId, reportDN);
		try
		{
			reporter.open(job.getName(), dateTimeStart);
			cout << "Getting CP name" << endl;
			RuleFilter::Context context(job.getCpName());
			cout << "Got CP name" << endl;
			//JobExecutor::initContext(jobId, context);
			JobExecutor::initContext(jobId1, context,time_Stamp);
			cout << "Initial rules passed" << endl;
			reporter.addAttributes(context.apzType, context.nodeType, context.swLevel, context.opMode);
			RuleFilter ruleFilter(job);
			RuleFilter::MappedItems        filteredRuleIds;
			RuleFilter::MappedItemsNoMatch itemsNoMatch;
			DEBUG("%s","before calling ruleFilter.filter");
			cout<<"before calling ruleFilter.filter"<<endl;
			this->nRulesTotal = ruleFilter.filter(filteredRuleIds, itemsNoMatch, context);
			cout<<"after calling ruleFilter.filter"<<endl;
			DEBUG("%s","After calling ruleFilter.filter");
			for (RuleFilter::MappedItemsNoMatch::const_iterator it = itemsNoMatch.begin(); it != itemsNoMatch.end(); ++it)
			{
				if(jobScheduler::stopRequested) //Call for graceful termination is received
                                {
                                          DEBUG("%s","statement 1");
                                          return;
                                }
				if ((*it).second.ruleIds.empty() && (*it).second.ruleCats.empty())
					continue;

				reporter.openRuleSetNoMatch((*it).first);

				for (RuleFilter::Items::const_iterator itSet = (*it).second.ruleIds.begin(); itSet != (*it).second.ruleIds.end(); ++itSet)
				{
					 if(jobScheduler::stopRequested)//Call for graceful termination is received
                                         {
						reporter.closeRuleSetNoMatch();
                                                DEBUG("%s","statement 2");
                                                return; 
                                         }

					reporter.addRuleNoMatch(*itSet);
				}

				for (RuleFilter::Items::const_iterator itSet = (*it).second.ruleCats.begin(); itSet != (*it).second.ruleCats.end(); ++itSet)
				{
					 if(jobScheduler::stopRequested)//Call for graceful termination is received
                                         {
						 reporter.closeRuleSetNoMatch();
                                                 DEBUG("%s","statement 3");
                                                return; 
                                         }

					reporter.addCategoryNoMatch(*itSet);
				}

				reporter.closeRuleSetNoMatch();
			}

			for (RuleFilter::MappedItems::const_iterator it = filteredRuleIds.begin(); it != filteredRuleIds.end(); ++it)
			{
				 if(jobScheduler::stopRequested)//Call for graceful termination is received
                                 {
                                      DEBUG("%s","statement 4");
                                      return; 
                                 }

				 DEBUG("%s","Check point 4 in for");
				if (this->hasBeenCancelled())
					ACS_HCS_THROW(JobExecutor::ExceptionJobExecution, "start()", "Cannot continue execution of job '" << job.getName() << "'.", "Job has been cancelled by the user.");

#if 0
				if (HealthCheckCmd::stopEventFlag)
					ACS_HCS_THROW(JobExecutor::ExceptionJobExecution, "start():", "HC Engine Stopped. Cannot continue execution of job '" << job.getName() << "'.", "HC Engine is Stopped.");
#endif

				const RuleFilter::Items& ruleIds = (*it).second;

				if (ruleIds.size() == 0)
					continue;

				const string& ruleSetFile = (*it).first;

				//ACS_HCS_TRACE("start(): Processing rule-set file '" << ruleSetFile << "'.");

				DocumentXml doc(ruleSetFile);
				DocumentXml::Nodes rules;
				DocumentXml::Nodes::Iterator rule;

				const string& ruleSetId = doc.getRulesetIdValue();
				reporter.openRuleSet(ruleSetId);

				doc.getNodesList(rules, "/ruleset/rule", NULL);

				// map to hold the execution status of rules
				map <string,ruleStat> rsts;

				// obtain the successLogStatus of the current job	
				//int successLogStatus = job.getSuccessLogs();
				cout << "SUCCESS logs: " << successLogStatus << endl;

				for (rule = rules.begin(); rule != rules.end(); ++rule)
				{
					 if(jobScheduler::stopRequested)//Call for graceful termination is received
                                         {
						reporter.closeRuleSet();
                                                DEBUG("%s","statement 5");
                                                return;  //      throw (int)10;
                                         }

					if (this->hasBeenCancelled())
						ACS_HCS_THROW(JobExecutor::ExceptionJobExecution, "start():", "Cannot continue execution of job '" << job.getName() << "'.", "Job has been cancelled by the user.");

#if 0
					if (HealthCheckCmd::stopEventFlag)
						ACS_HCS_THROW(JobExecutor::ExceptionJobExecution, "start():", "HC Engine Stopped. Cannot continue execution of job '" << job.getName() << "'.", "HC Engine is Stopped.");

#endif
					const string& ruleId = (*rule).getAttribute("id").getValue();
					cout << "Rule id: " << ruleId << endl;
					if (ruleIds.find(ruleId) == ruleIds.end())
						continue;

					const string& ruleName           = (*rule).getAttribute("name").getValue();
					const string& ruleSlogan = (*rule).getSpecifiedNode("./slogan",(*rule),false).getValue();
					const string& ruleRecommendation = (*rule).getSpecifiedNode("./recommendation", (*rule),false).getValue();

					res.status=ACS_HC_NOTOK;
					res.recommendedAction = ruleRecommendation;

					//ACS_HCS_TRACE("start(): Processing rule '" << ruleId << "(" << ruleName << ")'.");

					try
					{
						CmdResultFiles cmdResultFiles (jobId1, ruleSetId, ruleId,time_Stamp);
						RuleChecker ruleChecker(cmdResultFiles.getFileNames());
						cout << "Executing rule: " << ruleName << endl;
						//syslog(LOG_SYSLOG, "Executing rule : %s", ruleName);	
						const std::string cpName = job.getCpName();
						cout << "cpName: " << cpName << endl;
						if (ruleChecker.isPassed((*rule).getSpecifiedNode("./check", (*rule), false), cpName))
						{
							cout << "Rule passed " << ruleId << endl;
							reporter.addRule(ruleId, ruleName, ruleSlogan, "PASSED", "-");
							res.status = ACS_HC_OK;
							if(successLogStatus == ACS_HC_DISABLE)
							{
								cout << "Clearing success logs" << endl;
								cmdResultFiles.getFileNames().clear();
							}
							else
							{
								cout << "Success logs included" << endl;	
							}

							/*if (!Configuration::Singleton::get()->isDebugEnabled())
							{
								// This rule has been passed, no need for copying command result files.
								cmdResultFiles.getFileNames().clear();
							}*/
						}
						else
						{
							cout << "Rule failed " << ruleId << endl;
							reporter.addRule(ruleId, ruleName, ruleSlogan, "FAILED", ruleRecommendation);
						}
					}
					catch(const PrintoutParser::ExceptionParseError& ex)
					{
						cout << "Error 1" << endl;
						reporter.addRule(ruleId, ruleName, ruleSlogan, "ERROR", "No corresponding printout for the executed rule");
						syslog(LOG_INFO, "Exception caught 1 %s", ex.what());
						DEBUG("%s","Exception caught 1");
					}
					catch (const Cache::ExceptionIo& ex)
					{
						cout<<"Error creating file"<<endl;
						DEBUG("%s","In singleton catch");
						reporter.addRule(ruleId, ruleName, ruleSlogan, "ERROR", "Error creating file under Cache folder");
						ACS_HCS_THROW(JobExecutor::ExceptionDirectoryFileCreation, "start()", "Cannot continue execution of job as files are not getting created  '" << job.getName() << "'.", "Job has been cancelled as files are not getting created.");
							
						
					}
					catch (const AcsHcs::Exception& ex)
					{
						cout << "Error 2" << endl;
						cout << "Rule Error " << ruleId << endl;
						reporter.addRule(ruleId, ruleName, ruleSlogan, "ERROR", "Error executing rule");
						cout<<" ex.what() is:" <<ex.what()<<endl;
						syslog(LOG_INFO, "Exception caught 2 %s", ex.what());
						DEBUG("%s","Exception caught 2");

						//ACS_HCS_TRACE("start(): Caught exception of type '" << typeid(ex).name() << "'. Cause: " << ex.info());
					}
					catch (const exception& ex)
					{
						cout << "Error 3" << endl;
						cout << "Rule Error " << ruleId <<endl;
						reporter.addRule(ruleId, ruleName, ruleSlogan, "ERROR", "Error executing rule");
						syslog(LOG_INFO, "Exception caught 3 %s", ex.what());
						DEBUG("%s","Exception caught 3");
						//ACS_HCS_TRACE("start(): Caught exception of type '" << typeid(ex).name() << "'. Cause: " << ex.what());
					}
					catch (...)
					{
						cout << "Error 4" << endl;
						reporter.addRule(ruleId, ruleName, ruleSlogan, "ERROR", "Unknown error.");
						syslog(LOG_INFO, "Exception caught 4");
						DEBUG("%s","Exception caught 4");
						//ACS_HCS_TRACE("start(): Caught exception of unknown type.");
					}

					this->nRulesCompleted++;
					rsts[ruleId] = res; 
				}
				acs_hcs_global_ObjectImpl obj;
				updateRuleStatus(job.getName(),rsts);
				bool Waiting = obj.waitForSomeTime(10);//sleeping for 10 seconds, exit in case if thread is closed
                                if(Waiting == false && jobScheduler::stopRequested)
				{
					reporter.closeRuleSet();
                                        return;
				}
				//ACE_OS::sleep(5);
				reporter.closeRuleSet();
			}
		}
		catch (const JobExecutor::ExceptionJobExecution& ex)
		{
			//ACS_HCS_TRACE(ex.info());
			reporter.addInterruption("Initial Job Execution Context Failed");
			throw;
		}
		 catch (const JobExecutor::ExceptionInitialRuleFile& ex)
                {
                        //ACS_HCS_TRACE(ex.info());
			reporter.addInterruption("Initial Rule File not found");
                        throw;
                }
		catch (const JobExecutor::ExceptionDirectoryFileCreation& ex)
		{
			DEBUG("%s","Exception caught in singleton class");
			reporter.addInterruption("Error creating file");
			throw;
		}
		catch (const exception& ex)
		{
			reporter.addInterruption(ex.what());
			throw;
		}			 
		catch (...)
		{
			//ACS_HCS_TRACE("start(): Caught unknown exception.");
			reporter.addInterruption("Unknown error.");
			throw;
		}
	}	

	/*string JobExecutor::removeTrailingSpaces(string job_categories)
	{
		size_t spacePos;
		for( ; spacePos != std::string::npos ; )
		{
			spacePos = job_categories.find_first_of(" ");
			if(spacePos != std::string::npos)
			{
				job_categories.erase(spacePos,1);
			}
			else
				break;
		}
		return job_categories;
	}*/

	
	void JobExecutor::updateRuleStatus(string jobName, map<string,ruleStat> &ruleStatus)
	{
		DEBUG("%s","JobExecutor::updateRuleStatus entering");
		// Variable used to track HealthCheckM, its pass only when all the categories under it are passed
		int healthCheckMStatus = ACS_HC_OK; 
		int returnCode;
		OmHandler immHandler;
		acs_hcs_global_ObjectImpl obj;
		
		returnCode=immHandler.Init();
		if(returnCode!=ACS_CC_SUCCESS)
		{
			cout<<"ERROR: init FAILURE!!!\n";
			return;
		}

		string jobRdn;
		
		// RDN Name of Job Ex: jobId=1,HealthCheckMid=1
		jobRdn = "jobId=" + jobName + "," + "HealthCheckhealthCheckMId=1"; // TODO
			
		ACS_CC_ImmParameter catAttr;
		catAttr.attrName = (char*)categoriesAttr;
		returnCode = immHandler.getAttribute(jobRdn.c_str(),&catAttr);
		if(returnCode != ACS_CC_SUCCESS)
		{
			cout << "getAttribute failed: " << immHandler.getInternalLastError() << endl;
			return;
		}
		char categoryList[256];
		strncpy(categoryList,(reinterpret_cast<char *>(catAttr.attrValues[0])),256);
		string job_categories(categoryList);
		job_categories = obj.removeTrailingSpaces(job_categories);

		// Check if the categories is "ALL", if yes, consider all the categories.
		set<string> catList;
		if(job_categories == "ALL")
		{
			std::vector<std::string> catVector;
			returnCode = immHandler.getClassInstances("HealthCheckCategory", catVector);
			if(returnCode != ACS_CC_SUCCESS)
			{
				cout << "getClassInstances failed: " << immHandler.getInternalLastError() << endl;
				return;
			}
			for(unsigned int i = 0; i < catVector.size(); i++)
			{
				if(jobScheduler::stopRequested)//Call for graceful termination is received
                                {
                                          return;
                                }
				string categoryName; 
				size_t found_equal, found_comma;

				found_equal = 0;
				found_comma = 0;

				found_equal = catVector[i].find_first_of("=");

				if(found_equal != std::string::npos)
				{
					found_comma = catVector[i].find_first_of(",", found_equal+1);
					if(found_comma != std::string::npos)
					{
						categoryName = catVector[i].substr(found_equal+1, found_comma  - (found_equal+1));
					}
				}
				catList.insert(categoryName);
			}
		}
		else
		{
			char *temp = strtok(const_cast<char *>(job_categories.c_str()),",");
			while(temp != NULL)
			{
				catList.insert(temp);
				temp = strtok(NULL,",");
			}
			catList.insert("MANDATORY");  //status of "mandatory" category needs to check for every execution
		}
		std::string category;	
		for(set<string>::iterator it = catList.begin(); it != catList.end(); ++it)
		{
			if(jobScheduler::stopRequested)//Call for graceful termination is received
                        {
                              return;
                        }
			std::vector<std::string>ruleVector; 
			category  = "categoryId=" + (*it) + ",HealthCheckhealthCheckMId=1";
			returnCode = immHandler.getChildren(category.c_str(), ACS_APGCC_SUBTREE, &ruleVector);
			if(returnCode != ACS_CC_SUCCESS)
			{
				cout << "getChildren failed: " << immHandler.getInternalLastError() << endl;
				return;
			}

			map<string, ruleStat>::iterator rulePos;
			int categoryStatus = ACS_HC_OK; 
			for(unsigned int ind = 0; ind < ruleVector.size(); ind++)
			{
				if(jobScheduler::stopRequested)//Call for graceful termination is received
                                {
                                          return;
                                }
				//std::size_t found; 
				int result = ACS_HC_NOTOK;
				for(rulePos = ruleStatus.begin(); rulePos != ruleStatus.end(); ++rulePos)
				{
					//if(found!=std::string::npos)
					size_t pos_eql = ruleVector[ind].find_first_of(EQUALTO_STR);
					size_t pos_com = ruleVector[ind].find_first_of(COMMA_STR);
					string s1 = ruleVector[ind].substr(pos_eql+1,(pos_com-pos_eql-1));
					string s2=rulePos->first;
					if(s1.compare(s2) == 0 )
					{
						string rdn = ruleVector[ind];
						cout<<"rdn in update is: "<<rdn<<endl;
						if(!rulePos->second.status)
						{
							cout<<"ok"<<endl;
							 DEBUG("%s","ok");
							result = ACS_HC_OK;
						}
						else 
						{
							cout<<"not ok"<<endl;
							DEBUG("%s","not ok");
							healthCheckMStatus = categoryStatus = result = ACS_HC_NOTOK; 
						}


						// Updating the execution status
						void *value[1]={reinterpret_cast<void*>(&result)};
						obj.updateImmAttribute(rdn,"result",ATTR_INT32T,value);

						// Updating recommended Action
						string recAction = rulePos->second.recommendedAction;
						char* rcmdAction = const_cast<char*>(recAction.c_str());
						void* value1[1]={ reinterpret_cast<void*>(rcmdAction)};
						obj.updateImmAttribute(rdn,"recommendedAction",ATTR_STRINGT,value1);

						break;
					}
				}
			}

			string categoryRDN; 
			size_t found_equal, found_comma;

			found_equal = 0;
			found_comma = 0;

			found_equal = ruleVector[0].find_first_of("=");

			if(found_equal != std::string::npos)
			{
				found_comma = ruleVector[0].find_first_of(",", found_equal+1);
				if(found_comma != std::string::npos)
				{
					categoryRDN = ruleVector[0].substr(found_comma+1, ruleVector[0].size() - (found_comma+1));
				}
			}

			// Update the category status in Imm
			void *value2[1]={reinterpret_cast<void*>(&categoryStatus)};
			obj.updateImmAttribute(categoryRDN,"status",ATTR_INT32T,value2);
			
			// Updating last executed job name 
			cout<<"JobRdn->"<<jobRdn<<endl;
			string lastJobName = jobRdn;
			char* lstJobName = const_cast<char*>(lastJobName.c_str());
			void* value3[1]={ reinterpret_cast<void*>(lstJobName)};
			obj.updateImmAttribute(categoryRDN,"lastExecutedJob",ATTR_NAMET,value3);

			//Updating the lastUpdateTime of the category
			string crntTime = obj.getCurrentTime();
			char* cTime = const_cast<char*>(crntTime.c_str());
			void* value4[1]={ reinterpret_cast<void*>(cTime)};
			obj.updateImmAttribute(categoryRDN,"lastUpdateTime",ATTR_STRINGT,value4);		

		}

		// Update HealthCheckM Status in Imm
		void *value5[1]={reinterpret_cast<void*>(&healthCheckMStatus)};
		obj.updateImmAttribute("HealthCheckhealthCheckMId=1","status",ATTR_INT32T,value5);

		// Update Last updated time in Imm
		string currentTime = obj.getCurrentTime();
                char* crntTime = const_cast<char*>(currentTime.c_str());
                void* value6[1]={ reinterpret_cast<void*>(crntTime)};
                obj.updateImmAttribute("HealthCheckhealthCheckMId=1","lastUpdateTime",ATTR_STRINGT,value6);		

		
		returnCode=immHandler.Finalize();
		if(returnCode!=ACS_CC_SUCCESS)
		{
			cout<<"ERROR: init FAILURE!!!\n";
			return;
		}
		DEBUG("%s","JobExecutor::updateRuleStatus exiting");
	}

	string JobExecutor::toRuleSetFileName(const std::string& ruleSetId)
	{
		return Configuration::Singleton::get()->getRulesDirName() + "/" + ruleSetId + ".xml";
	}

	//================================================================================
	// Class JobExecutor::CmdResultFiles
	//================================================================================

	JobExecutor::CmdResultFiles::CmdResultFiles(const string& jobId, const string& ruleSetId, const string& ruleId, string timestamp)
		: dirTo(Configuration::Singleton::get()->getReportsTmpDirName(jobId))
		, fileNameTo(jobId + "__" + timestamp + "_"+ ruleSetId + "__" + ruleId.substr(0, ruleId.find_first_of(".")))
	{
	}

	JobExecutor::CmdResultFiles::~CmdResultFiles()
	{
		int i = 0;

		for (Directory::FileNames::const_iterator fileName = this->fileNames.begin(); fileName != this->fileNames.end(); ++fileName)
		{
			try
			{
				stringstream s;
				s << this->fileNameTo << "__" << setfill('0') << setw(2) << (++i) << ".txt";
				dirTo.copyFile(*fileName, s.str());
			}
			catch (const Directory::ExceptionIo& ex)
			{
				//ACS_HCS_TRACE("~CmdResultFiles(): Could not copy command result. Cause: " << ex.info());
			}
		}
	}

	Directory::FileNames& JobExecutor::CmdResultFiles::getFileNames()
	{
		return this->fileNames;
	}

	//================================================================================
	// Class JobExecutor::Job
	//================================================================================

	JobExecutor::Job::Job()
		: expertUser    (false)
		, nIterationsMax(1)
		, startTime     (0)
		, timeSpan      (/*Period::*/PD_INVALID)
		, iterExecuted  (-1)
		, iterPending   (0)
	{
	}

	JobExecutor::Job::Job(const string& name)
		: name          (name)
		, expertUser    (false)
		, nIterationsMax(1)
		, startTime     (0)
		, timeSpan      (/*Period::*/PD_INVALID)
		, iterExecuted  (-1)
		, iterPending   (0)
	{
	}

	JobExecutor::Job::Job(const Job& job)
		: name          (job.name)
		, ruleSetIds    (job.ruleSetIds)
		, ruleIds       (job.ruleIds)
		, ruleCats      (job.ruleCats)
		, cpName        (job.cpName)
		, timeStamp     (job.timeStamp)
		, iterations    (job.iterations)
		, expertUser    (job.expertUser)
		, nIterationsMax(job.nIterationsMax)
		, startTime     (job.startTime)
		, timeSpan      (job.timeSpan)
		, iterExecuted  (job.iterExecuted)
		, iterPending   (job.iterPending)
	{
	}

	JobExecutor::Job::~Job()
	{
	}

	JobExecutor::Job& JobExecutor::Job::operator=(const Job& job)
	{
		if (this == &job)
			return *this;

		this->name           = job.name;
		this->ruleSetIds     = job.ruleSetIds;
		this->ruleIds        = job.ruleIds;
		this->ruleCats       = job.ruleCats;
		this->cpName         = job.cpName;
		this->timeStamp      = job.timeStamp;
		this->iterations     = job.iterations;
		this->expertUser     = job.expertUser;
		this->nIterationsMax = job.nIterationsMax;
		this->startTime      = job.startTime;
		this->timeSpan       = job.timeSpan;
		this->iterExecuted   = job.iterExecuted;
		this->iterPending    = job.iterPending;
		this->transferQueue  = job.transferQueue;
		this->compression    = job.compression;

		return *this;
	}

	bool JobExecutor::Job::operator==(const Job& job)
	{
		if (this == &job)
			return true;

		return
			this->name       == job.name       &&
			this->ruleSetIds == job.ruleSetIds &&
			this->ruleIds    == job.ruleIds    &&
			this->ruleCats   == job.ruleCats   &&
			this->cpName     == job.cpName     &&
			this->timeStamp  == job.timeStamp  &&
			this->iterations == job.iterations &&
			this->expertUser == job.expertUser;
	}

	bool JobExecutor::Job::operator!=(const Job& job)
	{
		return !(*this == job);
	}

	void JobExecutor::Job::confirm() const
	{
		this->iterExecuted = this->iterPending;
	}

	const string& JobExecutor::Job::getName() const
	{
		return this->name;
	}

	const string& JobExecutor::Job::getRuleSetIds() const
	{
		return this->ruleSetIds;
	}

	const string& JobExecutor::Job::getRuleIds() const
	{
		return this->ruleIds;
	}

	const string& JobExecutor::Job::getRuleCats() const
	{
		return this->ruleCats;
	}

	const string& JobExecutor::Job::getCpName() const
	{
		cout << "Returning : " << this->cpName << endl;
		return this->cpName; 
	}

	const string& JobExecutor::Job::getTransferQueue() const
	{
		return this->transferQueue; 
	}
	
	const int& JobExecutor::Job::getCompression() const
	{
		return this->compression;
	}
	time_t JobExecutor::Job::getNextStartTime(time_t ttl) const
	{
		time_t now = /*ACE_OS::*/time(0);

		if (now < this->startTime)
			return this->startTime;

		if (this->timeSpan == /*Period::*/PD_INVALID)
		{
			// One shot Job (i.e. no iteration was provided).
			// Do not consider the Job if older than the time-to-live passed in ttl.

			return (now - this->startTime > ttl ? 0L : this->startTime);
		}

		time_t nextStartTime;
		int iterCurrent = (now - this->startTime) / this->timeSpan; // [0 - (nIterationsMax - 1)]

		if (iterCurrent < this->iterExecuted)
			return 0L; // This is actually indicating a fault-situation, because it means that time is moving backwards ...

		time_t iterPending;

		if (iterCurrent == this->iterExecuted)
		{
			iterPending = iterCurrent + 1;
		}
		else
		{
			nextStartTime = this->startTime + iterCurrent * this->timeSpan;

			if (now - nextStartTime > ttl)
				iterPending = iterCurrent + 1;
			else
				iterPending = iterCurrent;
		}

		nextStartTime = this->startTime + iterPending * this->timeSpan;

		if (ttl)
			this->iterPending = iterPending;

		return nextStartTime > this->getLastStartTime() ? 0 : nextStartTime;
	}

	time_t JobExecutor::Job::getLastStartTime() const
	{
		return this->startTime + (nIterationsMax - 1) * this->timeSpan;
	}

	JobExecutor::Job::Period JobExecutor::Job::getPeriod() const
	{
		return this->timeSpan;
	}

	bool JobExecutor::Job::isExpertUser() const
	{
		return this->expertUser;
	}

	void JobExecutor::Job::invalidate()
	{
		this->name = "";
	}

	bool JobExecutor::Job::isValid() const
	{
		return !this->name.empty();
	}

	string& JobExecutor::Job::readNextItem(string& item, size_t& posStart, const string& line)
	{
		size_t len;
		size_t posEnd;

		posStart = line.find_first_not_of(" ", posStart);
		posEnd   = line.find_first_of(" ", posStart);

		string lenStr = line.substr(posStart, posEnd - posStart);
		posStart += lenStr.size();

		len = atoi(lenStr.c_str());

		if (len == 0)
		{
			item = "";

			return item;
		}

		posStart = line.find_first_not_of(" ", posEnd);
		item = line.substr(posStart, len);
		posStart += len;

		return item;
	}

	void JobExecutor::Job::setName(const string& jobName)
	{
		this->name = jobName;
	}

	void JobExecutor::Job::setRuleSetIds(const string& ruleSetIds)
	{
		this->ruleSetIds = ruleSetIds;
	}

	void JobExecutor::Job::setRuleIds(const string& ruleIds)
	{
		this->ruleIds = ruleIds;
	}

	void JobExecutor::Job::setRuleCats(const string& ruleCats)
	{
		this->ruleCats = ruleCats;
	}

	void JobExecutor::Job::setCpName(const string& cpName)
	{
		this->cpName = cpName;
	}

	void JobExecutor::Job::setTransferQueue(string transferQueue)
	{
		this->transferQueue = transferQueue;
	}

        void JobExecutor::Job::setCompression(int compression)
	{
		this->compression = compression;
	}
        
        void JobExecutor::Job::setSuccessLogs(int successLogs)
	{
		this->successLogs = successLogs;
	}

        void JobExecutor::Job::setjobToTrigger(std::string jobToTrigger)
	{
		this->jobToTrigger = jobToTrigger;
	}

	void JobExecutor::Job::setTimeStamp(const string& timeStamp)
	{
		if (timeStamp.empty())
		{
			this->timeStamp = TimeUtils::timeToStr(/*ACE_OS::*/time(0));
		}
		else
		{
			// Store normalised timeStamp (YYYYmmddHHMMSS).
			// Format of input timeStamp: YYYYMMDD-HHMM

			if ( timeStamp.substr(8,1) == "-" )
			{
				stringstream s;
				s << timeStamp.substr(0, 4);
				s << timeStamp.substr(4, 2);
				s << timeStamp.substr(6, 2);
				s << timeStamp.substr(9, 4);
				s << "00";

				this->timeStamp = s.str();
			}
			else
			{
				this->timeStamp = timeStamp;
			}
		}

		this->startTime = TimeUtils::mkTime(this->timeStamp);
	}

	void JobExecutor::Job::setIterations(const string& iterations)
	{
		this->iterExecuted = -1;
		this->iterations   = iterations;

		if (iterations.empty())
		{
			this->timeSpan = /*Period::*/PD_INVALID;
			this->nIterationsMax = 1;

			return;
		}

		switch (iterations[0])
		{
		case 'H' :
			this->timeSpan = /*Period::*/PD_HOURLY;
			break;

		case 'D' :
			this->timeSpan = /*Period::*/PD_DAILY;
			break;

		case 'W' :
			this->timeSpan = /*Period::*/PD_WEEKLY;
			break;

		case 'B' :
			this->timeSpan = /*Period::*/PD_BIWEEKLY;
			break;

		default:
			this->timeSpan = /*Period::*/PD_INVALID;
			break;
		}

		string iters = iterations.substr(1);

		if (iters.empty())
			this->nIterationsMax = 1;
		else
			this->nIterationsMax = /*ACE_OS::*/atoi(iters.c_str());
	}

	void JobExecutor::Job::setExpertUser(bool expertUser)
	{
		this->expertUser = expertUser;
	}

	stringstream& JobExecutor::Job::toString(stringstream& ss, unsigned int level) const
	{
		(void)level;
		ss << "\""  << this->name           << "\"";
		ss << " \"" << this->ruleSetIds     << "\"";
		ss << " \"" << this->ruleIds        << "\"";
		ss << " \"" << this->ruleCats       << "\"";
		ss << " \"" << this->cpName         << "\"";
		ss << " \"" << this->timeStamp      << "\"";
		ss << " \"" << this->nIterationsMax << "\"";

		return ss;
	}

	//================================================================================
	// Class JobExecutor::Reporter
	//================================================================================

	const FormatterTxt::Table::Column JobExecutor::Reporter::cols[] = 
	{
		FormatterTxt::Table::Column("Rule Set"             , 25, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
			FormatterTxt::Table::Column("Rule"                 ,  4, FormatterTxt::Table::Column::/*Align::*/RIGHT, " "                     , FormatterTxt::Table::S_R),
			FormatterTxt::Table::Column(""                     , 16, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
			FormatterTxt::Table::Column("Result"               , 12, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
			FormatterTxt::Table::Column("Reason/Recommendation", 23, FormatterTxt::Table::Column::/*Align::*/LEFT , "\n"                    , "\n"),
			FormatterTxt::Table::Column()
	};

	const FormatterTxt::Table::Column JobExecutor::Reporter::colsNoMatch[] = 
	{
		FormatterTxt::Table::Column("Rule Set"  , 25, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
                FormatterTxt::Table::Column("Rule IDs"  , 21, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
               FormatterTxt::Table::Column("Categories", 38, FormatterTxt::Table::Column::/*Align::*/LEFT, "\n" , "\n"),
               FormatterTxt::Table::Column()
	};

	JobExecutor::Reporter::Reporter(const string& fileName,string time_Stamp, HealthCheckCmd* const responder)
		: reportGenerator(fileName,time_Stamp)
		, responder(responder)
		, table(JobExecutor::Reporter::cols)
		, tableNoMatch(JobExecutor::Reporter::colsNoMatch)
		, tableInitialized(false)
	{
	}

	JobExecutor::Reporter::~Reporter()
	{
	}

	void JobExecutor::Reporter::open(const std::string& jobName, const std::string& date)
	{
		this->reportGenerator.open(jobName, date);
	}

	void JobExecutor::Reporter::close()
	{
		if (this->responder)
		{
			if (!this->tableInitialized)
				this->responder->sendStatus("Check results:\nNo rule has been executed.\n");

			bool tableNoMatchInitialized = false;

			for (RuleFilter::MappedItems::const_iterator itMap = this->ruleIdsNoMatch.begin(); itMap != this->ruleIdsNoMatch.end(); ++itMap)
			{
				stringstream sIds;

				for (RuleFilter::Items::const_iterator itSet = (*itMap).second.begin(); itSet != (*itMap).second.end(); ++itSet)
				{
					if (itSet != (*itMap).second.begin())
						sIds << ", ";

					sIds << *itSet;
				}

				string ids = sIds.str();

				if (!ids.empty())
				{
					stringstream s;

					if (!tableNoMatchInitialized)
					{
						s << endl;
						s << "Rule IDs and categories for which no matching rule has been found:" << endl;
						s << endl;
						this->tableNoMatch.printHeader(s);
						tableNoMatchInitialized = true;
					}

					vector<string> colTexts;

					colTexts.push_back((*itMap).first);
					colTexts.push_back(ids);
					colTexts.push_back("");

					tableNoMatch.printLine(s, colTexts);
					this->responder->sendStatus(s.str());
				}
			}

			for (RuleFilter::MappedItems::const_iterator itMap = this->ruleCatsNoMatch.begin(); itMap != this->ruleCatsNoMatch.end(); ++itMap)
			{
				stringstream sIds;

				for (RuleFilter::Items::const_iterator itSet = (*itMap).second.begin(); itSet != (*itMap).second.end(); ++itSet)
				{
					if (itSet != (*itMap).second.begin())
						sIds << ", ";

					sIds << *itSet;
				}

				string ids = sIds.str();

				if (!ids.empty())
				{
					stringstream s;

					if (!tableNoMatchInitialized)
					{
						s << endl;
						s << "Rule IDs and categories for which no matching rule has been found:" << endl;
						s << endl;
						this->tableNoMatch.printHeader(s);
						tableNoMatchInitialized = true;
					}

					vector<string> colTexts;

					colTexts.push_back((*itMap).first);
					colTexts.push_back("");
					colTexts.push_back(ids);

					tableNoMatch.printLine(s, colTexts);
					this->responder->sendStatus(s.str());
				}
			}
		}

		this->reportGenerator.close();
	}

	void JobExecutor::Reporter::openRuleSet(const std::string& id)
	{
		if (this->responder)
		{
			this->ruleSetId = id;
		}

		this->reportGenerator.openRuleSet(id);
	}

	void JobExecutor::Reporter::closeRuleSet()
	{
		this->reportGenerator.closeRuleSet();
	}


	void JobExecutor::Reporter::openRuleSetNoMatch(const std::string& id)
	{
		if (this->responder)
		{
			this->ruleSetId = id;
		}

		this->reportGenerator.openRuleSetNoMatch(id);
	}

	void JobExecutor::Reporter::closeRuleSetNoMatch()
	{
		this->reportGenerator.closeRuleSetNoMatch();
	}

	void JobExecutor::Reporter::addAttributes(const std::string& apzType, const std::string& nodeType, const std::string& swLevel, const string& opMode)
	{
		this->reportGenerator.addAttributes(apzType, nodeType, swLevel, opMode);
	}

	void JobExecutor::Reporter::addRule(const std::string& id, const std::string& name, const std::string& slogan, const std::string& result, const std::string& recommendation)
	{
		if (this->responder)
		{
			stringstream s;

			if (!this->tableInitialized)
			{
				s << endl;
				s << "Check results:" << endl;
				s << endl;
				this->table.printHeader(s);
				this->tableInitialized = true;
			}

			vector<string> colTxts;

			colTxts.push_back(this->ruleSetId);
			colTxts.push_back(id.substr(0, id.find_first_of(".")));
			colTxts.push_back(name);
			colTxts.push_back(result);
			colTxts.push_back(recommendation);

			this->table.printLine(s, colTxts);
			this->responder->sendStatus(s.str());
		}

		string encoded;
		this->reportGenerator.addRule(id, name, slogan, result, JobExecutor::Reporter::encodeXml(encoded, recommendation));
	}

	std::string& JobExecutor::Reporter::encodeXml(std::string& out, const std::string& in)
	{
		set<string> xmlEntities;
		xmlEntities.insert("&amp;");
		xmlEntities.insert("&apos;");
		xmlEntities.insert("&gt;");
		xmlEntities.insert("&lt;");
		xmlEntities.insert("&quot;");

		size_t i;

		for (i = 0; i < in.size(); i++)
		{
			const char c = in[i];

			switch (c)
			{
			case '&':
				{
					size_t posEnd = in.find_first_of(";", i);
					if (posEnd == string::npos)
					{
						out.append("&amp;");
					}
					else
					{
						string str = in.substr(i, posEnd - i + 1);

						if (xmlEntities.find(str) == xmlEntities.end())
						{
							out.append("&amp;");
						}
						else
						{
							out.append(str);
							i += str.size();
						}
					}
				}
				break;

			case '"':
				out.append("&quot;");
				break;

			case '\'':
				out.append("&apos;");
				break;

			case '<':
				out.append("&lt;");
				break;

			case '>':
				out.append("&gt;");
				break;

			default:
				out.append(1, c);
			}
		}

		return out;
	}

	void JobExecutor::Reporter::addRuleNoMatch(const std::string& id)
	{
		if (this->responder)
			this->ruleIdsNoMatch[this->ruleSetId].insert(id);

		this->reportGenerator.addRuleNoMatch(id);
	}

	void JobExecutor::Reporter::addCategoryNoMatch(const std::string& id)
	{
		if (this->responder)
			this->ruleCatsNoMatch[this->ruleSetId].insert(id);

		this->reportGenerator.addCategoryNoMatch(id);
	}

	void JobExecutor::Reporter::addInterruption(const std::string& reason)
	{
		if (this->responder)
		{
			stringstream s;
			s << endl << reason << endl;
			this->responder->sendStatus(s.str());
		}

		this->reportGenerator.addInterruption(reason);
	}

	//================================================================================
	// Class JobExecutor::RuleFilter
	//================================================================================

	JobExecutor::RuleFilter::RuleFilter(const Job& job) : job(job)
	{
	}

	JobExecutor::RuleFilter::~RuleFilter()
	{
	}

	bool JobExecutor::RuleFilter::expandRuleCats(Items& expandedRuleCats) const
	{
		string ruleCats = this->job.getRuleCats();

		if (ruleCats.empty())
			return false;

		transform(ruleCats.begin(), ruleCats.end(), ruleCats.begin(), ::toupper);
		JobExecutor::RuleFilter::splitCommaSepList(expandedRuleCats, ruleCats);

		return true;
	
	}
	bool JobExecutor::RuleFilter::expandRuleIds(Items& expandedRuleIds) const
	{
		string ruleIds = this->job.getRuleIds();

		if (ruleIds.empty())
			return false;

		Items expandedRuleIdsWithRanges;
		JobExecutor::RuleFilter::splitCommaSepList(expandedRuleIdsWithRanges, ruleIds);
		Items::iterator ruleId;

		for (ruleId = expandedRuleIdsWithRanges.begin(); ruleId != expandedRuleIdsWithRanges.end(); ++ruleId)
		{
			// Format of ruleIds: "1,2..4,5".
			// Shall be expanded to "1,2,3,4,5".

			size_t pos = (*ruleId).find("..");

			if (pos != string::npos)
			{
				unsigned long lower = ACE_OS::strtoul((*ruleId).substr(0, pos).c_str() , 0, 10);
				unsigned long upper = ACE_OS::strtoul((*ruleId).substr(pos + 2).c_str(), 0, 10);

				if (lower > upper)
				{
					unsigned long buf = lower;
					lower = upper;
					upper = buf;
				}

				for (unsigned long i = lower; i <= upper; i++)
				{
					stringstream s;
					s << i;
					expandedRuleIds.insert(s.str());
				}
			}
			else
			{
				expandedRuleIds.insert(*ruleId);
			}
		}

		return true;
	}

	bool JobExecutor::RuleFilter::expandRuleSets(Directory::FileNames& expandedRuleSets) const
	{
		string ruleSetIds = this->job.getRuleSetIds();

		if (ruleSetIds.empty())
			return false;

		//if (ruleSetIds == "ALL")
		if (ruleSetIds == ALL_CATEGORIES)
		{
			Directory dir(/*Configuration::Singleton::get()->getRulesDirName()*/"/cluster/storage/system/config/acs_hc");
			dir.ls(expandedRuleSets);
		}
		else
		{
			Items expandedRuleSetIds;
			JobExecutor::RuleFilter::splitCommaSepList(expandedRuleSetIds, ruleSetIds);

			Items::const_iterator ruleSetId;

			for (ruleSetId = expandedRuleSetIds.begin(); ruleSetId != expandedRuleSetIds.end(); ++ruleSetId)
				expandedRuleSets.push_back(JobExecutor::toRuleSetFileName(*ruleSetId));
		}

		return true;
	}

	unsigned int JobExecutor::RuleFilter::filter(MappedItems& ruleIds, MappedItemsNoMatch& itemsNoMatch, const Context& context) const
	{
		Directory::FileNames ruleSetFiles;
		Items expandedRuleCats;
		Items expandedRuleIds;

		this->expandRuleSets(ruleSetFiles);
		this->expandRuleCats(expandedRuleCats);
		this->expandRuleIds(expandedRuleIds);

		for (Directory::FileNames::const_iterator ruleSetFile = ruleSetFiles.begin(); ruleSetFile != ruleSetFiles.end(); ++ruleSetFile)
		{
			//ACS_HCS_TRACE("filterRuleIds(): Filtering rule-set file '" << (*ruleSetFile) << "'.");
			DocumentXml doc(*ruleSetFile);
			DocumentXml::Nodes rules;
			DocumentXml::Nodes::Iterator rule;

			doc.getNodesList(rules, "/ruleset/rule", NULL);
			
			//const string& ruleSetId = doc.getNode("/ruleset").getAttribute("id").getValue();
			string ruleSetId = doc.getRulesetIdValue();
			itemsNoMatch[ruleSetId].ruleIds.insert(expandedRuleIds.begin(), expandedRuleIds.end());
			itemsNoMatch[ruleSetId].ruleCats.insert(expandedRuleCats.begin(), expandedRuleCats.end());

			// Filter out the appropriate rules.

			set<string> uniqueRuleIds;

			for (rule = rules.begin(); rule != rules.end(); ++rule)
			{
				if (! this->isApplicable(*rule, context))
				{
					cout << "not applicable" << endl;
					continue;
				}

				const string& ruleId = (*rule).getAttribute("id").getValue();

				size_t pos = ruleId.find_first_of(".");
				string ruleIdMajor = ruleId.substr(0, pos);

			/*	if (uniqueRuleIds.find(ruleIdMajor) != uniqueRuleIds.end())
				{
					ACS_HCS_THROW(JobExecutor::ExceptionJobExecution, "filterRuleIds()", "Detected ambiguous rule '" << ruleIdMajor << "' in rule set '" << ruleSetId << "'.", "Rule ID=" << ruleId);
				}*/

				if (!expandedRuleCats.empty())
				{
					if (expandedRuleCats.find("ALL") != expandedRuleCats.end())
					{
						uniqueRuleIds.insert(ruleIdMajor);
						ruleIds[*ruleSetFile].insert(ruleId);
						continue;
					}

					DocumentXml::Nodes categories;
					DocumentXml::Nodes::Iterator category;

					//(*rule).getNodes(categories, "./category");
					(*rule).getNodesList(categories, "./category", (*rule));

					bool inserted = false;

					for (category = categories.begin(); category != categories.end(); ++category)
					{
						const string cat = (*category).getValue();
						if (cat == "MANDATORY")
						{
							if (!inserted)
							{
								uniqueRuleIds.insert(ruleIdMajor);
								ruleIds[*ruleSetFile].insert(ruleId);
								inserted = true;
							}

							itemsNoMatch[ruleSetId].ruleCats.erase(cat);
						}
						else
						{
							if (expandedRuleCats.find(cat) != expandedRuleCats.end())
							{
								if (!inserted)
								{
									uniqueRuleIds.insert(ruleIdMajor);
									ruleIds[*ruleSetFile].insert(ruleId);
									inserted = true;
								}

								itemsNoMatch[ruleSetId].ruleCats.erase(cat);
							}
						}
					}
				}
				else if (!expandedRuleIds.empty())
				{
					if (expandedRuleIds.find(ruleIdMajor) != expandedRuleIds.end())
					{
						uniqueRuleIds.insert(ruleIdMajor);
						ruleIds[*ruleSetFile].insert(ruleId);
						itemsNoMatch[ruleSetId].ruleIds.erase(ruleIdMajor);
					}
				}
				else
				{
					uniqueRuleIds.insert(ruleIdMajor);
					ruleIds[*ruleSetFile].insert(ruleId);
				}
			}
		}

		return ruleIds.size();
	}

	bool JobExecutor::RuleFilter::isApplicable(const DocumentXml::Node& rule, const Context& context) const
	{
		//ACS_HCS_TRACE("isApplicable(): context=" << context.toString() << ", rule={" << rule.getAttribute("id") << rule.getNode("./attributes") << "}");

		{
			// Following decision matrix is implemented:
			//
			// +---------+-----------+--------------------------------+
			// |         |           |       Cluster-Session          |
			// | Blade   | Option -n +---------+----------+-----------+
			// | Cluster | Specified | ALLOWED | REQUIRED | FORBIDDEN |
			// +---------+-----------+---------+----------+-----------+
			// |    0    |     0     |    A    |     A    |     A     |
			// |    0    |     1     |    A    |     A    |     A     |
			// |    1    |     0     |    A    |     A    |    N/A    |
			// |    1    |     1     |    A    |    N/A   |     A     |
			// +---------+-----------+---------+----------+-----------+
			//
			// A = Rule is APPLICABLE,  N/A = Rule is NOT APPLICABLE
			// Cluster-session defaults to ALLOWED.

			cout<<"inside JobExecutor::RuleFilter::isApplicable "<<endl;

			if (context.nodeType == "MSC-SBC" || context.nodeType == "HLR-BC")
			{
				//const DocumentXml::Node& attrClusterSession = rule.getNode("./attributes/cluster-session", false);
				const DocumentXml::Node& attrClusterSession =  rule.getSpecifiedNode("./attributes/cluster-session",(*rule),false);

				if (attrClusterSession.isValid())
				{
					string attrClusterSessionStr = attrClusterSession.getValue();
					transform(attrClusterSessionStr.begin(), attrClusterSessionStr.end(), attrClusterSessionStr.begin(), ::toupper);

					if (attrClusterSessionStr != "ALLOWED")
					{
						if (context.cpName.empty()) // Rule to be applied on cluster session
						{
							if (attrClusterSessionStr == "FORBIDDEN")
								return false;
						}
						else // Rule to be applied on CP as specified in option -n
						{
							if (attrClusterSessionStr == "REQUIRED")
								return false;
						}
					}
				}
			}
		}

		//const DocumentXml::Node& attrInclude = rule.getNode("./attributes/include", false);
		const DocumentXml::Node& attrInclude = rule.getSpecifiedNode("./attributes/include",(*rule),false);

		if (attrInclude.isValid())
		{
			bool found = false;
			string value;

			{
				DocumentXml::Nodes nodes;
				//attrInclude.getNodes(nodes, "./apz-type");
				attrInclude.getNodesList(nodes,"./apz-type", attrInclude );


				if (!nodes.isEmpty())
				{
					found = false;

					for (DocumentXml::Nodes::Iterator node = nodes.begin(); (!found) && node != nodes.end(); ++node)
						found = ((value = (*node).getValue()) == "ALL" || value == context.apzType);

					if (!found)
						return false;
				}
			}
			{
				DocumentXml::Nodes nodes;
				//attrInclude.getNodes(nodes, "./node-type");
				attrInclude.getNodesList(nodes, "./node-type", attrInclude );

				if (!nodes.isEmpty())
				{
					found = false;

					for (DocumentXml::Nodes::Iterator node = nodes.begin(); (!found) && node != nodes.end(); ++node)
						found = ((value = (*node).getValue()) == "ALL" || value == context.nodeType);

					if (!found)
						return false;
				}
			}
			{
				DocumentXml::Nodes nodes;
				//attrInclude.getNodes(nodes, "./sw-level");
				attrInclude.getNodesList(nodes, "./sw-level", attrInclude );

				if (!nodes.isEmpty())
				{
					found = false;

					for (DocumentXml::Nodes::Iterator node = nodes.begin(); (!found) && node != nodes.end(); ++node)
						found = ((value = (*node).getValue()) == "ALL" || value == context.swLevel);

					if (!found)
						return false;
				}
			}
			{
				DocumentXml::Nodes nodes;
				//attrInclude.getNodes(nodes, "./op-mode");
				attrInclude.getNodesList(nodes, "./op-mode", attrInclude );

				if (!nodes.isEmpty())
				{
					found = false;

					for (DocumentXml::Nodes::Iterator node = nodes.begin(); (!found) && node != nodes.end(); ++node)
						found = ((value = (*node).getValue()) == "ALL" || value == context.opMode);

					if (!found)
						return false;
				}
			}
		}

		//const DocumentXml::Node& attrExclude = rule.getNode("./attributes/exclude", false);
		const DocumentXml::Node& attrExclude = rule.getSpecifiedNode("./attributes/exclude",(*rule),false);
		if (attrExclude.isValid())
		{
			bool found = false;

			{
				DocumentXml::Nodes nodes;
				//attrExclude.getNodes(nodes, "./apz-type");
				attrExclude.getNodesList(nodes,"./apz-type", attrExclude );

				if (!nodes.isEmpty())
				{
					found = false;

					for (DocumentXml::Nodes::Iterator node = nodes.begin(); (!found) && node != nodes.end(); ++node)
						found = ((*node).getValue() == context.apzType);

					if (!found)
						return true;
				}
			}
			{
				DocumentXml::Nodes nodes;
				//attrExclude.getNodes(nodes, "./node-type");
				attrExclude.getNodesList(nodes,"./node-type", attrExclude );
				if (!nodes.isEmpty())
				{
					found = false;

					for (DocumentXml::Nodes::Iterator node = nodes.begin(); (!found) && node != nodes.end(); ++node)
						found = ((*node).getValue() == context.nodeType);

					if (!found)
						return true;
				}
			}
			{
				DocumentXml::Nodes nodes;
				//attrExclude.getNodes(nodes, "./sw-level");
				attrExclude.getNodesList(nodes,"./sw-level", attrExclude );
				if (!nodes.isEmpty())
				{
					found = false;

					for (DocumentXml::Nodes::Iterator node = nodes.begin(); (!found) && node != nodes.end(); ++node)
						found = ((*node).getValue() == context.swLevel);

					if (!found)
						return true;
				}
			}
			{
				DocumentXml::Nodes nodes;
				//attrExclude.getNodes(nodes, "./op-mode");
				attrExclude.getNodesList(nodes,"./op-mode", attrExclude );
				if (!nodes.isEmpty())
				{
					found = false;

					for (DocumentXml::Nodes::Iterator node = nodes.begin(); (!found) && node != nodes.end(); ++node)
						found = ((*node).getValue() == context.opMode);

					if (!found)
						return true;
				}
			}

			if (found) // There was a node for exclusion and we are here --> exclude.
				return false;
		}

		return true;
	}

	void JobExecutor::RuleFilter::splitCommaSepList(Items& items, string commaSepList)
	{
		size_t pos = 0;

		while ((pos = commaSepList.find_first_of(" ", pos)) != string::npos)
			commaSepList.erase(pos++, 1);

		size_t posStart = 0;

		do
		{
			pos = commaSepList.find_first_of(",", posStart);

			const string& item = commaSepList.substr(posStart, pos - posStart);

			posStart = pos + 1;

			items.insert(item);
		}
		while (posStart);
	}



}


