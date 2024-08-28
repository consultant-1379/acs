#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       pwdmodtsuser
# Description:
#       A script to remove troubleshooting user on APG43L node  
#
##
# Usage: 
#			pwdmodtsuser
##
# Output:
#       Command print-out is printed on console.
#
##
# Changelog:
#
#- Fri  MAY 24 2013 -Roni Newatia (xronnew)
#       Changed Account lock implementation and format
#
#- Thur APR 18 2013 -Greeshmalatha C (xgrecha)
#	Changed the command format,usage and format of 
#	script for redability.
# - Tue Jan 16 2012 -Sonali Nanda 
#       First version.
##

# Commands used by this script
##############################
ECHO=/bin/echo
PASSWD="/usr/bin/passwd"
CAT="/bin/cat"
CHMOD="/bin/chmod"
CHAGE="sudo /usr/bin/chage"

# Exit codes
####################
exit_success=0
exit_usage=2
exit_pwd_fail=3
exit_expiry_fail=4

# Setting  global variables
###########################
script_name="pwdmodtsuser"
#username=`id -nu`

# Message functions
###################
function show_usage() {
    ${ECHO} "Usage: $script_name"
    ${ECHO} ""
}
#------------------------------------
function usage_error() {
    ${ECHO} "$1"
    show_usage
    exit 2
}
#------------------------------------
function error() {
    ${ECHO} "$1"
    ${ECHO} ""
    
    exit 2
}
#------------------------------------
function parse_args() {

# Check command line arguments and set up internal variables
if [ $#  -gt 1 ];then
        usage_error "Incorrect usage" $exit_usage
fi
if [ $# -eq 1 ];then
        case "$1" in
                -h)
                        show_usage
                        exit $exit_success
                        ;;

                *)
                        usage_error "Incorrect usage" $exit_usage
                        ;;
        esac
fi


}

#--------------------------------------
function passwd_mod() {
${PASSWD}
if [ $? != 0 ];then
	error "ERROR: Password could not be changed" $exit_pwd_fail
fi
#account_lock
exit $exit_success
}
#-------------------------------------------------------------------------
#function account_lock() {
#        expiry=5 #Expiry is set according to the model.
#	old_date=`date +"%Y-%m-%d %H:%M:%S"`
#        new_date=`date -d "$old_date $expiry days" +%Y-%m-%d`
#         ${CHAGE} -E $new_date $username &> /dev/null
#        if [ $? != 0 ];then
#                 error "ERROR: User add failed as setting expiry information failed"  $exit_expiry_fail
#        fi
#}
#-------------------------------------------------------------------------
# _____________________ _____________________ 
#|    _ _   _  .  _    |    _ _   _  .  _    |
#|   | ) ) (_| | | )   |   | ) ) (_| | | )   |   
#|_____________________|_____________________|
# Here begins the "main" function...

# Set the interpreter to exit if a non-initialized variable is used.
set -u
#set -x
#Parse command line arguments
parse_args $*

#Modify password
passwd_mod

exit $exit_success
