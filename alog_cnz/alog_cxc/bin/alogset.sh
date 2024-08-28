#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       alogset.sh
# Description:
#       A script to wrap the invocation of alogset from the COM CLI.
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
# - Wed Oct 27 2012 - Bhargava Chandra (xbhakat)
#       First version.
##
user=$USER

umask 002
/usr/bin/sudo /opt/ap/acs/bin/alogset -u $user "$@"

exit $?

