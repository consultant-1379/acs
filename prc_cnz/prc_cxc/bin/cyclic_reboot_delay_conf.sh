#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cyclic_reboot_delay_conf.sh
# Description:
#       A script to wrap the invocation of cyclic_reboot_delay_config.sh script from a TS user from bash shell.
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
# - Tue Jul 24 2013 - Luca Petrunti (xlucpet)
#	First version.
##

/usr/bin/sudo /opt/ap/acs/bin/cyclic_reboot_delay_config.sh "$@"

exit $?