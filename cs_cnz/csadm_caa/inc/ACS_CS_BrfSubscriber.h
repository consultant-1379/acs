#ifndef ACS_CS_BRFSUBSCRIBER_H_
#define ACS_CS_BRFSubscriber_H_
/*
 * ACS_CS_BrfSubscriber.h
 *
 *  Created on: APR 23, 2014
 *      Author: tcspada
 */



#include "ACS_CS_BrfPersistentDataOwner.h"
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_paramhandling.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperationtypes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_TRA_Logging.h"
#include <acs_apgcc_oihandler_V3.h>
#include <iostream>
#include <ace/Task.h>

class ACS_CS_BrfPersistentDataOwner;
class ACS_CS_BrfSubscriber: public ACE_Task_Base {
public:
        ACS_CS_BrfSubscriber(acs_apgcc_oihandler_V3 *handler, ACS_CS_BrfPersistentDataOwner* br, ACS_CS_EventHandle subscriptionEvent):
                m_oiHandlerBrfPersistentDataOwner(handler),
                m_oi_BrfPersistentDataOwner(br),
                m_subscriptionEvent(subscriptionEvent),
                m_stopEvent(ACS_CS_Event::CreateEvent(false,false,0)),
                m_svc_run(false)
        {};
        virtual ~ACS_CS_BrfSubscriber(){ACS_CS_Event::CloseEvent(m_stopEvent);};

        virtual int svc(void);

        virtual int open (void ) {activate(THR_DETACHED);return 0;};

        virtual void stop (){ACS_CS_Event::SetEvent(m_stopEvent);};

private:
        bool setBrfImplementer();

        acs_apgcc_oihandler_V3 *m_oiHandlerBrfPersistentDataOwner;
        ACS_CS_BrfPersistentDataOwner * m_oi_BrfPersistentDataOwner;
        ACS_CS_EventHandle m_subscriptionEvent;
        ACS_CS_EventHandle m_stopEvent;

        bool m_svc_run;

};

#endif /* ACS_CS_BRFSUBSCRIBER_H_ */
