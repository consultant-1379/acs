#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cpls.sh
# Description:
#       A script to wrap the invocation of cpls from the COM CLI.
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
# - Tue Aug 21 2013 - Andrea Formica
#	First version.
##

/usr/bin/sudo /opt/ap/acs/bin/cpls "$@"

exit $?
