/*
 * @file ACS_CS_TableOperationWorker.h
 * @author xmikhal
 * @date Jan 10, 2011
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


#ifndef ACS_CS_TableOperationWorker_h
#define ACS_CS_TableOperationWorker_h 1

#include "ACS_CS_Thread.h"
#include "ACS_CS_TableHandler.h"
#include "ACS_CS_ImModelSubset.h"

class ACS_CS_TableOperationWorker : public ACS_CS_Thread
{
    public:

        ACS_CS_TableOperationWorker(ACS_CS_TableHandler *handler, const ACS_CS_ImModelSubset *subset);

        virtual ~ACS_CS_TableOperationWorker();

        virtual int exec();

    private:

        ACS_CS_TableHandler *handler;

        ACS_CS_ImModelSubset *subset;

};

#endif // ACS_CS_TableOperationWorker_h
