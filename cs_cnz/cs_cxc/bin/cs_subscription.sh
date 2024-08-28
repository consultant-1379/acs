#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cs_subscription.sh
# Description:
#	This script is to subscribe the CS to receive notification at service restart.
# Note:
#	None.
##
# Usage:
#	None.
##
# Output:
#       None.
##
# Changelog:
# - Mon Nov 21 2016 - eanform
#       First version.

APOS_COMMON="/opt/ap/apos/conf/apos_common.sh"
source $APOS_COMMON

CMD_IPTABLES="/usr/bin/immadm -o 100 AxeEquipmentequipmentMId=1"

function abort() {
        if [ $# -eq 2 ]; then
                log "Aborted ($2)" 
                if [[ $1 =~ [0-9]+ ]]; then
                        exit $1
                else
                        exit 1
                fi
        else
                exit 1
        fi
}

#log to system-log
function log(){
        /bin/logger -t "$LOG_TAG" "$*"
}

kill_after_try 5 5 6 $CMD_IPTABLES 2>/dev/null || abort 1 'CS Subscription service failed.'

exit 0
