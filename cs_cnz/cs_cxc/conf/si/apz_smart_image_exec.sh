#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2017 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#   apz_smart_image_exec.sh
# Description:
#   A script to create the information required for the netconf transaction.
##
# Changelog:
# - Fri Dec 15 2017 - Raghavendra Koduri (xkodrag)
#       Detection of hardware for Red hat openstack platform
# - Thu Jan 23 2018 - Paolo Palmieri (epaopal)
#     CR_ROLE implementation
# - Wed Feb 22 2017 - Pranshu Sinha (xpransi)
#     Adapted for openstack
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
BC_TYPE_MARKER="00000"
CP_TYPE_MARKER="10000"
IPLB_TYPE_MARKER="30000"

# Get networks information according to supported infrastructures
case $VENDOR_ID in
  *vmware*)
    sudo $APZ_SMART_IMAGE_FOLDER/apz_smart_image_netinfo_vmware.sh
    [ $? -eq 0 ] || abort 'Failed to get network information in VMware.'
  ;;
  *)
    sudo $APZ_SMART_IMAGE_FOLDER/apz_smart_image_netinfo_openstack.sh
    [ $? -eq 0 ] || abort 'Failed to get network information in Openstack.'
  ;;
esac

# Fetch parameters stored in properties
UUID="$(getProperty UUID)"
CR_TYPE="$(getProperty CR_TYPE)"
CR_ROLE="$(getProperty CR_ROLE)"
NETCONF_SERVER="$(getProperty NETCONF_SERVER)"
NETCONF_PORT="$(getProperty NETCONF_PORT)"
IPNA_MAC="$(getProperty IPNA_MAC)"
IPNB_MAC="$(getProperty IPNB_MAC)"
IPNA_IP="$(getProperty IPNA_IP)"
IPNB_IP="$(getProperty IPNB_IP)"

CR_ROLE_ID=""
if [ "$CR_TYPE" ] && [ "$CR_ROLE" ]; then
  CR_ROLE="$(echo $CR_ROLE | sed -e 's@-@@g')"
  TYPE="$(echo $CR_TYPE | sed -e 's@_TYPE$@@g')"

  case $TYPE in
    BC)
      if [[ $CR_ROLE =~ ^BC[0-9]{1,2}$ ]]; then
        CR_ROLE=$(echo $CR_ROLE | sed -e 's@^BC@@g')
        CR_ROLE_ID="$(($CR_ROLE * 10))"
        CR_ROLE_ID="$(($CR_ROLE_ID + $BC_TYPE_MARKER))"
      else
        abort "Invalid CR_ROLE: $CR_ROLE specified for CR_TYPE: $CR_TYPE"
      fi
    ;;
    CP)
      if [[ $CR_ROLE =~ ^CP[0-9]{1,2}[A-B]$ ]]; then
        CR_ROLE=$(echo $CR_ROLE | sed -e 's@^CP@@g')
        CR_ROLE_ID="$(echo $CR_ROLE | sed -e 's@A$@1@g' | sed -e 's@B$@2@g')"
        CR_ROLE_ID="$(($CR_ROLE_ID + $CP_TYPE_MARKER))"
      else
        abort "Invalid CR_ROLE: $CR_ROLE specified for CR_TYPE: $CR_TYPE"
      fi
    ;;
    IPLB)
      if [[ $CR_ROLE =~ ^IPLB[0-9]{1,2}[A-B]$ ]]; then
        CR_ROLE=$(echo $CR_ROLE | sed -e 's@^IPLB@@g')
        CR_ROLE_ID="$(echo $CR_ROLE | sed -e 's@A$@1@g' | sed -e 's@B$@2@g')"
        CR_ROLE_ID="$(($CR_ROLE_ID + $IPLB_TYPE_MARKER))"
      else
        abort "Invalid CR_ROLE: $CR_ROLE specified for CR_TYPE: $CR_TYPE"
      fi
    ;;
    *)
      abort "Invalid CR_TYPE specified: $CR_TYPE"
    ;;
  esac
fi

# Creating the first part of APZ_SMART_IMAGE_TEMPLATE file
# Note: #MEID is a tag passed/used by sinetcc
#       sinetcc is replacing the tag with MEID identifier
cat > $APZ_SMART_IMAGE_FOLDER/$APZ_SMART_IMAGE_TEMPLATE << EOF
<ManagedElement>
  <managedElementId>#MEID</managedElementId>
  <SystemFunctions>
    <systemFunctionsId>1</systemFunctionsId>
    <AxeFunctions>
      <axeFunctionsId>1</axeFunctionsId>
      <SystemComponentHandling>
      <systemComponentHandlingId>1</systemComponentHandlingId>
        <EquipmentM>
          <equipmentMId>1</equipmentMId>
            <CrMgmt>
              <crMgmtId>1</crMgmtId>
              <ComputeResource>
                <computeResourceId>$UUID</computeResourceId>
                <crType>$CR_TYPE</crType>
                <macAddressEthA>$IPNA_MAC</macAddressEthA>
                <macAddressEthB>$IPNB_MAC</macAddressEthB>
                <uuid>$UUID</uuid>
EOF

# Get the vapz_dhcp parameter passed by APG
if [ "$(getParameter vapz_dhcp)" = "OFF" ];then
  log "Parameter vapz_dhcp = OFF, including IP addresses in the transaction"
  cat >> $APZ_SMART_IMAGE_FOLDER/$APZ_SMART_IMAGE_TEMPLATE << EOF
                <ipAddressEthA>$IPNA_IP</ipAddressEthA>
                <ipAddressEthB>$IPNB_IP</ipAddressEthB>
EOF

fi

# Add compute resource role id if available
if [ -n "$CR_ROLE_ID" ];then
cat >> $APZ_SMART_IMAGE_FOLDER/$APZ_SMART_IMAGE_TEMPLATE << EOF
				<crRoleId>$CR_ROLE_ID</crRoleId>
EOF
fi

if [ -e $APZ_CLOUD_SCRIPTS_FOLDER/$APZ_CLOUD_NETINFO_FILE ];then
  while read NETINFO_ITEM;do
    if [[ "$VENDOR_ID" =~ "vmware" ]]; then
    	NETINFO_NAME="$(echo $NETINFO_ITEM | awk -F';' '{print $1}')"
    	NETINFO_MAC="$(echo $NETINFO_ITEM | awk -F';' '{print $2}')"
	cat >> $APZ_SMART_IMAGE_FOLDER/$APZ_SMART_IMAGE_TEMPLATE << EOF
                <network>
	              <macAddress>$NETINFO_MAC</macAddress>
	              <netName>$NETINFO_NAME</netName>
                </network>
EOF
    else
        NETINFO_NAME="$(echo $NETINFO_ITEM | awk -F';' '{print $1}')"
        NETINFO_MAC="$(echo $NETINFO_ITEM | awk -F';' '{print $2}')"
	NETINFO_VNIC="$(echo $NETINFO_ITEM | awk -F';' '{print $3}')"
        cat >> $APZ_SMART_IMAGE_FOLDER/$APZ_SMART_IMAGE_TEMPLATE << EOF
                <network>
                      <macAddress>$NETINFO_MAC</macAddress>
                      <netName>$NETINFO_NAME</netName>
                      <nicName>$NETINFO_VNIC</nicName>
                </network>
EOF
    fi

  done < $APZ_CLOUD_SCRIPTS_FOLDER/$APZ_CLOUD_NETINFO_FILE
fi

# Creating the last part of APZ_SMART_IMAGE_TEMPLATE file
cat >> $APZ_SMART_IMAGE_FOLDER/$APZ_SMART_IMAGE_TEMPLATE << EOF
		      </ComputeResource>
            </CrMgmt>
          </EquipmentM>
        </SystemComponentHandling>
      </AxeFunctions>
    </SystemFunctions>
</ManagedElement>
EOF

exit 0

# End of file
