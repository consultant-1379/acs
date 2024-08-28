#!/bin/sh

##
# ------------------------------------------------------------------------
#     Copyright (C) 2016 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cyclic-boot-delay
# Description:
#       Prevents too fast cyclic reboots.
#	When this script detects that the system has rebooted more than
#       a certain number of times within a time period, a delay is added to
#       the boot sequence so that it is possible to manually log into the
#       system. It is then possible to extend the delay if necessary.
##
# Usage:
#       None.
##
# Output:
#       None.
##
# Changelog:
# - Wed Feb 10 2016 - Alfonso Attanasio (ealfatt) - Crescenzo Malvone (ecremal)
#       First version.
##

# You should customise the following variables:
# STATE_FILE
# CYCLE_RESTART_THRESHOLD
# PROBATION_TIME
# DELAY_TIME
# DELAY_BOOT_FILE
# EXTENDED_DELAY_TIME
#
# Note that if you change STATE_FILE or DELAY_BOOT_FILE, then
# cyclic-boot-delay.spec must also be updated.
#
# * X-Start-Before is not part of LSB. Make sure it is supported by your
#   Linux distribution.
# * If each reboot takes longer than (PROBATION_TIME / CYCLE_RESTART_THRESHOLD),
#   cyclic reboots will not be prevented by this script. Be sure to customise
#   these values according to the boot time of your system.
# * If system clock is reset at reboot, the script may think there is a cyclic
#   reboot even though there isn't. This could also happen if the system time
#   jumps backwards for some other reason. Choose a value for DELAY_TIME so
#   that the consequence of such an event is not unacceptable.

# A file that will contain the time stamps for the last boots. This file
# must reside in persistent storage but should not be included in backups.
# Each blade should have its own file.
STATE_FILE="/var/log/last_reboot_time_stamps"

# CYCLE_RESTART_THRESHOLD and PROBATION_TIME together specify the conditions
# for triggering the boot delay. If more than $CYCLE_RESTART_THRESHOLD
# boots happen within $PROBATION_TIME seconds, a boot delay is introduced.
CYCLE_RESTART_THRESHOLD=3
PROBATION_TIME=1800

# When a boot delay is triggered, the boot will be delayed for $DELAY_TIME
# seconds. During this time, the SSH server will be up and running but
# OpenSAF will not be started.
DELAY_TIME=600

# By creating this file, the boot delay can be extended.
DELAY_BOOT_FILE="/tmp/delay_boot"

# The number of seconds that the boot delay will be extended if the file
# $DELAY_BOOT_FILE exists. The file will be deleted once it is used.
# The boot delay can be extended again by re-creating this file.
EXTENDED_DELAY_TIME=3600

CHK_HAAGENTRCF(){
	EXTENDED_REBOOT_COUNT=1
	HAAGENT_RCF='/var/log/.haagentrcf'
	[ -f $HAAGENT_RCF ] && {
		HAAGENT_RC=$(cat $HAAGENT_RCF)
		CYCLE_RESTART_THRESHOLD=$(($HAAGENT_RC + $EXTENDED_REBOOT_COUNT))
	}
}




# usage: log_msg [<priority>] <message_to_log>
#        <priority> must be in the form <facility>.<level>
#        <facility> must be one of auth, authpriv, cron, daemon, kern, lpr,
#                                  mail, news, syslog, user, uucp, local[0-7].
#        <level> must be one of debug, info, notice, warning, err, crit, alert,
#                               emerg.
function log_msg() {	
	local LOG_APP
  local TAG=$(basename $0)
  if [ -x /bin/logger ]; then
	  LOG_APP='/bin/logger'
  else
	  LOG_APP='echo -e'
  fi
  
  MESSAGE=$1
	
	$LOG_APP -p warning -t $TAG -- "$MESSAGE"
}

LOCAL_HOSTNAME=$(cat /etc/cluster/nodes/this/hostname)

# TR :HV63307 change start

if [ "$LOCAL_HOSTNAME" == "SC-2-1" ]; then
	last_reboot_time_stamp=$(cat /opt/ap/acs/etc/nsf/conf/NODEA_NSF_Timestamp.txt)
else
	last_reboot_time_stamp=$(cat /opt/ap/acs/etc/nsf/conf/NODEB_NSF_Timestamp.txt)
fi

#TR :HV63307 change end

# Get current time in number of seconds since the epoch.
current_time_stamp=$(/usr/bin/date +%s)

# TR :HV63307 change start

if [ \( $last_reboot_time_stamp -gt $current_time_stamp \) ]; then
current_time_stamp=$last_reboot_time_stamp
fi

#TR :HV63307 change end

PRC_REBOOT="/cluster/etc/ap/acs/prc/conf/$LOCAL_HOSTNAME"

if [ -e "$PRC_REBOOT" ]; then
    exit 0
fi

if [ \! -d $(dirname "$STATE_FILE") ]; then
     mkdir -p $(dirname "$STATE_FILE")
fi

# Update CYCLE_RESTART_THRESHOLD to the count required for HA AGENT.	
CHK_HAAGENTRCF

# Add current time to the end of the file, and remove lines at the
# beginning so that the total number of lines does not exceed
# $CYCLE_RESTART_THRESHOLD

if [ -e "$STATE_FILE" ] && [ -s "$STATE_FILE" ];
then
tail -n $(($CYCLE_RESTART_THRESHOLD - 1)) "$STATE_FILE" >> \
    "$STATE_FILE.new"
fi

echo $current_time_stamp >> "$STATE_FILE.new"
mv "$STATE_FILE.new" "$STATE_FILE"

chmod 666 "$STATE_FILE"

# Get the oldest time stamp and the number of time stamps.

oldest_time_stamp=$(head -n 1 "$STATE_FILE")
number_of_time_stamps=$(wc -l "$STATE_FILE" | cut -d" " -f1 )

# Check if we have rebooted more than $CYCLE_RESTART_THRESHOLD times
# during $PROBATION_TIME

if [ -z $oldest_time_stamp ];
then
  oldest_time_stamp="0"
fi

if [ -z $number_of_time_stamps ];
then
  number_of_time_stamps="0"
fi

if [ \( $number_of_time_stamps -ge $CYCLE_RESTART_THRESHOLD \) -a \
\( $(($PROBATION_TIME + $oldest_time_stamp)) -gt \
$current_time_stamp \) ]
then
sleep_time=$(($PROBATION_TIME + $oldest_time_stamp - \
    $current_time_stamp))

log_msg "Cyclic reboot detected; boot sequence has been suspended"
sleep $DELAY_TIME
while [ -e "$DELAY_BOOT_FILE" ]
do
    sleep $EXTENDED_DELAY_TIME
done      
fi
exit 0
