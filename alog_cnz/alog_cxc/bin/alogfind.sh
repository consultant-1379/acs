#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       alogfind.sh
# Description:
#       A script to wrap the invocation of alogfind from the COM CLI.
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
# - Tue Oct 14 2012 - Nicola Muto (xnicmut)
#	First version.
##

user=$USER
/usr/bin/sudo /opt/ap/acs/bin/alogfind $user "$@"

exit $?
