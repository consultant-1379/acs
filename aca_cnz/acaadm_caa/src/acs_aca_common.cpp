#include "acs_aca_common.h"

bool ACS_ACA_Common::GetDataDiskPath (std::string & pszLogicalName, std::string & pszPath) {
	ACS_APGCC_CommonLib myAPGCCCommonLib;
	char pszPath1[1024] = {0};
	int myLen = 512;

	ACS_APGCC_DNFPath_ReturnTypeT errorCode = myAPGCCCommonLib.GetDataDiskPath(pszLogicalName.c_str(), pszPath1, myLen);

	if (errorCode == ACS_APGCC_DNFPATH_SUCCESS) {
		//std::string myStr1(pszPath1);
		pszPath = pszPath1;
	}

	return (errorCode == ACS_APGCC_DNFPATH_SUCCESS);
};

bool ACS_ACA_Common::GetDataDiskPathForCp (std::string & pszLogicalName, unsigned int cpId, std::string & pszPath) {
	ACS_APGCC_CommonLib myAPGCCCommonLib;

	char pszPath1[1024] = {0};
	int myLen = 512;

	ACS_APGCC_DNFPath_ReturnTypeT  errorCode =
			myAPGCCCommonLib.GetDataDiskPathForCp(pszLogicalName.c_str(), cpId, pszPath1, myLen);

	if (errorCode == ACS_APGCC_DNFPATH_SUCCESS) {
		//std::string myStr1(pszPath1);
		pszPath = pszPath1;
	}

	return (errorCode == ACS_APGCC_DNFPATH_SUCCESS);
}

bool ACS_ACA_Common::GetDDTDataSourceNames(std::vector<std::string> & dsNames)
{
	bool result = false;
	std::vector<std::string> dsList;
	OmHandler theOmHandler;

	if (ACS_CC_SUCCESS == theOmHandler.Init())
	{
		if (ACS_CC_SUCCESS == theOmHandler.getClassInstances(reinterpret_cast<const char *>(ACS_CONFIG_IMM_DDT_CLASS_NAME), dsList))
		{

			for(std::vector<std::string>::const_iterator it = dsList.begin(); it != dsList.end(); ++it)
			{
				std::string dataSourceID;
				std::string dn(*it);
				if (GetLastFieldValueFromDN(dn,dataSourceID))
				{
					ACS_ACA_LOG(LOG_LEVEL_INFO, "DataStore   =%s ",dataSourceID.c_str());
					dsNames.push_back(dataSourceID);
					result = true;
				}
				else
				{
					ACS_ACA_LOG(LOG_LEVEL_ERROR, " Error in parsing object  '%s' datasourceID= '%s', "
							"continuing with next message store", dn.c_str(), dataSourceID.c_str());
					result = false;
					break;
				}

			} //for loop
		}
		else
		{
			//ERROR
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getClassInstances' failed, error = '%s', returning -1",
					theOmHandler.getInternalLastErrorText());

            if( apgccErrorCodes::ACS_APGCC_ERR_NOT_FOUND == theOmHandler.getInternalLastError() )
			{
				//no datasource is defined in DirectDataTransfer service
				ACS_ACA_LOG(LOG_LEVEL_INFO, "NO DATASOURC DEFINED");
				result = true;
			}
		}
	}
	else
	{
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'OmHandler.Init' failed, returning -1");
	}

	theOmHandler.Finalize();
	return result;
}

bool ACS_ACA_Common::GetLastFieldValueFromDN(const std::string& stringToParser, std::string& value)
{
	bool result = false;

	value.clear();

	// Get the last value from DN e.g: class1Id=xyz,class2Id=lmk,....
	// Split the field in RDN and Value e.g. : class1Id xyz
	size_t equalPos = stringToParser.find_first_of("=");
	size_t commaPos = stringToParser.find_first_of(",");

	// Check if some error happens
	if( (std::string::npos != equalPos) )
	{
		// check for a single field case
		if( std::string::npos == commaPos )
			value = stringToParser.substr(equalPos + 1);
		else
			value = stringToParser.substr(equalPos + 1, (commaPos - equalPos - 1) );

		result = true;
		ACS_ACA_LOG(LOG_LEVEL_INFO, "ParseSuccess string:<%s>  value <%s>",  stringToParser.c_str() ,value.c_str() );
	}
	else
	{
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ParserTag:=  not found into the string:<%s>",  stringToParser.c_str() );
	}

	return result;
}



