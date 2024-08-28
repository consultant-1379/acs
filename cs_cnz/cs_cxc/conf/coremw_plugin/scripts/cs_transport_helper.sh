#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2018 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cs_transport_helper.sh
##
# Description:
#       A script to remove TransportM in Native Environment
##
# Changelog:

# -  28 Oct 2020 - Omkar Reddy(xsaured)
#    First version

HW_TYPE=`cat /storage/system/config/apos/installation_hw`
MODEL_FILE_LIST="/storage/system/config/com-apr9010443/etc/model/model_file_list.cfg"
TRANSPORT_MODEL_FILE="/storage/system/config/comsa_for_coremw-apr9010555/repository/AxeTransportM_mp.xml"

initInstall() {
    logger "cs_transport_helper.sh: initInstall()"
    if [ "$HW_TYPE" == "VM" ];then
        logger "cs_transport_helper.sh: Executing - HW_TYPE is VM. Linking CSBIN_TransportM_R1 COM_R1"
        `cmw-modeltype-link CSBIN_TransportM_R1 COM_R1`
        `sleep 3`
    fi
}

wrapupInstall() {
   logger "cs_transport_helper.sh: wrapupInstall()"
   /bin/true
}

initUpgrade() {
    logger "cs_transport_helper.sh: initUpgrade()"
    if [ "$HW_TYPE" != "VM" ];then
        logger "cs_transport_helper.sh: HW_TYPE is NATIVE. Remove AxeTransportM_mp.xml"
        if grep -q AxeTransportM_mp.xml "$MODEL_FILE_LIST"; then
                logger "cs_transport_helper.sh: TransportM entry is present in model_file_list.cfg. Deleting."
                `sed -i "/AxeTransportM_mp.xml/d" $MODEL_FILE_LIST`
        fi
        if [ -f "$TRANSPORT_MODEL_FILE" ]; then
                 logger "cs_tranport_helper.sh: TransportM file is present in /storage path. Deleting."
                `rm $TRANSPORT_MODEL_FILE`
        fi  
    fi
    if [ "$HW_TYPE" == "VM" ];then
        logger "cs_transport_helper.sh: Executing - HW_TYPE is VM. Linking CSBIN_TransportM_R1 COM_R1"
        `cmw-modeltype-link CSBIN_TransportM_R1 COM_R1`
        `sleep 3`
    fi

}

wrapupUpgrade() {
   logger "cs_transport_helper.sh: wrapupUpgrade()"
   /bin/true
}

case $1 in
    init_upgrade)
        initUpgrade
        ;;
    init_install)
	initInstall
        ;;
    wrapup_upgrade)
        wrapupUpgrade
        ;;
    wrapup_install)
	wrapupInstall
        ;;
esac

