#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cmw_ccb_patch.sh
# Description:
#       A script to fix upgrade issue (TR IA4189 )
# Note:
#       None.
##
# Changelog:
# - Wed Wed 28 2023 - P S Soumya(zpsxsou)
# First version.
##


TRUE=$( true; echo $? )
FALSE=$( false; echo $? )
CMD_LOGGER="/bin/logger"
base_dir=$(/usr/bin/dirname $0)
CCB_PATH="/usr/lib/python2.7/site-packages/pyosaf/utils/immom/"
CCB_FILE="ccb.py"

function log(){
    local MESSAGE=$1
    $CMD_LOGGER "$MESSAGE"
}

cp $base_dir/$CCB_FILE $CCB_PATH
[ $? -ne 0 ] && log "Failed to copy $CCB_FILE to $CCB_PATH"
log "Succesfully from $CCB_FILE to $CCB_PATH"

exit 0

