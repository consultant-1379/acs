#!/bin/bash
#******************************************************************************
#
# NAME
#      acs_ssu_pstat.sh
#
# COPYRIGHT Ericsson AB, Sweden 2010.
# All rights reserved.
#
#  The Copyright to the computer program(s) herein
#  is the property of Ericsson Utvecklings AB, Sweden.
#  The program(s) may be used and/or copied only with
#  the written permission from Ericsson Utvecklings AB or in
#  accordance with the terms and conditions stipulated in the
#  agreement/contract under which the program(s) have been
#  supplied.
#
# DESCRIPTION
#  The purpose of this script is to findout the the No of Handles,Minor Fault,Major Fault,Priortity,Process ID,Virtual size,Thread Count and Command Name of
#  each and every running process in the system.
#
# DOCUMENT NO
#
# AUTHOR
#   2011-07-28 by Shyam Chirania
#
#   REV NO      DATE        NAME        DESCRIPTION
#   A          20110728    XSHYCHI     First Version
#
#
#******************************************************************************

x=1
i=2
echo "HANDLES" >> /var/log/ssuhandle.log
AWK=`which awk`
# This command gives the minor fault,major fault,priority,pid,thread count and command name 
# for the different process and store it in the log file
	ps -eo min_flt:15,maj_flt:15,priority:15,pid:15,vsz:15,thcount:15,cmd | $AWK '{OFS="\t"} {print $1,$2,$3,$4,$5,$6,$7}'> /var/log/ssuinfo.log
	y=$(cat  /var/log/ssuinfo.log |wc -l)
	for (( i = 2 ; i <= $y ; i++ ))
	do
	processid=$( sed -n -e ""$i"p"  /var/log/ssuinfo.log|awk '{print $4}' )
	totalHandleincludeheading=$(lsof -p $processid | wc -l)
		if [ $totalHandleincludeheading != 0 ] ; then
			totalHandle=$(($totalHandleincludeheading - $x))
			echo $totalHandle >>/var/log/ssuhandle.log
		else
			echo $totalHandleincludeheading >> /var/log/ssuhandle.log
		fi
	done
	paste /var/log/ssuhandle.log /var/log/ssuinfo.log 
	rm -f /var/log/ssuhandle.log
	rm -f /var/log/ssuinfo.log 

