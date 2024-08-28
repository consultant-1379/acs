#!/bin/bash
###############################################################################
# -----------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# -----------------------------------------------------------------------------
###############################################################################
# Name:
#       acs_emf_activeshare.sh
# Description:
#       A script to nfs share of contents of Nbi root to passive node.
#       It will be executed by active node during export or formatAndExport
#       when DVD drive is connected to passive node
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

#Sharing internal_root folder to passive node using nfs share
cat /etc/exports | grep "internal_root" 1>/dev/null 2>/dev/null

if [ $? -ne 0 ]
then
	echo '/data/opt/ap/internal_root 169.254.208.0/255.255.255.0(rw,sync,no_subtree_check,no_root_squash)' >> /etc/exports
fi

result=$(/opt/ap/apos/bin/servicemgmt/servicemgmt status nfs-mountd.service)

if [ $result = "1" ]
then
    /opt/ap/apos/bin/servicemgmt/servicemgmt start nfs-mountd.service 1>/dev/null 2>/dev/null
fi

exportfs -ra 1>/dev/null 2>/dev/null
exportfs -ra 1>/dev/null 2>/dev/null
