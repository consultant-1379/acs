#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       acease.sh
# Description:
#       A script to wrap the invocation of acease from the COM CLI.
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
# - Tue Oct 14 2012 - Nicola Muto (xnicmut)
#	First version.
#
# - Sat Jun 15 2013 - Claudio Elefante (eclaele)
#       Second version.
##

user=$USER
/usr/bin/sudo /opt/ap/acs/bin/acease $user "$@"

exit $?
