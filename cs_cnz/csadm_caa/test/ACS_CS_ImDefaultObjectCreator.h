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
 * @file ACS_CS_ImDefaultObjectCreator.h
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

#ifndef ACS_CS_IMDEFAULTOBJECTCREATOR_H_
#define ACS_CS_IMDEFAULTOBJECTCREATOR_H_

#include "ACS_CS_ImVlan.h"
#include "ACS_CS_ImVlanCategory.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CS_ImOtherBlade.h"
#include "ACS_CS_ImHardwareConfigurationCategory.h"
#include "ACS_CS_ImCpGroupCategory.h"
#include "ACS_CS_ImCpGroup.h"
#include "ACS_CS_ImCpCategory.h"
#include "ACS_CS_ImCpBlade.h"
#include "ACS_CS_ImCp.h"
#include "ACS_CS_ImConfigurationInfo.h"
#include "ACS_CS_ImBladeClusterInfo.h"
#include "ACS_CS_ImApServiceCategory.h"
#include "ACS_CS_ImApService.h"
#include "ACS_CS_ImApCategory.h"
#include "ACS_CS_ImApBlade.h"
#include "ACS_CS_ImAp.h"
#include "ACS_CS_ImAdvancedConfiguration.h"

class ACS_CS_ImDefaultObjectCreator
{

public:

   static ACS_CS_ImAdvancedConfiguration * createAdvancedConfigurationObject();
   static ACS_CS_ImAp * createApObject();
   static ACS_CS_ImApBlade * createApBladeObject();
   static ACS_CS_ImApCategory * createApCategoryObject();
   static ACS_CS_ImApService * createApServiceObject();
   static ACS_CS_ImApServiceCategory * createApServiceCategoryObject();
   static ACS_CS_ImBladeClusterInfo * createBladeClusterInfoObject();
   static ACS_CS_ImConfigurationInfo * createConfigurationInfoObject();
   static ACS_CS_ImCp * createCpObject();
   static ACS_CS_ImCpBlade * createCpBladeObject();
   static ACS_CS_ImCpCategory * createCpCategoryObject();
   static ACS_CS_ImCpGroup * createCpGroupObject();
   static ACS_CS_ImCpGroupCategory * createCpGroupCategoryObject();
   static ACS_CS_ImHardwareConfigurationCategory * createHardwareConfigurationCategoryObject();
   static ACS_CS_ImOtherBlade * createOtherBladeObject();
   static ACS_CS_ImShelf * createShelfObject();
   static ACS_CS_ImShelf * createShelfObjectII();
   static ACS_CS_ImVlan * createVlanObject();
   static ACS_CS_ImVlanCategory * createVlanCategoryObject();


private:

   static void initBladeObject(ACS_CS_ImBlade * obj);

};



#endif /* ACS_CS_IMDEFAULTOBJECTCREATOR_H_ */
