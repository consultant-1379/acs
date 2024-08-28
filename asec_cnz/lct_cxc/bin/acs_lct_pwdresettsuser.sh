#bin/bash
##
# ------------------------------------------------------------------------
#    Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       pwdresettsuser
# Description:
#       A script to unlock troubleshooting user account or set a new password on APG43L node 
#
##
# Usage: 
#	pwdresettsuser -n | -o <username>
# 	pwdresettsuser -h
##
# Output:
#       Command print-out is printed on console.
#
##
# Changelog:
# - Thu Feb 16 2023 - Sainadth Pagadala (ZPAGSAI)
#       Updated prompt for pwdresettsuser command
#- Fri July 8 - P Soumya
#        Changed the file for removing the password chage for tsadmin user for securiy enhancement feature.
#- Fri  MAY 24 2013 -Roni Newatia (xronnew)
#       Changed Account lock implementation and format
#
# - Thur APR 18 2013 -Greeshmalatha C (xgrecha)
#	Changed the command format,usage and format of 
#	script for redability.
#	Changed -o to -u
#	Added account lock requirement
# 
# - Tue Jan 16 2012 -Sonali Nanda 
#       First version.
##


#Configuring Bash behavior
#########################
# set -x  # Trace executed lines, useful for debugging script
set -u  # Unset variables is an error

# Commands used by this script
##############################
ECHO=/bin/echo
PASSWD="/usr/bin/passwd"
GREP=/usr/bin/grep
AWK=/usr/bin/awk
PAM="/sbin/pam_tally2"
CHMOD="/bin/chmod"
CAT="/bin/cat"
CMD_IMMLIST="/usr/bin/immlist"
CMD_CUT="/usr/bin/cut"
CHAGE="/usr/bin/chage"
LOGIN_DEFS="/etc/login.defs"
DATE="/bin/date"
SSH="/usr/bin/ssh"
PING="/bin/ping"
peer_node_file="/etc/cluster/nodes/peer/hostname"

# Exit codes
####################
exit_success=0
exit_usage=2
exit_pwd_fail=3
exit_user_exists=4
exit_privilege_err=5
exit_reset_fail=6
exit_force_pwd=7
exit_invalid_user=8
exit_unlock_fail=9
exit_failure=1
exit_pwdx_lenx=14
exit_pwdx_hist=15
exit_expiry_fail=10

# Setting  global variables
###########################
root_id=0
admin_paswd="tsadmin1@"
status=0
admin="tsadmin"
root_user="root"
reset_old_f=0
df_maxdays=5
script_name="pwdresettsuser"
ts_group="tsgroup"
ZERO=0
# Message functions
###################
function show_usage() {
    ${ECHO} -e "Usage:	$script_name -n | -u username"
    ${ECHO} -e "	$script_name -h"
    ${ECHO} -e ""
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

function print_passwd_policy() {

	# hardcode the needed paramters for now.
	dn='AxeLocalTsUsersPolicylocalTsUsersPolicyMId=1'
	pwdlen_str='minimumPasswordLength'

	# retries the corresponding values from imm.
	pwdlen_val=$( $CMD_IMMLIST -a $pwdlen_str $dn | $CMD_CUT -d = -f2)
	[ -z $pwdlen_val ] && error "ERROR: Minimum password length could not be retrieved" $exit_pwdx_lenx

	# print format on console
        ${ECHO} -e 'Please note that the initial password set below must be changed'
        ${ECHO} -e 'at first login.'
        ${ECHO} -e
        ${ECHO} -e 'While resetting password it is required that'
        ${ECHO} -e "* At least $pwdlen_val characters are used."
        ${ECHO} -e '  (minimum password length is configurable in the'
        ${ECHO} -e '  MO LocalTsUsersPolicyM=1)'
        ${ECHO} -e '* The initial password must contain charaters from'
        ${ECHO} -e '  at least 3 out of the 4 classes, upper case characters,'
        ${ECHO} -e '  lower case characters, digits and special characters.'
        ${ECHO} -e '* The initial password must not contain at least 4 consecutive'
	${ECHO} -e "  characters of the user's account name.\n"
}

#Other script functions
##############################
function privilege_check() {
        userid=`id -u`
	tsadmin_id=`${GREP} "^$admin:" /cluster/etc/passwd | ${AWK} -F: '{print $3}'`
	root_id=`${GREP} "^$root_user:" /etc/passwd | ${AWK} -F: '{print $3}'`
        if [ "$userid" != "$tsadmin_id" ] && [ "$userid" != "$root_id" ];then
                error "ERROR: Not enough privileges to use this command" $exit_privilege_err
        fi
}
#---------------------------------------------------------
function passwd_change() {
	
	 username="$1"
	 #echo username:$username
	 if [ $reset_old_f -eq 0 ];then
	 	# print the policy first
	  	password_unlock 
		print_passwd_policy
		${PASSWD} $username
		if [ $? != 0 ];then
         		error "ERROR: Password setting failed for [$username]" $exit_pwd_fail
         	fi

         	# Setting force password change
         	#
         	${PASSWD} -e $username 1>/dev/null
         	if [ $? != 0 ];then
         		error "ERROR: Failed to set forced password change on account [$username]" exit_force_pwd
         	fi
	#	account_lock
         	${ECHO} "password reset with new password success for [$username]"
    		${ECHO} ""
	else
	#	account_lock
		password_unlock
		${ECHO} "[$username] account successfully unlocked"
		${ECHO} ""
	fi
}
#------------------------------------------
function parse_args() {
C_user=$1 && shift
arg_str=`${ECHO} $@`
#echo arg_str:$arg_str
arg_num=`${ECHO} $#`
#echo arg_num:$arg_num
#username=$2

# Check command line arguments and set up internal variables
if [ $arg_num -eq 0 ];then
	#echo arg_num:$arg_num
        usage_error "Incorrect usage" $exit_usage
fi


case "$1" in
	-h)
        	if [ $arg_num -gt 1 ];then
			#echo abc:$arg_num
                	usage_error "Incorrect usage" $exit_usage
                fi
                show_usage
                exit $exit_success
                ;;
	-u|-n)
		if [ $arg_num -ne 2 ] ;then
		#	echo $arg_num
			usage_error "Incorrect usage" $exit_usage
		fi
		if [ "$1" == "-u" ];then
			reset_old_f=1
		fi
		;;

        *)
		#echo $arg_num
		usage_error "Incorrect usage" $exit_usage
		;;
esac
}

#------------------------------------
function fetch_system_files() {
shift
arg_str=`${ECHO} $@`
#echo arg_str:$arg_str
arg_num=`${ECHO} $#`
#echo arg_num:$arg_num
#username=$2
username="$2"
}
#----------------------------------------
function reset_passwd() {
if [ $username == $root_user ];then
	error "ERROR: Password reset is not allowed for [$username] user" $exit_privilege_err
fi
${GREP} "^$username:" /cluster/etc/passwd 1>/dev/null
if [ $? != 0 ];then
	error "ERROR: User [$username] does not exist in APG" $exit_user_exists
fi

# Resetting password for tsadmin if forgotten
#
if [ $username == $admin ];then
	# Checking for privilege
         error "ERROR: Password reset is not allowed for [$username] user" $exit_privilege_err

     
	
else
	 # Resetting password for other users
         #
         # Checking the privilege of the user who is issuing the command
         #
         #privilege_check
         # Checking if the user is a troubleshooting user
         #
         #${GREP} "^$username:" /cluster/etc/passwd 1>/dev/null
         #if [ $? != 0 ];then
		user_grpid=`${GREP} "^$username:" /cluster/etc/passwd | ${AWK} -F: '{print $4}'`
		tsgroup_id=`${GREP} "^$ts_group:" /cluster/etc/group | ${AWK} -F: '{print $3}'` #group id of tsgroup
	#	echo user_grpid:$user_grpid
                if [ "$user_grpid" != "$tsgroup_id" ];then
                	error "ERROR: User [$username] is not a troubleshooting user" $exit_invalid_user
                fi
         #fi

         
	 # Unlocking the account using pam_tally
         #
	 #if [ $reset_old_f -eq 1 ];then
#		password_lock_check
#	 fi
	 # Setting new password for the user
	 #
	 passwd_change $username
	 #pass_chg_date=`passwd -S $username|awk '{print $3}'`
	 if [ $reset_old_f -eq 1 ];then
		password_expiry_check
	 fi		
fi

exit $exit_success
}
#-----------------------------------------------------------------------
function password_unlock() {

${PAM} --user $username --reset 1>/dev/null
if [ $? != 0 ];then
	error "ERROR: Password unlocking failed for [$username]" $exit_unlock_fail
fi	
check_peer_node_state
if [ $? == 0 ];then
        ${SSH} -n $remote_node ${PAM} --user $username --reset 1>/dev/null
        [ $? != 0 ] && error "ERROR: Password unlocking failed for [$username]" $exit_unlock_fail
#else
#       echo "Remote node down. Password unlocking not performed on remote node"
fi
}
#----------------------------------------------------------------------------------------
function check_peer_node_state(){

        remote_node=`$CAT $peer_node_file`
        $PING -c 2 -w 2 $remote_node &> /dev/null
        if [ $? == 0 ]; then
                return $exit_success
        else
                return $exit_failure
        fi
}

#------------------------------------------------------------------------
function password_expiry_check() {
pass_chg_date=`passwd -S $username|awk '{print $3}'`
pass_exp=`passwd -S $username|awk '{print $5}'`
curr_date=`$DATE +%s`
#pass_exp_date_sec=`date -d "$pass_chg_date" +%s`
pass_exp_date_sec=`date -d "$pass_chg_date $pass_exp days" +%s`
diff_sec=$(($curr_date-$pass_exp_date_sec))
diff_days=$(($diff_sec/86400))
if [ "$pass_chg_date" != "01/01/1970" ] && [ $diff_days -gt $ZERO ];then
	account_lock
fi
						
#if [ "$pass_chg_date" == "01/01/1970" ];then
#	error "ERROR: Password unlocking failed as account is active" $exit_unlock_fail
#elif [ $diff_days -gt $ZERO ] || [ $diff_days -eq $ZERO ];then
#	error "ERROR: Password unlocking failed as account is active" $exit_unlock_fail
#else
#	${PAM} --user $username --reset 1>/dev/null
#	if [ $? != 0 ];then
#		error "ERROR: Password unlocking failed for [$username]" $exit_unlock_fail
#	fi
#fi
}
#-------------------------------------------------------------------------
function account_lock() {
	maxdays=$(${CAT} $LOGIN_DEFS | ${GREP} ^PASS_MAX_DAYS | ${AWK} '{print $2}' 2>/dev/null)
	if [ -z $maxdays ];then
	  maxdays=$df_maxdays
	fi
	#maxdays=5 #Expiry period maximum is set to 5 in the model. Hence this can be changed depnding on the model	
        old_date=`date +"%Y-%m-%d"`
        #new_date=`date -d "$old_date $expiry days" +%Y-%m-%d`
        ${CHAGE} -d $old_date $username 1>/dev/null
        if [ $? != 0 ];then
                 error "ERROR: Password unlock failed as setting password information failed"  $exit_expiry_fail
        fi
}
#------------------------------------------------

# _____________________ _____________________ 
#|    _ _   _  .  _    |    _ _   _  .  _    |
#|   | ) ) (_| | | )   |   | ) ) (_| | | )   |   
#|_____________________|_____________________|
# Here begins the "main" function...

# Set the interpreter to exit if a non-initialized variable is used.
set -u

#Parse command line arguments
parse_args "$@"

#fetch system files
fetch_system_files "$@"

#Password unlock or set new password
reset_passwd

exit $exit_success

