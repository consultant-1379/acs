#!/bin/bash
# ------------------------------------------------------------------------
#     Copyright (C) 2016 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       prc_restart.sh
# Description:
#          Restart acs_prcmand.service, acs_prcispd.service
#	   during migration from SwM 1.0 to Swm 2.0
##
# Usage:
#       ./prc_restart.sh
##
# Output:
#       None.
##
# Changelog:
# - Wed Oct 31 2018 - Gnaneswara Seshu (zbhegna)
#       First version.
##
SYSTEMCTL_CMD=systemctl
MANDSERVICE=acs_prcmand.service
ISPDSERVICE=acs_prcispd.service

log() {
        /bin/logger -t $prog "$@"
}
$SYSTEMCTL_CMD restart $MANDSERVICE
if [ echo $? -ne 0 ];then 
	log " Unable to restart $MANDSERVICE"
	exit 1
fi
$SYSTEMCTL_CMD restart $ISPDSERVICE
if [ echo $? -ne 0 ];then 
	log " Unable to restart $ISPDSERVICE"
	exit 1
fi
