#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       listtsuser
# Description:
#       A script to list troubleshooting user on APG43L node 
#
##
# Usage: 
#	listtsuser <username>
##
# Output:
#       Command print-out is printed on console.
#
##
# Changelog:
# - Thursday 28th of August 2014 - Torgny Wilhelmsson
#      Added printout of gecos data (chfn -m )
#- Fri  MAY 24 2013 -Roni Newatia (xronnew)
#       Changed Account lock implementation and format
#
# - Thu APR 18 2013 - Greeshmalatha C (xgrecha)
#	Changed the command format,usage and format of 
#	script for redability.
# - Tue Jan 16 2012 -Sonali Nanda 
#       First version.
##


# Commands used by this script
##############################
ECHO=/bin/echo
GREP=/usr/bin/grep
AWK=/usr/bin/awk

# Exit codes
################################
exit_success=0
exit_usage=2


# Setting  global variables
##############################
ts_group="tsgroup"
status=0
script_name="listtsuser"
passwordfile="/cluster/etc/passwd"

# Message functions
####################
function show_usage() {
	${ECHO} "Usage: $script_name "
	${ECHO} ""
}

#-------------------------
function usage_error() {
    ${ECHO} $1
    show_usage
    exit $2
}

#-------------------------
function parse_args() {
arg_num=`${ECHO} $#`
if [ $arg_num -ge 1 ];then
	usage_error "Incorrect usage" $exit_usage
fi
}

#-------------------------------------
function  list_tsusers() {
tsgroup_id=`${GREP} "^$ts_group:" /cluster/etc/group | ${AWK} -F: '{print $3}'` #group id of tsgroup

for usr in `${AWK} -F: '{if($4 == '$tsgroup_id') print $1}' $passwordfile`
do
	cpuserstatus=$(  cat "$passwordfile" | grep -w $usr |awk -F ":" '{ print $5}' | awk -F "," '{print $5}')
	#${ECHO} -e "$usr  $cpuserstatus"
	if [ "$cpuserstatus" == "nomml" ];then
          cpuserstatus="no-mml"
    fi
    printf "%-32s  %s\n" "$usr" "$cpuserstatus"
	status=1
done

# Check if no troubleshooting user is found
if [ $status == 0 ];then 
	${ECHO} "No troubleshooting users found"
	
fi

${ECHO} ""
exit $exit_success
}
#-------------------------------------
# _____________________ _____________________ 
#|    _ _   _  .  _    |    _ _   _  .  _    |
#|   | ) ) (_| | | )   |   | ) ) (_| | | )   |   
#|_____________________|_____________________|
# Here begins the "main" function...

# Set the interpreter to exit if a non-initialized variable is used.
set -u

#Parse command line arguments
parse_args $@

#List troubleshooting users existing on node
list_tsusers
exit $exit_success

