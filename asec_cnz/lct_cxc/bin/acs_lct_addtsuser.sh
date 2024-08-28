#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       addtsuser
# Description:
#       A script to add TS user with expiry information
#
##
# Usage: 
#   addtsuser [-d ] [-e <expiry>][-r] <username>
#   addtsuser -h
##
# Output:
#       Command print-out is printed on console.
#
##
# Changelog:
# - Thu Feb 16 2023 - Sainadth Pagadala (ZPAGSAI)
#	Updated prompt for addtsuser command
# - Thu Feb 02 2017 - Pratap Reddy(xpraupp)
#    Changed home directory for ts user to /var/home/ts_users
# - Thu 16th of June 2016- Roni Newatia (XRONNEW)
#	Removed upper limit check for maximumAccountAge value	
# - Tue 27th of October 2015 - Andrea Formica (EANFORM)
#	Adding user that never expires
# - Wed 16th of April 2015 - Suryanarayana Pammi (XPAMSUR)
#      Removed system-adm group functionality
# - Monday 2nd of February 2015 - Suryanarayana Pammi (XPAMSUR)
#      add_tsuser function updated for system-adm group
# - Tuesday 9th of September 2014 - Torgny Wilhelmsson
#      white tab to space, proper assignment of mindays=$df_mindays
# - Monday 8th of September 2014 - Fabrizio Paglia (XFABPAG)
#      Fixed to let the "maximumPasswordAge" compliant with IMM model and login.defs
# - Thursday 28th of August 2014 - Torgny Wilhelmsson
#      Added option -d to add nomml into gecos data, (useradd -c option used)
#- Fri  MAY 24 2013 -Roni Newatia (xronnew)
#   Changed Account lock implementation and format
#- Thur APR 18 2013 -Greeshmalatha C (xgrecha)
#   Changed the command format,usage and format of 
#   script for redability.
#   Removed -g option
#   Added account lock requirement
# - Tue Jan 16 2012 -Sonali Nanda 
#   Adding user to APG, settin initial password with    
#   desired expiry information.
#       E- The range of values lies between 0 to 255 days.
#       - If the value of expiry is given as '0', it sets 
#   the account never to expire.
#       Hence, not including in usage.
#   Adding user to APG, setting initial password with 
#   desired additional secondary group.
#       Adding user to APG, setting initial password and 
#               default expiry information.
# - Tue Jan 16 2012 -Sonali Nanda 
#       First version.
##

# Commands 
ECHO=/bin/echo
USER_MGMT_SCRIPT=/opt/ap/apos/bin/usermgmt/usermgmt
USERADD="${USER_MGMT_SCRIPT} user add --global"
USERMOD="${USER_MGMT_SCRIPT} user modify"
USERDEL="${USER_MGMT_SCRIPT} user delete"
PASSWD="/usr/bin/passwd"
STTY="/bin/stty"
GREP=/usr/bin/grep
AWK=/usr/bin/awk
CHAGE="/usr/bin/chage"
SED="/usr/bin/sed"
RM="/bin/rm"
CP="/bin/cp"
MV="/bin/mv"
CHMOD="/bin/chmod"
CMD_CAT="/bin/cat"
LOGIN_DEFS="/etc/login.defs"
CMD_IMMLIST="/usr/bin/immlist"
CMD_CUT="/usr/bin/cut"

# Exit codes

exit_success=0
exit_usage=2
exit_privilege=3
exit_expiry_info=4
exit_user_exists=5
exit_max_users=6
exit_useradd=7
exit_force_pwd=8
exit_expiry_fail=9
exit_pwd_fail=10
exit_usermod=11
exit_file_updt_fail=12
exit_invalid_user=13
exit_pwdx_lenx=14
exit_pwdx_hist=15
exit_acct_fail=16
exit_term=17
exit_renew_fail=18
exit_user_notexist=19
# Global variables
login_allw_file=/cluster/etc/login.allow
login_allw_bkp_file=/cluster/etc/login.allow.bkp
expiry_info=0
root_user="root"
root_group="root"
ts_group="tsgroup"
gecos_data=''
gecos_data_set=0
com_group="com-emergency"
cmw_group="cmw-imm-users"
cba_esi_group="cba-esi"
admin="tsadmin"
tsusercount="tsuser_count"
tsusermax="max_tsuser_count"
linux_date="1970-01-01"
maxdays=""
maxdays_max=14 #maximum allowed value for maxdays
df_maxdays=-1
mindays=""
df_mindays=2
maxacntdays=""
df_maxacntdays=5
expire_input=0
expire_stat=0
isrenewal=0
isexpired=0
count=0
sg_info=0
never_expire=50000
secondary_group=""
passwd_perm=""
group_perm=""
shadow_perm=""
script_name="addtsuser"
common_home_dir=/var/home/ts_users/

# Message functions

#set -x 

function show_usage() {
	${ECHO} "Usage: $script_name [-d] [-e expiry][-r] username"
	${ECHO} "       $script_name -h"
	${ECHO} ""
}
#-------------------------------------------------------------------------

function usage_error() {
    ${ECHO} "$1"
    show_usage
    exit $2
}
#-------------------------------------------------------------------------
function abort () {
    ${ECHO} "ERROR: Failed to add user [$username]"
    slog  "ABORTING: <"$1">"
    exit $2
}

#------------------------------------------------------------------------
function slog () {
    /bin/logger -t addtsuser "$*"
}

#-------------------------------------------------------------------------
function error() {
    ${ECHO} "$1"
    ${ECHO} ""
    exit $2
}
#-------------------------------------------------------------------------
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
    ${ECHO} -e 'While assigning initial password it is required that'
    ${ECHO} -e "* At least $pwdlen_val characters are used."
    ${ECHO} -e '  (minimum password length is configurable in the'
    ${ECHO} -e '  MO LocalTsUsersPolicyM=1)'
    ${ECHO} -e '* The initial password must contain characters from'
    ${ECHO} -e '  at least 3 out of the 4 classes, upper case characters,'
    ${ECHO} -e '  lower case characters, digits and special characters.'
    ${ECHO} -e '* The initial password must not contain at least 4 consecutive'
    ${ECHO} -e "  characters of the user's account name.\n"
 }                                  
#-------------------------------------------------------------------------
function privilege_check() {
    userid=$(id -u)
    tsadmin_id=$(${GREP} "^$admin:" /cluster/etc/passwd | ${AWK} -F: '{print $3}')
    root_id=$(${GREP} "^$root_user:" /etc/passwd  | ${AWK} -F: '{print $3}')
    if [ "$userid" != "$tsadmin_id" ] && [ "$userid" != "$root_id" ];then
        error "ERROR: Not enough privileges to use this command" $exit_privilege
    fi 
}
#------------------------------------------------------------------------
function check_renew_user() {
	username=$1
	${GREP} "^$username:" /cluster/etc/passwd 1>/dev/null
	if [ $? == 0 ];then
		user_age=$(cat /etc/shadow |grep -w $username | awk -F':' '{print $8}')
		current_date=$(date +"%Y-%m-%d")
		current_age=$(($(date -ud "$current_date" "+%s") / 86400))
		if [ $current_age -ge $user_age ];then
			isexpired=1
		else
			error "ERROR: Account is already active" $exit_renew_fail
		fi
	else
		error "ERROR: Account not exist" $exit_user_notexist
	fi
}

#-------------------------------------------------------------------------
function expiry_info_set() {
	expiry=$1
	re='^[0-9]+$'
	if [[ $expiry != "-1" ]] ; then
		! [[ $expiry =~ $re ]] || [[ $expiry -lt 1 ]] || [[ $expiry -gt $maxacntdays ]] &&
		error "ERROR:Invalid expiry information provided" $exit_expiry_info
	else
		expiry=$never_expire
	fi
}

#-------------------------------------------------------------------------
function name_convention_chk() {
    username=$1
    if [ "$isexpired" -eq "0" ];then
    startstr=${username:0:3}
    number_check=$(${ECHO} "$username" | ${GREP} ^[0-9])
    [ "$number_check" ] && error "ERROR: Invalid way of naming tsusers. Prefix 'ts_' to be used" $exit_invalid_user
    if [ "$startstr" != "ts_" ];then
        error "ERROR: Invalid way of naming tsusers. Prefix 'ts_' to be used" $exit_invalid_user
    fi
    name=$(${ECHO} "$username" | ${GREP} "admin")
    if [ $name ];then
        error "ERROR: Invalid username [$username]" $exit_invalid_user
    fi  
fi
}

#-------------------------------------------------------------------------
function name_length_chk(){
    username=$1
    local len
    len=$(${ECHO} ${#username})
    if  [ $len -lt  4 ] || [ $len -gt 32 ]; then
        error "ERROR: Invalid username [$username]. The minimum number of characters for the username is 4 and maximum is 32" $exit_invalid_user
    fi
}

#-------------------------------------------------------------------------
function parse_args() {
    if [ $# -eq 0 ] || [ $# -gt 4 ];then
        usage_error "Incorrect usage" $exit_usage
    fi

    if [ $# -eq 1 ];then
        case "$1" in
            -h)
                show_usage
                exit $exit_usage
                ;;
            -*)
                usage_error "Incorrect usage" $exit_usage
                ;;
            *)
                username=$1
                ;;
        esac
    else
        validate_args "$@"
    fi
}

#---------------------------------------------------------------------------------------
function validate_args() {
 local parameters=($@)
    local num_parameters=${#parameters[@]}

    username=${parameters[$num_parameters - 1]}


    TEMP=$(getopt -q "d: e: r:" $@)

    #exit from code if invalid arguments
    if [ $? != 0 ]; then
        usage_error "Incorrect usage" $exit_usage
    fi


    # The following section is redirect the functionality as per options
#   eval set -- "$TEMP"

#   echo "$LINENO: param 1 $1, parm2 $2, param3 $3, DollarSTAR -$*-"

    ##  while true; do
    while [ $# -gt 0 ]; do
        case $1 in
                -d)
                    #Set gecos data for restricted users
                    gecos_data=",,,,nomml"
                    gecos_data_set=1
#                            echo "$LINENO: param 1 $1, parm2 $2, param3 $3, DollarSTAR -$*-"
                 ;;
                -e)
                        #Set expiry information
                        #    echo "$LINENO: param 1 $1, parm2 $2, param3 $3, DollarSTAR -$*-"
                        if [[ $2 =~ ^-{0,1}[0-9]+$ ]]; then
                            expiry_info_set $2
                            expiry_info=1
                            #  shift 2 extra shift outside case
                            shift
                        else
                            #echo "$LINENO: -e param"
                            usage_error "Incorrect usage" $exit_usage
                        fi
                 ;;
                -r)     check_renew_user $username
                        isrenewal=1

         esac
         shift
    done

#   echo "$LINENO: num_parameters $num_parameters, gecos set $gecos_data_set, -e set $expiry_info "
    case $num_parameters in
        2)
#          echo "Only -d or -r should have been set"
            if [ $gecos_data_set -ne 1 ] && [ $expiry_info -ne 0 ] || [ $isrenewal -ne 1 ] && [ $expiry_info -ne 0 ]; then
                usage_error "Incorrect usage" $exit_usage
            fi
         ;;
        3)
#          echo "Only -e plus num should have been set"
            if [ $gecos_data_set -ne 0 ] || [ $expiry_info -ne 1 ] || [ $isrenewal -ne 0 ]; then
                usage_error "Incorrect usage" $exit_usage
            fi
        ;;
        4)
#          echo "Both -d and -e should have been set"
            if  [ $gecos_data_set -ne "1" ]  || [ $expiry_info -ne "1" ] &&  [ $isrenewal -ne "1" ] || [ $expiry_info -ne "1" ] ;then
                usage_error "Incorrect usage" $exit_usage
            fi
         ;;
        *)
                usage_error "Incorrect usage" $exit_usage
        ;;
    esac

}
#--------------------------------------------------------------------------------------
function fetch_acnt_days() {
	maxacntdays=$($CMD_IMMLIST -a maximumAccountAge AxeLocalTsUsersPolicylocalTsUsersPolicyMId=1  | awk -F'=' '{print $2}')
	[ -z $maxacntdays ] && maxacntdays=$df_maxacntdays
}

#---------------------------------------------------------------------------------------
function fetch_max_days() {
	maxdays=$(${CMD_CAT} $LOGIN_DEFS | ${GREP} ^PASS_MAX_DAYS | ${AWK} '{print $2}' 2>/dev/null)
	[ -z $maxdays ] && maxdays=$df_maxdays
}

#-------------------------------------------------------------------------
function fetch_min_days() {
	mindays=$(${CMD_CAT} $LOGIN_DEFS | ${GREP} ^PASS_MIN_DAYS | ${AWK} '{print $2}' 2>/dev/null)
	[ -z $mindays ] && mindays=$df_mindays
}

#-------------------------------------------------------------------------
function fetch_system_files() {
	tsgroup_id=$(${GREP} "^$ts_group:" /cluster/etc/group | ${AWK} -F: '{print $3}') #group id of tsgroup
	usr_grpid_list=$(${AWK} -F: '{print $4}' /cluster/etc/passwd) #list of gid field of all users in system
}

#-------------------------------------------------------------------------
function set_expiry_max() {
	if [ "$expiry_info" -eq "0" ];then
		expiry=$maxacntdays
	fi
}

#-------------------------------------------------------------------------
function check_tsuser() {
	if [ "$isexpired" -eq "0" ];then
		if [ "$isrenewal" -eq "1" ];then
			error "ERROR: Invalid option to add new user [$username]" $exit_useradd
		fi
	    ${GREP} "^$username:" /cluster/etc/passwd 1>/dev/null
	    if [ $? == 0 ];then
	        error "ERROR: User [$username] already exists" $exit_user_exists
	    fi
	fi
}

#------------------------------------------------------------------------
function clean_up() {
	${USERDEL} --uname="$username" 1>/dev/null
    abort "Termination signal received" $exit_term
    #exit 1
}

#-------------------------------------------------------------------------
function add_tsuser() {
	if [ "$isrenewal" -eq "0" ];then
		local gecos_data_opt=""
		if [ ! -z $gecos_data ]; then
			gecos_data_opt="--comment=\"$gecos_data\""
		fi
		
		${USERADD} --homedir=$common_home_dir --gname="$ts_group" $gecos_data_opt --uname="$username" &>/dev/null
	    if [ $? != 0 ];then
	            error "ERROR: Failed to add user [$username]" $exit_useradd
	    fi
	    trap clean_up SIGHUP SIGINT SIGTERM
	    # To add com-emergency and cba-esi groups to ts user
		${USERMOD} --secgroups=$com_group --uname=$username 1>/dev/null
		${USERMOD} --appendgroup --secgroups=$cmw_group --uname=$username 1>/dev/null
		${USERMOD} --appendgroup --secgroups=$cba_esi_group --uname=$username 1>/dev/null
	    #if [ "$sg_info" == "1" ];then
	    #        ${USERMOD} -P /cluster/etc -G $secondary_group $username 1>/dev/null
	    #        if [ $? != 0 ];then
	    #                error "ERROR: Failed to make user [$username] member of $secondary_group" $exit_usermod
	    #        fi
	    #fi
	
	    sleep 5
	fi
}
#-------------------------------------------------------------------------
function set_passwd() {
	${PASSWD} $username
	if [ $? == 0 ];then
		# Setting force password change
		${PASSWD} -e $username 1>/dev/null
		if [ $? != 0 ];then
			if [ "$isrenewal" -eq "0" ];then
                ${USERDEL} --uname="$username" 1>/dev/null
			fi
			error "ERROR: User add failed as set forced password change failed" $exit_force_pwd
		fi

		# Setting expiry information for the user
		${CHAGE} -M $df_maxdays -I 0 $username 1>/dev/null
		if [ $? == 0 ];then
			account_lock 1 >/dev/null
			if [ "$isrenewal" -eq "0" ];then
				# Adding the user to ssh configuration file
				str_append="$username all"

				# Take the backup of login.allow file             
				${CP} $login_allw_file $login_allw_bkp_file
				${ECHO} "$str_append" >>${login_allw_file}
				if [ $? != 0 ];then
					${MV} $login_allw_bkp_file $login_allw_file
					error "ERROR: Failed to update a file related to login" $exit_file_updt_fail
				fi
				# Remove the backup file
				${RM} -f $login_allw_bkp_file
			fi
			if [ "$isrenewal" -eq "1" ];then
				${ECHO} "User Renew Success"
				${ECHO} ""
			else
				${ECHO} "User Add Success"
				${ECHO} ""
			fi
        else
			if [ "$isrenewal" -eq "0" ];then
				${USERDEL} --uname="$username" 1>/dev/null
			fi
			error "ERROR: User add failed as setting expiry information failed"  $exit_expiry_fail
		fi
	else
		if [ "$isrenewal" -eq "0" ];then
			${USERDEL} --uname="$username" 1>/dev/null
		fi
		if [ "$isexpired" -eq "1" ];then
			error "ERROR: User renew failed as password set failed" $exit_pwd_fail
		fi
		error "ERROR: User add failed as password set failed" $exit_pwd_fail
	fi
	exit $exit_success
}
#-------------------------------------------------------------------------
function account_lock() {
	old_date=$(date +"%Y-%m-%d %H:%M:%S")	
	new_date=$(date -d "$old_date $expiry days" +%Y-%m-%d)
	$CHAGE -E $new_date $username
	if [ $? != 0 ];then
		error "ERROR: User add failed as setting expiry information failed"  $exit_expiry_fail
	fi
}
#-------------------------------------------------------------------------

# _____________________ _____________________ 
#|    _ _   _  .  _    |    _ _   _  .  _    |
#|   | ) ) (_| | | )   |   | ) ) (_| | | )   |   
#|_____________________|_____________________|
# Here begins the "main" function...
#set -x
# Set the interpreter to exit if a non-initialized variable is used.
#set -u

# Privilege check: tsadmin or root
#privilege_check

#Fetch maximum days of account expiry information from imm
fetch_acnt_days
# Fetch maximum days of expiry information from login.defs
#fetch_max_days

# Fetch minimum days of expiry information from login.defs
#fetch_min_days

# Parse command line arguments.
parse_args $*

# Name check for the username
name_convention_chk $username

#Length check for username
name_length_chk $username

# Fetching values from system files
#fetch_system_files

# Setting expiry information for maximum days when expiry information is not given as input
set_expiry_max

# Checking if username already exists
check_tsuser

# Adding tsuser with given information and com-emergency to ts_user
add_tsuser

# Prints the password policy
print_passwd_policy

# Setting password for the given user
set_passwd

exit $exit_success                                                                                
