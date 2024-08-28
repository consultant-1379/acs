/*
 * @file ACS_CS_TableOperationWorker.cpp
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


#include "ACS_CS_TableOperationWorker.h"


ACS_CS_TableOperationWorker::ACS_CS_TableOperationWorker(ACS_CS_TableHandler *_handler, const ACS_CS_ImModelSubset *_subset) :
    handler(_handler),
    subset(NULL)
{
    this->subset = new ACS_CS_ImModelSubset(*_subset);
}


ACS_CS_TableOperationWorker::~ACS_CS_TableOperationWorker()
{
    if (subset)
        delete subset;
}

int ACS_CS_TableOperationWorker::exec()
{
    // Perform the table operation request
    this->handler->newTableOperationRequest(this->subset);

    return 0;
}
