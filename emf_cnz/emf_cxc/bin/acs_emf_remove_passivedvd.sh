#!/bin/bash
###############################################################################
# -----------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ----------------------------------------------------------------------------
###############################################################################
# Name:
#       acs_emf_remove_passivedvd.sh
# Description:
#       A script to remove nfs share of DVD contents on passive node.
#       It will be executed by passive node during lock operation or 
#       export or formatAndExport when DVD drive is connected to passive node 
#       or during start-up of service.    
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
# - Tue Oct 04 2012 - Ramakrishna Maturu (xrammat)
# - Thu Jul 25 2013 - Ramakrishna Maturu (xrammat)
#       First version.
###############################################################################

function abort(){
	exit 0
}

cat /etc/exports | grep "media" 1>/dev/null 2>/dev/null
if [ $? -eq 0 ]
then
	sed -i /media/d /etc/exports 1>/dev/null 2>/dev/null
	exportfs -ra 1>/dev/null 2>/dev/null
	drbd_res=$(/opt/ap/apos/conf/apos_drbd_status role drbd0 local)

	if [ "$drbd_res" != "Primary" ];then
		/opt/ap/apos/bin/servicemgmt/servicemgmt stop nfs-mountd.service 1>/dev/null 2>/dev/null
	fi

	mountpoint /media 1>/dev/null 2>/dev/null
	mountpoint_res=`echo $?`


	[ $mountpoint_res -ne 0 ] && abort

	umount /media 1>/dev/null 2>/dev/null
	unmount_res=`echo $?`

	if [ $unmount_res -ne 0 ]; then
	for i in {1..4};do
		sleep 20
		umount /media 1>/dev/null 2>/dev/null
		unmount_res=`echo $?`
		[ $unmount_res -eq 0 ] && abort
	done
	fi

	exit $unmount_res
else
	abort	
fi
