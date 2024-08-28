#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2022 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       rpswrprint.sh
# Description:
#       A script to wrap the invocation of rpswrprint from the COM CLI.
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
# - Thu June 233 2022  - debdutta chatterjee (xdebdch)
#	First version.
##

/usr/bin/sudo /opt/ap/acs/bin/rpswrprint "$@"

exit $?
