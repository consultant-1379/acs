#include <ComMgmtSpiCommon.h>
#include <ComMgmtSpiComponent_1.h>
#include <ComOamSpiEvent_1.h>
#include <ComOamSpiNotificationFm_1.h>
#include <ComMgmtSpiInterfacePortalAccessor.h>
#include <ComMgmtSpiInterfacePortal_1.h>
#include <ComMgmtSpiInterface_1.h>

#include <stdint.h>

#include "ACS_TRA_Logging.h"


class FmApg
{
public:

    FmApg();
    ~FmApg();

    static const uint64_t NANOS_PER_SEC = 1000000000L;
    
    ComReturnT init(ComMgmtSpiInterfacePortalAccessorT* accessor, const char* config);
    ComReturnT terminate();

    ComReturnT start();
    ComReturnT stop();

    ComReturnT notify(ComOamSpiEventConsumerHandleT handle, const char* eventType, ComNameValuePairT** filter, void* value);
    
    static FmApg& getInstance(void);

private:

    ComMgmtSpiComponent_1T _component;
    ComMgmtSpiInterface_1T* _ifArray[1];
    ComMgmtSpiInterface_1T* _depArray[4];

    // pointer to event router interface, consumer interface and consumer handle
    ComOamSpiEventRouter_1T* _eventRouter;
    ComOamSpiEventConsumer_1T _eventConsumer; 
    ComOamSpiEventConsumerHandleT _consumerHandle;
    
    ComNameValuePairT _fmNotificationFilter;
    ComNameValuePairT* _fmNotificationFilters[2];

    ComMgmtSpiInterfacePortal_1T* _portal;
    //ComReturnT loadConfguration( const std::string& config );

    static FmApg _instance;
    std::string getSeverityStr( ComOamSpiNotificationFmSeverityT severity );
    std::string getTimeStr( uint64_t datetime );
    ACS_TRA_Logging log;
};

