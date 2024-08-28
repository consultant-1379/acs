#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2024 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       permissions.sh
# Description:
#       A script to modify existing permissions of file/directory.
#
##
# Usage:
#       None
##
# Changelog:
# - Feb 29 2024 - Ramya Medichelmi (zmedram)
#       First Version
##

# Load the apos common functions.
. /opt/ap/apos/conf/apos_common.sh

apos_intro $0

#Global variables ------------------------------------------ BEGIN
CHMOD="/usr/bin/chmod"
MKTR_PATH="/data/opt/ap/internal_root/support_data/mktr"
#Global variables ------------------------------------------ END

function modify_perms {

PERMS=$1
FILE=$2
        if [[ -e $FILE ]]; then
           $CHMOD $PERMS $FILE
           [[ $? -eq 0 ]] && apos_log "successfully modified permissions for $FILE"
        fi
}

##M A I N

modify_perms 775 $MKTR_PATH

apos_outro $0
exit $TRUE 
