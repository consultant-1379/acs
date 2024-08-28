#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       removetsuser
# Description:
#       A script to start health check from COM CLI on APG43L node 
#
##
# Usage: 
#		removetsuser <username>
##
# Output:
#       Command print-out is printed on console.
#
##
# Changelog:
#
# - Fri 29 Jan 2016 - Alessio Cascone (EALOCAE) 
#       Implemented changes for SLES12 adaptation.
#- Fri  MAY 24 2013 -Roni Newatia (xronnew)
#       Changed Account lock implementation and format
#- Thur APR 18 2013 -Greeshmalatha C (xgrecha)
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
USER_MGMT_SCRIPT=/opt/ap/apos/bin/usermgmt/usermgmt
USERDEL="${USER_MGMT_SCRIPT} user delete"
SED="/usr/bin/sed"
RM="/bin/rm" 
CP="/bin/cp"
MV="/bin/mv"
CHMOD="/bin/chmod"
CAT="/bin/cat"

# Exit codes
################################
exit_success=0
exit_usage=2
exit_privilege=3
exit_no_exist=4
exit_invalid_user=5
exit_userdel=6
exit_file_updt_fail=7

# Setting  global variables
##########################
login_allw_file=/cluster/etc/login.allow
login_allw_bkp_file=/cluster/etc/login.allow.bkp
opasswd_file=/cluster/etc/opasswd
peer_node_file="/etc/cluster/nodes/peer/hostname"
root_user="root"
tsusercount="tsuser_count"
admin="tsadmin"
status=0
ts_group="tsgroup"
script_name="removetsuser"
username=
# Message functions
####################
function show_usage() {
	${ECHO} -e "Usage: $script_name username"
	${ECHO} ""
}

function usage_error() {
    ${ECHO} $1
    show_usage
    exit $2
}

function error() {
    ${ECHO} "$1"
    ${ECHO} ""
    exit $2
}

# Other sript related functions
###############################
function privilege_check() {
	userid=$(id -u)
	tsadmin_id=$(${GREP} "^$admin:" /cluster/etc/passwd | ${AWK} -F: '{print $3}')
	root_id=$(${GREP} "^$root_user:" /etc/passwd | ${AWK} -F: '{print $3}')
	if [ "$userid" != "$tsadmin_id" ] && [ "$userid" != "$root_id" ];then
		error "ERROR: Not enough privileges to use this command" $exit_privilege
	fi
}
#--------------------------------------------------
function parse_args() {
	if [ $# -eq 0 ] || [ $# -gt 1 ];then
		usage_error "Incorrect usage" $exit_usage
	elif [ $# -eq 1 ]; then
		case "$1" in
			-*)
				usage_error "Incorrect usage" $exit_usage
			;;
		esac
		username=$1
	fi
}

#------------------------------------
function fetch_system_files() {
	tsgroup_id=$(${GREP} "^$ts_group:" /cluster/etc/group | ${AWK} -F: '{print $3}') #group id of tsgroup
}
#-------------------------------------------------------------
function attempt_to_remove(){
	if [ "$username" == "$admin" ];then
		error "INFO: [$admin] cannot be removed" 
		$exit_success
	fi
	
	if [ "$username" == "$root_user" ];then
		error "INFO: [$root_user] cannot be removed" 
		$exit_success
	fi
}
#-----------------------------------
function check_user() {
	${GREP} "^$username:" /cluster/etc/passwd 1>/dev/null
	if [ $? != 0 ];then
		#Cleanup activity in progress
		line_num=$(${GREP} -wn "^$username" $login_allw_file | ${AWK} 'BEGIN {FS = "[:]"} {print $1}')
		if [[ ! -z $line_num ]];then
			# Take the backup of login.allow file
			${CP} $login_allw_file $login_allw_bkp_file
  			${SED} -i ''$line_num'd' $login_allw_file
			if [ $? != 0 ];then
				${MV} $login_allw_bkp_file $login_allw_file
				error "ERROR: Failed to update a file related to login" $exit_file_updt_fail
			fi

			# Remove the backup file
			${RM} -f $login_allw_bkp_file
		fi
		error "ERROR: User [$username] does not exist in APG" $exit_no_exist
	fi
}

#---------------------------------------------
function check_tsuser() {
	user_grpid=$(${GREP} "^$username:" /cluster/etc/passwd | ${AWK} -F: '{print $4}')
	if [ "$user_grpid" != "$tsgroup_id" ];then
		error "ERROR: User [$username] is not a troubleshooting user" $exit_invalid_user
	fi
}

#------------------------------------------------------------------------------
function delete_pass_history() {
	remote_node=$($CAT $peer_node_file)
	HOST_ID=$(gethostip -x $(uname -n))
	peer_HOSTID=$(gethostip -x $remote_node)
	line_opswd=$(${GREP} -wn "^$username" $opasswd_file | ${AWK} 'BEGIN {FS = "[:]"} {print $1}')
	line_opswd_host1=$(${GREP} -wn "^$username" $opasswd_file.$HOST_ID | ${AWK} 'BEGIN {FS = "[:]"} {print $1}')
	line_opswd_host2=$(${GREP} -wn "^$username" $opasswd_file.$peer_HOSTID | ${AWK} 'BEGIN {FS = "[:]"} {print $1}')

	[[ ! -z $line_opswd ]] && ${SED} -i ''$line_opswd'd' $opasswd_file && {
		if [ $? != 0 ];then
			error "ERROR: Failed to update a file related to login" $exit_file_updt_fail
		fi
	}

  ##### BEGIN: WORK AROND FOR LDE 4.6 #################
  line_pswd=$(${GREP} -wn "^$username" /cluster/etc/passwd | ${AWK} 'BEGIN {FS = "[:]"} {print $1}')
  line_shd=$(${GREP} -wn "^$username" /cluster/etc/shadow | ${AWK} 'BEGIN {FS = "[:]"} {print $1}')

  [[ ! -z $line_pswd ]] && ${SED} -i ''$line_pswd'd' /cluster/etc/passwd && {
		if [ $? != 0 ];then
				error "ERROR: Failed to update a file related to password" $exit_file_updt_fail
		fi
  }

  [[ ! -z $line_shd ]] && ${SED} -i ''$line_shd'd' /cluster/etc/shadow && {
		if [ $? != 0 ];then
				error "ERROR: Failed to update a file related to shadow" $exit_file_updt_fail
		fi
  }

  ##### END: WORK AROUND FOR LDE 4.6 ##################
	
	[[ ! -z $line_opswd_host1 ]] && ${SED} -i ''$line_opswd_host1'd' $opasswd_file.$HOST_ID && {
		if [ $? != 0 ];then
			error "ERROR: Failed to update a file related to login" $exit_file_updt_fail
		fi
	}
	
	[[ ! -z $line_opswd_host2 ]] && ${SED} -i ''$line_opswd_host2'd' $opasswd_file.$peer_HOSTID && {
		if [ $? != 0 ];then
			error "ERROR: Failed to update a file related to login" $exit_file_updt_fail
		fi
	}
}

#-------------------------------------------------------------------------------
function del_user() {
	${USERDEL} --uname="$username" 2>/dev/null
	if [ $? != 0 ];then
		error "ERROR: Failed to delete connected user [$username]" $exit_userdel
	else
		delete_pass_history		
		line_num=$(${GREP} -wn "^$username" $login_allw_file | ${AWK} 'BEGIN {FS = "[:]"} {print $1}')
		if [[ ! -z $line_num ]]; then
			# Take the backup of login.allow file
			${CP} $login_allw_file $login_allw_bkp_file

			${SED} -i ''$line_num'd' $login_allw_file
			if [ $? != 0 ];then
				${MV} $login_allw_bkp_file $login_allw_file
				error "ERROR: Failed to update a file related to login" $exit_file_updt_fail
			fi

			# Remove the backup file
			${RM} -f $login_allw_bkp_file
			#${RM} -rf "/home/$username"
			${ECHO} "INFO: Success in deleting user : [$username]"
			${ECHO} ""
		fi
	fi
	exit $exit_success
}
# _____________________ _____________________ 
#|    _ _   _  .  _    |    _ _   _  .  _    |
#|   | ) ) (_| | | )   |   | ) ) (_| | | )   |   
#|_____________________|_____________________|
# Here begins the "main" function...

# Set the interpreter to exit if a non-initialized variable is used.
set -u
#set -x

#previlege check to verify if the command is issued by tsadmin or ts user
#privilege_check 

#Parse command line arguments
parse_args $*

#To fetch system files
fetch_system_files

#Check if an attemp is made to remove root or admin
attempt_to_remove

#check if user exists in APG or not
check_user

#check if user is troubleshooting user or not
check_tsuser

#deleting troubleshooting user
del_user

exit $exit_success
