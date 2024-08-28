/*
 * MktrInputParams.h
 *
 *  Created on: Aug 31, 2010
 *      Author: xludesi
 */

#ifndef MKTRINPUTPARAMS_H_
#define MKTRINPUTPARAMS_H_

#include "MktrDate.h"
#include <string>

class MktrInputParams
{
public:

	enum MktrParseCmdLineConstants
	{
		MKTRPCL_SUCCESS = 0,					// Command line successfully parsed
		MKTRPCL_INCORRECT_NUM_ARGS = -1,		// Command line parsing failure caused by incorrect number of arguments
		MKTRPCL_PARAM_DUPLICATED = -2,			// Command line parsing failure caused by param duplicated
		MKTRPCL_UNKNOWN_PARAM = -3,				// Command line parsing failure caused by unkonw param
		MKTRPCL_INVALID_ARGVALUE = -4			// Command line parsing failure caused by invalid param VALUE
	};

	MktrInputParams();

	virtual ~MktrInputParams();

	// parses the application command line. Returns : 0 in case of success or a negative number in case of failure.
	// The return values are described by "MktrParseCmdLineConstants" enumeration.
	// In case of failure, the description of the error occurred is available invoking the "lastParseErrDesc" method
	int parseCommandLine(int argc, char *argv[], const unsigned int apgShelfArchitectureType);

	// returns the description of the last parse error
 	std::string lastParseErrDesc() { return _lastParseErrorDesc; };


	bool hasOption_a() { return _aOpt; };
	//bool hasOption_d() { return _dOpt; };
	//bool hasOption_p() { return _pOpt; };
	bool hasOption_t() { return _tOpt; };
	bool hasOption_l() { return _lOpt; };
	//bool hasOption_b() { return _bOpt; };
	bool hasOption_o() { return _oOpt; };
	bool hasOption_x() { return _xOpt; };
	bool hasOption_v() { return _vOpt; };
	bool hasOption_d() { return _dOpt; };
	void getDateParam(MktrDate & mktrDate);
	std::string getCommandLine() { return _commandLine; }

private:
	// Remember to keep this class attribute up to date, in case of adding or removing parameters
	static const int num_max_of_cmd_params = 7; // [time] a l x v d o (including the hidden parameter 'o')

	enum MktrParseParamConstants
	{
		PARAM_DUPL = -1,				//  param duplicated
		PARAM_INVALID = -2,				//  param found with invalid value
		PARAM_NOT_FOUND = 0,			//  param NOT found
		PARAM_FOUND = 1					//  param found for the first time
	};

	bool _aOpt;
	//bool _dOpt;
	//bool _pOpt;
	bool _tOpt;
	bool _lOpt;
	//bool _bOpt;
	bool _oOpt;
	bool _xOpt;
	bool _vOpt;
	bool _dOpt;
	MktrDate _mktrDate;
	std::string _commandLine;
	std::string _lastParseErrorDesc;

	// helper methods
	int _parseOption(const char *strToCheck, const char *opt, bool & optField);
	int _parseTIME_Param(const char *strToCheck);
};

#endif /* MKTRINPUTPARAMS_H_ */
