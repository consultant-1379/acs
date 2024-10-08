#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       show_licenses.sh
# Description:
#       A script to wrap the invocation of show_licenses from the COM CLI.
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
# - Tue Oct 04 2012 - Shyam Chirania (xshychi)
#       First version.
# - Wed Mar 23 2016 - Kiran Apuri (xkirapu)
#       Updated file due to SUGAR feature impacts in CLI exposed commands wrappers in SLES12.
##

umask 002
/usr/bin/sudo /opt/ap/acs/bin/show_licenses "$@"

exit $?

