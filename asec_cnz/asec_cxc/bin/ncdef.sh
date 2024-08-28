#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       tcdef.sh
# Description:
#       A script to wrap the invocation of tcdef from the COM CLI.
# Note:
#       None.
##
# Usage:
#       None.
##
# Output:
#       None.
##

/usr/bin/sudo /opt/ap/acs/bin/ncdef "$@"

exit $?

