#!/bin/bash
###############################################################################
# -----------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ----------------------------------------------------------------------------
###############################################################################
# Name:
#       acs_emf_removeshare.sh
# Description:
#       A script to remove nfs share of internal root contents on active node.
#       It will be executed by active node during lock operation or
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
# - Thu Jul 29 2013 - Ramakrishna Maturu (xrammat)
#       First version.
###############################################################################

#Remove nfs share on active node
#umount 169.254.208.$(</etc/cluster/nodes/peer/id):/data/opt/ap/internal_root/
cat /etc/exports | grep "internal_root" 1>/dev/null 2>/dev/null
if [ $? -eq 0 ]
then
        sed -i /internal_root/d /etc/exports 1>/dev/null 2>/dev/null
        
	exportfs -ra 1>/dev/null 2>/dev/null
	drbd_res=$(/opt/ap/apos/conf/apos_drbd_status role drbd0 local)
	if [ "$drbd_res" != "Primary" ]
	then
	/opt/ap/apos/bin/servicemgmt/servicemgmt stop nfs-mountd.service 1>/dev/null 2>/dev/null
	fi
fi
