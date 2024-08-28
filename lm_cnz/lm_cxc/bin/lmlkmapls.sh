#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       lmlkmapls.sh
# Description:
#       A script to wrap the invocation of lmlkmapls from the COM CLI.
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
# - Thu Jan 24 2013 - Shyam Chirania (xshychi)
#       First version.
##

/usr/bin/sudo /opt/ap/acs/bin/lmlkmapls "$@"

exit $?
