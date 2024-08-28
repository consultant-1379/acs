#!/bin/bash 
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       hcstart
# Description:
#       A script to start health check from COM CLI on APG43L node 
#
##
# Usage: 
# 	hcstart [-t <type>] [-v]
#       hcstart [-u] [-v] 
#	hcstart -h
##
# Output:
#       Command print-out is printed on console.
#
##
# Changelog:
# - Feb 03 2020 Pravalika(ZPRAPXX)
#       UFR phase 2 impacts: Minor bug fix for rule 52
# - Jan 05 2020 Suryanarayana pammi (XPAMSUR)
#       UFR phase 2 impacts: Few improvements for rule 39 handling
# - Dec 23 2020 Pravalika (ZPRAPXX)
#       UFR Phase 2 impacts: Restored the support for SW-UPGRADE ruleset,
#       Bug fixes:Removed support for "-t APSW-UPGRADE"
# - Dec 11 2020 Rajeshwari(XCSRPAD), Suryanarayana pammi (XPAMSUR), Pravalika P(ZPRAPXX)
#       UFR Phase 2 impacts: New rules, Removed SW-UPGRADE ruleset, TR fix:HY77563
# - Jul 31 2020 Siva Kumar Ganoz (XSIGANO)
#       HCSTART improvement
# - Jul 31 2020 Harika Bavana (XHARBAV)
#       HCSTART improvement
# - Jul 30 2020 Swapnika Baradi(XSWAPBA)
#        Implement -u option
# - Nov 07 2017 Furquan Ullah (XFURULL)
#	 Minor changes 
# - Apr 15 2015 Divya Chakkilam (XCHADIV)
#	 Rebase for CNI 109 22-APZ 212 33/6-460 for TR HT42512 
# - Apr 15 2015 Divya Chakkilam (XCHADIV)
#	 Rebase for CNI 109 22-APZ 212 33/6-460 for TR HT42512 
# - Feb 06 2014 - Trotta A. (xanttro)	
#	HCSTART improvement: 
#   integration with the AP, System Restore, Initiate step from 48 to 75
# - Thu Sep 12 2013 - Greeshmalatha C(xgrecha)
#	Modified to change result displayed in 
#	nov verbose mode
# - Tue July 30 2013  - Greeshmalatha C (XGRECHA)
#	Modified to remove option "-f" and add "-v"
# - Tue May 28 2013 -Furquan Ullah (xfurull)
#	Modified to validate "-l" option and
#	add IO_HCRS_32
# - Mon	May 13 2013 -Greeshmalatha C (XGRECHA)
#	Modified to add space in logs and end 
#	points for test case.
# - Mon	Apr 22 2013 -Greeshmalatha C (XGRECHA)
#	Modified to add Recomendation Warning 
# - Tue Apr 03 2013 -Greeshmalatha C (XGRECHA)
#       Enabling -l option to handle OSU issue. 
#       Option '-l' is not exposed to end user.
#       Hence, not including in usage.
# - Tue Mar 07 2013 - Madhu Muthyala (xmadmut)
#       Addressed TR HQ85046.
# - Tue May 15 2012 - Satya Deepthi (xsatdee)
#       First version.
##

# Source Common functions
. /opt/ap/acs/bin/acs_hcstart_common
. /opt/ap/acs/bin/acs_hcstart_reqid

# script-wide variables
a_silent=$TRUE
a_verbose=$FALSE
opt_u=$FALSE
STATUS=WARNING
a_reqid=
a_type=
arglist=
fail_flag=
pass_flag=
flag=0
eCode=0
eflag=0
wflag=0
pflag=0
nflag=0
HW_TYPE=$(GetHwType)
is_swm_2_0="/cluster/storage/system/config/apos/swm_version"
#Exit codes
exit_gen=1 #Exit code for General Fault
exit_usage=2 #Exit code for incorrect usage
exit_ioption=3 #Exit code for incompatible option
exit_req_fail=4 #Exit code for failed requirements
exit_active_node=5 #Exit code for failed requirements
this_node_file="/etc/cluster/nodes/this/hostname"
peer_node_file="/etc/cluster/nodes/peer/hostname"
this_node=`$CAT $this_node_file`
remote_node=`$CAT $peer_node_file`

# print running time for APG nodes
 function printuptime() {
 
        nodeType=$1
         this_node=`$CAT $this_node_file`
         remote_node=`$CAT $peer_node_file`
         node=
         days=
         print=$TRUE
         if [ $nodeType == "remote" ]; then
             $PING -c 2 $remote_node &> /dev/null
             if [ $? == 0 ]; then                
                node=`$ECHO $remote_node`
                launchCommand=`$SSH -n $remote_node $UPTIME | $AWK '{print $3" "$4}' | $GREP -i days`        
             else
                flog "remote node is not reachable"
                log "remote node is not reachable"
                print=$FALSE
             fi
         else
             launchCommand=`$UPTIME | $AWK '{print $3" "$4}' | $GREP -i days`
             node=`$ECHO $this_node`
         fi
 
        if [ $print -eq $TRUE ]; then
            days=`$ECHO $launchCommand`
            if [[ ! -z $days ]]; then
               flog "APG $node node is running from $days"
               log "APG $node node is running from $days"
            else
               flog "APG $node node is running for less than 1 day"
               log "APG $node node is running for less than 1 day"
            fi
         fi
 }

# print usage of the command
function usage() {
        ${ECHO}
        ${ECHO} -e "Usage: hcstart [-t <type>] [-v]"
	${ECHO} -e "       hcstart -h"
        ${ECHO} -e "       hcstart [-u] [-v]"
        ${ECHO}
        ${ECHO} -e "Where <type> can be:"
        ${ECHO} -e "GENERAL"
        ${ECHO} -e "SW-UPGRADE"
        ${ECHO} -e "SW-HEALTH"

	# XLUCPET - HW DECOUPLING in vAPZ
	# In Virtualized environment the argument HW-HEALTH is no more applicable
	if [[ $HW_TYPE != 'VM' ]] ;then
		${ECHO} -e "HW-HEALTH"
	fi
	# XLUCPET - HW DECOUPLING in vAPZ

        ${ECHO} -e "SECURITY"
	
	#BEGIN - xanttro - 15/01/2014 - HCSTART improvement - US6		
	${ECHO} -e "RESTORE"
	#END - xanttro - 15/01/2014 - HCSTART improvement - US6

	${ECHO}
}

function isActive(){
        local rCode
        local NODE_STATE
	local ACTIVE=1

        # check if we are running on 'active-node'
        node_id=$(</etc/cluster/nodes/this/id)
	if [ -f $is_swm_2_0 ];then
	        $(${CMD_IMMLIST} -a saAmfSISUHAState "safSISU=safSu=SC-$node_id\,safSg=2N\,safApp=ERIC-apg.nbi.aggregation.service,safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service" &>/dev/null)
	else
		$(${CMD_IMMLIST} -a saAmfSISUHAState "safSISU=safSu=$node_id\,safSg=2N\,safApp=ERIC-APG,safSi=AGENT,safApp=ERIC-APG" &>/dev/null)
	fi
        rCode=$?
        if [ $rCode -ne 0 ]; then
		$ECHO "Error when executing (General Fault)"
                abort "Node state undefined. Command can be executed only on active node. Exiting..." $exit_gen
	fi
	if [ -f $is_swm_2_0 ];then
        NODE_STATE=$(${CMD_IMMLIST} -a saAmfSISUHAState "safSISU=safSu=SC-$node_id\,safSg=2N\,safApp=ERIC-apg.nbi.aggregation.service,safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service" | ${CUT} -d = -f2)
	else
	NODE_STATE=$(${CMD_IMMLIST} -a saAmfSISUHAState "safSISU=safSu=$node_id\,safSg=2N\,safApp=ERIC-APG,safSi=AGENT,safApp=ERIC-APG" | ${CUT} -d = -f2)
	fi
        if [ $NODE_STATE -ne $ACTIVE ]; then
		$ECHO "Command can be executed only on the active node."
                abort "Command can be executed only on the active node. Exiting..." $exit_active_node
	fi
        return $exit_success
}

function trace_leave(){
	flog "$1() <<"
}

function check_files(){
        if [ ! -f $REQID_FILE ]; then
		${ECHO} "Error when executing (General Fault)"
		${ECHO} "$REQID_FILE does not exist."
                abort "$REQID_FILE does not exist. Exiting..." $exit_gen
        fi
	return $exit_success
}

function parse_args(){

	if [ $# -eq 0 ]; then
		a_verbose=$FALSE
		a_silent=$TRUE
		return $exit_success
	fi

	# To check the validity of the options issued
	args=$@
	count=$( $ECHO $args | $GREP -o "-" | $WC -l )
	if [ $count -eq 0 ]; then
	       $ECHO -e "Incorrect usage\nType \"hcstart -h\" for the command usage\n"
	        exit $exit_usage        # illegal option
	fi
	if [ `$ECHO $args | $GREP ^"[0-9]"` ]; then
                $ECHO -e "Incorrect usage\nType \"hcstart -h\" for the command usage\n"
                 exit $exit_usage        # illegal option
        fi
	count=$(( $count + 1 ))
	cnt=1
	flag=0
	for i in $(seq 1 $count)
	do
		# Check for illegal options like --,---,- etc
        	if [ $flag -gt 1 ]; then
                	$ECHO -e "Incorrect usage\nType \"hcstart -h\" for the command usage\n"
	                exit $exit_usage	# Illegal option
        	fi

		# To check if the 1st option is a valid option and begins with -
                if [[ ! `$ECHO $args | $GREP ^"-"` ]]; then
		        $ECHO -e "Incorrect usage\nType \"hcstart -h\" for the command usage\n"
			 exit $exit_usage        # illegal option
	        elif [ `$ECHO $args | $GREP -wc "-"` -gt 0 ];then
	                $ECHO -e "Incorrect usage\nType \"hcstart -h\" for the command usage\n"
		          exit $exit_usage
	        else
        	        val=`$ECHO $args | $CUT -d- -f $cnt`
                	ops=`$ECHO $val | $WC -w`
        	        if [[ -z "$val" || "$val" == " " ]]; then
                	        flag=$(( $flag + 1 ))
	                fi
			# To check if more than 1 arguments are given for an option
        	        if [ $ops -gt 2 ]; then
                	        $ECHO -e "Incorrect usage\nType \"hcstart -h\" for the command usage\n"
                        	exit $exit_usage	#Illegal option	
	                fi
	
			# Check to ensure that no option has an argument except -t or -r
        	        if [ $ops -eq 2 ]; then
                	        if [[ ${val:0:1} != "t" && ${val:0:1} != "r" ]]; then
                	        	$ECHO -e "Incorrect usage\nType \"hcstart -h\" for the command usage\n"
	                        	exit $exit_usage	#Illegal use of option	
	                        fi
        	        fi
	        fi	
        	(( cnt ++ ))
	done

	# Re-Initializing global variables 	
	a_local=$FALSE
        a_usage=$FALSE  
	opt_t=$FALSE        
        opt_u=$FALSE
	res=0 
	a_type=
	ARGS="t:vhlur:"
	while getopts "$ARGS" option 2> /dev/null
	do
        	case "$option" in
	        	t)
				if [ $opt_t -eq $TRUE ]; then
                                   
                                        ${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
                                        abort "Wrong option specified." $exit_usage
                                else
                                        opt_t=$TRUE
                                        a_type=$OPTARG
                                        if [ "$a_type" == "APSW-UPGRADE" ]; then
                                           ${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
                                           abort "Wrong option specified." $exit_usage
 
                                        fi
                                fi
        		;;
	        	l)
                                if [ $a_local -eq $TRUE ]; then
					${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
	                                abort "Wrong option specified." $exit_usage
                                else
					a_local=$TRUE
				fi
	        	;;
                        u)
                                if [ $opt_u -eq $TRUE ]; then
                                        ${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
                                        abort "Wrong option specified." $exit_usage
                                else
                                        a_type="APSW-UPGRADE"
                                        opt_u=$TRUE
                                fi
                        ;;
        		v)
				if [ $a_silent -eq $FALSE ] && [ $a_verbose -eq $TRUE ]; then
					${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
					abort "Wrong option specified." $exit_usage
				else	
					a_verbose=$TRUE
					a_silent=$FALSE
				fi	
        		;;
	    	    	#f)
                       #         if [ $a_silent -eq $TRUE ] && [ $a_verbose -eq $FALSE ]; then
		#			${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
                #                        abort "Wrong option specified." $exit_usage
                #                else
		#			a_silent=$TRUE
		#			a_verbose=$FALSE
                #                fi
        	#	;;

	        	r)
        			a_reqid=$OPTARG
	        	;;
        		h)    
				if [ $a_usage -eq $TRUE ]; then 
					${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
                                        abort "Wrong option specified." $exit_usage
				else
					a_usage=$TRUE
				fi      
                                #exit $exit_success                    
	        	;;
        		?)
				${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n" 
				abort "Wrong option specified." $exit_usage
			;;
	        esac
	done
        if [ $a_usage -eq $TRUE ]; then
		#if [ $# -gt 1 ]; then 
                # Modified to address TR HS44747 during Rebase1.2_CNI349
                if [[ $a_verbose -eq $TRUE || $a_local -eq $TRUE || $# -gt 1 ]]; then
                	${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
                         abort "Wrong option specified." $exit_usage
                else
        		usage
 			exit $exit_success
		fi
            
        fi
        if [[ $# -gt 1 ]] && [[ $opt_t -eq $TRUE ]] && [[ $opt_u -eq $TRUE ]]; then
                        ${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
                         abort "Wrong option specified." $exit_usage
        fi

        # implementing mutual exclusion between incompatible options
        #local sum=$(( $a_silent + $a_verbose ))
        #if [[ $a_silent -eq $TRUE && $a_verbose -eq $TRUE ]]; then
        #        ${ECHO} -e "Incompatible options.\nType \"hcstart -h\" for the command usage."
	#	abort "Incompatible options specified." $exit_ioption
        #fi

	return $exit_success
}

function launchJob(){

	#if [ $a_verbose -eq $TRUE ]; then
	#	${ECHO} -n "Executing Req-id[$a_reqid]: "
	#fi
	flog ""
	flog "Executing Req-id[$a_reqid]: "			
        $ECHO $a_reqid | $GREP "IO_HCRS_[0-9]" &> /dev/null
        if [ $? -ne 0 ]; then
		${ECHO} -e "Incorrect usage \nType \"hcstart -h\" for the command usage\n"
                abort "Wrong option specified." $exit_usage
	else	
		$a_reqid
		rCode=$?
        fi
	exitCode=0
	if [ $rCode -ne $exit_success ]; then
		#if [ $a_verbose -eq $TRUE ]; then
                #	$ECHO -e "Failed"
                #fi
                if [[ $rCode -eq $exit_ntp_local || $rCode -eq $exit_ntp_peer ]]; then
                    exitCode=$rCode
                else
                    exitCode=$exit_req_fail;
                fi
		#fail_flag=1
		log "Error Executing Req-id:$a_reqid"
	#else
	#	if [ $a_verbose -eq $TRUE ]; then
	#		${ECHO} "Passed"
	#	fi
	fi

	return $exitCode
}

function launchTmeOutThd(){

	pid=$1
	timeout=$2	
	count=0
	
	# convert timeout to wait for microseconds
	(( timeout = 2 * $timeout ))

	while ((count < timeout)); do
		if [[  -d "/proc/$pid" && "$count" -ne "$timeout" ]]; then
			# /proc is where information about running processes is found.
			# "-d" tests whether it exists (whether directory exists).
			# So, we're waiting for the job in question to show up.
			(( count ++ ))
			usleep 500000
		fi	
	done	
	
	if [ $count == $timeout ]; then
		flog "time-out: killing cmd-thread from timer-thread"
		$KILL -9 $pid &> /dev/null 
		#kill -15 $pid > /dev/null 2>&1
	fi
}

function getAPType(){
   local APType
   APType=$(${CMD_IMMLIST} -a apNodeNumber axeFunctionsId=1 | ${CUT} -d = -f2)
   echo $APType
}


function launchJobs(){

	# check if type-ordered is supported
        local TYPES=()
        local t_found=$FALSE
	#BEGIN - xanttro - 15/01/2014 - HCSTART improvement - US6 
	TYPES=('GENERAL' 'SW-UPGRADE' 'HW-HEALTH' 'SW-HEALTH' 'SECURITY' 'RESTORE' 'APSW-UPGRADE')
	#END - xanttro - 15/01/2014 - HCSTART improvement - US6

	# XLUCPET - HW DECOUPLING in vAPZ
	# In Virtualized environment the argument HW-HEALTH is no more applicable
	if [[ $HW_TYPE == 'VM' ]] ;then
		TYPES=('GENERAL' 'SW-UPGRADE' 'SW-HEALTH' 'SECURITY' 'RESTORE' 'APSW-UPGRADE')
	fi
	# XLUCPET - HW DECOUPLING in vAPZ

	if [ ! -z $a_type ]; then
        	for TYPE in ${TYPES[@]}; do
                	if [[ "$TYPE" = "$a_type" ]]; then
                        	t_found=$TRUE
                	fi
        	done

		if [[ "$t_found" = "$FALSE" ]]; then
			${ECHO} -e "Incorrect usage\nType \"hcstart -h\" for the command usage\n"
			abort "Type [$a_type] not supported" $exit_usage
		fi
	fi

	# form arglist to launchJob
	if [ $a_local -eq $TRUE ]; then
		arglist=" -l"	
	fi
	if [ $a_silent -eq $TRUE ]; then
	#	arglist=$arglist" -f"
		a_verbose=$FALSE
	else
		arglist=$arglist" -v"		
		a_verbose=$TRUE
	fi
	local linenr=0
	local type_compare=FALSE

	exec 4<"$REQID_FILE"
	while read cline <&4
	#old_IFS=$IFS
	#IFS=$'\n'
	#for cline in $(cat $REQID_FILE)
	do
		linenr=$(expr $linenr + 1);
		local line=$(trim "${cline%%;*}")
		
		#BEGIN - xanttro - 15/01/2014 - HCSTART improvement - US6
		typeLine=`echo $line | sed 's/\[//' | sed 's/\]//'`
		
		if [[ -z $a_type && ( "$typeLine" = "RESTORE" || "$typeLine" = "APSW-UPGRADE" )]]; then
			return $exit_success
		fi	
		#END - xanttro - 15/01/2014 - HCSTART improvement - US6
		
		# Remove if the line is empty
		if [ -z "$line" ]; then
			continue
		fi	
		
		# Parse Out comments
		if [[ "$line" =~ ^# ]]; then
			continue
		fi 		
	
		# Check if type is ordered
		if [[ "$line" =~ ^\[ ]]; then
			if [ $type_compare == $TRUE ]; then
				return $exit_success
			fi

			if [[ ! -z $a_type && "[$a_type]" = "$line" ]]; then
				type_compare=$TRUE
	                        if [ $a_verbose -eq $TRUE ]; then
					[[ $pass_flag == 1 ]] && STATUS="PASSED"
                	                ${ECHO} -e "Processing `echo $line | sed 's/\[//' | sed 's/\]//'` related checks:\n"
					RULE=`echo $line | sed 's/\[//' | sed 's/\]//'`
					fail_flag=0
					pass_flag=1
                        	fi
			else
				if [[ "$line" = "[HW-HEALTH]" && $HW_TYPE == 'VM' ]]; then
					type_compare=FALSE
					flog "HW-HEALTH rules is not executed in virtualized environment"
				fi
			fi
		
			if [[ $a_verbose -eq $TRUE && -z $a_type ]]; then
				
				if [[ "$line" = "[HW-HEALTH]" &&  $HW_TYPE == 'VM'  ]]; then
					flog "HW-HEALTH rules is not executed in virtualized environment"
					continue
				fi
                               
				[[ $pass_flag == 1 ]] && STATUS="PASSED"	
				${ECHO} -e "Processing `echo $line | sed 's/\[//' | sed 's/\]//'` related checks:\n"
				RULE=`echo $line | sed 's/\[//' | sed 's/\]//'`
				fail_flag=0
				pass_flag=1
			fi
			flog ""	
			flog "Processing "$line" Req-ids"	
			
			continue
		fi
	
		if [[ $type_compare == $TRUE || -z $a_type ]]; then 
			# Collect each token	
			local jobid=$(trim "${line%%:*}")
			local rest=$(trim "${line#*:}")
			local timeout=$(trim "${rest%%:*}")
			rest=$(trim "${rest#*:}")
			
			#BEGIN - xanttro - 31/03/2014 - HCSTART improvement option RESTORE
			export reason=$(trim "${rest%%:*}")
			#END - xanttro - 31/03/2014 - HCSTART improvement option RESTORE
			
			rest=$(trim "${rest#*:}")
			local slogan=$(trim "${rest%%:*}")

			#BEGIN - xanttro - 31/03/2014 - HCSTART improvement option RESTORE
			export exportSlogan=$slogan
			#END - xanttro - 31/03/2014 - HCSTART improvement option RESTORE			
			
			rest=$(trim "${rest#*:}")
			local recactn=$rest
			
			#local recactn=$rest
			local recactn=$(trim "${rest%%:*}")
			rest=$(trim "${rest#*:}")

			local mmljob=$(trim "${rest%%:*}")
			rest=$(trim "${rest#*:}")

			local aplist=$(trim "${rest%%:*}")
			rest=$(trim "${rest#*:}")
			
			local VM_Indication=$rest
			
			#local recactn=$(trim "${rest%%:*}")
			#rest=$(trim "${rest#*:}")
			#local mml_flag=$rest
			#echo job, mml: $jobid, $mml_flag
			#echo jobid :$jobid
			
			# Make sure each two tokens are not same
			if [[ "$jobid" = "$timeout" || "$timeout" = "$slogan" || "$slogan" = "$recactn" ]]
			then
				${ECHO} "Error when executing (General Fault)"
				${ECHO} "Error while parsing requirement ID file"
				abort "Parse Error" $exit_gen
			fi

			#check if requirement has to be executed on this AP type (AP1 or AP2)
			local AP=AP$(getAPType)
			
			aplist=`echo $aplist |sed 's/,/ /g'`
			#=~ 
			if [[ ! $aplist =~ $AP ]]; then
				flog $AP" not in aplist Requirement skipped on this AP type"
				continue
			fi
			
			#XLUCPET - HW DECOUPLING in vAPZ
			#check if the hcstart command has been executed in virtualized environment

			if [[ $HW_TYPE == $VM_Indication ]] ;then
				flog "The rule "$jobid" is not applicable in virtualized environment"
				continue
			fi

			# If requirement has already been checked, 
			# move on to the next requirement
			[ `$CAT $LOG_DIR/$LOG_FILE | $GREP -cw $jobid` -gt 0 ] && continue
		
			killed=$FALSE	
			#mml_line=`$ECHO $line | grep -c MML$`
			mml_line=`$ECHO $line | grep -c :MML:`
			if [ `$ECHO $line | grep -c MML$` == 1 ]; then
				recactn=$(echo $recactn | awk -F: '{$NF=NULL; print}')
				export PORT=4422 # added to make mml work with sudo 
				timeout $timeout $0 -r $jobid $arglist	
				cmdStatus=$?
			if [ $cmdStatus != $exit_success ]; then
				killed=$TRUE
				#if [ $a_verbose -eq $TRUE && $fail_flag == 0 ]; then
				#   $ECHO -e "Failed"
				#	fail_flag=1
				#fi
			fi
			else
				( $0 -r $jobid $arglist )&
				cmdPid=$!
			
				# Start the time-out thread
				( launchTmeOutThd $cmdPid $timeout )&
				tmrPid=$!
				
				# Don't leak the children if we're interrupted by ^C etc.
				trap "$KILL $cmdPid $tmrPid; exit 5" INT TERM EXIT
				
				# Block until the command exits or times out.
				wait $cmdPid &> /dev/null
				cmdStatus=$?
		
				# Clear out the ^C trap.
				trap - INT TERM EXIT
			
				killed=$FALSE	
				if [ $cmdStatus -le 128 ] ; then
					# The command exited on its own; kill the timeout process.
					# This will kill the background shell, not the sleep
					# process started by it, so the sleep itself may hang around
					# for a while. But, since the subshell will be gone when it
					# exits, nothing will try to kill $cmdPid.
					trap 'exit 0' KILL
					$KILL -9 $tmrPid &> /dev/null 
				else
					killed=$TRUE
                                        #if [ $a_verbose -eq $TRUE && $fail_flag == 0 ]; then
                                        #        $ECHO -e "Failed"
                                        #fi
				fi
			fi	# for MML
			# Check launchJob exit Code now
			if [[ $killed == $TRUE || $cmdStatus != $exit_success ]]; then
                                if [[ $a_verbose -eq $TRUE && $fail_flag == 0 ]]; then
					STATUS=WARNING
					fail_flag=1
					pass_flag=0
                                fi
#BEGIN - xanttro - 31/03/2014 - HCSTART improvement option RESTORE			
                                if [[ "$a_type" == "APSW-UPGRADE" && $a_verbose -eq $TRUE &&  $slogan =~ "Mismatches between the data disk" ]]; then
                                        $ECHO -e "Recommended Action: $recactn\n"

				elif [[ "$a_type" != "RESTORE" ]]; then	
					if [ $a_verbose -eq $TRUE ]; then
                                               if [[ $slogan =~ "NTP" && $cmdStatus -eq $exit_ntp_local ]]; then
                                                $ECHO -e "$reason: $slogan active side"
                                               elif [[ $slogan =~ "NTP" && $cmdStatus -eq $exit_ntp_peer ]]; then
                                                $ECHO -e "$reason: $slogan passive side"
                                               else
						$ECHO -e "$reason: $slogan"
                                               fi
						$ECHO -e "Recommended Action: $recactn\n"
					fi

					flog "Slogan:$slogan"
					flog "Recommended Action:$recactn"
				elif [ $a_verbose -eq $TRUE ]; then
						#$ECHO -e "$reason: $slogan"
						$ECHO -e "Recommended Action: $recactn\n"
				fi
#END - xanttro - 31/03/2014 - HCSTART improvement option RESTORE				
				trace_leave $jobid
				eCode=$exit_req_fail
			fi
			if [ $cmdStatus != $exit_success ];then
				stat=( `echo $reason` )	
				for status in ${stat[@]};do
					if [[ "$status" == "ERROR" ]]; then
					       	eflag=1
					elif [[ "$status" == "WARNING" ]]; then
						wflag=1
                                                if [[ "$jobid" == "IO_HCRS_43" ]]; then
                                                       nflag=1
						fi
                                                if [[ "$jobid" == "IO_HCRS_50" ]]; then
                                                       nflag=1
                                                fi
                                                if [[ "$jobid" == "IO_HCRS_52" ]]; then
                                                       nflag=1
                                                fi
                                                if [[ "$jobid" == "IO_HCRS_54" ]]; then
                                                       nflag=1
                                                fi 
					else
					         pflag=1
				 	fi		 
				done
			fi
			[ $flag == 1 ] && { 
				STATUS=ERROR 
				flag=0
			}

					
		fi
	#done <$REQID_FILE	
	done 
	return $exit_success
}

# _____________________ _____________________ 
#|    _ _   _  .  _    |    _ _   _  .  _    |
#|   | ) ) (_| | | )   |   | ) ) (_| | | )   |   
#|_____________________|_____________________|
# Here begins the "main" function...

# Set the interpreter to exit if a non-initialized variable is used.
set -u

# Check if the command is executed on the active node only
isActive

# Parse command line arguments.
parse_args "$@"

# if requirment-id exists, script is launched from with-in
if [ ! -z $a_reqid ]; then
	launchJob
	eCode=$?
else
	# Log the parameters in the system log
	log "START: <$0 $*>"

	# Rotate hc log if required
	logrotate
	
	# Checks for requirements files existance
	check_files

	# launch all req-ids
	launchJobs

        [[ $pass_flag == 1 ]] && STATUS="PASSED"

	# message to appear on COM-CLI if all-things go right.
#	echo $eCode
	if [ $a_verbose -eq $TRUE ]; then
	if [[ $eflag -eq 1 ]]; then
		${ECHO} -e "RESULT: APG is not healthy\n"
	elif [[ $wflag -eq 1 && $nflag -eq 0 ]]; then
		${ECHO} -e "RESULT: APG is healthy\n"
        elif [[ $wflag -eq 1 && $nflag -eq 1 ]]; then
                ${ECHO} -e "RESULT: APG is partially healthy\n"
	else 
		${ECHO} -e "RESULT: APG is healthy\n"
  	fi
	else
		if [[ $eflag -eq 1 ]]; then
                        if [ $opt_u -eq $TRUE ]; then
                                ${ECHO} -e "Recommended actions must be followed: give command hcstart -u -v"
                        else
                  		${ECHO} -e "Recommended actions must be followed: give command hcstart -v"
		        fi
                       	${ECHO} -e "RESULT: APG is not healthy\n"
		elif [[ $wflag -eq 1 ]]; then
                        if [ $opt_u -eq $TRUE ]; then                        
                                ${ECHO} -e "Recommended actions require user attention: give command hcstart -u -v"
                        else
				${ECHO} -e "Recommended actions require user attention: give command hcstart -v"
                        fi
                	if [[ $nflag -eq 0 ]]; then
                        	${ECHO} -e "RESULT: APG is healthy\n"
                	else
                        	${ECHO} -e "RESULT: APG is partially healthy\n"
                	fi
		else
			${ECHO} -e "RESULT: APG is healthy\n"
		fi		
	fi

#	if [[ $eflag -eq 1 ]]; then
#		${ECHO} -e "RESULT: APG is not healthy\n"
#	elif [[ $wflag -eq 1 ]]; then
#		${ECHO} -e "Recommendation action must be followed before proceeding"
#		${ECHO} -e "RESULT: APG is healthy\n"
#	else 
#		${ECHO} -e "RESULT: APG is healthy\n"
#  	fi
	
	#if [ $eCode -ne 0 ]; then
	#	if [ "$STATUS" == "WARNING" ];then
	#		${ECHO} -e "Recommendation action must be followed before proceeding"
	#		${ECHO} -e "RESULT: APG is healthy"
	#	elif [ "$STATUS" == "PASSED" ];then
	#		${ECHO} -e "RESULT: APG is healthy"
	#	else 
	#		${ECHO} -e "RESULT: APG is not healthy"
	#	fi
	#else
	#	${ECHO} -e "RESULT: APG is healthy"
	#fi
        # Print running time on both the nodes
        printuptime "local"
        printuptime "remote"
	
	log "END: <$0 $*>"
	eCode=$?
fi

# the script returns the status of the last invoked function
exit $eCode

