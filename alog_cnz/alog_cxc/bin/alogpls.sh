#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       alogpls.sh
# Description:
#       A script to wrap the invocation of alogpls from the COM CLI.
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
# - Wed Oct 17 2012 - Bhargava Chandra (xbhakat)
#       First version.
##
/usr/bin/sudo /opt/ap/acs/bin/alogpls $@

exit $?

