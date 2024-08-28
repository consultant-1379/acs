#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       ipsecdef.sh
# Description:
#       A script to wrap the invocation of ipsecdef from the COM CLI.
##
# Changelog:
#     - Monday 9th of September 2013 - by Fabrizio Paglia (xfabpag)
#       First version.
##
umask 002
/usr/bin/sudo /opt/ap/acs/bin/ipsecdef "$@"

exit $?

