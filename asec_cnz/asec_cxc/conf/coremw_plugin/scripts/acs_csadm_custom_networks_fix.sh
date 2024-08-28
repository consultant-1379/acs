#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2020 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       acs_csadm_custom_networks_fix.sh
# Description:
#       A script to update the iptable and ip6table rules for
#       custom networks for FTP, TELNET and MTS protocols.
# Usage:
#       Used during APG upgrade installation only for Virtual.
##
# Output:
#       None.
##
# Changelog:
# - Thr Feb 17 2022 - Sowjanya Medak (XSOWMED)
#   Removed Telnet code
# - Mon Jun 01 2020 - Neelam Kumar (XNEELKU)
#   First version.

# Common variables
TRUE=$( true; echo $? )
FALSE=$( false; echo $? )

CLUSTERCONF="/opt/ap/apos/bin/clusterconf/clusterconf"
IMMFIND="/usr/bin/cmw-utility immfind"
IMMLIST="/usr/bin/cmw-utility immlist"
RHOST=$(</etc/cluster/nodes/peer/hostname)
STATE_FTP=""

function log(){
  local LOG_TAG='acs_csadm_custom_networks_fix'
  local MESSAGE="${*:-notice}"
  /bin/logger $LOG_TAG "$MESSAGE" &>/dev/null
}

function retry_imm() {
  local COMMANDLINE=$@
  for (( i=0; i<5; i++ )); do
    ${COMMANDLINE}
    local RETCODE=$?
    if [ $RETCODE -eq 0 ]; then
      return $RETCODE
    else
      sleep 2
    fi
  done
  exit $false
}

function fetch_imm_status() {

  for protocol in FTP 
  do
    OBJECT=$(echo "asecConfigdataId=$protocol,acsSecurityMId=1")
    $(retry_imm ${IMMFIND}| grep $OBJECT > /dev/null)
    if [ $? == 0 ];then
      STATE=$(retry_imm ${IMMLIST} $OBJECT | grep "state" | awk -F= '{print $2}')
      eval STATE_$protocol=${STATE} > /dev/null
    else
      eval STATE_$protocol="INVALID"
    fi
  done

}

function fetchCustomInterfaces() {

  CUST_INTERFACE_LIST=''
  INTERFACE_LIST=$( ${CLUSTERCONF} interface -D | grep -w ethernet | awk '{print $4}' | sort | uniq)

  for eth in $INTERFACE_LIST; do
      if echo $eth | grep -Eq 'eth7|eth8|eth9|eth10' ; then
        CUST_INTERFACE_LIST="$CUST_INTERFACE_LIST $eth"
      fi
  done

  if [ -z "$CUST_INTERFACE_LIST" ]; then
    log "CUSTOM interfaces found NULL, skipping the execution"
    exit $TRUE
  fi 
}

function isvAPG() {
  SHELF_ARCH=$(retry_imm ${IMMLIST} -a apgShelfArchitecture axeFunctionsId=1 |  cut -d = -f2)
  [ $SHELF_ARCH -eq 3 ] && return $TRUE
  return $FALSE
}

function addRule() {

  local PORT=$1
  local STATE=$2
  local ETHERNET=$3

  $( ${CLUSTERCONF} iptables --m_add all -A INPUT -p tcp --dport $PORT -i $ETHERNET -j $STATE > /dev/null)
  if [ $? -ne 0 ]; then
    log "addRule: IPv4 iptable rule add failed for port[$PORT]"
    exit $TRUE
  fi

  if [ "$PORT" == '21' ]; then
    # add rule for ipv6 as well.
    $( ${CLUSTERCONF} ip6tables --m_add all -A INPUT -p tcp --dport $PORT -i $ETHERNET -j $STATE > /dev/null)
    if [ $? -ne 0 ]; then
      log "addRule: IPv6 iptable rule add failed for port[$PORT]"
      exit $TRUE
    fi
  fi
  CLUSTER_RELOAD=1
}

function align_ftp_rules() {

  local PORT=21
  local INPUT_STATE
  local iptables='iptables'
  if [ "$STATE_FTP" == 'ACCEPT' ]; then
   INPUT_STATE=ACCEPT
  elif [ "$STATE_FTP" == 'DROP' ]; then
    INPUT_STATE=DROP
  fi

  for CUST_ETH in eth1 $CUST_INTERFACE_LIST; do
    if echo $CUST_ETH | grep -Eq 'eth1|eth7|eth8' ; then 
      iptables='ip6tables'
    fi 
    `${CLUSTERCONF} $iptables -D |grep "tcp \-\-dport $PORT" | grep tcp | grep "[[:space:]]${CUST_ETH}" > /dev/null`
     if [ $? -eq 1 ];then
       addRule $PORT $INPUT_STATE ${CUST_ETH}
     fi
  done

}

function  reload_cluster_config() {
  $CLUSTERCONF mgmt --cluster --verify > /dev/null
  if [ $? -ne 0 ]; then
    log 'cluster verification failed'
    $CLUSTERCONF mgmt --cluster --abort >/dev/null
    exit $TRUE
  fi

  # sleep(3): allow verify to settle down'
  sleep 3

  $CLUSTERCONF mgmt --cluster --reload > /dev/null
  if [ $? -ne 0 ]; then
    log 'cluster reload failed'
    $CLUSTERCONF mgmt --cluster --abort >/dev/null
    exit $TRUE
  fi

  $CLUSTERCONF mgmt --cluster --commit > /dev/null
  if [ $? -ne 0 ]; then
  log 'cluster commit failed'
   $CLUSTERCONF mgmt --cluster --abort >/dev/null
   exit $TRUE
  fi
}

function reload_iptables() {
    systemctl restart lde-iptables.service
    if [ $? -ne 0 ]; then
      log 'iptables restart failed'
      exit $TRUE
    fi

    local peer_id=$(</etc/cluster/nodes/peer/id)
    local peer_ip=$(</etc/cluster/nodes/all/$peer_id/networks/internal/primary/address)
    if $(ping -c 1 -W 1 $peer_ip &> /dev/null); then
      /usr/bin/ssh ${RHOST} 'systemctl restart lde-iptables.service' >/dev/null
      if [ $? -ne 0 ]; then
        log 'iptables restart failed on peer node'
        exit $TRUE
      fi
    fi
}

# _____________________
#|    _ _   _  .  _    |
#|   | ) ) (_| | | )   |
#|_____________________|
# Here begins the "main" function...

if isvAPG; then

  # Check for insecure protocols initialized or not.
  # If not initialized then exit.
  `${CLUSTERCONF} iptables -D |grep "tcp \-\-dport 21" | grep tcp | grep "[[:space:]]eth1" > /dev/null`
  if [ $? -eq 1 ];then
    log 'Insecure protocols not initialized, Skipping the script execution'
    exit $TRUE
  fi

  # Get the IMM status in order to add 
  # the rules for Insecure protocols
  fetch_imm_status

  # fetch the custom networks available on node
  fetchCustomInterfaces

  for protocol in ftp; do
    log "Aligning $protocol rules for custom networks"
    align_"$protocol"_rules
  done

  if [ $CLUSTER_RELOAD -eq 1 ]; then
    # reload the cluster configuration 
    reload_cluster_config 

    # restart the iptable rules 
    reload_iptables 
  fi
else 
  log 'Nothing to do, skipping the execution in native'
fi

exit $TRUE
