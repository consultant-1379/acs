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
 *----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImIPAssignmentHelper.h
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

#ifndef ACS_CS_IMIPASSIGNMENTHELPER_H_
#define ACS_CS_IMIPASSIGNMENTHELPER_H_

#include "ACS_CS_ImModel.h"


class ACS_CS_ImIPAssignmentHelper {
public:
   ACS_CS_ImIPAssignmentHelper();
   virtual ~ACS_CS_ImIPAssignmentHelper();

   bool assignIp(const ACS_CS_ImModel *model, ACS_CS_ImBlade *blade);
   bool storeStaticIpToDisk(const ACS_CS_ImModel *model);
   bool readStaticIpFromDisk(std::set<std::string> &table);


private:
   bool assignIp(ACS_CS_ImModel *model, std::string &etha, std::string &ethb);
   bool assignIp(ACS_CS_ImModel *model, std::string bladeRdn);
   bool getIPfromTable(std::set<std::string> &table, std::string shelf, uint16_t slot, std::string &ethA, std::string &ethB);

};

#endif /* ACS_CS_IMIPASSIGNMENTHELPER_H_ */
