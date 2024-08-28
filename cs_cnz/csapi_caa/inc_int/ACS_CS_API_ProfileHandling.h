/*
 * @file ACS_CS_API_ProfileHandling.h
 * @author xmikhal
 * @date Jan 9, 2011
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 */


#ifndef ACS_CS_API_ProfileHandling_h
#define ACS_CS_API_ProfileHandling_h 1

#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"
#include "ACS_CS_API_NeHandling.h"

class ACS_CS_API_ProfileHandling
{
    public:

        bool checkQueuedApaProfiles ();

        bool ignoreApaProfileRequest (ACS_CS_API_OmProfileChange& omProfileChange);

        bool rollbackOmProfile ();

        std::string convertOmProfileToString (const ACS_CS_API_OmProfileChange& omProfile);

        std::string convertReasonToString (ACS_CS_API_Set::ReasonType reason);

};

#endif // ACS_CS_API_ProfileHandling_h
