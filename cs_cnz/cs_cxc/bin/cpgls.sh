#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cpgls.sh
# Description:
#       A script to wrap the invocation of cpgls from the COM CLI.
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

/usr/bin/sudo /opt/ap/acs/bin/cpgls "$@"

exit $?
