#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       acs_lct_pwdresetlaadmin
# Description:
#       A script to wrap the invocation of the sec command to change the password of la-admin.
# Note:
#       None.
##
# Usage:
#       None.
##
# Output:
#       None.
##
# Changelog:
# - Tue Sep 12 2018 - Gnaneswara Seshu (zbhegna)
#       changes done for ESM adaptations(AMF related changes).
# - Tue Mar 17 2016 - Antonio Buonocunto (eanbuon)
#       First version.
##

CHECK_USER="$(/usr/bin/id --user --name)"
EXECUTABLE_NAME="$(basename $0)"
SEC_LA_PASSWD="/opt/eric/sec-la-cxp9026994/bin/sec-la-passwd"
OPT_HASH="$1"
OPT_HASH_ARG="$2"
LA_ADMINISTRATOR="laadmin"
is_swm_2_0="/cluster/storage/system/config/apos/swm_version"
function print_usage(){
  echo -e "Usage:  pwdresetlaadmin [--hash | -h <hashed password>]"
  echo
  echo "Change password for $LA_ADMINISTRATOR"
}

function print_screen(){
  echo "$1"
}
function abort(){
  /bin/logger -t $EXECUTABLE_NAME "$1"
  exit 1
}

if [ ! -z "$OPT_HASH" ];then
  if [ "$OPT_HASH" = "-h" ] || [ "$OPT_HASH" = "--hash" ];then
    if [ -z "$OPT_HASH_ARG" ];then
      print_screen "Invalid usage"
      print_usage
      abort "Invalid usage"
    else
      OPT_HASH="TRUE"
    fi
  else
    print_screen "Invalid usage"
    print_usage
    abort "Invalid usage"
  fi
fi

# MAIN

if [ -z "$CHECK_USER" ];then
  print_screen "Internal fault"
  abort "Failure while fetching user information"
fi
#Check if SEC_LA_PASSWD is executable
if [ ! -x "$SEC_LA_PASSWD" ];then
  print_screen "Internal fault"
  abort "Command $SEC_LA_PASSWD not executable"
fi

#GET ACTIVE SEC LA NODE:
ACTIVE_SEC_LA_NODE=""
NODE_A_SEC_LA_STATUS=""
NODE_A_SEC_LA_STATUS=""
if [ -f $is_swm_2_0 ];then
	NODE_A_SEC_LA_STATUS=$(immlist -a saAmfSISUHAState safSISU=safSu=SC-1\\,safSg=2N\\,safApp=ERIC-sec.la.aggregation,safSi=sec.la.aggregation-2N-1,safApp=ERIC-sec.la.aggregation | awk -F'=' '{print $2}')
	NODE_B_SEC_LA_STATUS=$(immlist -a saAmfSISUHAState safSISU=safSu=SC-2\\,safSg=2N\\,safApp=ERIC-sec.la.aggregation,safSi=sec.la.aggregation-2N-1,safApp=ERIC-sec.la.aggregation | awk -F'=' '{print $2}')

else
	NODE_A_SEC_LA_STATUS=$(immlist -a saAmfSISUHAState safSISU=safSu=SC-1\\,safSg=2N\\,safApp=ERIC-SecLa,safSi=2N-0,safApp=ERIC-SecLa | awk -F'=' '{print $2}')
	NODE_B_SEC_LA_STATUS=$(immlist -a saAmfSISUHAState safSISU=safSu=SC-2\\,safSg=2N\\,safApp=ERIC-SecLa,safSi=2N-0,safApp=ERIC-SecLa | awk -F'=' '{print $2}')
fi
if [ $NODE_A_SEC_LA_STATUS -eq 1 ];then
  ACTIVE_SEC_LA_NODE="SC-2-1"
elif [ $NODE_B_SEC_LA_STATUS -eq 1 ];then
  ACTIVE_SEC_LA_NODE="SC-2-2"
else
  print_screen "Internal fault"
  abort "Failure while fetching SEC LA role"
fi

if [ "$CHECK_USER" = "root" ] || [ "$CHECK_USER" = "tsadmin" ];then
  if [ "$SUDO_USER" = "" ] || [ "$SUDO_USER" = "tsadmin" ] || [ "$SUDO_USER" = "root" ];then
    if [ "$OPT_HASH" = "TRUE" ];then
      ssh $ACTIVE_SEC_LA_NODE "/usr/bin/sudo $SEC_LA_PASSWD $LA_ADMINISTRATOR -h '$OPT_HASH_ARG'" &> /dev/null
    else
      echo -n "New password:"
      read -s PASSWORD_1
      echo ""
      echo -n "Re-enter new password:"
      read -s PASSWORD_2
      echo ""
      if [ "$PASSWORD_1" != "$PASSWORD_2" ];then
          print_screen "passwords do not match"
          abort "passwords do not match"
      fi
      PASSWORD_HASH="$(/usr/bin/python -c "import crypt, getpass, pwd; print crypt.crypt('$PASSWORD_1', '\$6\$10\$')")"
      ssh $ACTIVE_SEC_LA_NODE "/usr/bin/sudo /opt/ap/acs/bin/acs_lct_pwdresetlaadmin.sh -h '$PASSWORD_HASH'"
    fi
    if [ $? -ne 0 ];then
      print_screen "Internal fault"
      abort "Failure while changing password for la-admin"
    fi
  else
    print_screen "ERROR: Not enough privileges to use this command"
    abort "ERROR: command $EXECUTABLE_NAME executed with user $CHECK_USER and sudo $SUDO_USER"
  fi
else
  print_screen "ERROR: Not enough privileges to use this command"
  abort "ERROR: command $EXECUTABLE_NAME executed with user $CHECK_USER"
fi

