#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2014 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# aca_disk_cache.sh
##
# Description:
# This script is used to enable/disable the usage of OS caching in ACA.
##
# Note:
# -
##
# Changelog:
# - 2014-11-14 - Alessio Cascone (XCASALE)
# First version.
##

usage()
{
	echo "Incorrect usage"
	echo "Usage:"
	echo "aca_disk_cache.sh --enable|--disable"
	echo
}


# Check input parameters
if [ $# != 1 ] ; then
	usage
	exit 1	
fi

# Check the option provided and update accordingly the attribute value
if [ $1 = "--enable" ] ; then
	ATTRIBUTE_VALUE=1
elif [ $1 = "--disable" ] ; then
	ATTRIBUTE_VALUE=0
else
	usage
	exit 2
fi

# Lock ACA service
#amf-adm lock safSi=ACA,safApp=ERIC-APG &>/dev/null

# Try to change the parameter in IMM for N_ITER times
N_ITER=5
FAILED=1

for (( i = 0; i < ${N_ITER}; i++ )); do
	CMD="immcfg -a osCachingEnabled=${ATTRIBUTE_VALUE} AxeDataRecorddataRecordMId=1"

	timeout --signal=INT --kill-after=11 10 $CMD &>/dev/null
	if [ $? -eq 0 ]; then
		FAILED=0;
		break;
	fi
	sleep 1
done

# Unlock ACA service
#amf-adm unlock safSi=ACA,safApp=ERIC-APG &>/dev/null

if [ $FAILED == 1 ] ; then 
	echo "Failed to perform the operation, keeping the old values!"
	exit 3
fi

exit 0
