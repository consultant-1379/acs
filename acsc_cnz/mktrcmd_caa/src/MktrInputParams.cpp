/*
 * MktrInputParams.cpp
 *
 *  Created on: Aug 31, 2010
 *      Author: xludesi
 */

#include "MktrInputParams.h"
#include <string.h>
#include "MktrDate.h"
#include "MktrDateFormat.h"
#include "APGInfo.h"

const int MktrInputParams::num_max_of_cmd_params;
MktrInputParams::MktrInputParams()
{
	_aOpt = false;
	//_bOpt = false;
	//_dOpt = false;
	_lOpt = false;
	//_pOpt = false;
	_tOpt = false;
	_oOpt = false;
	_xOpt = false;
	_vOpt = false;
	_dOpt = false;
	_commandLine = "";
	_lastParseErrorDesc = "";
}


MktrInputParams::~MktrInputParams()
{
	// TODO Auto-generated destructor stub
}


int MktrInputParams::parseCommandLine(int argc, char *argv[], const unsigned int apgShelfArchitectureType)
{
	int retval = MKTRPCL_SUCCESS;

	if(argc > MktrInputParams::num_max_of_cmd_params + 1)
	{
		this->_lastParseErrorDesc = "Incorrect number of parameters";
		return MKTRPCL_INCORRECT_NUM_ARGS;
	}

	_commandLine = "mktr";

	for(int i=1; i < argc; i++)
	{
		int res=0;
		if(apgShelfArchitectureType == MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED)
		{
			res = _parseOption(argv[i],"-a",_aOpt);
			(res == PARAM_NOT_FOUND) && (res = _parseOption(argv[i],"-l",_lOpt));
			(res == PARAM_NOT_FOUND) && (res = _parseOption(argv[i],"-o",_oOpt));			
			(res == PARAM_NOT_FOUND) && (res = _parseOption(argv[i],"-v",_vOpt));
			(res == PARAM_NOT_FOUND) && (res = _parseTIME_Param(argv[i]));
		}
		else
		{
			res = _parseOption(argv[i],"-a",_aOpt);
			(res == PARAM_NOT_FOUND) && (res = _parseOption(argv[i],"-l",_lOpt));
			(res == PARAM_NOT_FOUND) && (res = _parseOption(argv[i],"-o",_oOpt));
			(res == PARAM_NOT_FOUND) && (res = _parseOption(argv[i],"-x",_xOpt));
			(res == PARAM_NOT_FOUND) && (res = _parseOption(argv[i],"-d",_dOpt));
			(res == PARAM_NOT_FOUND) && (res = _parseOption(argv[i],"-v",_vOpt));
			(res == PARAM_NOT_FOUND) && (res = _parseTIME_Param(argv[i]));
		}
		
		if(res == PARAM_NOT_FOUND )
		{
			// Unknown param encountered ! Terminate parsing process
			this->_lastParseErrorDesc = std::string("Incorrect option: ")+argv[i];
			retval = MKTRPCL_UNKNOWN_PARAM;
			break;
		}
		else if(res == PARAM_INVALID )
		{
			// Invalid param found ! Terminate parsing process
			retval = MKTRPCL_INVALID_ARGVALUE;
			break;
		}
		else if(res == PARAM_DUPL)
		{
			// param duplication ! Terminate parsing process
			retval = MKTRPCL_PARAM_DUPLICATED;
			break;
		}

		// since we are here , res == PARAM_FOUND !  Continue parsing process analyzing next argument
		_commandLine.append(" ");
		_commandLine.append(argv[i]);
	}


	return retval;
}


int MktrInputParams::_parseOption(const char *strToCheck, const char *opt, bool & optField)
{
	int retval = PARAM_NOT_FOUND;

	if(strcmp(strToCheck, opt)==0)
	{
		if(optField==true)
		{
			this->_lastParseErrorDesc = std::string("Duplicated option:") +  strToCheck;
			retval = PARAM_DUPL;
		}
		else
		{
			retval =  PARAM_FOUND;
		}

		optField = true;
	}

	return retval;
}


int MktrInputParams::_parseTIME_Param(const char *strToCheck)
{
	int retval = PARAM_NOT_FOUND;

	MktrDateFormat dft(MktrDateFormat::YYMMDD_HHmm);
	MktrDate* cmdDate = dft.parse(strToCheck);			// MttrDate object is allocated by "parse" method

	if(cmdDate != NULL)
	{
		if(_tOpt == true)
		{
			// param duplicated on command line !
			this->_lastParseErrorDesc = std::string("Duplicated TIME operand: ") +  strToCheck;
			retval = PARAM_DUPL;
		}
		else
		{
			MktrDate today;
			if(*cmdDate > today)
			{
				// the format of the param is correct but it's value is invalid !
				this->_lastParseErrorDesc = std::string("Time shall be less than or equal to current time");
				retval = PARAM_INVALID;
			}
			else
			{
				// param is valid
				_mktrDate = *cmdDate;
				_tOpt = true;
				retval = PARAM_FOUND;
			}
		}

		delete cmdDate;
	}

	return retval;
}



void MktrInputParams::getDateParam(MktrDate & mktrDate)
{
	mktrDate = this->_mktrDate;
}

