#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       swrprint.sh
# Description:
#       A script to wrap the invocation of swrprint from the COM CLI.
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
# - Thu Oct 4 2012  - Sameer Chandra (xsamech)
#	First version.
##

/usr/bin/sudo /opt/ap/acs/bin/swrprint "$@"

exit $?
