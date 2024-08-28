#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       lmtestls.sh
# Description:
#       A script to wrap the invocation of lmtestls with sudo user.
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
# - Thu May 29 2013 - Anil Achary (xsunach)
#       First version.
##

/usr/bin/sudo /opt/ap/acs/bin/lmtestls "$@"

exit $?

