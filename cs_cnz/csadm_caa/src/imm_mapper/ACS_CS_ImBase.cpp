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
 * @file ACS_CS_ImBase.cpp
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

#include "ACS_CS_ImBase.h"
#include <boost/serialization/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/assume_abstract.hpp>


bool ACS_CS_ImBase::operator==(const ACS_CS_ImBase &other) const
{
   return ((typeid(*this) == typeid(other))
         &&  equals(other));
}

bool ACS_CS_ImBase::operator!=(const ACS_CS_ImBase &other) const
{
   return !(*this == other);
}


bool ACS_CS_ImBase::equals(const ACS_CS_ImBase &other) const
{
   if (this == &other)
      return true;

   if (rdn != other.rdn)
      return false;
   if (type != other.type)
      return false;
   if (action != other.action)
      return false;


   return true;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImBase & base, const unsigned int /*version*/)
      {
         ar &base.rdn;
         ar &base.type;
         ar &base.action;
      }
   }
}

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImBase);

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImBase & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImBase & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImBase & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImBase & base, const unsigned int /*version*/);

   }
}
