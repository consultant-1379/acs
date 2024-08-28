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
 * @file ACS_CS_ImEntryIdHelper.h
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
 * 2011-11-23  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#ifndef ACS_CS_IMENTRYIDHELPER_H_
#define ACS_CS_IMENTRYIDHELPER_H_

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImTypes.h"

class ACS_CS_ImEntryIdHelper {
public:
   ACS_CS_ImEntryIdHelper();
   virtual ~ACS_CS_ImEntryIdHelper();

   static bool calculateEntryIdNumber(const ACS_CS_ImModel *model, ClassType type, uint16_t &entryId);
   static uint16_t getNewEntryId();


};

#endif /* ACS_CS_IMENTRYIDHELPER_H_ */
