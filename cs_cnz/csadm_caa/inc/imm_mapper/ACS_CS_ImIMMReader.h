/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2010
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImBase.h
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XBJOAXE
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2011-08-29  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/
#ifndef ACS_CS_IMIMMREADER_H_
#define ACS_CS_IMIMMREADER_H_

#include <vector>
#include "ACS_CS_ImModel.h"
#include "acs_apgcc_omhandler.h"

#include "ACS_CS_ImVlan.h"

extern "C" {
#include <saImmOm.h>
#include <saImm.h>
}

class ACS_CS_ImIMMReader {
public:

	ACS_CS_ImIMMReader();
	~ACS_CS_ImIMMReader();

	bool loadModel(ACS_CS_ImModel *model);
	bool loadModel(ACS_CS_ImModel *model, const std::string &rootRdn, ACS_APGCC_ScopeT scope) const;
	int loadBladeObjects(ACS_CS_ImModel *model);
	int loadCpObjects(ACS_CS_ImModel *model);
	int loadBladeShelfObjects(ACS_CS_ImModel *model);
	static ACS_CS_ImBase * getObject(const string &rdn);
	static ACS_CS_ImBase * getObject(const string &rdn, OmHandler &immHandle);

	static std::vector<std::string> getChildren(const string &rdn, ACS_APGCC_ScopeT scope);
	static std::vector<std::string> getClassInstancesRdns(const std::vector<std::string> &classNames);
	static bool populateModel(ACS_CS_ImModel *model, const std::vector<std::string> &rdn_list);
	static void printChildren(const char* p_rootName, const std::vector<std::string> &p_rdnList);
	static bool populateModel(ACS_CS_ImModel *model, OmHandler &immHandle, const std::vector<std::string> &rdn_list);

private:

//   ACS_CS_ImModel *model;
//   string root;
	std::vector<std::string> classNames;
};

#endif /* ACS_CS_IMIMMREADER_H_ */
