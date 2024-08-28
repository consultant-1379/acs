#include <stdio.h>
#include <limits>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "acs_chb_aadst_TDstCore.h"
#include "acs_chb_aadst_TDstExtractor.h"

TDstExtractor::TDstExtractor()
{
}

TDstExtractor::~TDstExtractor()
{
}

bool TDstExtractor::getDstFromZone(const std::string &strTimeZone, int iStartYear, int iEndYear, TZdumpData **pZdumpObj)
{
	bool bRet;
	TDstCore tdcore;
	// Initialization
	bRet = true;
	*pZdumpObj = NULL;
	// Get info for timezone
	tdcore.getDstList(strTimeZone.c_str(), iStartYear, iEndYear, objZdumpData.getZdumpLst());
	// Set the Zone name
	objZdumpData.setZoneName(strTimeZone);
	// Set the output argument to internal list point.
	*pZdumpObj = &objZdumpData;
	// Exit from method
	return (bRet);
}
