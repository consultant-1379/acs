#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       prcboot.sh
# Description:
#       A script to wrap the invocation of prcboot from the COM CLI.
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
# - Tue Oct 14 2012 - Luca Petrunti (xlucpet)
#	First version.
##

umask 002
/usr/bin/sudo /opt/ap/acs/bin/prcboot "$@"

exit $?