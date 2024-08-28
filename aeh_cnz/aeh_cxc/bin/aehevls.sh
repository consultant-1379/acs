#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       aehevls.sh
# Description:
#       A script to wrap the invocation of aehevls from the COM CLI.
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

/usr/bin/sudo /opt/ap/acs/bin/aehevls "$@"

exit $?
