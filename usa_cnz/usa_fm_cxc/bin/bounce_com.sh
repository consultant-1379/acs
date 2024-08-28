#!/bin/bash
#
#       Description     :       
#                               
#                               
#       Author          :       
#       Created On      :      
#

exit_success=0
exit_failure=1
TRUE=$(true;echo $?)
FALSE=$(false; echo $?)

log() {
	/bin/logger -t bounce_com.sh "$@"

}

die() {
	log "$@"
	exit $exit_failre

}	

function lock_comsa() {

	dn=$1

	log "Performing LOCK on COM"
	amf-adm lock $dn 
	rCode=$?
	if [ ${rCode} -ne 0 ]; then 
	  	die "LOCK operation on COM Failed"
	fi
	return

}

function unlock_comsa() {

	dn=$1

	log "Performing LOCK on COM"
	amf-adm unlock $dn
	rCode=$?
	if [ ${rCode} -ne 0 ]; then
		die "UNLOCK operation on COM Failed"
	fi
	return

}

function check_com_process() {

	if `ps ax | grep -v grep | grep com>/dev/null`
	then
		exit $exit_success 
	else
		die "COM process is not Started after stopping with com.sh"
	fi
	
}

function stop_com() {

	dn=$1

	log "Stopping COM using com.sh"
	/opt/com/bin/com.sh stop
	rCode=$?
	if [ ${rCode} -ne 0 ]; then
		unlock_comsa $dn
		die "Stopping COM Process is Failed"
	fi
	
}

#Get COMSA DN
is_swm_2_0="/cluster/storage/system/config/apos/swm_version" 
 if [ -f $is_swm_2_0 ]; then

 # COM restart
  amf-adm restart safComp=com-oam-server,safSu=SC-1,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service
  amf-adm restart safComp=com-oam-server,safSu=SC-2,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service

  else
   comsa_dn=`amf-state si | grep -i COMSA`
   log "COM-SA Dn: $comsa_dn"

   #perform comsa lock
   lock_comsa $comsa_dn

   #Stop com process 
   stop_com $comsa_dn

   #Unlock COMSA DN
   unlock_comsa $comsa_dn

 fi
#verify weather com is running
 check_com_process

