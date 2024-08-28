#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2018 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cs_helper.sh
##
# Description:
#       A script to load complete CSBIN model during Maiden Installation(MI) 
##
# Changelog:

# -  24 Jul 2019 - Sankara Jayanth (xsansud)
#    First version

procInit() {
     logger "cs_helper.sh: Executing ($SDP_VER) - linking CSBIN_R1 to IMM_R1"
     `cmw-modeltype-link CSBIN_R1 IMM_R1`
     `sleep 3`
}

procWrapup() {
     logger "cs_helper.sh: Verifying Function MO instances creation under Apg=AP1"
     `immfind -c AxeFunctionDistributionFunction 2>&1 | logger`
     logger "cs_helper.sh: Unlinking CSBIN_R1 from IMM_R1"
     `cmw-modeltype-unlink CSBIN_R1`
}

case $1 in
    init)
        SDP_VER=$2
        procInit
        ;;
    wrapup)
	procWrapup
	;;
esac

