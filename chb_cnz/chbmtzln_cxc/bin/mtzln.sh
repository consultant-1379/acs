#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       mtzln.sh
# Description:
#       A script to wrap the invocation of mtzln from the COM CLI.
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
# - Tue Oct 04 2012 - Umesh Kumar (xkumume)
#       First version.
##

/usr/bin/sudo /opt/ap/acs/bin/mtzln "$@"

exit $?
