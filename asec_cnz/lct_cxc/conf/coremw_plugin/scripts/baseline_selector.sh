#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2018 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       baseline_selector.sh
# Description:
#       Deploy on the target machine the APG baseline according to HW type
# Note:
#       <script_notes>
##
# Usage:
#       ./baseline_selector.sh <PATH_OF_BASELINE_FILES>
##
# Output:
#       Copy the correct baseline files ( GEP7_acs_lct_sha1verify.conf,GEP5_acs_lct_sha1verify.conf, GEP12_acs_lct_sha1verify.conf or VM_acs_lct_sha1verify.conf) from the 
#		UP package to the folder /cluster/storage/system/config/acs_lct/ 
#		Rename the file in acs_lct_sha1verify.conf
##
#  Fri 11 Jan 2019 : Yeswanth Vankayala (xyesvan)
#    Changed abort to log statement
#  Mon 17 Dec 2018 : Yeswanth Vankayala (xyesvan)
#  Modified version of baseline selector from UP framework

#Loading Environment variables"

# Global Variables ------------------------------------------------------- BEGIN
TRUE=$( true; echo $? )
FALSE=$( false; echo $? )

BASELINE_PATH=''
CMD_LOGGER=''
CMD_CP=''
SOURCE_FILE=''
HW_TYPE=''

BASELINE_PATH="$(dirname "$(readlink -f $0)")" 
APOS_PATH="/opt/ap/apos/bin"
CMD_APOS_HWTYPE='/opt/ap/apos/conf/apos_hwtype.sh'
BASELINE_FILE_NAME='acs_lct_sha1verify.conf'
DESTINATION_PATH='/cluster/storage/system/config/acs_lct/'
LOG_TAG='-t baseline_selector'

# Global Variables --------------------------------------------------------- END

# The function will log and print an error message and will terminate the script
#  with a $FALSE return code.
function log(){
    local MESSAGE=$1
    log_error "$MESSAGE"		
}

# The function will log an error message in the system log. 
function log_error(){
  
    local PRIO='-p user.notice'
	local MESSAGE="${*:-notice}"	
    
    $CMD_LOGGER $PRIO $LOG_TAG $MESSAGE &>/dev/null 
}

# The function will check for the script's prerequisites to be satisfied.
function sanity_check(){
 
    CMD_LOGGER=$( which logger 2>/dev/null )
	[ -z "$CMD_LOGGER" ] && CMD_LOGGER='/bin/logger'
	
	CMD_CP=$( which cp 2>/dev/null )
	[ -z "$CMD_CP" ] && CMD_CP='/bin/cp'

	if [ ! -f $CMD_APOS_HWTYPE ]; then
		log "apos_hwtype.sh script not found!!!"
	fi
}

function get_ap_type(){
   local CMD_PARMTOOL="$APOS_PATH/parmtool/parmtool"
   [ ! -x $CMD_PARMTOOL ] && log "parmtool command not executible" 
   AP_TYPE=$( $CMD_PARMTOOL get --item-list ap_type 2>/dev/null | awk -F'=' '{print $2}')
   [ -z "$AP_TYPE" ] && log "ap_type is not found"
   echo "$AP_TYPE"        
}

function deploy_source_file() {
  local AP_TYPE=$(get_ap_type)
  local PREFIX_GEP=''
  local PREFIX_AP1='AP1_'
  local PREFIX_AP2='AP2_'

  case "$1" in 
    "GEP7")
      PREFIX_GEP='GEP7_'
    ;;
    "GEP5")
      PREFIX_GEP='GEP5_'
    ;;
    "GEP1")
      PREFIX_GEP='GEP1_'
    ;;
    "GEP2")
      PREFIX_GEP='GEP2_'
    ;;
    "VM")
      PREFIX_GEP='VM_'
    ;;
  esac

  if [ "$AP_TYPE" = 'AP1' ];then
    SOURCE_FILE=$BASELINE_PATH/$PREFIX_AP1$PREFIX_GEP$BASELINE_FILE_NAME
  else
    SOURCE_FILE=$BASELINE_PATH/$PREFIX_AP2$PREFIX_GEP$BASELINE_FILE_NAME
  fi
}

# MAIN ------------------------------------------------------------------- BEGIN

sanity_check


HW_TYPE=$($CMD_APOS_HWTYPE 2>&1)

if [[ "$HW_TYPE" == 'GEP1' ]]; then
  deploy_source_file GEP1
elif [[ $HW_TYPE == 'GEP2' ]]; then
  deploy_source_file GEP2
elif [[ "$HW_TYPE" =~ "GEP5" ]];then
  deploy_source_file GEP5
elif [[ "$HW_TYPE" =~ "GEP7" ]];then
  deploy_source_file GEP7
elif [[ "$HW_TYPE" =~ "VM" ]];then
  deploy_source_file VM
else 
  log "Hardware Type[$HW_TYPE] not supported"
fi	

if [ -f $SOURCE_FILE ]; then 
  $CMD_CP -f $SOURCE_FILE $DESTINATION_PATH/$BASELINE_FILE_NAME
else
  log "Baseline file for $HW_TYPE missing!!!"
fi	


# MAIN --------------------------------------------------------------------- END
exit $TRUE
