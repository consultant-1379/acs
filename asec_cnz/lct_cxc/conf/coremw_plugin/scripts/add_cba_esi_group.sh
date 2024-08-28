#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2022 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       add_cba-esi_group
# Description:
#       A script to add existing TS users into cba_esi group
#
##
# Usage:
#	None
##
# Changelog:
# - Aug 18 2022 - Sowjanya Medak (xsowmed)
##

#!/bin/bash

USER_MGMT_SCRIPT=/opt/ap/apos/bin/usermgmt/usermgmt
USERMOD="${USER_MGMT_SCRIPT} user modify"
LOGDIR=/var/log/acs/tra/logging
LOG_FILE=$LOGDIR/ACS_LCTBIN.log
ECHO=/bin/echo

tsusers=$(/opt/ap/acs/bin/acs_lct_listtsuser.sh | /usr/bin/awk '{print $1;}')

function log() {
        ${ECHO} "[$(date +'%Y-%m-%d %H:%M:%S')] $@" >>$LOG_FILE
}


if [ "$tsusers" == "No" ]; then
        log "$tsusers not found!"
else
        #ts user found, so check for cba-esi group
        for username in $tsusers;do
                if [ $(getent group cba-esi) ];then
			#cba-esi group found, append ts user to group
                        log "$username and cba-esi group found, appending group to $username!"
                        ${USERMOD} --appendgroup --secgroups=cba-esi --uname=$username
                        rcode=$?
                        if [ $rcode -ne 0 ];then
                                log "Failed to add $username into cba-esi group."
                                exit 1
                        fi
                else
                        log "cba-esi group not found!"
                fi
        done
fi
