#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       csadm.sh
# Description:
#       A script to wrap the invocation of csadm from the COM CLI.
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
# - Wed Oct 03 2012 - Satya Deepthi (xsatdee)
#       First version.
##

/usr/bin/sudo /opt/ap/acs/bin/csadm $*

exit $?
