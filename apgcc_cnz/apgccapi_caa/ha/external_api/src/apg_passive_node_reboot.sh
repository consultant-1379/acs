#!/bin/bash
#
# This script when invoked from any of the APG nodes, reboots the passive node.
#
#

#Global variables
#
EXIT_SUCCESS=0
EXIT_FAILURE=1
CURR_NODE_ID=0
NODE1=1
NODE2=2
CURR_NODE_STATE=0
PASSIVE_NODE_ID=0
ACTIVE=1
STANDBY=2
QUIESCED=3
QUIESCING=4
CMW_CMD_PATH=/usr/bin
LOG_DIR=/var/opt/ha
LOG_FILE=$LOG_DIR/apg_passive_node_reboot.log

# Create log location
#
mkdir -p $LOG_DIR

CURR_NODE_ID=`cat /etc/opensaf/slot_id`

#check node state
#
CURR_NODE_STATE=`immlist -a saAmfSISUHAState "safSISU=safSu=SC-$CURR_NODE_ID\,safSg=2N\,safApp=OpenSAF,safSi=SC-2N,safApp=OpenSAF" | cut -d = -f2`

echo "CURR_NODE_STATE: $CURR_NODE_STATE"

if [ "$CURR_NODE_STATE" == "1" ]; then
        echo "CURRENT NODE_STATE: ACTIVE(1)" >> $LOG_FILE
elif [ "$CURR_NODE_STATE" == "2" ]; then
        echo "CURRENT NODE_STATE: STANDBY(2)" >> $LOG_FILE
elif [ "$CURR_NODE_STATE" == "3" ]; then
        echo "CURRENT NODE_STATE: QUIESCED(3)" >> $LOG_FILE
elif [ "$CURR_NODE_STATE" == "4" ]; then
        echo "CURRENT NODE_STATE: QUIESCING(4)" >> $LOG_FILE
else
        echo "NODE_STATE: UNKNOWN($val)" >> $LOG_FILE
fi

echo "Curr Node ID: $CURR_NODE_ID" >> $LOG_FILE
echo "Curr Node State: $CURR_NODE_STATE" >> $LOG_FILE

if [ $CURR_NODE_ID -eq $NODE1 ] && [ $CURR_NODE_STATE -eq $ACTIVE ]; then
        PASSIVE_NODE_ID=$NODE2
elif [ $CURR_NODE_ID -eq $NODE1 ] && [ $CURR_NODE_STATE -eq $STANDBY ]; then
        PASSIVE_NODE_ID=$NODE1
elif [ $CURR_NODE_ID -eq $NODE2 ] && [ $CURR_NODE_STATE -eq $ACTIVE ]; then
        PASSIVE_NODE_ID=$NODE1
elif [ $CURR_NODE_ID -eq $NODE2 ] && [ $CURR_NODE_STATE -eq $STANDBY ]; then
        PASSIVE_NODE_ID=$NODE2
else
        echo "Invalid passive nodeid: $PASSIVE_NODE_ID" >> $LOG_FILE
        exit $EXIT_FAILURE
fi

$CMW_CMD_PATH/cmwea node-reboot SC-2-$PASSIVE_NODE_ID
if [ $? -ne 0 ];then
        echo "Passive Node Reboot Failed" >> $LOG_FILE
        exit $EXIT_FAILURE
fi

echo "Passive node reboot initiated" >> $LOG_FILE
exit $EXIT_SUCCESS

