#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cs_vlan_conf.sh
# Description:
#	This script is providing the CS VLAN Table configuration.
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
# - Tue Jul 25 2017 - Sowjanya M (xsowmed)
#	Added GEP7 support
# - Thu Apr 13 2017 - Anjali M (xanjali)
#       Moved the creation of AXE-DEF vlan to APOS in Virtual env 
# - Mon Sep 09 2013 - Stefano Volpe (estevol)
#	First version.
# - Fri Nov 22 2013 - Stefano Volpe (estevol)
# 	Added check to skip population during restore phase
# - Fri Jan 31 2014 - Stefano Volpe (estevol)
# 	Added fetching of BGCI Vlans from cluster.conf
# - Fri Jan 30 2015 - Stefano Volpe (estevol)
# 	Modified APZ-CH-A/B and AXE-DEF-A/B vlans with link-local subnets
# - Fri Feb 02 2017 - Stefano Volpe (estevol)
# 	Removed APZ-C2C and APZ-CH VLANs for VIRTUALIZED environment

arr_sm[0]="0.0.0.0"
arr_sm[1]="128.0.0.0"
arr_sm[2]="192.0.0.0"
arr_sm[3]="224.0.0.0"
arr_sm[4]="240.0.0.0"
arr_sm[5]="248.0.0.0"
arr_sm[6]="252.0.0.0"
arr_sm[7]="254.0.0.0"
arr_sm[8]="255.0.0.0"
arr_sm[9]="255.128.0.0"
arr_sm[10]="255.192.0.0"
arr_sm[11]="255.224.0.0"
arr_sm[12]="255.240.0.0"
arr_sm[13]="255.248.0.0"
arr_sm[14]="255.252.0.0"
arr_sm[15]="255.254.0.0"
arr_sm[16]="255.255.0.0"
arr_sm[17]="255.255.128.0"
arr_sm[18]="255.255.192.0"
arr_sm[19]="255.255.224.0"
arr_sm[20]="255.255.240.0"
arr_sm[21]="255.255.248.0"
arr_sm[22]="255.255.252.0"
arr_sm[23]="255.255.254.0"
arr_sm[24]="255.255.255.0"
arr_sm[25]="255.255.255.128"
arr_sm[26]="255.255.255.192"
arr_sm[27]="255.255.255.224"
arr_sm[28]="255.255.255.240"
arr_sm[29]="255.255.255.248"
arr_sm[30]="255.255.255.252"
arr_sm[31]="255.255.255.254"
arr_sm[32]="255.255.255.255"

AXE_FUNCTIONS="axeFunctionsId=1"

SYS_TYPE_ATTR="systemType"
SHELF_ARCHITECTURE_ATTR="apgShelfArchitecture"
AP_TYPE_ATTR="apNodeNumber"

IMM_LIST="immlist"
IMM_CFG="immcfg"
IMM_FIND="immfind"
GREP="grep"

VLAN_CLASS="AxeEquipmentVlan"
VLAN_ID="vlanId"
VLAN_CATEGORY="vlanCategoryId=1,AxeEquipmentequipmentMId=1"

NET_ADDRESS="networkAddress"
NETMASK="netmask"
NAME="name"
STACK="stack"
PCP="pcp"
VLANTYPE="vlanType"
RESERVED="reserved"

NODEA_HOSTNAME_ATTR="nodeAName"
NORTH_BOUND="northBoundId=1,networkConfigurationId=1"

ACS_BIN_DIR="/cluster/storage/system/config/acs_csbin/"
VLAN_FILE="vlan_list"
SOL_VLAN_FILE="solVlan_list"
UPD_VLAN_FILE="updVlan_list"
APOS_COMMON="/opt/ap/apos/conf/apos_common.sh"
source $APOS_COMMON


CLUSTERCONF="/opt/ap/apos/bin/clusterconf/clusterconf"
BGCI_A="bgci_a"
BGCI_B="bgci_b"

#variables for fetching hw_type
hw_type=''
cmd_hwtype='/opt/ap/apos/conf/apos_hwtype.sh'

#AP_TYPE enumerate
AP1=1
AP2=2

#SHELF_ARCHITECTURE enumerate
SCB=0
SCX=1
DMX=2
VIRTUALIZED=3

#SYSTEM_TYPE enumerate
SCP=0
MCP=1

function abort() {
        if [ $# -eq 2 ]; then
                echo -e "Aborted ($2)" >&2
                if [[ $1 =~ [0-9]+ ]]; then
                        exit $1
                else
                        exit 1
                fi
        else
                exit 1
        fi
}

function get_hw_type() {
	hw_type=$( $cmd_hwtype)
	if [ ! $hw_type ]; then
		abort 1  "Cannot read hardware Type"
	fi
}
function get_shelf_arch() {
	shelf_architecture=$($IMM_LIST -a $SHELF_ARCHITECTURE_ATTR $AXE_FUNCTIONS | awk 'BEGIN { FS = "=" } ; {print $2}')
	if [ ! $shelf_architecture ]; then
		abort 1  "Cannot read APG Shelf Architecture"
	fi
}

if  [ $# -gt 0 ] && [ $1 == 'u' ]; then
	#handling the upgrade scenario for sol vlan creation
	echo "checkin sol vlan file existence " 
	stat $ACS_BIN_DIR$SOL_VLAN_FILE > /dev/null  2>&1
	if [ $? == 0 ]; then
		echo "SOL vlan are already existing and hence vlan table population skipped."
	else
		get_hw_type >/dev/null
		get_shelf_arch >/dev/null
		if [[ "$hw_type" == "GEP5" || "$hw_type" == "GEP7" ]]; then

			CMD_SOL_A="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=169.254.214.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-SOL-A -a $STACK=0 $VLAN_ID=APZ-SOL-A,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_SOL_A 2>/dev/null || abort 1 'VLAN Table Population Failed'
			
			CMD_SOL_B="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=169.254.215.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-SOL-B -a $STACK=0 $VLAN_ID=APZ-SOL-B,$VLAN_CATEGORY" 			        
			kill_after_try 5 5 6 $CMD_SOL_B 2>/dev/null || abort 1 'VLAN Table Population Failed'
			
			if [ ! -d "$ACS_BIN_DIR" ];then
				mkdir $ACS_BIN_DIR > /dev/null  2>&1
			fi
			$IMM_FIND -c $VLAN_CLASS | $GREP -i APZ-SOL |xargs -i $IMM_LIST {} > $ACS_BIN_DIR$SOL_VLAN_FILE
		fi
	fi
	
	echo "checkin upd vlan file existence " 
	stat $ACS_BIN_DIR$UPD_VLAN_FILE > /dev/null  2>&1

	if [ $? == 0 ]; then
		echo "UPD vlan are already existing and hence vlan table population skipped."
	else
		get_hw_type >/dev/null
		get_shelf_arch >/dev/null
		if [ $shelf_architecture == $DMX ]; then
			CMD_UPD_A="$IMM_CFG -c $VLAN_CLASS -a $NAME=APZ-UPD-A -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-UPD-A,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_UPD_A 2> /dev/null || abort 1 'UPD VLAN Table Population Failed'
			CMD_UPD_B="$IMM_CFG -c $VLAN_CLASS -a $NAME=APZ-UPD-B -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-UPD-B,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_UPD_B 2> /dev/null || abort 1 'UPD VLAN Table Population Failed'
			CMD_UPD2_A="$IMM_CFG -c $VLAN_CLASS -a $NAME=APZ-UPD2-A -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-UPD2-A,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_UPD2_A 2> /dev/null || abort 1 'UPD VLAN Table Population Failed'
			CMD_UPD2_B="$IMM_CFG -c $VLAN_CLASS -a $NAME=APZ-UPD2-B -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-UPD2-B,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_UPD2_B 2> /dev/null || abort 1 'UPD VLAN Table Population Failed'

			if [ ! -d "$ACS_BIN_DIR" ];then
				mkdir $ACS_BIN_DIR > /dev/null  2>&1
			fi
			$IMM_FIND -c $VLAN_CLASS | $GREP -i APZ-UPD |xargs -i $IMM_LIST {} > $ACS_BIN_DIR$UPD_VLAN_FILE
		fi
	fi
	
	for VLAN_OBJ in $($IMM_FIND -c $VLAN_CLASS |grep -i BGCI );
	do
		VLAN_ATTR=$($IMM_LIST $VLAN_OBJ)
		VLAN_NAME=$(echo "$VLAN_ATTR" | awk '/name/{print $3}' )
		VLAN_RESERVED=$(echo "$VLAN_ATTR" | awk '/reserved/{print $3}' )
		if [ $VLAN_RESERVED != '1' ] ; then
			CMD="$IMM_CFG -a reserved=1 $VLAN_ID=$VLAN_NAME,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD 2>/dev/null || abort 1 'Failed to set as reserved VLAN'
			echo "changing $VLAN_NAME VLAN as reserved"
		fi
	done
else
	echo "checkin file existence " 
	stat $ACS_BIN_DIR$VLAN_FILE > /dev/null  2>&1
	if [ $? == 0 ]; then
		echo "Installation after restore. Vlan Table population skipped."
		exit 0
	fi

	#Get node A hostname
	nodea_name=$($IMM_LIST -a $NODEA_HOSTNAME_ATTR $NORTH_BOUND | awk 'BEGIN { FS = "=" } ; {print $2}')
	if [ ! $nodea_name ]; then
		abort 1  "Cannot read node A hostname"
	fi

	#Execute only on node A
	if [ $HOSTNAME != $nodea_name ]; then
		echo "This script can be executed only on node A"
		exit 0
	fi

	#Get ap_type
	ap_type=$($IMM_LIST -a $AP_TYPE_ATTR $AXE_FUNCTIONS | awk 'BEGIN { FS = "=" } ; {print $2}')
	
	if [ ! $ap_type ]; then
		abort 1  "Cannot read AP Type"
	fi

	#Get system_type
	system_type=$($IMM_LIST -a $SYS_TYPE_ATTR $AXE_FUNCTIONS | awk 'BEGIN { FS = "=" } ; {print $2}')
	if [ ! $system_type ]; then
		abort 1  "Cannot read APG System Type"
	fi

	#Get hw_type
	get_hw_type >/dev/null

	#Get shelf_architecture
	get_shelf_arch >/dev/null

	### configure VLAN Table
	if [ $AP1 == $ap_type ]; then
		# BSP Architecture
		if [ $shelf_architecture == $DMX ]; then
			echo "Populating VLAN Table in BSP configuration..."
		
			# Fetching BGCI VLANs from cluster.conf
		
			network_a=$($CLUSTERCONF network -D | grep $BGCI_A | awk '{print $4}')
			network_b=$($CLUSTERCONF network -D | grep $BGCI_B | awk '{print $4}')
		
			if [ ! $network_a ] || [ ! $network_b ]; then
				abort 1  "Cannot read BGCI VLANs from cluster configuration"
			fi

			subnet_a=$(echo $network_a | awk -F/ '{print $1}')
			mask_a=$(echo $network_a | awk -F/ '{print $2}')
			mask_a=${arr_sm[$mask_a]}

			subnet_b=$(echo $network_b | awk -F/ '{print $1}')
			mask_b=$(echo $network_b | awk -F/ '{print $2}')
			mask_b=${arr_sm[$mask_b]}
			
			CMD_C2C_A="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=192.168.171.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-C2C-A -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-C2C-A,$VLAN_CATEGORY" 
			kill_after_try 5 5 6 $CMD_C2C_A 2>/dev/null || abort 1 'VLAN Table Population Failed'

			CMD_C2C_B="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=192.168.174.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-C2C-B -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-C2C-B,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_C2C_B 2>/dev/null || abort 1 'VLAN Table Population Failed'

			CMD_CH_A="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=169.254.209.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-CH-A -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=1 $VLAN_ID=APZ-CH-A,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_CH_A 2>/dev/null || abort 1 'VLAN Table Population Failed'

			CMD_CH_B="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=169.254.210.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-CH-B -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=1 $VLAN_ID=APZ-CH-B,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_CH_B 2>/dev/null || abort 1 'VLAN Table Population Failed'

			CMD_DEF_A="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=169.254.211.0 -a $NETMASK=255.255.255.0 -a $NAME=AXE-DEF-A -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=1 $VLAN_ID=AXE-DEF-A,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_DEF_A 2>/dev/null || abort 1 'VLAN Table Population Failed'

			CMD_DEF_B="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=169.254.212.0 -a $NETMASK=255.255.255.0 -a $NAME=AXE-DEF-B -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=1 $VLAN_ID=AXE-DEF-B,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_DEF_B 2>/dev/null || abort 1 'VLAN Table Population Failed'

			CMD_BGCI_A="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS="$subnet_a" -a $NETMASK="$mask_a" -a $NAME=BGCI-A -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=1 -a $STACK=0  $VLAN_ID=BGCI-A,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_BGCI_A 2>/dev/null || abort 1 'VLAN Table Population Failed'

			CMD_BGCI_B="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS="$subnet_b" -a $NETMASK="$mask_b" -a $NAME=BGCI-B -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=1 -a $STACK=0  $VLAN_ID=BGCI-B,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_BGCI_B 2>/dev/null || abort 1 'VLAN Table Population Failed'			

			
			# UPD vlan creation 
			CMD_UPD_A="$IMM_CFG -c $VLAN_CLASS -a $NAME=APZ-UPD-A -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-UPD-A,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_UPD_A 2> /dev/null || abort 1 'UPD VLAN Table Population Failed'
			CMD_UPD_B="$IMM_CFG -c $VLAN_CLASS -a $NAME=APZ-UPD-B -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-UPD-B,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_UPD_B 2> /dev/null || abort 1 'UPD VLAN Table Population Failed'
			CMD_UPD2_A="$IMM_CFG -c $VLAN_CLASS -a $NAME=APZ-UPD2-A -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-UPD2-A,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_UPD2_A 2> /dev/null || abort 1 'UPD VLAN Table Population Failed'
			CMD_UPD2_B="$IMM_CFG -c $VLAN_CLASS -a $NAME=APZ-UPD2-B -a $PCP=6 -a $VLANTYPE=0 -a $RESERVED=0 -a $STACK=0 $VLAN_ID=APZ-UPD2-B,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_UPD2_B 2> /dev/null || abort 1 'UPD VLAN Table Population Failed'
		# Legacy BC Configuration
		elif [ $system_type == $MCP ]; then

			echo "Populating VLAN Table in BC configuration..."
			if [ $shelf_architecture != $VIRTUALIZED ]; then
			
				CMD_C2C="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=192.168.171.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-C2C -a $STACK=0 $VLAN_ID=APZ-C2C,$VLAN_CATEGORY"
				kill_after_try 5 5 6 $CMD_C2C 2>/dev/null || abort 1 'VLAN Table Population Failed'

				CMD_CH="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=192.168.172.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-CH -a $STACK=1 $VLAN_ID=APZ-CH,$VLAN_CATEGORY"
				kill_after_try 5 5 6 $CMD_CH 2>/dev/null || abort 1 'VLAN Table Population Failed'

                                CMD_DEF="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=192.168.173.0 -a $NETMASK=255.255.255.0 -a $NAME=AXE-DEF -a $STACK=1 $VLAN_ID=AXE-DEF,$VLAN_CATEGORY"
                                kill_after_try 5 5 6 $CMD_DEF 2>/dev/null || abort 1 'VLAN Table Population Failed'
			fi

		fi
		# SOL vlan creation for SMAU native
		if [[ "$hw_type" == "GEP5" || "$hw_type" == "GEP7" ]]; then

			CMD_SOL_A="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=169.254.214.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-SOL-A -a $STACK=0 $VLAN_ID=APZ-SOL-A,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_SOL_A 2>/dev/null || abort 1 'VLAN Table Population Failed'

			CMD_SOL_B="$IMM_CFG -c $VLAN_CLASS -a $NET_ADDRESS=169.254.215.0 -a $NETMASK=255.255.255.0 -a $NAME=APZ-SOL-B -a $STACK=0 $VLAN_ID=APZ-SOL-B,$VLAN_CATEGORY"
			kill_after_try 5 5 6 $CMD_SOL_B 2>/dev/null || abort 1 'VLAN Table Population Failed'
		fi
	fi
	echo "VLAN Table Configuration Completed"

	mkdir $ACS_BIN_DIR > /dev/null  2>&1
	$IMM_FIND -c $VLAN_CLASS | xargs -i $IMM_LIST {} > $ACS_BIN_DIR$VLAN_FILE
	if [[ "$hw_type" == "GEP5" || "$hw_type" == "GEP7" ]]; then
		$IMM_FIND -c $VLAN_CLASS | $GREP -i APZ-SOL |xargs -i $IMM_LIST {} > $ACS_BIN_DIR$SOL_VLAN_FILE				
	fi
	if [ $shelf_architecture == $DMX ]; then
		$IMM_FIND -c $VLAN_CLASS | $GREP -i APZ-UPD |xargs -i $IMM_LIST {} > $ACS_BIN_DIR$UPD_VLAN_FILE
	fi
	exit 0
fi
