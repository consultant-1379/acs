#!/bin/bash
###############################################################################
# -----------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# -----------------------------------------------------------------------------
###############################################################################
# Name:
#       acs_emf_passiveshare.sh 
# Description:
#       A script to nfs share of DVD contents on passive node.
#       It will be executed by passive node during unlock operation or 
#       export or formatAndExport when DVD drive is connected to passive node 
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

#Mounting the contents of DVD on /media folder
#mount $(lsscsi | grep DVD | awk {'print $7'}) /media 1>/dev/null 2>/dev/null

#mount_res=$?
#if [ $mount_res -eq 0 ] || [ $mount_res -eq 32 ]
#then
	# Sharing /media folder using nfs share
	result=""
	$( grep -q "media" /etc/exports 2>/dev/null)
	if [ $? -eq 0 ]; then
		result=$(/opt/ap/apos/bin/servicemgmt/servicemgmt status nfs-mountd.service)
		if [ $result = "1" ]; then
			/opt/ap/apos/bin/servicemgmt/servicemgmt start nfs-mountd.service 1>/dev/null 2>/dev/null
		fi
	else
		echo '/media 169.254.208.0/255.255.255.0(rw,sync,no_subtree_check,no_root_squash)' >> /etc/exports
		result=$(/opt/ap/apos/bin/servicemgmt/servicemgmt status nfs-mountd.service)
		if [ $result = "1" ]; then
	    		/opt/ap/apos/bin/servicemgmt/servicemgmt start nfs-mountd.service 1>/dev/null 2>/dev/null
		fi
		exportfs -ra 1>/dev/null 2>/dev/null
	fi
#fi
