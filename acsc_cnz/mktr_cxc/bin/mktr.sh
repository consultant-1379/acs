#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       mktr.sh
# Description:
#       A script to wrap the invocation of mktr from the COM CLI.
# Note:
#	None.
##
# Usage:
#	None.
##
# Output:
#       None.
##
# Changelog:
# - Tue Oct 15 2012 - Luca Petrunti (xlucpet)
#	First version.
# - Jan 28 2021 - Sankara Jayanth (xsansud)
#       Delete 'mktruser' after MKTR process execution is finished if the user exists 
##

CMD_GETENT="/usr/bin/getent"
CMD_USERMGMT="/opt/ap/apos/bin/usermgmt/usermgmt"
MKTR_USERNAME="mktruser"

umask 002
/usr/bin/sudo /opt/ap/acs/bin/mktr "$@"

$CMD_GETENT "passwd" $MKTR_USERNAME &> /dev/null
retcode="$?"

if [ $retcode -eq 0 ]; then
   $CMD_USERMGMT "user delete --uname="$MKTR_USERNAME
fi

exit $?
