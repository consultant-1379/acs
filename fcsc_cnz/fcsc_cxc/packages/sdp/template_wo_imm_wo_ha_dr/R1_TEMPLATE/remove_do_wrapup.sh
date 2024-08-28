#! /bin/sh
##
## Copyright (c) Ericsson AB, 2010.
## 
## All Rights Reserved. Reproduction in whole or in part is prohibited
## without the written consent of the copyright owner.
## 
## ERICSSON MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
## SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING
## BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. ERICSSON
## SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A
## RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
## DERIVATIVES.
##
##

# This script is executed as a remove campaign wrapup action.

echo '###########################################################'
echo 'This script is executed as a remove campaign wrapup action.'
echo '###########################################################'
#HOME_COM_DIR="/home/com"
#TMP_HOME_DIR="/tmp/home"
#TMP_HOME_COM_DIR="$TMP_HOME_DIR/com"

# sdp name is passed as first argument
#SDP_NAME=$1
#BACKUP_SCRIPT=backup_com.sh

# Check the exit value from the last command and return 1 if it failed.
check_exit_value () {
    if [ $? -ne 0 ]; then
        echo "$1"
        exit 1
    fi
}

#ERROR_MESSAGE="ERROR: Could not create directory $TMP_HOME_DIR"
#mkdir -p $TMP_HOME_DIR
#check_exit_value "$ERROR_MESSAGE"

#ERROR_MESSAGE="ERROR: Could not copy $HOME_COM_DIR to $TMP_HOME_COM_DIR"
#cp -rf $HOME_COM_DIR $TMP_HOME_COM_DIR
#check_exit_value "$ERROR_MESSAGE"

#ERROR_MESSAGE="ERROR: Could not remove $HOME_COM_DIR"
#rm -rf $HOME_COM_DIR
#check_exit_value "$ERROR_MESSAGE"

#cmw-partial-backup-unregister $SDP_NAME $BACKUP_SCRIPT

exit 0
