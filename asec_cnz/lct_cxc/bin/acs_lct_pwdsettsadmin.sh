#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2021 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       pwdsettsadmin	
# Description:
#       A script to reset the password for tsadmin and create boot-admin user.
# Note:
#       None.
##
# Usage:
#       pwdsettsadmin
##
# Output:
#       None.
##
# Changelog:
# - Thu Jun 3 2021 - Sravanthi T (xsravan)

HELP=''
TRUE=$( true; echo $? )
FALSE=$( false; echo $? )
ECHO=/bin/echo
PASSWD=/usr/bin/passwd
CHPASSWD=/usr/sbin/chpasswd
CHAGE=/usr/bin/chage
PASSWORD=""
PRINTOUT_USER="reset"
username="tsadmin"
#Exit codes
####################
exit_success=0
exit_create_fail_boot=1
exit_usage=2
exit_pwd_fail=3
exit_pwd_fail_boot=4
exit_pwd_mismatch=5
#######################################################################################
# function parse_args();                                                              #
#                                                                                     #
# parses the options and arguements                                                   #
#######################################################################################
function parse_args(){

         if [ $# != 1 ]; then
           usage_error
         fi

 return $exit_success
}
		
#######################################################################################
# function usage();                                                                   #
#                                                                                     #
# Displays the command usage                                                          #
#######################################################################################
function usage() {

        ${ECHO}
        ${ECHO} -e "Usage: pwdsettsadmin"
        ${ECHO}
	${ECHO}
       

}
#######################################################################################
# function usage_error();                                                             #
#                                                                                     #
# Displays the command usage and exits with $EXIT_USAGE_ERROR (3)                     #
#                                                                                     #
# Exits:                                                                              #
# - $EXIT_USAGE_ERROR                                                                 #
#######################################################################################
function usage_error() {
        $ECHO
        $ECHO "Incorrect Usage$(usage)"
	exit $exit_usage
        
}


#######################################################################################
# function passwd_change();                                                           #
                                                                                      
# "ERROR: Failed to set new password for tsadmin user account.Failed to create boot-admin user account." $exit_pwd_fail
# to change the password for tsadmin                                                  #
#######################################################################################
function passwd_change() {

                username="tsadmin"
		print=""
		tmpfile="/tmp/password_change_output.txt"
                read -rsp "New password: " PASSWORD1
                echo
                read -rsp "Retype new password: " PASSWORD2
                echo
		if [[ $PASSWORD1 == "" ]];then
			echo "Failed to set password."
			echo "BAD PASSWORD: it is WAY too short"
			error "" $exit_pwd_fail

		fi
		if [[ $PASSWORD2 == "" ]];then
                        echo "Failed to set password."
                        echo "Sorry,Passwords do not match."
                        error "" $exit_pwd_fail

                fi

		if [[ $PASSWORD1 != "" && $PASSWORD2 != "" ]];then
		if [ $PASSWORD1 != $PASSWORD2 ];then
		  	echo "Failed to set password."
        		echo "Sorry,Passwords do not match."
			error "" $exit_pwd_mismatch
		fi
		fi
		PASSWORD=$PASSWORD1
		password_policy_check
                ${ECHO} tsadmin:$PASSWORD1 | ${CHPASSWD} 2>$tmpfile
                if [ $? != 0 ];then
			 echo "Failed to set password."
		        cat $tmpfile | grep -iv "chpasswd:"
			if [ -f "$tmpfile" ];then
                		rm $tmpfile
			fi
		error "" $exit_pwd_fail
                else
		PASSWORD=$PASSWORD1
                fi
	
		if [ -f "$tmpfile" ];then
		rm $tmpfile
		fi
}

#######################################################################################
# function create_bootadmin();                                                        #
                                                                                      #
# to create and change the password for bootadmin                                     #
#######################################################################################

function create_bootadmin() {
 output=$(lde-grub-user show | grep -i "boot-admin")
 if [[ $output == "boot-admin" ]];then
#	echo "boot-admin already exists.So updating password"
echo -ne "\b"
lde-grub-user update boot-admin <<EOF
$PASSWORD
$PASSWORD
EOF
 if [ $? != 0 ];then
        error " Failed to set password." $exit_pwd_fail
#  else
       # ${ECHO} "Password Changed."
   fi

 else
echo -ne "\b"
lde-grub-user add boot-admin <<EOF
$PASSWORD
$PASSWORD
EOF
   if [ $? != 0 ];then
	error "Failed to set password." $exit_pwd_fail
 # else
	# ${ECHO} "Password successfully set for $username user account."
   fi
fi
}

#######################################################################################
# function error();                                                           #
                                                                                      #
# to display the error message and exitcode                                                  #
#######################################################################################

function error() {
    ${ECHO} -e "$1"
    exit $2
}

########################################################################################
#function password_policy_check()
#
#to check if passowrd is following the password policy
########################################################################################
function password_policy_check(){

echo $PASSWORD | grep -e "[0-9]" 1>/dev/null
output=$?
if [ $output != 0 ];then
      echo "Failed to set password."
      echo "BAD PASSWORD: it is based on a dictionary word"
      echo "BAD PASSWORD: is too simple"
      error " " $exit_pwd_fail

fi

echo $PASSWORD | grep -e "[a-z]" 1>/dev/null
output=$?
if [ $output != 0 ];then
      echo "Failed to set password."
  	echo "BAD PASSWORD: it is based on a dictionary word"
	echo "BAD PASSWORD: not enough character classes"
       error " " $exit_pwd_fail

fi

echo $PASSWORD | grep -e "[A-Z]" 1>/dev/null
output=$?
if [ $output != 0 ];then
      echo "Failed to set password."
     echo "BAD PASSWORD: it is based on a dictionary word"
	echo "BAD PASSWORD: not enough character classes"
      error " " $exit_pwd_fail

fi

echo $PASSWORD | grep -e "[@#$%^&-+=().]" 1>/dev/null
output=$?
if [ $output != 0 ];then
      echo "Failed to set password."
     echo "BAD PASSWORD: it is based on a dictionary word"
	echo "BAD PASSWORD: not enough character classes"
      error " " $exit_pwd_fail

fi


}


# _____________________ _____________________
#|    _ _   _  .  _    |    _ _   _  .  _    |
#|   | ) ) (_| | | )   |   | ) ) (_| | | )   |
#|_____________________|_____________________|
# Here begins the "main" function...

# Set the interpreter to exit if a non-initialized variable is used.
set -u
# Parse command line arguments.
parse_args "$@"
passwd_change 
create_bootadmin
exit $exit_success























