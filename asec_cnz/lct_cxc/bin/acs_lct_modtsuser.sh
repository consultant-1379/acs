#!/bin/bash
##
# ------------------------------------------------------------------------
#    Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       modtsuser
# Description:
#       A script to modufy expiry troubleshooting user on APG43L node  
#
##
# Usage: 
#			modtsuser -e expiry <username>
##
# Output:
#       Command print-out is printed on console.
#
##
# Changelog:
#
# - Thu 16th of June 2016 - Roni Newatia(XRONNEW)
#       Removed upper limit check for maximumAccountAge value
# - Tue 27th of October 2015 - EANFORM
#       Adding -1 as never expiration account
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
ECHO="/bin/echo"
USERADD="/usr/sbin/useradd"
PASSWD="/usr/bin/passwd"
STTY="/bin/stty"
GREP="/usr/bin/grep"
AWK="/usr/bin/awk"
CHAGE="/usr/bin/chage"
USERDEL="/usr/sbin/userdel"
SED="/usr/bin/sed"
USERMOD="/usr/sbin/usermod"
RM="/bin/rm"
CP="/bin/cp"
MV="/bin/mv"
CHMOD="/bin/chmod"
CMD_CAT="/bin/cat"
CMD_IMMLIST="/usr/bin/immlist"
LOGIN_DEFS="/etc/login.defs"
SHADOW_FILE="/etc/shadow"

# Exit codes
############
exit_success=0
exit_usage=2
exit_privilege=3
exit_expiry_info=4
exit_user_exists=5
exit_expiry_fail=6
exit_pwd_fail=7
exit_usermod=8
exit_error=9


# Setting  global variables
##########################
expiry_info=0
root_user="root"
root_group="root"
ts_group="tsgroup"
admin="tsadmin"
df_maxdays=5
maxdays=0
maxdays_max=14 #maximum allowed value for maxdays
mindays=0
df_mindays=2
maxacntdays="0"
df_maxacntdays="5"
never_expire="50000"
sg_info=0
secondary_group=""
script_name="modtsuser"
Integer_check=""
linux_date="1970-01-01"
# Message functions
####################
function show_usage() {
        ${ECHO} "Usage: $script_name -e expiry username"
        ${ECHO} "       $script_name -h"
	${ECHO} ""
}
function usage_error() {
    ${ECHO} "$1"
    show_usage
    exit $2
}

function error() {
    ${ECHO} "$1"
    ${ECHO} ""
    exit $2
}

# Other sript related functions

#-------------------------------------
function privilege_check() {
        userid=$(id -u)
	tsadmin_id=$(${GREP} "^$admin:" /cluster/etc/passwd | ${AWK} -F: '{print $3}')
	root_id=$(${GREP} "^$root_user:" /etc/passwd | ${AWK} -F: '{print $3}')
        if [ "$userid" != "$tsadmin_id" ] && [ "$userid" != "$root_id" ];then
                error "ERROR: Not enough privileges to use this command" $exit_privilege
        fi
}

#-------------------------------------
function expiry_info_set() {
 	expiry=$1
	if [[ $expiry != "-1" ]] ; then
		Integer_check=$(echo $expiry| tr -d "[0-9]")
		! [[ -z $Integer_check ]] || [[ $expiry -eq 0 ]] || [[ $expiry -gt $maxacntdays ]] &&
		error "ERROR:Invalid expiry information provided" $exit_expiry_info			
	else
		expiry=$never_expire
	fi
}
#-------------------------------------------------------
function fetch_acnt_days() {
maxacntdays=$($CMD_IMMLIST -a maximumAccountAge AxeLocalTsUsersPolicylocalTsUsersPolicyMId=1  | awk -F'=' '{print $2}')
[ -z $maxacntdays ] && maxacntdays=$df_maxacntdays
}

#--------------------------------------------------------
function fetch_max_days() {
      maxdays=$(${CMD_CAT} $LOGIN_DEFS | ${GREP} ^PASS_MAX_DAYS | ${AWK} '{print $2}' 2>/dev/null)
      [ -z $maxdays ] && maxdays=$df_maxdays
}

#-----------------------------------------------------------
function fetch_min_days() {
     mindays=$(${CMD_CAT} $LOGIN_DEFS | ${GREP} ^PASS_MIN_DAYS | ${AWK} '{print $2}' 2>/dev/null)
     [ -z $mindays ] && mindays=$df_mindays
}

#-----------------------------------------------------
function parse_args() {
arg_str=`${ECHO} $@`
arg_num=`${ECHO} $#`

# Check command line arguments and set up internal variables
#
if [[ $arg_num -gt 0 ]];then
    case "$1" in
            -h)
                if [ $arg_num -ge 2 ];then
                        usage_error "Incorrect usage" $exit_usage
                fi

                show_usage
                exit $exit_success
                ;;
            -e)
           	if [[ $arg_num -gt 3 ]];then
            		usage_error "Incorrect usage" $exit_usage
            	elif [[ $arg_num -eq 3 ]];then
            		username=$3
				check_tsuser
            		TEMP=$(getopt e:g: $@)
			#exit from code if invalid arguments
	                if [ $? != 0 ]; then
  		                  usage_error "Incorrect usage" $exit_usage
        	         fi

                        #The following section is redirect the functionality as per options
                	eval set -- "$TEMP"
                 	while true; do
                    		case $1 in
                        	    -e)
			    		expiry_info_set $2
                                      	expiry_info=1
					#account_lock 1 >/dev/null
                                    	shift 2
                                     ;;
                                 --)    
					 break
                                     ;;
	             		*)  
					 usage_error "Incorrect usage" $exit_usage
                                     ;;
      					  esac
                 		done
		else 		
            		usage_error "Incorrect usage" $exit_usage
            	fi	 	
		;;
		*)
 			usage_error "Incorrect usage" $exit_usage
	      	;;
	  esac
else
       usage_error "Incorrect usage" $exit_usage
fi
}
#-------------------------------------
function check_tsuser() {
if [ $username == "root" ];then
	error "ERROR: User [$username] is not a troubleshooting user" $exit_error
fi
	
${GREP} "^$username:" /cluster/etc/passwd 1>/dev/null
if [ $? != 0 ];then
	error "ERROR: User [$username] does not exist" $exit_error
fi
tsgroup_id=$(${GREP} "^$ts_group:" /cluster/etc/group | ${AWK} -F: '{print $3}') #group id of tsgroup
user_grpid=$(${GREP} "^$username:" /cluster/etc/passwd | ${AWK} -F: '{print $4}')
if [ "$user_grpid" != "$tsgroup_id" ];then

	        error "ERROR: User [$username] is not a troubleshooting user" $exit_error
fi
}
#-------------------------------------
function mod_expiry() {
if [ "$expiry_info" == "1" ];then
        user_age=$(cat $SHADOW_FILE |$GREP -w $username | awk -F':' '{print $8}')
        current_date=$(date +"%Y-%m-%d")
        new_date=$(date -d "$current_date $expiry days" +%Y-%m-%d)
        current_age=$(($(date -ud "$current_date" "+%s") / 86400))
        if [ $current_age -ge $user_age ];then
        error "ERROR: Account is expired and locked" $exit_expiry_fail
        else
        ${CHAGE} -E $new_date $username 1>/dev/null
        if [ $? != 0 ];then
                error "ERROR: Expiry information set failed" $exit_expiry_fail
        fi
fi
fi
exit $exit_success
}
#-------------------------------------------------------------------------
function account_lock() {
        old_date=$(date +"%Y-%m-%d %H:%M:%S")
        new_date=$(date -d "$old_date $expiry days" +%Y-%m-%d)
        $CHAGE -E $new_date $username
        if [ $? != 0 ];then
                 error "ERROR: Expiry information set failed"  $exit_expiry_fail
        fi
}
#-------------------------------------------------------------------------

# _____________________ _____________________ 
#|    _ _   _  .  _    |    _ _   _  .  _    |
#|   | ) ) (_| | | )   |   | ) ) (_| | | )   |   
#|_____________________|_____________________|
# Here begins the "main" function...

# Set the interpreter to exit if a non-initialized variable is used.
set -u
#checking if the command is issued by tsadmin or root
#privilege_check 

#Fetch maximum days of expiry information from imm
fetch_acnt_days

# Fetch maximum days of expiry information from login.defs
fetch_max_days

# Fetch minimum days of expiry information from login.defs
#fetch_min_days

#Parse command line arguments
parse_args $@

#Check if the user exists
#check_tsuser

#modify the expiry period of existing troubleshooting user
mod_expiry

exit $exit_success
