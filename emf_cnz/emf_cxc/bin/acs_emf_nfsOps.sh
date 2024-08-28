#!/bin/bash
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       acs_emf_nfsOps.sh
# Description:
#       This script is used for usb adminstrative operations.
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
# - Wed Mar 30 2016 - Paola Mazzone (xpaomaz), Cuono Crimaldi (xcuocri):
#                     Updated to support systemctl instead of deprecated service command
# - Tue Aug 18 2015 - Malangsha Shaik (XMALSHA)
# 	script enhancements.
# - Mon Dec 23 2013 - Shyam Chirania(XSHYCHI)
#   First version.
##

# global variable-set
TRUE=$( true; echo $? )
FALSE=$( false; echo $? )
LOG_TAG='-t EMF'
LOG_FILE='acs_emfd.log'
LOG_DIR='/var/log/acs/tra/logging/'
SCRIPT_NAME='acs_emf_nfsOps'
ETC_EXPORTS='/etc/exports'
NBI_PATH='/data/opt/ap/internal_root'
MEDIA='/media'

# command-set
CMD_LOGGER='/bin/logger'
#CMD_SERVICE='/sbin/service'
CMD_SERVICEMGMT='/opt/ap/apos/bin/servicemgmt/servicemgmt'
CMD_EXPORTFS='/usr/sbin/exportfs'
CMD_CAT='/bin/cat'
CMD_RAIDMGR='/opt/ap/apos/bin/raidmgr'
CMD_DRBD_OVERVIEW='/usr/sbin/drbd-overview'
CMD_KILL='/bin/kill'
CMD_MOUNT='/bin/mount' 
CMD_DRBD_STATUS='/opt/ap/apos/conf/apos_drbd_status'
# exit codes set
exit_success=0
exit_failure=1

# This function logs the information messages in /var/log/messages
# ------------------------------------------------------------------------
function log() {
    echo "[$(date --utc +'%Y-%m-%d %H:%M:%S')] [$SCRIPT_NAME] $@" >>$LOG_DIR/$LOG_FILE
}

# This function logs the error messages in /var/log/messages
# ------------------------------------------------------------------------
function log_error(){
    local PRIO='-p user.err'
    local MESSAGE="${*:-error}"
    $CMD_LOGGER $PRIO $LOG_TAG "$MESSAGE" &>/dev/null
}

# This function is used to abort script execution.
# ------------------------------------------------------------------------
function abort() {
	log_error "ABORTING: <"$1">"
	exit $exit_failure
}

# This function prints the usage on the console
# ------------------------------------------------------------------------
function usage(){
	$CMD_CAT << HEREDOC
Usage:
    $SCRIPT_NAME --export-nbi
    $SCRIPT_NAME --remove-nbi
    $SCRIPT_NAME --export-media
    $SCRIPT_NAME --remove-media
    $SCRIPT_NAME --clean-up <pid-list>
    $SCRIPT_NAME --mount-nfs <mount-point>
HEREDOC
    exit $exit_failure
}

# This function parses the cmdline.
# ------------------------------------------------------------------------
function parse_invoke(){

	ARGS=$#
	[ $ARGS -eq 0 ] && usage	
	
	case $1 in
	--export-nbi)
		[ $ARGS -ne 1 ] && usage	
		export_nbi			
		;;	
	--remove-nbi)
		[ $ARGS -ne 1 ] && usage	
		remove_nbi
		;;
	--export-media)
		[ $ARGS -ne 1 ] && usage	
		export_media
		;;
	--remove-media)
		[ $ARGS -ne 2 ] && usage	
		shift
  	remove_media "$*"
		;;
	--clean-up)
		[ $ARGS -eq 1 ] && usage	
		shift
		cleanup "$*"
		;;
		--mount-nfs)
		[ $ARGS -ne 3 ] && usage
   	shift
		mount_nfs "$*"
		;;
		*)
		usage
	esac
}

# This function check the Node is Active or Passive
# ------------------------------------------------------------------------
function isactive(){
	status=$( $CMD_RAIDMGR -s); status=$( echo $status)
	[ "$status" != "DOWN" ] && return $TRUE
	return $FALSE
}

# This function share the NBI path to Passive Node
# ------------------------------------------------------------------------
function export_nbi(){
	
	local NBI_INFO=$FALSE
	local NBI_MEDIA_INFO=$FALSE

	if ! isactive; then
		abort 'nbi export is done only on active node'
	fi	
	
	# updating exports file with external media entries
	log "Updating '/etc/exports':"

	# check if the NBI PATH is already available in exports
	$(grep -q "^$NBI_PATH 169.254.208.0/255.255.255.0(rw,sync,no_subtree_check,no_root_squash)" $ETC_EXPORTS 2>/dev/null)
	[ $? -eq 0 ] && NBI_INFO=$TRUE

	$(grep -q "^$NBI_PATH/media 169.254.208.0/255.255.255.0(rw,sync,no_subtree_check,no_root_squash)" $ETC_EXPORTS 2>/dev/null)
	[ $? -eq 0 ] && NBI_MEDIA_INFO=$TRUE

	if [[ $NBI_INFO -eq $FALSE && $NBI_MEDIA_INFO -eq $FALSE ]]; then
		$CMD_CAT << HEREDOC >>$ETC_EXPORTS
$NBI_PATH/media 169.254.208.0/255.255.255.0(rw,sync,no_subtree_check,no_root_squash)
$NBI_PATH 169.254.208.0/255.255.255.0(rw,sync,no_subtree_check,no_root_squash)
HEREDOC

		# start nfs service if not up.
		nfs_start

		log "Success"
		return $exit_success
	fi

	if [[ $NBI_INFO -eq $TRUE && $NBI_MEDIA_INFO -eq $FALSE ]]; then
			log "nbi information already available, adding media details to exports"
			$CMD_CAT << HEREDOC >>$ETC_EXPORTS
$NBI_PATH/media 169.254.208.0/255.255.255.0(rw,sync,no_subtree_check,no_root_squash)
HEREDOC
	elif [[ $NBI_INFO -eq $FALSE && $NBI_MEDIA_INFO -eq $TRUE ]]; then
			log "media information already available, adding nbi details to exports"
			$CMD_CAT << HEREDOC >>$ETC_EXPORTS
$NBI_PATH/media 169.254.208.0/255.255.255.0(rw,sync,no_subtree_check,no_root_squash)
HEREDOC
	else
			log "media & nbi details are already available..skipping the update"
			return $exit_success
	fi	
		
	# start nfs service if not up.
	nfs_start

	log "Success"

	return $exit_success
}

# This function remove the NBI path 
# ------------------------------------------------------------------------
function remove_nbi(){
	
	if ! isactive; then
		abort 'remove nbi is done only on active node'
	fi	
	# updating exports file with external media entries
	#Remove nfs share on active node
	#umount 169.254.208.$(</etc/cluster/nodes/peer/id):/data/opt/ap/internal_root/
	log "Updating exports file..."
	$( grep -q "media" $ETC_EXPORTS 2>/dev/null)
	[ $? -eq 0 ] &&  sed -i /media/d  $ETC_EXPORTS 2>/dev/null
	$( grep -q "internal_root" $ETC_EXPORTS 2>/dev/null)
	[ $? -eq 0 ] &&  sed -i /internal_root/d $ETC_EXPORTS 2>/dev/null
	log "Success"
	
	# stop nfs service if it is a passive node
	nfs_stop
	return $exit_success
}

# This function share the media path to active Node
# ------------------------------------------------------------------------
function export_media(){
	
	if isactive; then
		abort 'media export is done only on passive node'
	fi
	# updating exports file with external media entries
	log "Updating exports file..."

	# check if the media is already available in exports
	$(grep -q "^$MEDIA" $ETC_EXPORTS 2>/dev/null)
	if [ $? -eq 0 ]; then
		log "media information already available in exports"
		return $exit_success
	fi	

	# update media info in exports
	$CMD_CAT << HEREDOC >>$ETC_EXPORTS
$MEDIA 169.254.208.0/255.255.255.0(rw,sync,no_subtree_check,no_root_squash)
HEREDOC
	log "Success"
	
	# start nfs service if not up and working
	nfs_start
	return $exit_success
}

# This function remove the media path 
# ------------------------------------------------------------------------
function remove_media(){
	
	if [ "$2" == "false" ]; then
 		if isactive; then
   		abort 'media remove is done only on passive node'
    fi
	fi

	# updating exports file with external media entries
	log "Updating exports file..."
	$(grep -q "^$MEDIA" $ETC_EXPORTS 2>/dev/null)
	[ $? -eq 0 ] &&  sed -i /media/d  $ETC_EXPORTS 2>/dev/null
	log "Success"

	# stop nfs service if it is a passive node
	nfs_stop
	return $exit_success
}

# This function start the nfs server
# ------------------------------------------------------------------------
function nfs_start(){
	# Check if service already running
	$( $CMD_SERVICEMGMT status nfs-mountd.service | grep -q '1')
	[ $? -eq 0 ] && {
		log "Starting nfs server "
		$CMD_SERVICEMGMT start nfs-mountd.service &>/dev/null
		[ $? -ne 0 ] && abort "Failed to start NFS server"
	}
	# re-export all directories.It synchronizes 
	# /var/lib/nfs/etab file with /etc/exports
    $CMD_EXPORTFS -ra &>/dev/null
	return $exit_success
}

# This function stop the nfs server
# ------------------------------------------------------------------------
function nfs_stop(){
	# re-export all directories.It synchronizes 
	# /var/lib/nfs/etab file with /etc/exports
	$CMD_EXPORTFS -ra &>/dev/null
	
	local DRBD0_ROLE=$( $CMD_DRBD_STATUS role drbd0 local)
	[ "$DRBD0_ROLE" != "Primary" ] && {
		# Check if service already running
		$( $CMD_SERVICEMGMT status nfs-mountd.service | grep -q '1')
		[ $? -ne 0 ] && {
			log "Stopping nfs server "
			$CMD_SERVICEMGMT stop nfs-mountd.service &>/dev/null
			[ $? -ne 0 ] && abort "Failed to start NFS server"
		}	
	}
	return $exit_success
}

# This function kill the running the pid
# ------------------------------------------------------------------------
function cleanup() {
	# cleanup of unwanted PIDs
 	local PIDLIST="$*"
	for i_PID in $PIDLIST; do
		i_PPID=$(ps --pid $i_PID -o ppid h)
		[ -d "/proc/$i_PID" ] && {
   		log "Terminating [SIGKILL] PID [$i_PID]:"
     	$CMD_KILL -s SIGKILL $i_PID &>/dev/null
			[ $? -ne 0 ] && abort "Terminating [SIGKILL] PID [$i_PID] Failed"
		}
		[[ ! -z "$i_PPID" && $i_PPID -ne 1 && -d "/proc/$i_PPID" ]] && {
   		log "Terminating [SIGKILL] PPID [$i_PPID]:"
     	$CMD_KILL -s SIGKILL $i_PPID &>/dev/null
			[ $? -ne 0 ] && abort "Terminating [SIGKILL] PPID [$i_PPID] Failed"
		}
	done
	return $exit_success
}

# This function mount the shared nfs path
# ------------------------------------------------------------------------
function mount_nfs() {
	#mount the shared nfs path to the denstination folder
	local NETWORK_IP=$(</etc/cluster/nodes/peer/networks/internal/primary/address) 
	local SOURCENAME=$1
	local MOUNTPOINT=$2

	log "Mounting $NETWORK_IP:/$SOURCENAME to $MOUNTPOINT"
	$($CMD_MOUNT -t nfs $NETWORK_IP:/$SOURCENAME $MOUNTPOINT &>/dev/null)
	[ $? -ne 0 ] && abort "mounting media path using nfs failed"
	log "Success"
	return $exit_success
} 

# Function definitions end here =====>>
# _____________________
#|    _ _   _  .  _    |
#|   | ) ) (_| | | )   |
#|_____________________|
# Here begins the "main" function...

# parse the command-line
parse_invoke $@

exit $exit_success

# End of file

