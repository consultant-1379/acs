#ifndef Com_Oam_Fm_Event_1_h_
#define Com_Oam_Fm_Event_1_h_

#include <ComMgmtSpiCommon.h>
#include <ComMgmtSpiInterface_1.h>
#include <ComOamSpiNotificationFm_1.h>

/**
 * The event type defines what kind of event is sent
 *  - ComOamFmNotificationEventType_1 is for Alarm and Alert
 *  - ComOamFmHeartbeatEventType_1 is for Heartbeat
 *  - ComOamFmAlarmListRebuiltEventType_1 is for AlarmListRebuilt
 */
#define ComOamFmNotificationEventType_1 "ComOamFmNotificationEventType_1"
#define ComOamFmHeartbeatEventType_1    "ComOamFmHeartbeatEventType_1"
#define ComOamFmAlarmListRebuiltEventType_1    "ComOamFmAlarmListRebuiltEventType_1"

/**
 * The filter type specifies that notification is sent only for the
 * event matching the filter type
 *
 * If the filter value is NULL, notification is sent for all events of
 * the filter type
 */
#define ComOamFmNotificationEventFilterType_1 "ComOamFmNotificationEventFilterType_1"
#define ComOamFmHeartbeatEventFilterType_1    "ComOamFmHeartbeatEventFilterType_1"
#define ComOamFmAlarmListRebuiltEventFilterType_1    "ComOamFmAlarmListRebuiltEventFilterType_1"

/**
 * Data structure for the Alarm/Alert event value
 *
 * If is stateful, it is an Alarm event, otherwise
 * it is an Alert event
 */
typedef struct ComOamFmNotificationEventValue_1 {

    // The SPI notification to FM
    ComOamSpiNotificationFmStructT* notification;

    // Extending notification SPI with
    bool stateful;  // true if alarm, false if alert

    uint64_t seqNo;
    uint64_t eventTime;

    uint32_t eventType;
    uint32_t probableCause;
    char* specificProblem;

    // Original values for this notification
    uint64_t originalSeqNo;
    uint64_t originalEventTime;
    char*    originalAdditionalText;
    ComOamSpiNotificationFmSeverityT originalSeverity;

} ComOamFmNotificationEventValue_1T;

/**
 * Data structure for the Heartbeat event value
 */
typedef struct ComOamFmHeartbeatEventValue_1 {

    char* target;  // DN for SNMP target

    // status of alarms
    uint64_t lastStatefulTimestamp;
    uint64_t lastStatefulSeqNo;

    // status of alerts
    uint64_t lastStatelessTimestamp;
    uint64_t lastStatelessSeqNo;

} ComOamFmHeartbeatEventValue_1T;

/**
 * Data structure for the AlarmListRebuilt event value
 */
typedef struct ComOamFmAlarmListRebuiltEventValue_1 {

} ComOamFmAlarmListRebuiltEventValue_1T;

#endif // Com_Oam_Fm_Event_1_h_
