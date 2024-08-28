#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2022 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       apg_sw_control.sh
# Description:
#       A script to control the APG43L SW Upgrade on GEP2
#
##
# Usage:
#       None
##
# Changelog:
# - Mar 10 2023 - Koti Kiran Maddi(zktmoad)
#	Script Rework
# - Oct 07 2022 - Swapnika Baradi(xswapba)
##

# Load the apos common functions.
. /opt/ap/apos/conf/apos_common.sh

apos_intro $0

CLUSTER_MI_PATH="/cluster/mi/installation"
CMD_HWTYPE='/opt/ap/apos/conf/apos_hwtype.sh'
[[ -x $CMD_HWTYPE ]] && HW_TYPE=$($CMD_HWTYPE)
[ -z "$HW_TYPE" ] && apos_abort 'Hardware type found NULL'

apos_log "Detected hardware type $HW_TYPE"

if [[ "$HW_TYPE" == "GEP2" ]]; then
    apos_abort "The package is not applicable on GEP2 node"
fi

apos_outro $0
exit $TRUE

