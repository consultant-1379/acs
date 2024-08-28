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
 * @file ACS_CS_ImModelSubset.h
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
 * 2011-09-05  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#ifndef ACS_CS_IMMODELSUBSET_H_
#define ACS_CS_IMMODELSUBSET_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImModel.h"
#include "ACS_CC_Types.h"



class ACS_CS_ImModelSubset : public ACS_CS_ImModel {
public:


   ACS_CS_ImModelSubset();
   ACS_CS_ImModelSubset(ACS_APGCC_CcbId ccbId);
   ACS_CS_ImModelSubset(const ACS_CS_ImModelSubset &other);
   virtual ~ACS_CS_ImModelSubset();

   ACS_APGCC_CcbId getCcbId();
   bool isValid(int &errNo);

   bool isComplete;

   ACS_APGCC_CcbId ccbId;

private:



   friend class ACS_CS_ImRepository;

};



namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImModelSubset & modelSubset, const unsigned int version);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImModelSubset, "ACS_CS_ImModelSubset");


#endif /* ACS_CS_IMMODELSUBSET_H_ */
