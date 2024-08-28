#!/bin/bash
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       acs_emf_generalOps.sh
# Description:
#       This script is used for usb operations.
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
# - Tue Dec 17 2013 -  Shwetha N(XSHWETH)
#   First version.
##

# global variable-set
TRUE=$( true; echo $? )
FALSE=$( false; echo $? )
LOG_TAG='-t EMF'
LOG_FILE='acs_emfd.log'
LOG_DIR='/var/log/acs/tra/logging/'
SCRIPT_NAME='acs_emf_generalOps'
NBI_PATH='/data/opt/ap/internal_root'
MEDIA='media'
#THUMBDRIVE='/dev/sdm'
THUMBDRIVE='/dev/eri_thumbdrive'
EXT3FS='ext3'
ACTIVE=1
PASSIVE=2
OPTIONS='barrier=1,relatime,commit=2,data=ordered,noatime'
PROC_MOUNTS='/proc/mounts'
#TR HU81933
PATH_DEV_DIR='/dev/'
ERI_THUMBDRIVE='eri_thumbdrive'

# command-set
CMD_LOGGER='/bin/logger'
CMD_MOUNT='/bin/mount'
CMD_UNMOUNT='/bin/umount'
CMD_BLKID='/sbin/blkid'
CMD_E2LABEL='/sbin/e2label'
CMD_LSSCSI='/usr/bin/lsscsi'
CMD_MKFSEXT3='/sbin/mkfs.ext3'
CMD_DU='/usr/bin/du'
CMD_CP='/bin/cp'
CMD_AWK='/usr/bin/awk'
CMD_GREP='/usr/bin/grep'
CMD_WC='/usr/bin/wc'
CMD_CAT='/bin/cat'
CMD_RM='/bin/rm'
CMD_TR='/usr/bin/tr'
CMD_AWK='/usr/bin/awk'
CMD_PS='/bin/ps'
CMD_PKILL='/usr/bin/pkill'
CMD_KILLPROC='/sbin/killproc'
#TR HU81933
CMD_LSL='/bin/ls'

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
function abort() {
	log "ABORTING: <"$1">"
	log_error "ABORTING: <"$1">"
	exit $exit_failure
}

# This function prints the usage on the console
# ----------------------------------------------------------------------
function usage(){
	$CMD_CAT << HEREDOC
Usage:
    $SCRIPT_NAME --apply-label <label>
    $SCRIPT_NAME --check-fs 
    $SCRIPT_NAME --copy <argv>
    $SCRIPT_NAME --mount <node-state>
    $SCRIPT_NAME --unmount
    $SCRIPT_NAME --is-usb-available
    $SCRIPT_NAME --format
    $SCRIPT_NAME --remove

HEREDOC
    exit $exit_failure
}

# This function parses the cmdline.
# ----------------------------------------------------------------------
function parse_invoke(){

    local ARGS=$#
    [ $ARGS -eq 0 ] && usage

    case $1 in
    --apply-label)
        [ $ARGS -ne 2 ] && usage
		shift
        apply_label $*
        ;;
    --check-fs)
        [ $ARGS -ne 1 ] && usage
        check_fs
        ;;
    --copy)
        [ $ARGS -eq 0 ] && usage
		shift
    	copy $*
    	;; 
    --mount)
        [ $ARGS -ne 2 ] && usage
		shift
        mount $*
        ;;
    --unmount)
        [ $ARGS -ne 2 ] && usage
        shift
        unmount $*
        ;;
    --is-usb-available)
        [ $ARGS -ne 1 ] && usage
        is_usb_available
        ;;
    --format)
        [ $ARGS -ne 1 ] && usage
        format
        ;;
    --remove)
        [ $ARGS -eq 1 ] && usage
		shift
        remove $*
        ;;

	*)
        usage
    esac
}

# This function is used to apply the label
# ----------------------------------------------------------------------
function apply_label()
{
	local LABEL=$1
		
	log "Applying the label.."
	$($CMD_E2LABEL $THUMBDRIVE $LABEL &>/dev/null)
	[ $? -ne 0 ] && abort "Applying label Failed"
	
	log "Success"
	return $exit_success
}

# This function is used to check the filesystem type
# ----------------------------------------------------------------------
function check_fs()
{
	log "Checking for $EXT3FS FS on Usb:"
	$($CMD_BLKID $THUMBDRIVE -t TYPE=$EXT3FS &>/dev/null)
	[ $? -ne 0 ] && abort "Not Found"

	log "Found"
	return $exit_success
}

# This function is used to copy the files
# ----------------------------------------------------------------------
function copy()
{
	local ARGLIST=$*
	local SOURCE=($ARGLIST)
	local SIZE=${#SOURCE[@]}
	local DEST=''
	local CMD_CP_ERROR=0

	# assign last elemement to DEST and remove it from the array
	((SIZE--))
	DEST=${SOURCE[$SIZE]}
	unset SOURCE[$SIZE]
	
	for INDEX in ${SOURCE[@]}; do
		for i in {1..10};do
			log "Copying ($INDEX -> $DEST):"
			$($CMD_CP -rf $INDEX $DEST &>/dev/null)
			CMD_CP_ERROR=$?
			[ $CMD_CP_ERROR -eq 0 ] && break
			[ $CMD_CP_ERROR -ne 0 ] && log "Failed to Copy [$INDEX] n_retry == [$i] error == [$CMD_CP_ERROR]"
			sleep 3
		done
		[ $CMD_CP_ERROR -ne 0 ] && abort "Failed to Copy [$INDEX]"
		log "Success"
	done	
	return $exit_success
}


# This function is used to mount the Thumb Drive
# ----------------------------------------------------------------------
function mount()
{
	local NODESTATE=$1
	local rCode
	local MOUNTPOINT=''
	[ $NODESTATE -eq $ACTIVE ] && MOUNTPOINT="$NBI_PATH/$MEDIA"
	[ $NODESTATE -eq $PASSIVE ] && MOUNTPOINT="/$MEDIA"

	log "Mounting thumb drive to $MOUNTPOINT"
	$CMD_GREP -q "$MOUNTPOINT" $PROC_MOUNTS
	rCode=$?

	if [ $rCode -eq 0 ]; then
		log "Mount point($MOUNTPOINT) already mounted."
		return $exit_success
	fi

	# execute the mount instruction
 	$($CMD_MOUNT -o $OPTIONS -t $EXT3FS $THUMBDRIVE $MOUNTPOINT &>/dev/null)
	rCode=$?
	# If mount fails with -t option, attempt normal mount
	[ $rCode -ne 0 ] && {
		log "Mount with -t failed, attempting normal mount"
		$($CMD_MOUNT $THUMBDRIVE $MOUNTPOINT &>/dev/null)
		rCode=$?
		[ $rCode -ne 0 ] && abort "mount the Thumb Drive failed on active node"
	} 
	
	log "Success"
	return $exit_success
}
 
# This function is used to unmount the Thumb Drive 
# ----------------------------------------------------------------------
function unmount()
{
	local MOUNTPOINT=$1
	local ATTEMPTS=1
 	local MAX_ATTEMPTS=3
 	local rCode
 	local PIDS
 	log "Unmounting thumb drive: $MOUNTPOINT"

	# check if the mountpoint already exist on the node
 	$CMD_GREP -q "$MOUNTPOINT" $PROC_MOUNTS
 	rCode=$?

 	if [ $rCode -ne 0 ]; then
 		log "Mount point($MOUNTPOINT) already unmounted."
 		return $exit_success
	fi

 	# executes the umount instruction
	log "Attempt # $ATTEMPTS to unmount:"
 	$($CMD_UNMOUNT $MOUNTPOINT &>/dev/null)
 	rCode=$?
 	while [[ $rCode -ne 0 && $ATTEMPTS -lt $MAX_ATTEMPTS ]]; do
		log "Failed. Retrying again..."
		ATTEMPTS=$(( $ATTEMPTS + 1 ))
		# sleeps only if the umount command hasn't timed-out (RETURN_CODE!=124)
		[ $rCode -ne 124 ] && /bin/sleep 1
		# check the process still accessing the media
		lsof $MOUNTPOINT >> /var/log/acs/tra/logging/emf_mountfail_lsof.log

		#get all the pids and try to kill
		local PIDS="$(get_pids)"
		log "List of pids having open handles: $PIDS"
		[ ! -z "$PIDS" ] && {
			while read P; do
				log "Trying to kill $P using pkill"
				$($CMD_PKILL -f $P &>/dev/null)
			done < <(echo "$PIDS")
		}	
	
		# Now try to unmount again
		log "Attempt # $ATTEMPTS to unmount:"
		$($CMD_UNMOUNT $MOUNTPOINT &>/dev/null)
		rCode=$?
 	done

	if [ $rCode -ne 0 ]; then
		# its been observed that rpc server holds the handles open without any reason,
		# in such case, restarting the rpc server would release the open handles.
		log "Attempting to restart rpc mountd:"
		$CMD_KILLPROC -TERM /usr/sbin/rpc.mountd &>/dev/null # restarted by monitord
		rCode=$?
		[ $rCode -ne 0 ] && abort "failed to restart rpc.mountd"
		log "Success"

		log "Attempt # $(( $ATTEMPTS + 1 )) to unmount:"
		$($CMD_UNMOUNT $MOUNTPOINT &>/dev/null)
		rCode=$?

		if [ $rCode -ne 0 ]; then
			log "Failed. Attempt to flush cache and lazy unmount"
			sync; sync; sync
			$($CMD_UNMOUNT -l -f $MOUNTPOINT &>/dev/null)
			rCode=$?
			[ $rCode -ne 0 ] && abort "Umount failed" # give up
		fi	
	fi	
		
 	log "Success"
 	return $exit_success
}

# This function, lists the active users of the mount $MOUNT_POINT of MEDIA
# -----------------------------------------------------------------------------
function get_pids(){
  local PIDS=''
  PIDS=$($CMD_FUSER -m $MOUNT_POINT &>/dev/null)
  if [ -n "$PIDS" ]; then
    PIDS="$(echo $PIDS | $CMD_TR '[:space:]' '\n' | $CMD_AWK ' !x[$0]++')"
    for P in $PIDS; do
      echo -e "$P:\"$($CMD_PS -p $P -o cmd h)\""
    done
  fi
}

# This function is used to check the Thumbdrive availability 
# ----------------------------------------------------------------------
function is_usb_available()
{
	log "Checking for the Thumb drive availability"
	#$($CMD_LSSCSI | $CMD_GREP -qw $THUMBDRIVE)
	#TR HU81933 - ON SLES12 thumb drive is under /dev/eri_thumbrive not /dev/sdm 
	$CMD_LSL -l $PATH_DEV_DIR | $CMD_GREP $ERI_THUMBDRIVE > /dev/null
	[ $? -ne 0 ] && abort "Thumb drive is not available on the node"
	
	log "Success"
	return $exit_success
}

# This function is used format the Thumbdrive
# ----------------------------------------------------------------------
function format()
{
	log "Formatting the Thumbdrive($THUMBDRIVE):"
	$($CMD_MKFSEXT3 -F $THUMBDRIVE &>/dev/null)
	[ $? -ne 0 ] && abort "Formating the Thumbdrive failed"

	log "Success"
	return $exit_success
}

# This function is used remove the files/folders from the Thumbdrive
# ----------------------------------------------------------------------
function remove()
{
	local REMOVE_DATA=$*
	log "Removing the  the files/folders from the Thumbdrive($THUMBDRIVE):"
 	$($CMD_RM -rf  $REMOVE_DATA &>/dev/null)
 	[ $? -ne 0 ] && abort "Removing the  the files/folders from the Thumbdrive failed"

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

