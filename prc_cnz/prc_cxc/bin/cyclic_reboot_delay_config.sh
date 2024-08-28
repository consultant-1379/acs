#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       ciclyc_reboot_delay_conf.sh
# Description:
#       This script is used to reset ( -r ) the cyclic reboot time counter;
#		the script is also used to extend ( -e ) the reboot time delay from 10 minute
#		to 1 hour
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
# - Tue Feb 16 2013 - Luca Petrunti (xlucpet)
#	First version.
##

usage()
{
        echo ""
        echo "usage: `basename $0` -r [-e]"
        echo "usage: `basename $0` -e [-r]"
        echo ""
        echo "Options :"
        echo "  -r Reset the cyclic reboot time counter"
        echo "  -e Extend the reboot time delay"
        echo ""
}

if [ $# -eq 0 ]; then
        usage
        exit 1
elif [ $# -eq 1 ]; then
	if [ $1 = "-r" ]; then
	    >/var/log/last_reboot_time_stamps 
	    exit 0
	elif [ $1 = "-e" ]; then
	    touch /tmp/delay_boot
	    exit 0
	else
        usage
        exit 1
    fi	
elif [ $# -eq 2 ]; then
    if [[ $1 = "-r" || $1 = "-e" ]]; then
        if [[ $2 = "-r" || $2 = "-e" ]]; then
	        >/var/log/last_reboot_time_stamps
	        touch /tmp/delay_boot
	        exit 0
	    else
	    	usage
	    	exit 1
	    fi
	else
		usage
        exit 1	
	fi
else
        usage
        exit 1
fi
