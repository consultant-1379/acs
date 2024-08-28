#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2014 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       wssadm.sh
# Description:
#       A script to wrap the invocation of wssadm from the COM CLI.
##
# Changelog:
#     - Friday 3rd of October 2014 - by Fabrizio Paglia (XFABPAG)
#       First version.
##
umask 002
/usr/bin/sudo /opt/ap/acs/bin/wssadm "$@"

exit $?

