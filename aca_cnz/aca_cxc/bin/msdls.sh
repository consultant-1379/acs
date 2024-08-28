#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       msdls.sh
# Description:
#       A script to wrap the invocation of msdls from the COM CLI.
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
# - Wed Dec 05 2012 - Harika Bavana (xharbav)
#       First version.
##

/usr/bin/sudo /opt/ap/acs/bin/msdls "$@"

exit $?
