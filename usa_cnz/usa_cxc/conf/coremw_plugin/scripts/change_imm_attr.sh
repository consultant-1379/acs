#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       change_imm_attr.sh
# Description
##      A script to modifies imm attributes and changing the error code for ldap
# Usage:
#       Used during usa csp package upgrade.
##
# Output:
#       None.
##
# Changelog:
# - Thur Sep 14 2022 -P S SOUMYA(ZPSXSOU)
#   Fix for TR HZ83597
# - Mon Nov 11 2019 - Nazeema Begum (XNAZBEG)
#   Fix for TR HX33144
# - Tue Sep 18 2018 - Nazeema Begum (XNAZBEG)
#   First version.

#Global Variables

TRUE=$( true; echo $? )
FALSE=$( false; echo $? )
EXHAUSTED=255
CMD_ECHO='/usr/bin/echo'

#------------------------------------------------------------------------
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
#------------------------------------------------------------------------

# usage:
#   try <attempts> <interval> <command> [<argument1> ... <argumentN>]
#
# The function executes <command> for a maximum of <attempts> times and waits
# <interval> seconds between each attempt. It returns <command>'s return code
# upon completion, $EXHAUSTED in the case the command has failed for all
# available attempts, $FALSE in the case of wrong usage.
function try(){
  if [ $# -lt 3 ]; then
    echo "wrong number of parameters ($#)" >&2
    return $FALSE
  elif [[ ! $1 =~ ^[0-9]+$ ]]; then
    echo "positive integer expected (found \"$1\")" >&2
    return $FALSE
  elif [[ ! $2 =~ ^[0-9]+(\.[0-9]+)*$ ]]; then
    echo "positive decimal expected (found \"$2\")" >&2
    return $FALSE
  else
    local MAX_ATTEMPTS=$1
    local SLEEP_TIME=$2
    shift; shift
    local COMMANDLINE=$@

    for ((i=0; i<${MAX_ATTEMPTS}; i++)); do
      ${COMMANDLINE}
       local RETCODE=$?
      if [ $RETCODE -eq $TRUE ]; then
        return $RETCODE
      fi
      sleep ${SLEEP_TIME}
    done
    return $EXHAUSTED
  fi
}

#------------------------------------------------------------------------
# usage:
#   kill_after <timeout> <command> [<argument1> ... <argumentN>]
#
# The function executes <command> and awaits for its completion for a maximum of
# <timeout> seconds before interrupting (SIGINT) the process.
# If after <timeout>+2 seconds the process is still executing (SIGINT has not
# successfully interrupted it), SIGKILL gets sent.
# The function returns 124 if timeout has expired before command completion,
# $FALSE in the case of wrong usage or the return code of $COMMAND otherwise.
function kill_after(){
  if [ $# -lt 2 ]; then
    echo "wrong number of parameters ($#)" >&2
    return $FALSE
  elif [[ ! $1 =~ ^[0-9]+$ ]]; then
    echo "positive integer expected (found \"$1\")" >&2
    return $FALSE
  else
    local SIGINT_TMOUT=$1
    shift
    local SIGKILL_TMOUT=$((${SIGINT_TMOUT}+2))
    local COMMANDLINE=$@

    /usr/bin/timeout --signal=INT --kill-after=${SIGKILL_TMOUT} ${SIGINT_TMOUT} ${COMMANDLINE}
    return $?
  fi
}

#------------------------------------------------------------------------
# usage:
#   kill_after_try <attempts> <interval> <timeout> <command> [<argument1> ... <argumentN>]
#
# The function executes <command> for a maximum of <attempts> times and waits
# <interval> seconds between each attempt. If each command invocation does not
# terminate after <timeout> seconds, it gets interrupted (SIGINT).
# If after <timeout>+2 seconds the process is still executing (SIGINT has not
# successfully interrupted it), SIGKILL gets sent.
# The function returns 124 if timeout has expired before command completion,
# $FALSE in the case of wrong usage or the return code of $COMMAND otherwise.
function kill_after_try(){
  if [ $# -lt 4 ]; then
    $CMD_ECHO "wrong number of parameters ($#)" >&2
    return $FALSE
  else
    local MAX_ATTEMPTS=$1
    local SLEEP_TIME=$2
    local SIGINT_TMOUT=$3
    shift; shift; shift
    local COMMANDLINE=$@

    try $MAX_ATTEMPTS $SLEEP_TIME kill_after $SIGINT_TMOUT $COMMANDLINE
    return $?
  fi
}


#-------------------------------------------------------------------------------
function is_ldapclass_exist(){
  local CMD_11=$( kill_after_try 1 1 2 /usr/bin/immlist usaSupervisonId=11,usaSupervisonConfigId=1 2>/dev/null)
  [ $? -ne 0 ] && abort 'Class usaSupervisonId=11,usaSupervisonConfigId=1 does not exists'
  
  local CMD_12=$( kill_after_try 1 1 2 /usr/bin/immlist usaSupervisonId=12,usaSupervisonConfigId=1 2>/dev/null)
  [ $? -ne 0 ] && abort 'Class usaSupervisonId=12,usaSupervisonConfigId=1 does not exists'
  if [[ -n "$CMD_11" && -n "$CMD_12" ]]; then
    return $TRUE
  fi
  return $FALSE
}

#------------------------------------------------------------------------
function change_imm_attributes(){

  $CMD_ECHO "--- change_imm_attributes() begin"
  
  #Fix for TR HZ83597 : modifying the error messages  . 
  if is_ldapclass_exist; then
   /usr/bin/immcfg -a pattern="Error response received from LDAP client : -1" usaSupervisonId=11,usaSupervisonConfigId=1
   [ $? -ne 0 ] && abort 'Failure while modifying the class attribute pattern for ldap autherization failure alarm'
   /usr/bin/immcfg -a pattern="Error response received from LDAP client : -1" usaSupervisonId=12,usaSupervisonConfigId=1
   [ $? -ne 0 ] && abort 'Failure while modifying the class attribute pattern for ldap autherization failure alarm'
  fi

  $CMD_ECHO "--- change_imm_attributes() end"
}

function delTmp(){
  amf-adm lock safSi=apg.usa.service-NWA-1,safApp=ERIC-apg.usa.service || echo "Lock operation of usa.service has been failed "
  #Remove the usa.tmp file after modifying imm attributes
  if [ -f /opt/ap/acs/bin/usa.tmp ]; then
        rm -f /opt/ap/acs/bin/usa.tmp
        echo "Removed usa.tmp file..."
  else
        echo "usa.tmp file doesn't exists..."
  fi

  amf-adm unlock safSi=apg.usa.service-NWA-1,safApp=ERIC-apg.usa.service || echo "Unlock Operation of usa.service failed"

}

##              ##
##     MAIN     ##
##              ##

$CMD_ECHO "## change_imm_attributes entering... ##"
    change_imm_attributes
    "$@"
$CMD_ECHO "## change_imm_attributes end... ##"

exit $TRUE


