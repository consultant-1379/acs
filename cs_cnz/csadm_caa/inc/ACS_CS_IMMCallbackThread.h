//  *********************************************************
//   COPYRIGHT Ericsson 2010.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2010.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2010 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#ifndef ACS_CS_IMMCallbackThread_h_
#define ACS_CS_IMMCallbackThread_h_ 1

#include "ACS_CS_Thread.h"
#include "saImmOi.h"
#include "saImmOm.h"
#include <acs_apgcc_objectimplementerinterface_V2.h>
#include <acs_apgcc_objectimplementerinterface_V3.h>

class ACS_CS_IMMCallbackThread : public ACS_CS_Thread
{
    public:
        ACS_CS_IMMCallbackThread(int fd, acs_apgcc_objectimplementerinterface_V3 *handle);

        virtual int exec ();

    private:
        int fd_;
        acs_apgcc_objectimplementerinterface_V3 *handle_;
};

#endif // ACS_CS_IMMCallbackThread_h_
