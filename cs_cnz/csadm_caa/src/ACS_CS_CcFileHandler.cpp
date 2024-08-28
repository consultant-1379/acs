/*
 * ACS_CS_CcFileHandler.cpp
 *
 *  Created on: Feb 5, 2013
 *      Author: eanform
 */


#include "ACS_CS_CcFileHandler.h"
#include "ACS_APGCC_CommonLib.h"



ACS_CS_CcFileHandler::ACS_CS_CcFileHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
		ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
		ACS_APGCC_CcbHandle &ccbHandleVal,
		ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal)
{
	this->oiHandle=oiHandle;
	this->ccbId=ccbId;
	this->className=0;
	this->parentName=0;
	this->attr=attr;
	this->ccbHandleVal=ccbHandleVal;
	this->adminOwnwrHandlerVal=adminOwnwrHandlerVal;
	this->base=base;

}



ACS_CS_CcFileHandler::~ACS_CS_CcFileHandler() {
	// TODO Auto-generated destructor stub
	std::cout<<"DISTRUTTORE ACS_CS_CcFileHandler\n";
}



int ACS_CS_CcFileHandler::create()
{
	std::cout<<"create from ACS_CS_CcFileHandler\n";

	if (base->type != CCFILE_T)
		return 0;

	///////////////////////////////////////////////////////////
	/// copy xml file
	///////////////////////////////////////////////////////////
	if(!ACS_CS_NEHandler::moveMmlFileToAdhFolder(false))
	{
		std::cout<<"failed to copy xml CcFile"<< endl;
		return 1;
	}

	return 0;
}

int ACS_CS_CcFileHandler::remove()
{
	if (base->type != CCFILE_T)
		return 0;

	return 0;
}

int ACS_CS_CcFileHandler::modify()
{
	return 0;
}
