#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       emfcopy.sh
# Description:
#       A script to wrap the invocation of emfcopy from the COM CLI.
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
# - Tue Oct 04 2012 - Ramakrishna Maturu (xrammat)
#       First version.
##

/usr/bin/sudo /opt/ap/acs/bin/emfcopy "$@"

exit $?
