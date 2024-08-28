#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       rtrfe.sh
# Description:
#       A script to wrap the invocation of rtrfe from the COM CLI.
# Note:
#       None.
##
# Usage:
#       None.
##
# Output:
#       None.
##
# Changelog:
# - Thu Aug 22 2013 - Harika Bavana (xharbav)
#       First version.
##

/usr/bin/sudo /opt/ap/acs/bin/rtrfe "$@"

exit $?
