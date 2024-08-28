//======================================================================
//
// NAME
//      JobToTxt.h
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
//      Implementation of HCJLS Command .
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_JobToTxt_h
#define AcsHcs_JobToTxt_h

#include <sstream>
#include <string>

#include "acs_hcs_exception.h"
#include "acs_hcs_formattertxt.h"
#include "acs_hcs_jobexecutor.h"

namespace AcsHcs
{
	/**
	* JobToTxt - Convert Job to text.
	*/
	class JobToTxt
	{
		friend std::ostream& operator<<(std::ostream& os, const JobToTxt& converter);

	public:

		/**
		* JobToTxt::ExceptionJobNotFound - Exception indicating that a job has not been found.
		* JobToTxt::ExceptionJobNotFound is thrown in the case of a job has not been found.
		*/
		class ExceptionJobNotFound : public Exception
		{
		public:
			ExceptionJobNotFound( const std::string& where, const std::string& what, const std::string& cause ) : Exception( where, what, cause ) {}
			~ExceptionJobNotFound() throw(){}
		};

		/**
		* Constructor.
		* @param[in] jobName     The name of the Job to be converted.
		* @param[in] showDetails Indicates whether or not details shall be shown.
		*/
		JobToTxt(const std::string& jobName = "", bool showDetails = false);

		/**
		* Destructor.
		*/
		~JobToTxt();

		/**
		* Serializes the Job.
		* @param[inout] ss The stream to which to write the Job data.
		* @param[in]    level
		* @return The stream as passed in 'ss'.
		*/
		std::stringstream& toString(std::stringstream& ss, unsigned int level = 0) const;

	private:
		static const FormatterTxt::Table::Column colsJob[];
		static const FormatterTxt::Table::Column colsJobDetails[];

		static std::string timeToStr(time_t time);
		static std::string periodToStr(JobExecutor::Job::Period period);

		JobToTxt(const JobToTxt&);
		JobToTxt& operator=(const JobToTxt&);

		void printFormatJob(std::stringstream& ss, const FormatterTxt::Table::Column cols[]) const;

		const std::string jobName;
		const bool        showDetails;
	};
}

#endif // AcsHcs_JobToTxt_h
