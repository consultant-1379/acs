#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       aloglist.sh
# Description:
#       A script to wrap the invocation of aloglist from the COM CLI.
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
/usr/bin/sudo /opt/ap/acs/bin/aloglist $@

exit $?

