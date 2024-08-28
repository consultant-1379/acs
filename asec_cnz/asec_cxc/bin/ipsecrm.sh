#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       ipsecrm.sh
# Description:
#       A script to wrap the invocation of ipsecrm from the COM CLI.
##
# Changelog:
#     - Monday 9th of September 2013 - by Fabrizio Paglia (xfabpag)
#       First version.
##

/usr/bin/sudo /opt/ap/acs/bin/ipsecrm "$@"

exit $?

