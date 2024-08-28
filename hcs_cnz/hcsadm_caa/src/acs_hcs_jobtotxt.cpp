//======================================================================
//
// NAME
//      JobToTxt.cpp
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
//      2011-08-05 by EEDSTL
// CHANGES
//     
//======================================================================

#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include "acs_hcs_formattertxt.h"
#include "acs_hcs_jobexecutor.h"
#include "acs_hcs_scheduler.h"
#include "acs_hcs_timeutils.h"
#include "acs_hcs_jobtotxt.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Friends
	//================================================================================

	ostream& operator<<(ostream& os, const JobToTxt& converter)
	{
		stringstream ss;

		return os << converter.toString(ss, 0).str();
	}

	//================================================================================
	// Class JobToTxt
	//================================================================================

	const FormatterTxt::Table::Column JobToTxt::colsJob[] = 
	{
		FormatterTxt::Table::Column("Job Name"      , 16, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Next Execution", 16, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Period"        ,  8, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Last Execution", 16, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Status"        ,  7, FormatterTxt::Table::Column::/*Align::*/LEFT, "\n"                    , "\n"                     ),
		FormatterTxt::Table::Column()
	};

	const FormatterTxt::Table::Column JobToTxt::colsJobDetails[] = 
	{
		FormatterTxt::Table::Column("Job Name"      , 16, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Next Execution", 16, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Rule Set"      , 25, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Rule ID"       ,  8, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Category"      , 15, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Status"        ,  7, FormatterTxt::Table::Column::/*Align::*/LEFT, "\n"                    , "\n"                     ),
		FormatterTxt::Table::Column()
	};

	JobToTxt::JobToTxt(const string& jobName, bool showDetails)	: jobName(jobName) , showDetails(showDetails)
	{
	}

	JobToTxt::~JobToTxt()
	{
	}

	string JobToTxt::periodToStr(JobExecutor::Job::Period period)
	{
		DEBUG("%s","Entering JobToTxt::periodToStr ");
		switch (period)
		{
		case JobExecutor::Job::/*Period::*/PD_HOURLY :
			return "HOURLY";

		case JobExecutor::Job::/*Period::*/PD_DAILY :
			return "DAILY";

		case JobExecutor::Job::/*Period::*/PD_WEEKLY :
			return "WEEKLY";

		case JobExecutor::Job::/*Period::*/PD_BIWEEKLY :
			return "BIWEEKLY";

		default:
			return "-";
		}
		DEBUG("%s","Leaving from JobToTxt::periodToStr ");
	}

	void JobToTxt::printFormatJob(stringstream& ss, const FormatterTxt::Table::Column cols[]) const
	{
		DEBUG("%s","Entering JobToTxt::printFormatJob ");
		Scheduler::JobMap jobs;
		Scheduler::Singleton::get()->jobsGet(jobs);
		JobExecutor::Job currentJob = JobExecutor::Singleton::get()->getCurrentJob();
		
		if (currentJob.isValid() && !jobs.empty() && jobs.begin()->second == currentJob)
			jobs.erase(jobs.begin());

		if (this->jobName.empty())
		{
			if (jobs.size() == 0 && !currentJob.isValid())
			{
				DEBUG("%s","return from no jobs found JobToTxt::printFormatJob ");
				ss << "No jobs found." << endl;
				return;
			}
		}
		else
		{
			if (currentJob.getName() == this->jobName)
			{
				jobs.erase(jobs.begin(), jobs.end());
			}
			else
			{
				currentJob.invalidate();

				while (jobs.begin() != jobs.end() && jobs.begin()->second.getName() != this->jobName)
					jobs.erase(jobs.begin());

				while (jobs.rbegin() != jobs.rend() && jobs.rbegin()->second.getName() != this->jobName)
					jobs.erase(jobs.rbegin()->first);
			}

			if (jobs.size() == 0 && !currentJob.isValid())
				ACS_HCS_THROW(JobToTxt::ExceptionJobNotFound, "printFormatJob()", "Job '" << this->jobName << "' does not exist.", "Job not found.");
		}

		FormatterTxt::Table table(cols);
		unsigned int numCols = table.printHeader(ss);

		if (currentJob.isValid())
		{
			vector<string> colTxts;

			if (this->showDetails)
			{
				colTxts.push_back(currentJob.getName());
				colTxts.push_back(JobToTxt::timeToStr(currentJob.getNextStartTime()));
				colTxts.push_back(currentJob.getRuleSetIds());
				colTxts.push_back(currentJob.getRuleIds());
				colTxts.push_back(currentJob.getRuleCats());
				colTxts.push_back("RUNNING");
			}
			else
			{
				colTxts.push_back(currentJob.getName());
				colTxts.push_back(JobToTxt::timeToStr(currentJob.getNextStartTime()));
				colTxts.push_back(JobToTxt::periodToStr(currentJob.getPeriod()));
				time_t lastStartTime = currentJob.getLastStartTime();
				colTxts.push_back(JobToTxt::timeToStr(lastStartTime > TimeUtils::now() ? lastStartTime : 0));
				colTxts.push_back("RUNNING");
			}

			table.printLine(ss, colTxts);
		}

		for (Scheduler::JobMap::const_iterator it = jobs.begin(); it != jobs.end(); ++it)
		{
			const JobExecutor::Job job = it->second;
			vector<string> colTxts;

			if (this->showDetails)
			{
				colTxts.push_back(job.getName());
				colTxts.push_back(JobToTxt::timeToStr(it->first));
				colTxts.push_back(job.getRuleSetIds());
				colTxts.push_back(job.getRuleIds());
				colTxts.push_back(job.getRuleCats());
				colTxts.push_back("WAITING");
			}
			else
			{
				colTxts.push_back(job.getName());
				colTxts.push_back(JobToTxt::timeToStr(it->first));
				colTxts.push_back(JobToTxt::periodToStr(job.getPeriod()));
				colTxts.push_back(JobToTxt::timeToStr(job.getLastStartTime()));
				colTxts.push_back("WAITING");
			}

			table.printLine(ss, colTxts);
		}
		DEBUG("%s"," Leaving JobToTxt::printFormatJob");
	}

	string JobToTxt::timeToStr(time_t timeVal)
	{
		DEBUG("%s","return from JobToTxt::timeToStr ");
		return (timeVal > 0 ? TimeUtils::timeToStr(timeVal, "%Y-%m-%d %H:%M") : "-");
	}

	stringstream& JobToTxt::toString(stringstream& ss, unsigned int level) const
	{
		DEBUG("%s","Entering JobToTxt::toString( ");
		if (this->showDetails)
			this->printFormatJob(ss, JobToTxt::colsJobDetails);
		else
			this->printFormatJob(ss, JobToTxt::colsJob);
		DEBUG("%s","Leaving JobToTxt::toString( ");
		return ss;
	}
}
