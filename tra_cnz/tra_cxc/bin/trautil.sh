#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       trautil.sh
# Description:
#       A script to wrap the invocation of prcstate from the COM CLI.
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
# - Tue Jul 15 2013 - Luca Petrunti (xlucpet)
#	First version.
##

/usr/bin/sudo /opt/ap/acs/bin/trautil "$@"

exit $?
