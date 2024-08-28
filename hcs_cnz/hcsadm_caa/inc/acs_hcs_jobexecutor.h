//======================================================================
//
// NAME
//      JobExecutor.h
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
//      Implementation of HC Job.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_JobExecutor_h
#define AcsHcs_JobExecutor_h

#include <map>
#include <set>
#include <string>

#include <ace/ACE.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Atomic_Op.h>

#include "acs_hcs_directory.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_exception.h"
#include "acs_hcs_formattertxt.h"
#include "acs_hcs_reportgenerator.h"
#include "acs_hcs_synchron.h"
#include "acs_hcs_healthcheckcmd.h"
#include "acs_hcs_global_ObjectImpl.h"

namespace AcsHcs
{
	/**
	* JobExecutor - Determines the rules of a rule-set to be executed and performs all operations necessary.
	*/
	class JobExecutor
	{
		friend class ACE_Singleton<JobExecutor, ACE_Thread_Mutex>;

	public:
		/**
		* Use type Singleton to obtain the singleton instance.
		* Example: JobExecutor::Singleton::get();
		*/
		typedef Synchron::Singleton<JobExecutor> Singleton;

		/**
		* JobExecutor::ExceptionJobDataSize - Exception when trying to reduce the data size.
		* JobExecutor::ExceptionJobDataSize is thrown in the case of problems reducing the data size.
		*/
		class ExceptionJobDataSize : public Exception
		{
		public:
			ExceptionJobDataSize(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionJobDataSize() throw() {}
		};

		/**
		* JobExecutor::ExceptionJobExecution - Exception during job-execution.
		* JobExecutor::ExceptionJobExecution is thrown in the case of problems executing a job.
		*/
		class ExceptionJobExecution : public Exception
		{
		public:
			ExceptionJobExecution(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionJobExecution() throw() {}
		};

		/**
		* JobExecutor::ExceptionJobInProgress - Exception indicating that there is a Job in progress.
		* JobExecutor::ExceptionJobInProgress is thrown in the case there is a Job in progress.
		*/
		class ExceptionJobInProgress : public Exception
		{
		public:
			ExceptionJobInProgress(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionJobInProgress() throw() {}
		};
		
		

		class ExceptionDirectoryFileCreation : public Exception
                {
                public:
                        ExceptionDirectoryFileCreation(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
                        ~ExceptionDirectoryFileCreation() throw() {}
                };



                /**
                * JobExecutor::ExceptionInitialRuleFile - Exception indicating that initial rule file is not found.
                * JobExecutor::ExceptionInitialRuleFil is thrown in the case of initial rule file not found.
                */
                class ExceptionInitialRuleFile : public Exception
                {
                public:
                        ExceptionInitialRuleFile(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
                        ~ExceptionInitialRuleFile() throw() {}
                };


		/**
		* JobExecutor::CmdResultFiles - Responsible for copying a rule's command result files in the case of failure.
		*/
		class CmdResultFiles
		{
		public:
			CmdResultFiles(const std::string& jobId, const std::string& ruleSetId, const std::string& ruleId, string);			
			~CmdResultFiles();

			Directory::FileNames& getFileNames();

		private:
			CmdResultFiles(const CmdResultFiles&);
			CmdResultFiles& operator=(const CmdResultFiles&);

			Directory::FileNames fileNames;
			Directory dirTo;
			const string fileNameTo;
		};

		/**
		* JobExecutor::Job - The representation of a job to be executed by the JobExecutor.
		*/
		class Job
		{
			friend ostream& operator<<(ostream& os, const Job& job);
			friend istream& operator>>(istream& is, Job& job);

		public:
			enum Period
			{
				PD_INVALID  = 1,
				PD_HOURLY   = 60 * 60,
				PD_DAILY    = 24 * PD_HOURLY,
				PD_WEEKLY   =  7 * PD_DAILY,
				PD_BIWEEKLY =  2 * PD_WEEKLY
			};

			/**
			* Default constructor.
			* Needed only to satisfy STL-containers.
			*/
			Job();

			/**
			* Constructor.
			* @param[in] name The name of the Job.
			*/
			Job(const std::string& name);

			/**
			* Copy constructor.
			*/
			Job(const Job& job);

			/**
			* Destructor.
			*/
			~Job();

			/**
			* Assignment operator.
			*/
			Job& operator=(const Job& job);

			/**
			* Equality operator.
			*/
			bool operator==(const Job& job);

			/**
			* Unequality operator.
			*/
			bool operator!=(const Job& job);

			/**
			* Returns the name of this Job.
			* @return The Job name.
			*/
			const std::string& getName() const;

			/**
			* Returns the rule set IDs as defined for this Job.
			* @return The rule set IDs as defined for this Job.
			*/
			const std::string& getRuleSetIds() const;

			/**
			* Returns the rule IDs as defined for this Job.
			* @return The rule IDs as defined for this Job.
			*/
			const std::string& getRuleIds() const;

			/**
			* Returns the rule categories as defined for this Job.
			* @return The rule categories as defined for this Job.
			*/
			const std::string& getRuleCats() const;

			/**
			* Returns the name of the CP where this Job is to be executed.
			* @return The name of the CP where this Job is to be executed.
			*/
			const std::string& getCpName() const;

			/**
			* Returns the name of the transfer queue where the result of this Job is to be sent to.
			* @return The name of the transfer queue where the result of this Job is to be sent to.
			*/
			const std::string& getTransferQueue() const;
			
			const int& getCompression() const;

			const int& getSuccessLogs() const;

			/**
			* Returns the time of the next execution of this Job.
			* Value 0 is returned if the is no further iteration.
			* @param[in] ttl Time-to-live [s] for the Job-registration.
			* @return The time of the next execution of this Job. Value 0 if there is no further iteration.
			*/
			time_t getNextStartTime(time_t ttl = 0L) const;

			/**
			* Returns the time of the last execution of this Job.
			* @return The time of the last execution of this Job.
			*/
			time_t getLastStartTime() const;

			/**
			* Returns the period of iterative execution as defined for this Job.
			* @return The period of iterative execution as defined for this Job.
			*/
			Period getPeriod() const;

			/**
			* Returns whether or not the user who has defined the Job is an expert user.
			* @return Whether or not the user who has defined the Job is an expert user.
			*/
			bool isExpertUser() const;

			/**
			* Confirms the execution of this Job.
			* To be called by a user when this Job has been executed.
			*/
			void confirm() const;

			/**
			* Invalidates this Job.
			*/
			void invalidate();

			/**
			* Checks the validity of this Job. A Job is valid if it has got a name.
			* @return true if the Job is valid, false otherwise.
			*/
			bool isValid() const;

			/**
			* Sets the name of the Job.
			* @param[in] jobName The name of the Job.
			*/
			void setName(const string& jobName);

			/**
			* Sets the IDs of the rule sets to be considered by the Job.
			* @param[in] ruleSetIds The IDs of the rule sets to be considered by the Job.
			*/
			void setRuleSetIds(const std::string& ruleSetIds);

			/**
			* Sets the IDs of the rules to be executed by the Job.
			* @param[in] ruleIds The IDs of the rules to be executed by the Job.
			*/
			void setRuleIds(const std::string& ruleIds);

			/**
			* Sets the categories of the rules to be executed by the Job.
			* @param[in] ruleCats The categories of the rules to be executed by the Job.
			*/
			void setRuleCats(const std::string& ruleCats);

			/**
			* Sets the name of the CP to connect to (valid for blade systems only).
			* @param[in] cpName The name of the CP to connect to (valid for blade systems only).
			*/
			void setCpName(const std::string& cpName);

			/**
			* Sets the name of the TQ to connect to transfer the job related files.
			* @param[in] transferQueue The name of the TQ to connect to transfer the job related files.
			*/
			void setTransferQueue(std::string transferQueue);

			void setCompression(int compression);

			void setSuccessLogs(int successLogs);
			
			void setjobToTrigger(std::string jobToTrigger);

			/**
			* Scheduled jobs only: The timestamp when the Job shall be scheduled.
			* If timeStamp is empty, the current time will be taken.
			* @param[in] timeStamp The timestamp when to schedule the Job.
			*/
			void setTimeStamp(const std::string& timeStamp = "");

			/**
			* Scheduled jobs only: The number of iterations of the Job.
			* @param[in] iterations The number of iterations of the Job.
			*/
			void setIterations(const std::string& iterations);

			/**
			* Sets the flag indicating that the user is an expert.
			* @param[in] expertUser true if the user is an expert, false otherwise.
			*/
			void setExpertUser(bool expertUser);

			/**
			* Serialize this Job.
			*/
			virtual std::stringstream& toString(std::stringstream& ss, unsigned int level = 0) const;

		private:
			static std::string& readNextItem(std::string& item, size_t& posStart, const std::string& line);

                        std::string name;
			std::string ruleSetIds;
			std::string ruleIds;
			std::string ruleCats;
			std::string cpName;
			std::string transferQueue;
			int compression;
			std::string jobToTrigger;
			int successLogs;
			std::string timeStamp;
			std::string iterations;
			bool        expertUser;
			int         nIterationsMax;
			time_t      startTime;
			Period      timeSpan;

			mutable int iterExecuted;
			mutable int iterPending;
		};

		/**
		* JobExecutor::RuleFilter - Filters the Rules according to the user input and system environment
		*/
		class RuleFilter
		{
		public:
			typedef std::set<std::string>              Items;

			struct ItemsNoMatch
			{
				Items ruleIds;
				Items ruleCats;
			};

			typedef std::map<std::string, Items>        MappedItems;
			typedef std::map<std::string, ItemsNoMatch> MappedItemsNoMatch;

			struct Context
			{
				/** Context stores the context of a job
				* CP name,
				* APZ type,
				* node type,
				* software level
				* OP mode
				*/
				Context(const std::string& cpName) : cpName(cpName)
				{
				}

				std::string toString() const
				{
					std::stringstream ss;

					ss << "{";
					ss << "cpName='"    << cpName   << "'";
					ss << ",apzType='"  << apzType  << "'";
					ss << ",nodeType='" << nodeType << "'";
					ss << ",swLevel='"  << swLevel  << "'";
					ss << ",opMode='"   << opMode   << "'";
					ss << "}";

					return ss.str();
				};

				const std::string cpName;
				std::string apzType;
				std::string nodeType;
				std::string swLevel;
				std::string opMode;
			};

			/**
			* Perform the filtering of rules.
			* @param[inout] ruleIds          The IDs of the filtered rules per rule-set are returned in here.
			* @param[inout] itemsNoMatch     The categories and IDs of the rules per rule-set with no match in a rule-set.
			* @param[in]    context          The context for the rule-execution.
			* @return The total number of rules matching the filter.
			*/
			unsigned int filter(MappedItems& ruleIds, MappedItemsNoMatch& itemsNoMatch, const Context& context) const;

			RuleFilter(const Job& job);
			~RuleFilter();

		private:
			static void splitCommaSepList(Items& items, string commaSepList);

			RuleFilter(const RuleFilter&);
			RuleFilter& operator=(const RuleFilter&);

			bool expandRuleCats(Items& expandedRuleCats) const;	
			bool expandRuleIds(Items& expandedRuleIds) const;	
			bool expandRuleSets(Directory::FileNames& expandedRuleSets) const;

			bool isApplicable(const DocumentXml::Node& rule, const Context& context) const;

			const Job& job;
		};

		/**
		* JobExecutor::Reporter - Reports the progress of the job execution.
		* This class is a wrapper around ReportGenerator and reporting direct feedback to the user.
		*/
		class Reporter
		{
		public:
			/**
			* Constructor.
			* @param[in] fileName  The name of the report file without suffix.
			* @param[in] responder Used for sending status messages to the client.
			*/
			Reporter(const std::string& fileName,string time_Stamp, HealthCheckCmd* const responder);

			/**
			* Destructor.
			*/
			~Reporter();

			/**
			* Opens the report.
			* @param[in] jobName  Name of the job.
			* @param[in] date     Start date for the job execution.
			* @throws ReportGenerator::ExceptionIo
			*/
			void open(const std::string& jobName, const std::string& date);

			/**
			* Closes the Report.
			* @throws ReportGenerator::ExceptionIo
			*/
			void close();

			/**
			* Opens a rule set part in the report.
			* @param[in] id Rule set identity.
			* @throws ReportGenerator::ExceptionIo
			*/
			void openRuleSet(const std::string& id);

			/**
			* Closes a rule set part in the report.
			* @throws ReportGenerator::ExceptionIo
			*/
			void closeRuleSet();

			/**
			* Opens a rule set part for non-matching rules/categories in the report.
			* @param[in] id Rule set identity.
			* @throws ReportGenerator::ExceptionIo
			*/
			void openRuleSetNoMatch(const std::string& id);

			/**
			* Closes a rule set part for non-matching rules/categories in the report.
			* @throws ReportGenerator::ExceptionIo
			*/
			void closeRuleSetNoMatch();

			/**
			* Add context attributes to the report.
			* @param[in] apzType  The type of APZ which the rule is executed for.
			* @param[in] nodeType The type of node which the rule is executed for.
			* @param[in] swLevel  The SW level which the rule is executed for.
			* @param[in] opMode   The operation mode which the rule is executed for.
			* @throws ReportGenerator::ExceptionIo
			*/
			void addAttributes(const std::string& apzType, const std::string& nodeType, const std::string& swLevel, const std::string& opMode);

			/**
			* Add a rule part to the report.
			* @param[in] id             Rule identity.
			* @param[in] name           Rule name.
			* @param[in] slogan         Rule slogan.
			* @param[in] result         Rule check result.
			* @param[in] recommendation Rule recommendation.
			* @throws ReportGenerator::ExceptionIo
			*/
			void addRule(const std::string& id, const std::string& name, const std::string& slogan, const std::string& result, const std::string& recommendation);

			/**
			* Add a rule-no-match part to the report.
			* @param[in] id Rule identity.
			* @throws ReportGenerator::ExceptionIo
			*/
			void addRuleNoMatch(const std::string& id);

			/**
			* Add a category-no-match part to the report.
			* @param[in] id Rule category.
			* @throws ReportGenerator::ExceptionIo
			*/
			void addCategoryNoMatch(const std::string& id);

			/**
			* Adds interruption from exception or from remove user command.
			* @param[in] reason Reason for interruption.
			* @throws ReportGenerator::ExceptionIo
			*/
			void addInterruption(const std::string& reason);

		private:
			static const FormatterTxt::Table::Column cols[];
			static const FormatterTxt::Table::Column colsNoMatch[];

			static std::string& encodeXml(std::string& out, const std::string& in);

			Reporter(const Reporter&);
			Reporter& operator=(const Reporter&);

			ReportGenerator         reportGenerator;
			HealthCheckCmd* const   responder;
			FormatterTxt::Table     table;
			FormatterTxt::Table     tableNoMatch;
			string                  ruleSetId;
			bool                    tableInitialized;
			RuleFilter::MappedItems ruleIdsNoMatch;
			RuleFilter::MappedItems ruleCatsNoMatch;
		};

		/**
		* Destructor.
		*/
		~JobExecutor();

		/**
		* Cancel current Job-execution.
		* @param[in] reason The reason for cancellation.
		*/
		void cancel(const char* reason = 0);

		/**
		* Checks if the Job execution has been cancelled before.
		* @return true if the Job execution has been cancelled.
		*/
		bool hasBeenCancelled() const;

		/**
		* Gets the currently executed Job.
		* @return The currently executed Job.
		*/
		const Job& getCurrentJob() const;

		/**
		* Returns the percentage of the number of executed rules.
		* @return The percentage of the number of executed rules.
		*/
		int progress();

		/**
		* Start execution of a Job.
		* @param[in] job       The Job to be executed.
		* @param[in] responder Used for sending status messages to the client.
		* @return true if the Job could be started, false if there is an ongoing Job.
		* @throws AcsHcs::Synchron::ExceptionAcquireLock
		* @throws AcsHcs::JobExecutor::ExceptionJobExecution
		* @throws AcsHcs::JobExecutor::ExceptionJobInProgress
		* @throws AcsHcs::Exception
		*/
		void start(const Job& job, HealthCheckCmd* const responder = 0, string reportDN =" ");

		void updateRuleStatus(string jobName, map<string,ruleStat> &ruleStatus);
		string removeTrailingSpaces(string job_categories);
		/**
		* Sends the report and Log files to the remote destination using the Transfer Queue.
		* @param[in] fileName       The file to be transfered.
		* @return true if the file is successfully transfered, false otherwise.
		*/
		static bool compress_flag;

	private:
		static std::string getJobId(const string& jobName, string dateTime);
		static void        initContext(const std::string& jobId, RuleFilter::Context& context,string);
		static void	       removeReportLog(const Directory& dirLogs, const std::string& reportFileName);
		static bool        setupDirectories();
		
		static bool        setupDirectories(string jobName);

		static std::string toRuleSetFileName(const std::string& ruleSetId);

		JobExecutor();
		JobExecutor(const JobExecutor&);
		JobExecutor& operator=(const JobExecutor&);

		ACE_Thread_Mutex mutexStart;
		typedef ACE_Atomic_Op<ACE_Thread_Mutex, bool> busyDef;
		busyDef busy;
		typedef ACE_Atomic_Op<ACE_Thread_Mutex, bool> cancelledDef;
		cancelledDef cancelled;
		int nRulesTotal;
		int nRulesCompleted;
		Job currentJob;
	};
}

#endif // AcsHcs_JobExecutor_h
