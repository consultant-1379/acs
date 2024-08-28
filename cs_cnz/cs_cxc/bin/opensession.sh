#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       opensession.sh
# Description:
#       A script to wrap the invocation of opensession from the COM CLI.
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
# - Tue Jul 24 2012 - Giuseppe Fusco
#   First version.
#
# - Fri Nov 08 2013 - Nhat Pham (nhat.pham@dektech.com.au)
#   Removal of sudo - The opensession is called directly not via sudo. 
#   The set-user-ID bit of opensession is set during installation so that it is executed under 
#   root account. This effect doesn't change real-user-ID. Thus the account which executes 
#   opensession is kept. This allows openning AP session or file transfer session without username specified.
##
umask 022
/opt/ap/acs/bin/opensession "$@"

exit $?
