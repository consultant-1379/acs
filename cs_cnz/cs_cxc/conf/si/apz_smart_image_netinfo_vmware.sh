#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2023 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#   apz_smart_image_netinfo_vmware.sh
# Description:
#   A script to create the network information required for the netconf transaction in vmware.
##
# Changelog:
# - Fri Dec 27 2023 - Rajeshwari Padavala (XCSRPAD)	
#     Flexible naming convention for internal networks											  
# - Fri June 16 2023 - Anjali M (XANJALI)
#     Script updated to work in case of multiple entries of properties
#     section in ovf-env.xml
# - Thu Apr 20 2023 - Srimukhi Nagalla (ZSGRAIN)
#     Script enhanced to support TIPC-A and TIPC-B networks
#     in vBSC
# - Mon Apr 09 2018 - Pratap Reddy Uppada (xpraupp)
#     Drop2 enhancement for vMSC-HC(i.e BC VM's can have 
#     networks less than the ones included in <PropertySection>)
# - Wed Feb 21 2018 - Pratap Reddy Uppada (xpraupp)
#     Impacts performed to handle internal networks
#     for MSC with SCP configuration
# - Wed Feb 07 2018 - Pratap Reddy Uppada (xpraupp)
#     Updated with UPD2 interface impacts
# - Thu Jan 23 2018 - Paolo Palmieri (epaopal)
#     CR_ROLE implementation
# - Thu Jan 18 2018 - Pratap Reddy Uppada (xpraupp)
#     Updated with vCD External Network impacts and 
#     included functions and comments for maintainance purpose
# - Mon Jan 8 2018 - Samantha Mv (xmanven)
#     Updated with vCD Networking impacts for vAPP networks
# - Thu Jan 11 2017 - Antonio Buonocunto (eanbuon)
#     First version.
##

# Sourcing common library
if [ -e /usr/local/tce.installed/apz_cloud_common.sh ]; then
  . /usr/local/tce.installed/apz_cloud_common.sh
else
  echo "ERROR: Failure while sourcing apz_cloud_common.sh"
  sudo reboot  
fi

### Local variables
OVF_ENV_FILE_MOUNT_POINT='/mnt/config'
OVF_ENV_FILE="$OVF_ENV_FILE_MOUNT_POINT/ovf-env.xml"
OVF_ENV_FILE_DEVICE_LABEL="OVF ENV"
MEID="$(getProperty MEID)"
CR_TYPE="$(getProperty CR_TYPE)"
CR_ROLE="$(getProperty CR_ROLE)" 
MEID_FILTER="$MEID"_""
EXTERNAL_NETWORK_KEYS=()
INTERNAL_NETWORK_KEYS=()
INTERNAL_PG_NETWORK_KEYS=()									
DUPLICATE_PORTGROUP_FOUND=0 
DUPLICATE_NETWORKNAME_FOUND=0
TEMP_MAPPING_FILE=$(mktemp -t tmp_mapping.XXXXXX)
SYSTEM_TYPE=''
APT_TYPE=''

# This function fetches the portgroup name and netname from the 
# temporary property file and stores them in a array 'EXTERNAL_NETWORK_KEYS[@]'. 
# And elements in this array are in below format:
#   <portgroup_name1>,<networkname1> <portgroup_name2>,<networkname2>
# Ex: EXTERNAL_NETWORK_KEYS[@]=( 'SIG1,DATALINK' 'SIG2,SIGTRAN' ..)
# This function invoked only in case of vCloud director deployments.
function fetch_portgrpname_and_netname() {
  local index=0
  local PORTGRP_NAME=''
  while read PORTGRP_NAME
  do
    if echo $PORTGRP_NAME | grep -Eq 'PORTGROUP_*'; then
      local SUB_STR=$(echo $PORTGRP_NAME | awk -F'oe:key=' '{print $2}'| awk '{print $1}' | tr -d '"')
      local PORTGROUP_NAME=$(echo $PORTGRP_NAME | awk -F'oe:value=' '{print $2}'|awk '{print $1}' | tr -d '"/>')
      if [ -z "$PORTGROUP_NAME" ]; then 
        continue
      else
        if [[ ${EXTERNAL_NETWORK_KEYS[@]} ]]; then 
          # check if already same port-group is present in the array 
          if echo ${EXTERNAL_NETWORK_KEYS[@]} | tr ' ' '\n'| awk -F',' '{print $1}'| grep -wq "^$PORTGROUP_NAME$" ; then
            DUPLICATE_PORTGROUP_FOUND=1
            abort_execution "Duplicate port-group name[$PORTGROUP_NAME] found. Please enter unique port-group name in property section during deployment phase"
          fi 
        fi 
      fi
      local SUFFIX=$(echo $SUB_STR| awk -F'_' '{print $NF}' )
      NETWORK_NAME_TAG=$(grep -w "NETWORK_NAME_$SUFFIX" $TEMP_PROPERTY_FILE 2>/dev/null)
      if [ -n "$NETWORK_NAME_TAG" ]; then
        NETWORK_NAME=$(echo $NETWORK_NAME_TAG | awk -F'oe:value=' '{print $2}'| awk '{print $1}' | tr -d '"/>')
        if [ -z "$NETWORK_NAME" ]; then 
          continue
        else
          if [[ ${EXTERNAL_NETWORK_KEYS[@]} ]]; then
            # check if already duplicate network name is present in the array
            if echo ${EXTERNAL_NETWORK_KEYS[@]} | tr ' ' '\n' | awk -F',' '{print $2}'| grep -wq "^$NETWORK_NAME$" ; then
              DUPLICATE_NETWORKNAME_FOUND=1
              abort_execution "Duplicate network name[$NETWORK_NAME] found. Please enter unique network name in property section during deployment phase"
            fi
          fi
        fi
      else
        continue
      fi
      EXTERNAL_NETWORK_KEYS[$index]="$PORTGROUP_NAME,$NETWORK_NAME"; ((index++))
    fi
  done < $TEMP_PROPERTY_FILE

  # To make the list of elements in sorting order
  EXTERNAL_NETWORK_KEYS=( $(echo ${EXTERNAL_NETWORK_KEYS[@]} | tr ' ' '\n' | sort -ru | tr '\n' ' '))
}
function fetch_portgrpname_and_netname_int_nw() {
  local index=0
  local PORTGRP_NAME=''
  local NET_KEY_FOUND
  local SUB_STR=''
  local PORTGROUP_NAME=''
  # Get internal network keys based on the configuration type
  get_internal_network_keys
  
  while read PORTGRP_NAME
  do
    if echo $PORTGRP_NAME | grep -Eq 'PORTGROUP_*'; then
       SUB_STR=$(echo $PORTGRP_NAME | awk -F'oe:key=' '{print $2}'| awk '{print $1}' | sed 's/\/>//' |tr -d '"' | awk -F'_' '{print $2}')
          NET_KEY_FOUND='0'
          for KEY in ${INTERNAL_NETWORK_KEYS[@]}; do
              if echo "${SUB_STR}" | grep  -w "$KEY"; then
                        NET_KEY_FOUND='1'
                        NETWORK_NAME=$KEY
                        break
                fi
          done
      PORTGROUP_NAME=$(echo $PORTGRP_NAME | awk -F'oe:value=' '{print $2}'|awk '{print $1}' |  sed 's/\/>//' |tr -d '"')
      [  "$NET_KEY_FOUND" == '0'  ] && continue
      [  -z "$PORTGROUP_NAME"  ]     && continue
      if [[ ${INTERNAL_PG_NETWORK_KEYS[@]} ]]; then
          # check if already same port-group is present in the array
          if echo ${INTERNAL_PG_NETWORK_KEYS[@]} | tr ' ' '\n'| awk -F',' '{print $1}'| grep -wq "^$PORTGROUP_NAME$" ; then
            DUPLICATE_PORTGROUP_FOUND=1
            abort_execution "Duplicate port-group name[$PORTGROUP_NAME] found. Please enter unique port-group name in property section during deployment phase"
          fi
      fi
      
            
      INTERNAL_PG_NETWORK_KEYS[$index]="$PORTGROUP_NAME,$NETWORK_NAME"; ((index++))
    fi
  done < $TEMP_PROPERTY_FILE

  # To make the list of elements in sorting order
  if [[ ${INTERNAL_PG_NETWORK_KEYS[@]} ]]; then
	INTERNAL_PG_NETWORK_KEYS=( $(echo ${INTERNAL_PG_NETWORK_KEYS[@]} | tr ' ' '\n' | sort -ru | tr '\n' ' '))
  
    for KEY in ${INTERNAL_NETWORK_KEYS[@]}; do
      if echo "${INTERNAL_PG_NETWORK_KEYS[@]}" | grep -wq "$KEY"; then
	    continue;
	  else
        abort_execution "No Port-Group found for [$KEY].\
          Please verify the port-group name on infrastructure and enter the correct value in the property section."
      fi
    done
  fi
}

# This function populates INTERNAL_NETWORK_KEYS array based on the 
# type of the configuration
function get_internal_network_keys() {
  
  if [ -n "$CR_TYPE" ]; then
    local VM_TYPE="$(echo $CR_TYPE | sed -e 's@_TYPE$@@g')"
    case $VM_TYPE in 
      CP)
        # system_property value is required to take decision of applicable networks to attach. 
        # For example:
        #  MSC+SCP = APZ-A, APZ-B, UPD 
        #  MSC+MCP = APZ-A, APZ-B, INT-SIG, UPD 
        #  HLR+MCP = APZ-A, APZ-B, INT-SIG, UPD
        #  BSC+SCP = APZ-A, APZ-B, UPD, TIPC-A, TIPC-B
        if [[ $SYSTEM_TYPE == 'SCP' ]]; then
          if [[ $APT_TYPE == 'BSC' ]]; then
            #SCP+BSC case
            INTERNAL_NETWORK_KEYS=('APZ-A' 'APZ-B' 'UPD' 'TIPC-A' 'TIPC-B')
          else
            #MSC+SCP case
            INTERNAL_NETWORK_KEYS=('APZ-A' 'APZ-B' 'UPD')
          fi
        else
          #MCP case
          local VM_ROLE="$(echo $CR_ROLE | sed -e 's@-@@g')"
          if [[ "$VM_ROLE" =~ 'CP1A'|'CP1B' ]] ; then
            # If VM_TYPE is CP and CR_ROLE = CP1A or CP1B then UPD is mandatory
            # MSC+MCP and HLR+MCP case for CP1
            INTERNAL_NETWORK_KEYS=('APZ-A' 'APZ-B' 'INT-SIG' 'UPD')
          elif [[ "$VM_ROLE" =~ 'CP2A'|'CP2B' ]] ; then
            # If VM_TYPE is CP and CR_ROLE = CP2A or CP2B then UPD2 is mandatory
            # MSC_MCP and HLR+MCP case for CP2
            INTERNAL_NETWORK_KEYS=('APZ-A' 'APZ-B' 'INT-SIG' 'UPD2')
          fi
        fi
      ;;
      BC)
        INTERNAL_NETWORK_KEYS=('APZ-A' 'APZ-B' 'AXE-DEF' 'INT-SIG') 
      ;;
      IPLB)
        INTERNAL_NETWORK_KEYS=('APZ-A' 'APZ-B' 'INT-SIG')
      ;;
      *)
        abort "Invalid CR_TYPE specified: $VM_TYPE"
      ;;
    esac
  else
    abort "apz_smart_image_netinfo_vmware: CR_TYPE found NULL!"
  fi
}

function fetch_netname_and_mac(){
  while read OVF_NETINFO_ITEM; do
    local OVF_NETINFO_ITEM_NAME="$(echo $OVF_NETINFO_ITEM | awk -F've:network=' '{print $2}'|\
      awk '{print $1}' | tr -d '"')"
    local OVF_NETINFO_ITEM_MAC="$(echo $OVF_NETINFO_ITEM | awk -F've:mac=' '{print $2}'|\
      awk '{print $1}' | tr -d '"')"
    echo "$OVF_NETINFO_ITEM_NAME;$OVF_NETINFO_ITEM_MAC" >> $TEMP_MAPPING_FILE
  done < $TEMP_FILE
}

# This function invokes only in case of vSphere deployments.
# And sets the network information.
function set_netname_and_mac() {
  while read OVF_NETINFO_ITEM; do
    local OVF_NETINFO_ITEM_NAME="$(echo $OVF_NETINFO_ITEM | awk -F';' '{print $1}')"
    local OVF_NETINFO_ITEM_MAC="$(echo $OVF_NETINFO_ITEM | awk -F';' '{print $2}')"
    # Logic to parse the Internal Network values
    local NET_KEY_FOUND='0'
    # First it checks for predefined internal networks keys. If any internal
    # network key match with port group name present in Ethernetadapter section,
    # then it sets the 'NET_KEY' and respective MAC Address to it.
    for NET_KEY in ${INTERNAL_NETWORK_KEYS[@]}; do
      if echo "$OVF_NETINFO_ITEM_NAME" | grep -q "$NET_KEY"; then
        setNetInfo "$NET_KEY" "$OVF_NETINFO_ITEM_MAC"
        NET_KEY_FOUND='1'
      fi
    done
    if [ "$NET_KEY_FOUND" == '0' ]; then
      OVF_NETINFO_ITEM_NAME="$(echo ${OVF_NETINFO_ITEM_NAME##$MEID_FILTER})"
      setNetInfo "$OVF_NETINFO_ITEM_NAME"  "$OVF_NETINFO_ITEM_MAC"
      NET_KEY_FOUND='1'
    fi
  done < $TEMP_MAPPING_FILE
}

# This function displays error message on CP/BC/IPLB VM's consoles when 
# port-group names are not macthed (or) two/more network names are matched 
# for single port-group name. It sends the error logs to Active AP VM and 
# stores the logs under /data/apz/data/smartimage path.
# Note: This function invokes only on vCloud director.
function abort_execution() {
  local message="$*"
  local PORTGRP_NAME=''
  local NETWORK_NAME=''
  log "ERROR[vCD]: $message"
  sleep 5
  if [ $DUPLICATE_PORTGROUP_FOUND -eq 1 ]; then 
    log "Entered PORTGROUP values in property section:"
    log ""
    while read PORTGRP_NAME
    do
      if echo $PORTGRP_NAME | grep -Eq 'PORTGROUP_*'; then
        local PORTGROUP_NAME=$(echo $PORTGRP_NAME | awk -F'oe:value=' '{print $2}'|awk '{print $1}' | tr -d '"/>')
        local PORTGROUP_KEY=$(echo $PORTGRP_NAME | awk -F'oe:key=' '{print $2}'| awk '{print $1}' | tr -d '"')
        log "$PORTGROUP_KEY : $PORTGROUP_NAME"; sleep 1 
      fi
    done < $TEMP_PROPERTY_FILE
    abort "Please re-deploy the VNF again with unique port-group names!!"
  elif [ $DUPLICATE_NETWORKNAME_FOUND -eq 1 ]; then
    log "Entered NETWORK_NAME values in property section"
    log ""
    while read NETWORK_NAME
    do 
      if echo $NETWORK_NAME | grep -Eq 'NETWORK_NAME_*'; then
        local NET_NAME=$(echo $NETWORK_NAME | awk -F'oe:value=' '{print $2}'|awk '{print $1}' | tr -d '"/>')
        local NET_KEY=$(echo $NETWORK_NAME | awk -F'oe:key=' '{print $2}'| awk '{print $1}' | tr -d '"')
        log "$NET_KEY : $NET_NAME"; sleep 1 
      fi
    done
    abort "Please re-deploy the VNF again with unique network names!!"
  else
    log "Actual port-group names in EthernetAdapterSection of ovf-env.xml:"
    while read line; do
      local line="$(echo $line|awk -F've:network=' '{print $2}'|\
        awk '{print $1}' | tr -d '"')"
      log "[PortGroup-Name]: $line" ; sleep 1
    done < $TEMP_FILE
    abort "Please re-deploy the VNF again with correct port-group names!!"
  fi

}

# This function invoked only in vCloud director.
function set_vcloud_netname_and_mac() {
  local count=0
  local DUPLICATE_PG_KEYS=()
  local UNMATCHED_NETWORK_KEYS=()
  
  # Get internal network keys based on the configuration type
  get_internal_network_keys
  local NETWORK_KEYS
  if [[ ${INTERNAL_PG_NETWORK_KEYS[@]} ]]; then
	  # Create NETWORK_KEYS array which contains both EXTERNAL and INTERNAL network elements
	  NETWORK_KEYS=( "${EXTERNAL_NETWORK_KEYS[@]}" "${INTERNAL_PG_NETWORK_KEYS[@]}" )
  else
      NETWORK_KEYS=( "${EXTERNAL_NETWORK_KEYS[@]}" "${INTERNAL_NETWORK_KEYS[@]}" )
  fi
  for ((n=0;n<${#NETWORK_KEYS[@]};n++)); do
    local PATTERN_MATCH_COUNT=0
    local PG="${NETWORK_KEYS[$n]}"
    local SEARCH_PATTERN=$(echo $PG| awk -F',' '{print $1}')
    local NETWORK_NAME=$(echo $PG | awk -F',' '{print $2}')
    [ -z "$NETWORK_NAME" ] && NETWORK_NAME="$SEARCH_PATTERN"

    # Find the lines matches with port group name and count the no of line matches 
    PATTERN_MATCH=$(cat $TEMP_MAPPING_FILE | awk -F';' '{print $1}'| grep -F "$SEARCH_PATTERN")
    MAC=$(cat $TEMP_MAPPING_FILE |  grep -F "$PATTERN_MATCH;" | awk -F';' '{print $2}')
    [ -n "$PATTERN_MATCH" ] && PATTERN_MATCH_COUNT=$(echo $PATTERN_MATCH | wc -w)

    # If PATTERN_MATCH_COUNT is 0 then no port-group is found in <EternetAdapterSection>. 
    # In this case maintain unmacthed port group names to evaluate in further steps.
    if [ $PATTERN_MATCH_COUNT -eq 0 ]; then 
      # This array contains the list of network keys that are not matched in <EternetAdapterSection>.
      UNMATCHED_NETWORK_KEYS=( ${UNMATCHED_NETWORK_KEYS[@]} "$SEARCH_PATTERN" )
    # In this case Only one match found and set network information.
    elif [ $PATTERN_MATCH_COUNT -eq 1 ]; then 
      setNetInfo "$NETWORK_NAME" "$MAC"
	  #line num works well in case of special chars line /
      line_num=$(grep -Fn "$PATTERN_MATCH" $TEMP_MAPPING_FILE | cut -d : -f 1)
      sed -i "${line_num}d" $TEMP_MAPPING_FILE 2>/dev/null

    # Multiple matches found
    elif [ $PATTERN_MATCH_COUNT -gt 1 ]; then
      # Below IF condition handles the sub string contains special characters(-.:;@!'*^$#)
      if echo $SEARCH_PATTERN | grep -Eq '^.*[[:punct:]].*' ; then
        local PATTERN_MATCHED=0; local UNIQUE_STRING=''
        for PATTERN in $PATTERN_MATCH; do
          SUB_STR=$(echo $SEARCH_PATTERN | tr '[:punct:]' '@'); SUB_STR=${SUB_STR%@*}
          # Replace any special characters in the pattern that is matched with the character '@'
          STRING_TO_MATCH=$(echo ${PATTERN} | tr '[:punct:]' '@')
          # Format/design the sub-string from the lines matched and it should 
          # same as the sub-string that is in NETWORK_KEYS array.
          # If only one match found after forming the sub-string then set network name and MAC
          # If more than one match found then exit with error 
          STRING_TO_MATCH=$(echo ${STRING_TO_MATCH##*$SUB_STR} | awk -F'@' '{print "'"$SUB_STR@"'"$2}')
          if echo "$STRING_TO_MATCH" | grep -qw "$(echo $SEARCH_PATTERN|tr '[:punct:]' '@')" ; then
            ((PATTERN_MATCHED++)); UNIQUE_STRING="$PATTERN"
          fi
        done
        if [ $PATTERN_MATCHED -eq 1 ]; then
          setNetInfo "$NETWORK_NAME" "$(echo "$UNIQUE_STRING" | awk -F';' '{print $2}')"
          sed -i "/${UNIQUE_STRING}/d" $TEMP_MAPPING_FILE 2>/dev/null
        elif [ $PATTERN_MATCHED -gt 1 ]; then
          abort_execution "Multiple port-groups found for:[$SEARCH_PATTERN]. Please specify a different sub-string or the exact port-group name."
        fi
      else
        # Short port-group name handling(i.e port groups having multiple matches
        #  - Add port-group to NETWORK_KEYS array which is having multiple line matches.
        #  - Count the port-group entry that is added again to NETWORK_KEYS array
        #  - If count greaterthan one then exit with error.
        #  - If count is zero, then set network name and MAC address 
        NETWORK_KEYS=( ${NETWORK_KEYS[@]} "$PG" )
        DUPLICATE_PG_KEYS=( ${DUPLICATE_PG_KEYS[@]} "$SEARCH_PATTERN" )
        if echo ${DUPLICATE_PG_KEYS[@]} | grep -wq "$SEARCH_PATTERN"; then 
          ((count++))
        fi
        if [ "$count" -gt 1 ]; then
          DUPLICATE_PG_KEYS=( $(echo ${DUPLICATE_PG_KEYS[@]} | tr ' ' '\n' | sort -ru | tr '\n' ' '))
          abort_execution "Multiple port-groups found for:[${DUPLICATE_PG_KEYS[@]}]. Please specify a different sub-string or the exact port-group name."
        fi
      fi
    fi
  done

  # All the SIG networks in <PropertySection> of VNF descriptor are may not 
  # attached to BC VM's. In this case check if mapping file is empty or not. 
  # If it is empty then parsing is completed and exit with success
  # If it is not empty then one of network is not matched and exit with error.
  if [[ ${UNMATCHED_NETWORK_KEYS[@]} ]]; then
    # Check all mandatory internal networks are attached to CP's and BC's. If any 
    # mandatory internal network is not attached to CP/BC VM's then exit with error.
	if ! [[ ${INTERNAL_PG_NETWORK_KEYS[@]} ]]; then        
		for KEY in ${UNMATCHED_NETWORK_KEYS[@]}; do
		  if echo "${INTERNAL_NETWORK_KEYS[@]}" | grep -wq "$KEY"; then
			abort_execution "No Port-Group found for [$KEY].\
			  Please verify the port-group name on infrastructure and enter the correct value in the property section."
		  fi
		done
	fi
    if [ -s ${TEMP_MAPPING_FILE} ]; then
      abort_execution "No Port-Group found for network keys "${UNMATCHED_NETWORK_KEYS[@]}". \
        Please verify the port-group name on infrastructure and enter the correct value in the property section."
    fi
  fi

}

# This function initializes/creates the files that are required.
function init() {
  # Extract the name of the device in which the OVF environment file ISO is available
  local DEVICE_NAME=$(blkid -t LABEL="$OVF_ENV_FILE_DEVICE_LABEL" -o device)
  [ -z "$DEVICE_NAME" ] && abort "OVF environment file device not found!"

  # Device correctly found: create the mount point and mount it
  mkdir -p $OVF_ENV_FILE_MOUNT_POINT
  mount $DEVICE_NAME $OVF_ENV_FILE_MOUNT_POINT
  [ $? -ne 0 ] && abort "Failed to mount the OVF environment file device."

  # Create a temporary file to store the data extracted from the OVF environment file
  TEMP_FILE=$(mktemp -t vmware.XXXXXX)
  cat $OVF_ENV_FILE | sed -n -e '/<\/PlatformSection>/,/<\/ve:EthernetAdapterSection>/p' | grep "<ve:Adapter"> $TEMP_FILE

  TEMP_PROPERTY_FILE=$(mktemp -t vmware_property_keys.XXXXXX)
  cat $OVF_ENV_FILE | sed -n -e '/<\/PlatformSection>/,/<\/ve:EthernetAdapterSection>/p'| grep "<Property" |\
    grep -E 'PORTGROUP_*|NETWORK_NAME_*'| grep -Ev '.*_OM|.*_CUST[1-4]' > $TEMP_PROPERTY_FILE

  SYSTEM_TYPE=$(cat $OVF_ENV_FILE  | grep -w 'system_type' | awk -F'oe:value=' '{print $2}' |awk '{print $1;exit}' | tr -d '"/>')
  [ -z "$SYSTEM_TYPE" ] && abort "apz_smart_image_netinfo_vmware: system_type property found NULL!!"

  APT_TYPE=$(cat $OVF_ENV_FILE  | grep -w 'apt_type' | awk -F'oe:value=' '{print $2}' |awk '{print $1;exit}' | tr -d '"/>')
  [ -z "$APT_TYPE" ] && abort "apz_smart_image_netinfo_vmware: apt_type property found NULL!!"
}

function cleanup() {
  # Remove the previously created all temporary file
  rm -f {$TEMP_FILE,$TEMP_PROPERTY_FILE}

  # dismount the OVF environment file device
  umount $OVF_ENV_FILE_MOUNT_POINT
  [ $? -ne 0 ] && log "WARNING: Failed to dismount the OVF environment device!"
  
}

#                                              __    __   _______   _   __    _
#                                             |  \  /  | |  ___  | | | |  \  | |
#                                             |   \/   | | |___| | | | |   \ | |
#                                             | |\  /| | |  ___  | | | | |\ \| |
#                                             | | \/ | | | |   | | | | | | \   |
#                                             |_|    |_| |_|   |_| |_| |_|  \__|

# This function finds the device in which the ovf-env.xml
# is available. If device found then creates the mount 
# point and mount it. Creates the temporary files that 
# contains property section and Ethernetadapter section 
init

# This function creates a temporary mapping file that contains
# network name(i.e port group name assigned by Vsphere and 
# MAC address in the format like <netname;mac address>
# Ex: vAPZ111_APZ-A;00:11:22:33:44:55
fetch_netname_and_mac

# If the deployment is in vCD,
#    then property section contains keys like 
#    'PROPERGROUP_SIGx' and 'NETWORK_NAME_SIGx'. 
#    And temporary property file contains only 
#    lines macthed with these tags.
# If the deployment is in vSphere,
#    The temporary property file doesn't contain
#    PROPERGROUP and NETWORK_NAME tags and 
#    below check is skipped.
if grep -Eq 'PORTGROUP_*|NETWORK_NAME_*' $TEMP_PROPERTY_FILE; then
  fetch_portgrpname_and_netname
  fetch_portgrpname_and_netname_int_nw
  #####to check if external portgroups are not present but internal portgroups are present
  if [[ ${EXTERNAL_NETWORK_KEYS[@]} ]]; then
    # If EXTERNAL_NETWORK_KEYS array populated with 
    # port group and network name elements then it 
    # is running on vCD with vAPP network.
    set_vcloud_netname_and_mac
  else
    # To support the scenario where we use the new OVF file, 
    # containing the new properties, but the networks are 
    # called according the old naming convention
    set_netname_and_mac
  fi
else
  # If Property section doesn't contain PORTGROUP and 
  # NETWORK_NAME tags then it is running on VMWare 
  # vSphere 6.0 and higher versions 
  set_netname_and_mac
fi

# This function removes all temporary files 
# created and dismount the OVF environment file device
cleanup

exit 0

# End of file
