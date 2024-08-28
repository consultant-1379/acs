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
 * @file ACS_CS_ImModelSubset.cpp
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
//#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImModelSubset.h"
#include "ACS_CS_ImRepository.h"

#include "ACS_CS_Trace.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

ACS_CS_Trace_TDEF(ACS_CS_ImModelSubset_TRACE);

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImModelSubset);

ACS_CS_ImModelSubset::ACS_CS_ImModelSubset()
   : isComplete(false), ccbId(0)
{}

ACS_CS_ImModelSubset::ACS_CS_ImModelSubset(ACS_APGCC_CcbId ccbId)
   : isComplete(false), ccbId(ccbId)
{}

ACS_CS_ImModelSubset::ACS_CS_ImModelSubset(const ACS_CS_ImModelSubset &other):
		ACS_CS_ImModel(other),
		isComplete(other.isComplete),
		ccbId(other.ccbId)

{}

ACS_CS_ImModelSubset::~ACS_CS_ImModelSubset()
{}

ACS_APGCC_CcbId ACS_CS_ImModelSubset::getCcbId()
{
   return ccbId;
}

bool ACS_CS_ImModelSubset::isValid(int &errNo)
{
	ACS_CS_TRACE((ACS_CS_ImModelSubset_TRACE, "ACS_CS_ImModelSubset::isValid() - ENTER"));

	bool result = true;

	///////////////////////////////////////////////////////////////////////////////
	/// Check shelf object: Should not allow deletion if it has blade children! ///
	///////////////////////////////////////////////////////////////////////////////

	set<const ACS_CS_ImBase *>::iterator shelfIt;
	std::set<const ACS_CS_ImBase *> shelfObjects;
	std::set<const ACS_CS_ImBase *> bladeObjects;

	getObjects(shelfObjects, SHELF_T);
	getObjects(bladeObjects, BLADE_T);

	for (shelfIt = shelfObjects.begin(); shelfIt != shelfObjects.end(); shelfIt++)
	{
		const ACS_CS_ImBase * shelfBase = *shelfIt;
		const ACS_CS_ImShelf * shelf = dynamic_cast<const ACS_CS_ImShelf*>(shelfBase);

		// Marked DELETE ?
		if (shelf && shelf->action == ACS_CS_ImBase::DELETE)
		{
			set<const ACS_CS_ImBase *>::iterator bladeIt;

			// Check if blades in subset belongs to deleted Shelf
			for (bladeIt = bladeObjects.begin(); bladeIt != bladeObjects.end(); bladeIt++)
			{
				const ACS_CS_ImBase * bladeBase = *bladeIt;
				const ACS_CS_ImBlade * blade = dynamic_cast<const ACS_CS_ImBlade*>(bladeBase);

				if (blade)
				{
					ACS_CS_ImBase *parentShelf = getParentOf(blade->rdn);

					if(parentShelf && parentShelf->rdn.compare(shelf->rdn) == 0)
					{
						ACS_CS_FTRACE((ACS_CS_ImModelSubset_TRACE, LOG_LEVEL_INFO,
								"ACS_CS_ImModelSubset::isValid()\n"
								"Detected try to delete non empty Shelf object. Returning not valid and exit!"
						));

						errNo = TC_SHELFNOTEMPTY;
						return false;
					}
				}
			}
		}
	}


	set<const ACS_CS_ImBase *>::iterator bladeIt;

	//Reject creation of CMX in case SCX in APZ magazine is missing
	for (bladeIt = bladeObjects.begin(); bladeIt != bladeObjects.end(); bladeIt++)
	{
		const ACS_CS_ImBlade * blade = dynamic_cast<const ACS_CS_ImBlade*>(*bladeIt);
		if (blade && blade->functionalBoardName == CMXB && blade->action == ACS_CS_ImBase::CREATE)
		{
			ACS_CS_TRACE((ACS_CS_ImModelSubset_TRACE, "%s@%d Checking creation of CMX board", __FUNCTION__, __LINE__));

			const ACS_CS_ImBlade * scx0 = 0;
			const ACS_CS_ImBlade * scx25 = 0;

			ACS_CS_ImModel * allModel = ACS_CS_ImRepository::instance()->getModelCopy();

			if (allModel) {
				allModel->applySubset(this);
				allModel->getApzMagazineScx(scx0, scx25);

				if (!scx0 || !scx25) {
					ACS_CS_TRACE((ACS_CS_ImModelSubset_TRACE, "%s@%d SCX missing in APZ magazine", __FUNCTION__, __LINE__));
					errNo = TC_MISSINGSCXINAPZMAGAZINE;
					result = false;
				}

				delete allModel;
			}

		}
	}

	///*************************************************************************///

	// result = ACS_CS_ImModel::isValid(errNo);

	ACS_CS_TRACE((ACS_CS_ImModelSubset_TRACE, "ACS_CS_ImModelSubset::isValid() - EXIT"));

	return result;
}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImModelSubset & modelSubset, const unsigned int version)
      {
         ar & boost::serialization::base_object<ACS_CS_ImModel>(modelSubset);
         ar & modelSubset.isComplete;
         ar & modelSubset.ccbId;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImModelSubset & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImModelSubset & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImModelSubset & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImModelSubset & base, const unsigned int /*version*/);
   }
}
