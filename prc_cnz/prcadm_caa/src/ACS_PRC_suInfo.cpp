/*
 * ACS_PRC_suInfo.cpp
 *
 *  Created on: Nov 17, 2010
 *      Author: xpaomaz
 */

#include "ACS_PRC_suInfo.h"
#include "ACS_TRA_Logging.h"

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

ACS_PRC_suInfo::ACS_PRC_suInfo() : runLevelSC1(0), runLevelSC2(0) {
}

ACS_PRC_suInfo::~ACS_PRC_suInfo() {
}

int ACS_PRC_suInfo::getInfo() {

	char strErr_no[1024] = {0};

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_suInfo::getInfo()- Enter", PRCBIN_REVISION);
	Logging.Write(strErr_no, LOG_LEVEL_TRACE);

	ACS_PRC_API internal;
	if (internal.omHandler_init() != 0)
	{
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_suInfo::getInfo()- getSUList failed - omHandler_init() failed", PRCBIN_REVISION);
		Logging.Write(strErr_no, LOG_LEVEL_WARN);
		Logging.Close();
		return -1;
	}

	//TR HR93829 : iterator locally defined. Using it instead of class member itSu
	vector<string>::iterator itSu_local;

	/* Get the list of Service Unit and store it in the related map */
	vector<string> listSu;
	if (internal.getSUList("", listSu) == false)
	{
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_suInfo::getInfo()- getSUList failed", PRCBIN_REVISION);
		Logging.Write(strErr_no, LOG_LEVEL_WARN);


		if (internal.omHandler_finalize() != 0)
		{
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_suInfo::getInfo()- getSUList failed - omHandler_init() failed", PRCBIN_REVISION);
			Logging.Write(strErr_no, LOG_LEVEL_WARN);
		}            

		Logging.Close();

		return -1;
	}
	//changes for TR HW49302
	vector<string> listSu_new;
	if (internal.getSUList_new("", listSu_new) == false)
	{
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_suInfo::getInfo()- getSUList_temp failed", PRCBIN_REVISION);
		Logging.Write(strErr_no, LOG_LEVEL_WARN);

		if (internal.omHandler_finalize() != 0)
		{
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_suInfo::getInfo()- getSUList failed - omHandler_init() failed", PRCBIN_REVISION);
			Logging.Write(strErr_no, LOG_LEVEL_WARN);
		}   

		Logging.Close();
		return -1;
	}


	for (itSu_local=listSu.begin(); itSu_local<listSu.end(); itSu_local++ ){
		string suName;
		string token;
		string CompName;
		string SuSide;

		/*prepare to tokenize the string*/

		/**here the string with all the DN name of SU is tokenized to estract only the name of
		 * the SU. According to Naming convention the SU name is the section of DN starting
		 * with safSu=APG_SU1_xxxx or
		 * with safSu=APG_SU1_xxxx or
		 */
		char * pch;
		char * cstr;
		cstr = new char [itSu_local->size()+1];
		strcpy (cstr, itSu_local->c_str());

		pch = strtok (cstr,",");

		size_t pos;
		while (pch != NULL){

			token.assign ( const_cast <char*> ( pch ) );

			if ( ( pos = token.find("safComp=")) != string::npos ){

				CompName = token.substr(pos);

				string CompName_with_coma = CompName + "\\,";

				pch = strtok (NULL, ",");
				// TR HR93829 : added check if pch is  NULL. if pch NULL break the while
				// and passes to the next SU string otherwise parses it.
				if (pch == NULL)
					break;
				token.assign ( const_cast <char*> ( pch ) );

				if ( (pos = token.find("safSu=")) != string::npos ){
						SuSide = token.substr ( pos + 6);
					//changes for TR HW49302

					int suState = internal.getSuState_new(listSu_new, const_cast<char*> ( CompName_with_coma.c_str()), const_cast<char*> ( SuSide.c_str()) );

					if ( suState != ACS_PRC_IMM_ERROR ){
						
        	                                if ( SuSide == "SC-1")
				    			  suStateSC1Map[CompName] = suState;
						else if ( SuSide == "1")  
							suStateSC1Map[CompName] = suState;
						else
							suStateSC2Map[CompName] = suState;
					}
				}
			}

			pch = strtok (NULL, ",");
		}/**end while on strtok*/
		delete[] cstr;
	}/*end for on Su list*/

	if (internal.omHandler_finalize() != 0)
	{
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_suInfo::getInfo()- getSUList failed - omHandler_init() failed", PRCBIN_REVISION);
		Logging.Write(strErr_no, LOG_LEVEL_WARN);
		Logging.Close();
		return -1;
	}

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,1024,"acs_prcmand %s - ACS_PRC_suInfo::getInfo()- End", PRCBIN_REVISION);
	Logging.Write(strErr_no, LOG_LEVEL_TRACE);
	Logging.Close();
	return 0; /**on success*/

}/**end getInfo*/

uint32_t ACS_PRC_suInfo::getRunLevel(string node){

	if (node == "SC-2-1" ){ /*local node requested*/
		computeRunLevelSC1();
		return runLevelSC1; /*local is sc-2-1*/
	}else{
		computeRunLevelSC2();
		return runLevelSC2; /*local is sc-2-2*/
	}
}/*end getRunLevel */

int ACS_PRC_suInfo::computeRunLevelSC1(){

	runLevelSC1 = 2;

	bool RunningSuSC1 = false;
	bool StoppedSuSC1 = false;

	map<string,int>::iterator it;

	for( it= suStateSC1Map.begin(); it != suStateSC1Map.end(); it++ ){

		if ( it->second > 0){
			RunningSuSC1 = true;
		}else{
			StoppedSuSC1= true;
		}
	}/*end for*/

	if ( RunningSuSC1 ){
		if (!StoppedSuSC1)
			runLevelSC1= 5; /*running is true and  StoppedSC1 is false:no SU is not running*/
		else
			runLevelSC1= 2;/*running is true and  StoppedSC1 is true: at least one SU is not running*/
	}
	else
		runLevelSC1 = 1;
	return 0;

}/*end getRunLevelSC1 */

int ACS_PRC_suInfo::computeRunLevelSC2(){

	runLevelSC2 = 2;

	bool RunningSuSC2=false;
	bool StoppedSuSC2=false;

	map<string,int>::iterator it;

	for(it = suStateSC2Map.begin(); it != suStateSC2Map.end(); it++ ){

		if ( it->second > 0){
			RunningSuSC2 = true;
		}else{
			StoppedSuSC2= true;
		}
	}

	/*compute runlevel*/
	if ( RunningSuSC2 ){
		if (!StoppedSuSC2)
			runLevelSC2= 5; /*running is true and  StoppedSC1 is false:no SU is not running*/
		else
			runLevelSC2= 2;/*running is true and  StoppedSC1 is true: at least one SU is not running*/
	}
	else
		runLevelSC2 = 1;
	return 0;


}/*end getRunLevelSC2 */

void ACS_PRC_suInfo::getMap ( map<string,int>* suStateMap_SC1, map<string,int>* suStateMap_SC2 ){
	*suStateMap_SC1 = suStateSC1Map;
	*suStateMap_SC2 = suStateSC2Map;
}
