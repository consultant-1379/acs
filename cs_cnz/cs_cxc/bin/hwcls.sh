#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       hwcls.sh
# Description:
#       A script to wrap the invocation of hwcls from the COM CLI.
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
# - Tue Jul 24 2012 - Stefano Volpe
#	First version.
##

/usr/bin/sudo /opt/ap/acs/bin/hwcls "$@"

exit $?
