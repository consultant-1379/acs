#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2020 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       configure_tsuser.sh
# Description:
#       Deploy on the SC-2-1 node to configure ts_user properties during 
#       initial deployment process.
##
# Usage:
#       ./configure_tsuser.sh
##
##
#  Fri 17 Jul 2020 : Uppada Pratap Reddy (xpraupp)
#     First draft version

# Global Variables ------------------------------------------------------- BEGIN
CMD_LOGGER=''
HW_TYPE=''
CMD_APOS_HWTYPE='/opt/ap/apos/conf/apos_hwtype.sh'
LOG_TAG='-t config_ts_user'
# Global Variables --------------------------------------------------------- END

function abort(){
  local ERROR_STRING=""
  if [ "$1" ]; then
    ERROR_STRING="ERROR: $1"
    echo "$ERROR_STRING"
  fi
  echo "ABORTING..."
  echo ""
  exit 1
}

# The function will log and print an error message and will terminate the script
#  with a $FALSE return code.
function log(){
    local MESSAGE=$1
    local PRIO='-p user.notice'
    local MESSAGE="${*:-notice}"
    $CMD_LOGGER $PRIO $LOG_TAG $MESSAGE &>/dev/null
}

# The function will check for the script's prerequisites to be satisfied.
function sanity_check(){

  CMD_LOGGER=$( which logger 2>/dev/null )
  [ -z "$CMD_LOGGER" ] && CMD_LOGGER='/bin/logger'

  HW_TYPE=$($CMD_APOS_HWTYPE 2>&1)
  [ -z "$HW_TYPE" ] && abort 'HW_TYPE found NULL'
  
}

function set_tsuser_properties() {
  local SSH_LOGIN_FILE='/cluster/etc/login.allow'
  local expiry=2
  local TS_USER='ts_user'

  log "Setting ts_user properties in-progress..."
 
  # verify, if ts_user is already defined or not on the node
  /usr/bin/getent passwd $TS_USER &>/dev/null
  if [ $? -ne 0 ]; then
    abort "ts_user does not exist on the node"
  else
    log "ts_user exist on the node"
  fi

  # setting default password to "ts_user1@"
  echo  "$TS_USER:ts_user1@" | /usr/sbin/chpasswd 2>/dev/null
  if [ $? -eq 0 ]; then
    /usr/bin/passwd -e "$TS_USER" 2>/dev/null || abort "Failed to force ts_user to set new password"
  else
    log "setting default password for ts_user success"
  fi

  # set account expiry to 1 day
  local old_date=$(date +"%y-%m-%d %H:%M:%S")
  local new_date=$(date -d "$old_date $expiry day" +%y-%m-%d)
  /usr/bin/chage -E $new_date "$TS_USER" || abort "Failed to set expiry information for ts_user"
  log "setting account expiry for ts_user success"

  # Adding ts_user to login.allow file
  echo "$TS_USER all" >>${SSH_LOGIN_FILE}
  log "Setting ts_user properties done."
}

## M A I N 

sanity_check

if [[ -n "$HW_TYPE" && "$HW_TYPE" =~ "VM" ]];then
  set_tsuser_properties  
fi 

exit 0
