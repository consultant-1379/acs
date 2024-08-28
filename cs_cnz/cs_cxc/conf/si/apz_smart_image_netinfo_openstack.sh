#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2017 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#   apz_smart_image_netinfo_openstack.sh
# Description:
#   A script to create the network information required for the netconf transaction in openstack.
##
# Changelog:
# - Thu Jan 23 2018 - Paolo Palmieri (epaopal)
#     CR_ROLE implementation
# - Wed Feb 22 2017 - Pranshu Sinha (xpransi)
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
HOT_ENV_FILE_MOUNT_POINT='/mnt/config'
HOT_ENV_FILE_DEVICE_LABEL="config-2"
MEID="$(getProperty MEID)"
MEID_FILTER="$MEID"_""

# Extract the name of the device in which the HOT environment file ISO is available
DEVICE_NAME=$(blkid -t LABEL="$HOT_ENV_FILE_DEVICE_LABEL" -o device)
if [ -z "$DEVICE_NAME" ]; then
  abort "HOT environment file device not found!"
fi

# Device correctly found: create the mount point and mount it
mkdir -p $HOT_ENV_FILE_MOUNT_POINT
mount $DEVICE_NAME $HOT_ENV_FILE_MOUNT_POINT
if [ $? -ne 0 ]; then
  abort "Failed to mount the HOT environment file device."
fi

# HOT environment file with full path
HOT_ENV_FILE_PATH="$(find "$HOT_ENV_FILE_MOUNT_POINT/openstack/latest" -maxdepth 1 -type f -name "user_data")"

# Create a temporary file to store the data extracted from the HOT environment file
TEMP_FILE=$(mktemp -t openstack.XXXXXX)
cat $HOT_ENV_FILE_PATH | grep -E '^mac=.*,network=.*,vnic=.*' > $TEMP_FILE

# Read network info
while read HOT_NETINFO_ITEM; do
  HOT_NETINFO_ITEM_NAME="$(echo $HOT_NETINFO_ITEM | awk -F',' '{ print $2 }' | awk -F'=' '{ print $2 }' | sed "s@^${MEID_FILTER}@@g")"   
  HOT_NETINFO_ITEM_MAC="$(echo $HOT_NETINFO_ITEM | awk -F',' '{ print $1 }' | awk -F'=' '{ print $2 }')"
  HOT_NETINFO_ITEM_VNIC="$(echo $HOT_NETINFO_ITEM | awk -F',' '{ print $3 }' | awk -F'=' '{ print $2 }' | sed 's@^.*_@@g')" 
  setNetInfo_withVNIC "$HOT_NETINFO_ITEM_NAME" "$HOT_NETINFO_ITEM_MAC" "$HOT_NETINFO_ITEM_VNIC"
done < $TEMP_FILE

# Remove the previously created temporary file
rm -f $TEMP_FILE

# Dismount the HOT environment file device
umount $HOT_ENV_FILE_MOUNT_POINT
if [ $? -ne 0 ]; then
  log "WARNING: Failed to dismount the HOT environment device!"
fi

exit 0

# End of file
