#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       aehls.sh
# Description:
#       A script to wrap the invocation of aehls from the COM CLI.
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

user=$(id -g $USER)
/usr/bin/sudo /opt/ap/acs/bin/aehls $user "$@"

exit $?
