#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2017 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cs_transportdn.sh
# Description:
#	This script is used to delete the TransportM DN (fix for HV90938).
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
# - Thu  20 2017 - Harika Bavana (xharbav)
#       First version.


IMM_FIND="/usr/bin/immfind"
IMM_CFG="/usr/bin/immcfg"
grep='/usr/bin/grep'

APOS_COMMON="/opt/ap/apos/conf/apos_common.sh"
source $APOS_COMMON
TRANSPORTMDN="AxeTransportMtransportMId=1,systemHandlingId=1,axeFunctionsId=1"
LOG_TAG="cs_transportm"
LOGGER="/bin/logger"

#----------------------------------------------------------------------------------------
# log to system-log
function log(){
        $LOGGER -t "$LOG_TAG" "$*"
}

#----------------------------------------------------------------------------------------

log "checking TransportM existence"


		CMD="$IMM_FIND | grep -q $TRANSPORTMDN && $IMM_CFG -d $TRANSPORTMDN"
		kill_after_try 2 1 1 $CMD 2>/dev/null
exit 0
