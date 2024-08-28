#!/bin/bash
#
# This script performs stop & start
# operations on Applications
# integrated with CoreMW
# Author: Malangshs Shaik


#setting  global variables

exit_success=0
exit_failure=1
true=1
false=0
SERVICE_GROUP=
node_id=0
node_state=0
active=1
standby=2
admin_unlock=1
admin_lock=2
admin_lock_instantiation=3
admin_unlock_instantiation=4
out_of_service=1
in_service=2
locked=2
timeout=5
locked_admin_state=
is_srvce_alrdy_stoped=
is_srvce_alrdy_running=
node1_active=0
node2_active=0
LOGGER=`which logger`
prog="apg_service_admin_operations"

## The usual usage information
##
function usage_error() {
        echo >&2
        echo "Usage: $0 <start/stop> <apg_service_name> [node_id]"
        echo "Where:"
        echo "apg_service_name: is the name of apg service to start/stop."
        echo "node_id: node number on which the apg \"No Red\" service is to stopped/started"
        echo "Example:"
        echo "2N Service:"
        echo "apg_service_control start/stop App1"
        echo "No Red Service:"
        echo "apg_service_control start/stop App1 1/2"
        echo >&2
}

[[ $# < 2 ]] && {
        usage_error
        exit $exit_failure
}

SERVICE_NAME=$2

[[ $# == 3 ]] && {
        slot_id=$3
}


die(){
        echo -e "$@"
        return $exit_failure
}


function validate_node_and_sg_info() {

        #check node id
        #
        node_id=`cmwea tipcaddress-get | cut -d , -f 3`

        if [ "$node_id" == "1" ] && [ "$node_id" == "2" ]; then
                echo "Node Id is invalid: " $node_id
                exit $exit_failure
        fi

        #check node state
        #
        node_state=`immlist -a saAmfSISUHAState "safSISU=safSu=SC-$node_id\,safSg=2N\,safApp=OpenSAF,safSi=SC-2N,safApp=OpenSAF" | cut -d = -f2`

        if [ "$node_state" == "1" ] && [ "$node_state" == "2" ]; then
                echo "Invalid state: \"$node_state\" for node: \"$node_id\""
                exit $exit_failure
        fi


        #check whether component exist in the system or not
        #
        if ! immfind -c SaAmfComp 2>/dev/null | grep "safComp=APG_Comp_$SERVICE_NAME" >/dev/null 2>&1; then
                echo "$SERVICE_NAME does not exist"
                exit $exit_failure
        fi



        #check service group of the service to be act upon
        #
        if immfind -c SaAmfSG 2>/dev/null | grep "safSg=APG_2NSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME" >/dev/null 2>&1; then
                SERVICE_GROUP=2N
        elif immfind -c SaAmfSG 2>/dev/null | grep "safSg=APG_NoRedSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME" >/dev/null 2>&1; then
                SERVICE_GROUP=NoRed
        else
                echo "Service Group is invalid: " $SERVICE_GROUP
                exit $exit_failure
        fi





        if [ "$node_id" == "1" ] && [ "$node_state" == "$active" ]; then
                node1_active=1
        elif [ "$node_id" == "2" ] && [ "$node_state" == "$active" ]; then
                node2_active=1
        else
                if [ "$SERVICE_GROUP" == "2N" ]; then
                        ${LOGGER} -s -t $prog "This script is to be executed from Active Node"
                        exit $exit_failure
                fi
        fi


}

function is_service_stopped(){

        su_admin_state=
        su_readiness_state=
        su_operational_state=
        dn=$1

        su_readiness_state=`immlist -a saAmfSUReadinessState $dn | cut -d = -f2`
        su_admin_state=`immlist -a saAmfSUAdminState $dn | cut -d = -f2`

        if [ "$su_admin_state" == "$locked" ]; then
                locked_admin_state=1
        fi

        if [ "$su_readiness_state" == "$out_of_service" ] && [ "$locked_admin_state" == "1" ]; then
                return $false
        fi

        if  [ "$su_readiness_state" == "$out_of_service" ]; then
                return $true
        fi

        return $false
}

function is_service_running(){

        su_admin_state=
        su_readiness_state=
        su_operational_state=
        dn=$1

        su_readiness_state=`immlist -a saAmfSUReadinessState $dn | cut -d = -f2`
        su_admin_state=`immlist -a saAmfSUAdminState $dn | cut -d = -f2`

        if [ "$su_admin_state" == "$locked" ];then
                locked_admin_state=1
        fi

        if [ "$su_readiness_state" == "$in_service" ]; then
                return $true
        fi

        if [ "$su_readiness_state" == "$out_of_service" ] && [ "$locked_admin_state" == "1" ]; then
                return $true
        fi

        return $false
}

function wait_for_adminstate() {

    local dn=$1
    local state=$2
    local start=$(date +"%s")
    local now=$start
    while test $((now-start)) -le $timeout; do
        eval $(immlist -a saAmfSUAdminState $dn)
        test "$saAmfSUAdminState" = "$state" && return $exit_success
        sleep 1
        echo .
        now=$(date +"%s")
    done
    die "Timeout wait_for_adminstate ($state) [$dn]"
}

function stop_app_service(){

        dn=$1
        active=$2
        su_admin_state=
        retval=
        is_srvce_alrdy_stoped=0

        if [ "$active" == "1" ]; then
                state="Active"
        else
                state="Passive"
        fi

        is_service_stopped $dn
        retval=$?
        if [ "$retval" == "$true" ]; then
                echo "Service not running."
                is_srvce_alrdy_stoped=1
                return $exit_success
        fi

        su_admin_state=`immlist -a saAmfSUAdminState $dn | cut -d = -f2`
        if [ "$su_admin_state" == "$admin_lock" ]; then
                immadm -o $admin_lock_instantiation $dn || die "Failed [immadm -o $admin_lock_instantiation $dn]"
                return `wait_for_adminstate $dn $admin_lock_instantiation`
        fi


        immadm -o $admin_lock $dn || die "Failed [immadm -o $admin_lock $dn]"
        retval=`wait_for_adminstate $dn $admin_lock`
        if [ "$retval" == "$exit_failure" ]; then
                return $exit_failure
        fi

        immadm -o $admin_lock_instantiation $dn || die "Failed [immadm -o $admin_lock_instantiation $dn]"
        return `wait_for_adminstate $dn $admin_lock_instantiation`
}

function check_for_inservice_state(){
        dn=$1
        servicestate=`immlist $dn | grep saAmfSUReadinessState | awk '{print $3}'`
        if [ "$servicestate" == "$in_service" ];then
                return $exit_success
        else
                return $exit_failure
        fi
}

function start_app_service(){

        dn=$1
        active=$2
        su_admin_state=
        is_srvce_alrdy_running=0

        if [ "$active" == "1" ]; then
                state="Active"
        else
                state="Passive"
        fi

        is_service_running $dn
        retval=$?
        if [ "$retval" == "$true" ]; then
                echo "Service Already running."
                is_srvce_alrdy_running=1
                return $exit_success
        fi

        su_admin_state=`immlist -a saAmfSUAdminState $dn | cut -d = -f2`
        if [ "$su_admin_state" == "$admin_lock_instantiation" ]; then
               immadm -o $admin_unlock_instantiation $dn || die "Failed [immadm -o $admin_unlock_instantiation $dn]"
               wait_for_adminstate $dn $admin_lock
               retval=$?
        fi

        su_admin_state=`immlist -a saAmfSUAdminState $dn | cut -d = -f2`
        if [ "$su_admin_state" == "$admin_lock" ]; then
                immadm -o $admin_unlock $dn || die "Failed [immadm -o $admin_unlock $dn]"
                wait_for_adminstate $dn $admin_unlock
                retval=$?
                if [ "$retval" != "$exit_success" ];then
                        return $exit_failure
                fi
                check_for_inservice_state $dn
                retval=$?
                if [ "$retval" != "$exit_success" ];then
                         return $exit_failure
                fi
                return $retval
        fi

        return $exit_failure
}

function apg_service_stop() {

        retval=
        #handle NoRed service stop first
        #
        if [ "$SERVICE_GROUP" = "NoRed" ]; then

                #stop service on node 1
                if [ "$slot_id" == "1" ]; then
                        echo -n "Stopping the service on Node 1..."
                        dn="safSu=APG_SU1_$SERVICE_NAME,safSg=APG_NoRedSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"
                        stop_app_service $dn 1
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_stoped" == "1" ]; then
                                echo -n
                        else
                                echo "Success!"
                        fi
                        return $exit_success
                fi


                #if stop service on node 2
                if [ "$slot_id" == "2" ]; then
                        echo -n "Stopping the service on Node 2..."
                        dn="safSu=APG_SU2_$SERVICE_NAME,safSg=APG_NoRedSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"
                        stop_app_service $dn 1
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_stoped" == "1" ]; then
                                echo -n
                        else
                                echo "Success!"
                        fi
                        return $exit_success
                fi

                echo "Input the Node_Id to stop the NoRed Service. Valid Node IDs are 1 and 2"
                return $exit_failure
        fi


        #handle 2N service stop now.
        #
        if [ "$SERVICE_GROUP" = "2N" ]; then
                #if node1 is active
                if [ "$node1_active" == "1" ]; then

                        #stop service running on passive node
                        echo -n "Stopping the service on Passive Node..."
                        dn="safSu=APG_SU2_$SERVICE_NAME,safSg=APG_2NSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"

                        stop_app_service $dn 0
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_stoped" == "1" ]; then
                                echo -n
                        else
                                echo "Success!"
                        fi

                        #stop service running on active node
                        echo -n "Stopping the service on Active Node..."
                        dn="safSu=APG_SU1_$SERVICE_NAME,safSg=APG_2NSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"

                        stop_app_service $dn 1
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_stoped" == "1" ]; then
                                return $exit_failure
                        else
                                echo "Success!"

                        fi

                        return $exit_success

                fi

                #if node1 is standby, node2 is active
                if [ "$node2_active" == "1" ]; then
                        #stop service running on passive node
                        echo -n "Stopping the service on Passive Node..."
                        dn="safSu=APG_SU1_$SERVICE_NAME,safSg=APG_2NSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"

                        stop_app_service $dn 0
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_stoped" == "1" ]; then
                                echo -n
                        else
                                echo "Success!"
                        fi

                        #stop service running on active node
                        echo -n "Stopping the service on Active Node..."
                        dn="safSu=APG_SU2_$SERVICE_NAME,safSg=APG_2NSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"

                        stop_app_service $dn 1
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_stoped" == "1" ]; then
                                echo -n
                        else
                                echo "Success!"
                        fi

                        if [ "$is_srvce_alrdy_stoped" == "1" ]; then
                                return $exit_failure
                        fi

                        return $exit_success
                fi
        fi
}

function apg_service_start() {


        #handle NoRed service start first
        #
        if [ "$SERVICE_GROUP" = "NoRed" ]; then

                #start service on node 1

                if [ "$slot_id" == "1" ]; then
                        echo -n "Starting the service APP_$SERVICE_NAME on Node 1..."
                        dn="safSu=APG_SU1_$SERVICE_NAME,safSg=APG_NoRedSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"
                        start_app_service $dn 1
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_running" == "1" ]; then
                                echo -n
                        else
                                echo "Success!"
                        fi
                        return $retval
                fi

                #start service on node 2
                if [ "$slot_id" == "2" ]; then
                        echo -n "Starting the service APP_$SERVICE_NAME on Node 2..."
                        dn="safSu=APG_SU2_$SERVICE_NAME,safSg=APG_NoRedSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"
                        start_app_service $dn 1
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_running" == "1" ]; then
                                echo -n
                        else
                                echo "Success!"
                        fi
                        return $retval
                fi
                echo "Input the Node Id to start the NoRed Service. Valid Node IDs are 1 and 2"
                return $exit_failure
        fi

        #handle 2N service start now.
        #
        if [ "$SERVICE_GROUP" = "2N" ]; then

                #if node1 is active
                if [ "$node1_active" == "1" ]; then
                        #start the service on active node first

                        echo -n "Starting the service APP_$SERVICE_NAME on Active Node..."
                        dn="safSu=APG_SU1_$SERVICE_NAME,safSg=APG_2NSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"

                        start_app_service $dn 1
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_running" == "1" ]; then
                                echo -n
                        else
                                echo "Success!"
                        fi


                        #start the service on passive node
                        echo -n "Starting the service APP_$SERVICE_NAME on Passive Node..."
                        dn="safSu=APG_SU2_$SERVICE_NAME,safSg=APG_2NSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"
                        start_app_service $dn 0
                        retval=$?

                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed!"
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_running" == "1" ]; then
                                return $exit_failure
                        else
                                echo "Success!"
                        fi

                        return $exit_success
                fi

                #if node1 is standby, node2 is active
                if [ "$node2_active" == "1" ]; then

                        #start the service on active  node
                        echo -n "Starting the service APP_$SERVICE_NAME on Active Node..."
                        dn="safSu=APG_SU2_$SERVICE_NAME,safSg=APG_2NSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"

                        start_app_service $dn 1
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed! "
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_running" == "1" ]; then
                                echo -n
                        else
                                echo "Success! "
                        fi

                        #start the service on passive node
                        echo "Starting the service APP_$SERVICE_NAME on Passive Node..."
                        dn="safSu=APG_SU1_$SERVICE_NAME,safSg=APG_2NSG_$SERVICE_NAME,safApp=APG_$SERVICE_NAME"
                        start_app_service $dn 0
                        retval=$?
                        if [ "$retval" == "$exit_failure" ]; then
                                echo "Failed! "
                                return $exit_failure
                        elif [ "$is_srvce_alrdy_running" == "1" ]; then
                                return $exit_failure
                        else
                                echo "Success!"
                        fi

                        if [ "$is_srvce_alrdy_running" == "1" ]; then
                                return $exit_failure
                        fi

                        return $exit_success
                fi
        fi
}

case $1 in
        stop)
                validate_node_and_sg_info
                apg_service_stop
                retval=$?
                ;;
        start)
                validate_node_and_sg_info
                apg_service_start
                retval=$?
                ;;
        *)      usage_error
                exit $exit_failure
                ;;
esac

exit $retval

