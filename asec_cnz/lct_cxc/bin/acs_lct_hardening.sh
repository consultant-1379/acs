#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# acs_lct_hardening.sh
# A script to modify linux configuration files and COM configuration
# file (libcom_cli_agent.cfg) when hardening parameters(inactivity timer 
# and welcome message) are changed.
##
# Usage:
# acs_lct_hardening.sh parameter_list
##
# Changelog:
# - Thu Nov 23 2017 - Pratap Reddy Uppada(xpraupp)
#       Fix for TR HW44358(updated sanity_check function)   
# - Mon Oct 17 2017 - Rajashekar Narla (xcsrajn)
#       Modified function "sanity_check","update_timeout" for TR HW34297 
# - Thu Feb 25 2016 - Roni Newatia (xronnew)
#       Modified function "modify_welcomemessage" for TR HU61308 
# - Mon Feb 22 2016 - Antonio Buonocunto
#	Reduce number of syslog restart
# - Mon Feb 08 2016 - Alessio Cascone (EALOCAE)
#       Impacts for SLES12 adoption.
# - Mon Apr 14 2014 - Furquan Ullah (xfurull)
#       Removed option 'w'
#	Added option 'g'
#	Added function modify_welcomemessage
# 	Removed welcome message related code
# - Fri Mar 14 2014 - Roni Newatia (xronnew)
#       Modified functions "update_pswd_length" and "update_pswd_history" for TR HS37456 
# - Mon Jul 02 2012 - Satya Deepthi (xsatdee)
#       Second version.
# - Fri Jun 22 2012 - Satya Deepthi (xsatdee)
#       First version.
##


# Global variables
LOTC_API="/usr/share/pso/storage-paths/config"
LOG_TAG='acs_lct_hardening.sh'
CFG_FILE=''
CFG_LOCK_FILE=''
CFG_TMP=''
COM_LOCAL_FOLDER='/opt/com/lib/comp'
AUTH_FILE="/etc/pam.d/acs-apg-lockout-tsgroup"
AUTH_LOCK_FILE="$AUTH_FILE.lock"
PSWD_FILE1="/etc/pam.d/acs-apg-password-local"
PSWD_LOCK_FILE=''
PSWD_FILE=''
LOGINDEFS_FILE="/etc/login.defs"
SHADOW_FILE="/etc/shadow"
LOGINDEFS_LOCK_FILE=''
BANNER_FILE="/cluster/storage/system/config/apos/welcomemessage.conf"
peer_node_file="/etc/cluster/nodes/peer/hostname"
var_log_path="/var/log"
var_log_path_1="$var_log_path/SC-2-1"
var_log_path_2="$var_log_path/SC-2-2"
startup_flag=0
df_mindays=0
df_maxdays=-1
GROUP_FILE="/etc/group"
car_ret_var="<BR>"
linux_date="1970-01-01"
# exit flags
exit_success=0
exit_failure=1
exit_pswd_age=3
exit_inactivity_cfg=4
# command-list
CMD_SED=/usr/bin/sed
CMD_GREP=/usr/bin/grep
CMD_CAT=/bin/cat
CMD_RM=/bin/rm
CMD_CP=/bin/cp
CMD_MV=/bin/mv
SSH="/usr/bin/ssh"
PING="/bin/ping"
CMD_WC=/usr/bin/wc
CMD_CUT=/usr/bin/cut
AWK=/usr/bin/awk
CMD_LN=/bin/ln
CMD_SYSLOG_RESTART="/opt/ap/apos/bin/servicemgmt/servicemgmt restart rsyslog.service"
CMD_TOUCH=/usr/bin/touch
CMD_LISTTSUSER=/usr/bin/listtsuser
CMD_CHAGE=/usr/bin/chage
CMD_WHO=/usr/bin/who
# script-wide variables
a_wMsg=''
a_timeout=''
a_lockoutBadCount=''
a_lockoutDuration=''
a_minlen=''
a_history=''
remote_node=''
audit_policies="cron user syslog daemon ftp ssh telnet"
SHADOW="/etc/shadow"
maxpswdage_file="/tmp/maxpswdage"
minpswdage_file="/tmp/minpswdage"
maxacntage_file="/tmp/maxacntage"
com_tmp_file="/tmp/com_lock"
com_restart_tmp_file="/tmp/com_restart"
audit_restart_flag="/tmp/.audit_flag"

is_swm_2_0="/cluster/storage/system/config/apos/swm_version" 

#----------------------------------------------------------------------------------------
function log(){
	/bin/logger -t $LOG_TAG "$@"
}
#----------------------------------------------------------------------------------------
function abort(){
	log "ABORTING: <$@>"
	exit $exit_failure
}
#----------------------------------------------------------------------------------------
function sanity_check(){
	[ ! -f $LOTC_API ] && abort "$LOTC_API File Not Found"
	CFG_PATH=$( cat $LOTC_API)
	[ -z $CFG_PATH ] && abort "$CFG_PATH is NULL"

	# It tries to find the libcom_cli_agent.cfg file under COM PSO path
	# (i.e. '/storage/syste/config/com-apr9010443/lib/comp'). If file not found 
	# under COM PSO folder, it fall backs to COM LOCAL path (i.e. /opt/com/lib/comp).
	# During MI:
	#  According to COM new behavior(i.e 7.1 CP8), It creates COM PSO path during 
	#  instantiation of COM only. So at this phase hardening service fetches the 
	#  COM configuration file in COM LOCAL folder.
	COM_PSO_FOLDER="$CFG_PATH/com-apr9010443/lib/comp"
	CFG_FILE=$(find $COM_PSO_FOLDER -mindepth 1 -maxdepth 1 -type f -name 'libcom_cli_agent.cfg' 2>/dev/null)
	if [[ -z "$CFG_FILE" || ! -f $CFG_FILE ]]; then
		CFG_FILE=$(find $COM_LOCAL_FOLDER -mindepth 1 -maxdepth 1 -type f -name 'libcom_cli_agent.cfg' 2>/dev/null)
	fi 
	[[ -z "$CFG_FILE" || ! -f $CFG_FILE ]] && abort "$CFG_FILE File Not Found"
	CFG_LOCK_FILE="$CFG_FILE.lock"

	BANNER_LOCK_FILE=$BANNER_FILE.lock
	BANNER_TMP=/tmp/welcomemessage.tmp

	CFG_TMP=/tmp/libcom_cli_agent.cfg.tmp

	[ ! -f $LOGINDEFS_FILE ] && abort "$LOGINDEFS_FILE File Not Found"
	LOGINDEFS_LOCK_FILE=$LOGINDEFS_FILE.lock

	[ ! -f $AUTH_FILE ] && abort "$AUTH_FILE File Not Found"

	[ ! -f $PSWD_FILE1 ] && abort "$PSWD_FILE1 File Not Found"
	PSWD_LOCK_FILE=$PSWD_FILE1.lock
	PSWD_FILE=$PSWD_FILE1

	if [ -f $CFG_TMP ]; then
		$CMD_RM -f $CFG_TMP
	fi
}
#----------------------------------------------------------------------------------------
function check_peer_node_state(){
	remote_node=$($CMD_CAT $peer_node_file)
	$PING -c 2 -w 2 $remote_node &> /dev/null
	if [ $? == 0 ]; then
		return $exit_success
	else
		return $exit_failure
	fi
}
#----------------------------------------------------------------------------------------
function lock_config_file(){
	[ ! -f $CFG_LOCK_FILE ] && $CMD_CP $CFG_FILE $CFG_LOCK_FILE
	[ ! -f $AUTH_LOCK_FILE ] && $CMD_CP $AUTH_FILE $AUTH_LOCK_FILE
	[ ! -f $PSWD_LOCK_FILE ] && $CMD_CP $PSWD_FILE $PSWD_LOCK_FILE
	[ ! -f $BANNER_LOCK_FILE ] && $CMD_CP $BANNER_FILE $BANNER_LOCK_FILE
	[ ! -f $LOGINDEFS_LOCK_FILE ] && $CMD_CP $LOGINDEFS_FILE $LOGINDEFS_LOCK_FILE
}
#----------------------------------------------------------------------------------------
function restore_config_file(){
	# Restore cluster and local files
	[ -f $CFG_LOCK_FILE ] && $CMD_MV $CFG_LOCK_FILE $CFG_FILE
	[ -f $AUTH_LOCK_FILE ] && $CMD_MV $AUTH_LOCK_FILE $AUTH_FILE
	[ -f $PSWD_LOCK_FILE ] && $CMD_MV $PSWD_LOCK_FILE $PSWD_FILE
	[ -f $BANNER_LOCK_FILE ] && $CMD_MV $BANNER_LOCK_FILE $BANNER_FILE
	[ -f $LOGINDEFS_LOCK_FILE ] && $CMD_MV $LOGINDEFS_LOCK_FILE $LOGINDEFS_FILE

	# Restore files on peer node
	check_peer_node_state
	if [ $? == 0 ]; then
		${SSH} -n $remote_node $CMD_MV $LOGINDEFS_LOCK_FILE $LOGINDEFS_FILE
		[ $? != 0 ] && abort "Abort operation failed on remote node"
	else
		abort "Remote node down. Abort operation not performed on remote node"
	fi
	
	# Restore audit policy
	abort_audit_policy
	$CMD_RM -f $maxpswdage_file
	$CMD_RM -f $com_tmp_file
}
#----------------------------------------------------------------------------------------
function update_olduser_expiry(){
	for TSUSER in $($CMD_LISTTSUSER)
	do
		tsuser_prefix=${TSUSER:0:3}
		[ "$tsuser_prefix" != 'ts_' ] && continue
		paswd_exp=$(cat $SHADOW_FILE |$CMD_GREP -w $TSUSER | awk -F':' '{print $5}')
		if [ ! -z "$paswd_exp" ];then
			last_day=$(cat $SHADOW_FILE |$CMD_GREP -w $TSUSER | awk -F':' '{print $3}')
			if [ $last_day -eq 0 ]; then		
				current_date=$(date +"%Y-%m-%d")
				new_date=$(date -d "$current_date $paswd_exp days" +%Y-%m-%d)
				new_age=$(($(date -ud "$new_date" "+%s") / 86400))
				${CMD_CHAGE} -E $new_age -M $df_maxdays -m $df_mindays -I 0 $TSUSER &>/dev/null
				[ $? != 0 ] && abort "Applying account info to tsuser [$TSUSER] failed"
				log "Applying account info for old user [$user_age] to tsuser [$TSUSER] Success." 
			else
				user_age=$((paswd_exp+last_day+1))
				${CMD_CHAGE} -E $user_age -M $df_maxdays -m $df_mindays -I 0 $TSUSER &>/dev/null
				[ $? != 0 ] && abort "Applying account info to tsuser [$TSUSER] failed"
				log "Applying account info for old user [$user_age] to tsuser [$TSUSER] Success."
			fi
		fi
	done
}
#----------------------------------------------------------------------------------------
function unlock_config_file(){
	# Commit cluster and local files
	log "Commiting transaction"
	$CMD_RM -f $CFG_LOCK_FILE
	$CMD_RM -f $AUTH_LOCK_FILE
	$CMD_RM -f $PSWD_LOCK_FILE
	$CMD_RM -f $LOGINDEFS_LOCK_FILE
	$CMD_RM -f $BANNER_LOCK_FILE

	# Commit audit policy
	for word in $audit_policies
	do
		file_name="$word""_log"
		[ -f $var_log_path_1/$file_name.1 ] && $CMD_RM -f $var_log_path_1/$file_name.1
		[ -f $var_log_path_1/$file_name.0 ] && $CMD_RM -f $var_log_path_1/$file_name.0
	done

	# Restart COM service if inactivity timer / welcome message are modified
	if [[ -f $com_tmp_file && ! -f $com_restart_tmp_file ]]; then
		(com_restart)&
	fi
	# Commit files on peer node
	check_peer_node_state
	if [ $? == 0 ]; then
		${SSH} -n $remote_node $CMD_RM -f $LOGINDEFS_LOCK_FILE
		[ $? != 0 ] && abort "Commit operation failed on remote node"
	else
		log "Remote node down. Commit operation not performed on remote node"
	fi
}
#----------------------------------------------------------------------------------------
function finalize_audit(){
	if [ -e $audit_restart_flag ];then
		rm -f $audit_restart_flag
		$CMD_SYSLOG_RESTART
		log "syslog restarted"
	else
		log "syslog restart not required"	
	fi
}
#----------------------------------------------------------------------------------------
function com_restart(){
	touch $com_restart_tmp_file
	system_oam_id=$($AWK -F: '/system-oam/{print $3}' $GROUP_FILE)
	com_emer_id=$($AWK -F: '/com-emergency/{print $3}' $GROUP_FILE)
	${CMD_WHO} | while read line
	do
		COM_USER=$(echo "$line"|$AWK '{print $1}')
		user_tnl=$(echo "$line"|$AWK '{print $2}')
		for id in $(id -G $COM_USER)
		do
			if [[ $id == $system_oam_id || $id == $com_emer_id ]];then
				echo "AP session will be closed in 10 seconds"|write $COM_USER $user_tnl
			fi
		done
	done
	sleep 10
	log "Requesting amf-adm restart of COM"
        if [ -f $is_swm_2_0 ];then
		amf-adm restart safComp=com-oam-server,safSu=SC-1,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service
		local ret_val1=$?
		amf-adm restart safComp=com-oam-server,safSu=SC-2,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service
		local ret_val2=$?
        else
        	amf-adm restart safComp=COMSA,safSu=1,safSg=2N,safApp=ERIC-APG
		local ret_val1=$?
        	amf-adm restart safComp=COMSA,safSu=2,safSg=2N,safApp=ERIC-APG
		local ret_val2=$?
        fi

	if [[ ! $ret_val1 || ! $ret_val2 ]]; then
		log "amf-adm restart of COM failed. Restarting COM by killing PID"
		pid_of_com=$(pidof com)
		echo pid: $pid_of_com
		kill $pid_of_com
	else
		log "amf-adm restart of COM success on both nodes"
	fi
	$CMD_RM -f $com_restart_tmp_file
	$CMD_RM -f $com_tmp_file
}
#----------------------------------------------------------------------------------------
function modify_welcome_msg(){
	wMsg="$a_wMsg"
	LINE=60
	wFlag=1
	single_word=0
	wVar=$((${#car_ret_var}-1))
	for i in $(seq 0 ${#wMsg})
	do
		if [ "${wMsg:$i:1}" == " " ]; then
			single_word=1
		fi
	done

	if [[ ${#wMsg} -le $LINE || $single_word -eq 0 ]]; then
		msg=
		echo $wMsg|grep $car_ret_var 1> /dev/null
		if [ $? == 0 ];then
			for i in $(seq 0 ${#wMsg})
			do
				msg+=${wMsg:$i:1}
				if [ $i -eq ${#wMsg} ]; then
					echo  "$msg</IntroductoryMessage>" >> $CFG_TMP
				fi
				
				if [ "${wMsg:$((i-wVar)):${#car_ret_var}}" == "$car_ret_var" ];then
					if [[ $wFlag == 1 ]]; then
						echo  "<IntroductoryMessage>${msg:0:$((i-wVar))}"  >> $CFG_TMP
						wFlag=0
						msg=""
					else
						echo  "${msg:0:$((${#msg}-${#car_ret_var}))}" >> $CFG_TMP
						msg=""
					fi
				fi
			done
			single_word=0;
		else
			echo  "<IntroductoryMessage>$wMsg</IntroductoryMessage>" >> $CFG_TMP
			return $exit_success
		fi
	fi
	
	if [ $single_word -eq 1 ]; then
		msg=
		for i in $(seq 0 ${#wMsg})
		do
			msg+=${wMsg:$i:1}
			if [ $i -eq ${#wMsg} ]; then
				echo  "$msg</IntroductoryMessage>" >> $CFG_TMP
			fi
			
			if [ "${wMsg:$((i-wVar)):${#car_ret_var}}" == "$car_ret_var" ];then
				if [[ $wFlag == 1 ]]; then
					echo "<IntroductoryMessage>${msg:0:$((i-wVar))}"  >> $CFG_TMP
					wFlag=0
					msg=""
				else
					echo  "${msg:0:$((${#msg}-${#car_ret_var}))}" >> $CFG_TMP
					msg=""
				fi
			fi
			
			if [[ ${#msg} -ge $LINE && ${wMsg:$i:1} == " " ]]; then
				if [[ $wFlag == 1 ]]; then
					echo  "<IntroductoryMessage>$msg" >> $CFG_TMP
					wFlag=0
					msg=""
				else
					echo  "$msg" >> $CFG_TMP
					msg=""
				fi
			fi
		done
	fi
}
#----------------------------------------------------------------------------------------
function modify_welcomemessage(){
	wMsg="$a_wMsg"
	LINE=80
	echo -n "" > $BANNER_TMP
	[ ! -z "$wMsg" ] && print_border && echo -ne "\n" >> $BANNER_TMP
	
	if [[ ${#wMsg} -le $LINE ]] ; then
		msg=""
		for (( i=0; i <= ${#wMsg}; i++ ))
		do
			msg+=${wMsg:$i:1}
			if [ $i -eq ${#wMsg} ]; then
				echo -n "$msg" >> $BANNER_TMP
			fi
			
			if [ "${wMsg:$i:1}" == "<" ] && [ "${wMsg:$i:4}" == "<BR>" ]; then
				[ ! -z ${#msg} ] &&  msg=${msg:0:$((${#msg}-1)) }&& echo -n "$msg" >> $BANNER_TMP
				echo -ne "\n" >> $BANNER_TMP
				msg=""
				i=$(($i + 3))
			fi
			
			if [ "${wMsg:$i:1}" ==  " " ]; then
				echo ${wMsg:$i:1}
				echo -n "${msg}" >> $BANNER_TMP
				msg=""
			fi
		done
		
		[ ! -z "$wMsg" ] && echo -ne "\n" >> $BANNER_TMP && print_border
	fi
    
	if [[ ${#wMsg} -gt $LINE ]]; then
		j=1;
		msg=""
		lastspaceposition=0
		for (( i=0; i <= ${#wMsg}; i++ ))
		do
			msg+=${wMsg:$i:1}
			if [ $i -eq ${#wMsg} ]; then
				echo -n "$msg" >> $BANNER_TMP
			fi
			
			if [ "${wMsg:$i:1}" == "<" ] && [ "${wMsg:$i:4}" == "<BR>" ]; then
				[ ! -z ${#msg} ] &&  msg=${msg:0:$((${#msg}-1)) }&& echo -n "$msg" >> $BANNER_TMP
				echo -ne "\n" >> $BANNER_TMP
				j=1 && msg="" &&  i=$(($i +3))
				continue
			fi

			if [ $j -ge $LINE ] || [ ${#msg} -eq $LINE ]; then
				if [ $((${#msg}))  -eq  $j ] || [ ${#msg} -eq $LINE ]; then
					echo -n "${msg}" >> $BANNER_TMP && echo -ne "\n" >> $BANNER_TMP
					msg="" && j=1
				else
					if [ $(( $lastspaceposition + ${#msg} )) -eq $j ]; then
						if [ "${wMsg:$i:1}" ==  " " ] || [ "${wMsg:$i:2}" == " " ]; then
							echo -n "${msg}" >> $BANNER_TMP &&  echo -ne "\n" >> $BANNER_TMP
							lastspaceposition=0 && msg="" && j=1
						else
							if [ ! -z $lastspaceposition ]; then
								echo -ne "\n" >> $BANNER_TMP
							fi
							j=${#msg};
							lastspaceposition=0
						fi
					fi
				fi
			else
				if [ "${wMsg:$i:1}" ==  " " ]; then
					echo -n "${msg}" >> $BANNER_TMP
					msg="" && lastspaceposition=$j;
				fi
				j=$(( $j + 1 ))
			fi
		done
		echo -ne "\n" >> $BANNER_TMP && print_border
	fi
}
#--------------------------------------------------
function update_timeout(){
	CFG_TMP=/tmp/libcom_cli_agent.cfg.tmp
	$CMD_CP $CFG_FILE $CFG_TMP
	timeout_in_sec=$(( $a_timeout * 60 ))
	$($CMD_SED -i 's/<connectionTimeOut>.*<\/connectionTimeOut>/<connectionTimeOut>'$timeout_in_sec'<\/connectionTimeOut>/g' $CFG_TMP) 2> /dev/null
	rCode=$?
	if [ $rCode -ne $exit_success ]; then
		restore_config_file
		unlock_config_file
		abort "update_timeout Failed with eCode[$rCode]"
	fi

	$CMD_CP $CFG_TMP $CFG_FILE

	if [ -f $CFG_TMP ]; then
		$CMD_RM -f $CFG_TMP
	fi
	
	if [ $startup_flag -eq 1 ]; then 
		$CMD_RM -f $CFG_LOCK_FILE
	fi
	 
	touch $com_tmp_file	
	return $exit_success
}
#------------------------------------------------------------------------------------------
function update_welcome_msg(){
	flag=0
	while read -r cLine
	do
		if [ $(echo $cLine | grep -c "<IntroductoryMessage>") == 1 ]; then
			# Flag to check whether the welcome message lines have been started / ended in the file 
			flag=1
			if [ $(echo $cLine | grep -c "</IntroductoryMessage>") == 1 ]; then
				flag=0
				modify_welcome_msg
				continue
			fi
		fi
		
		if [ $flag == 1 ]; then
			if [ $(echo $cLine | grep -c "</IntroductoryMessage>") == 1 ]; then
				flag=0
				modify_welcome_msg
				continue
			else
				continue
			fi
		fi

		echo "$cLine" >> $CFG_TMP	
	done < $CFG_FILE

	$CMD_CP $CFG_TMP $CFG_FILE

	if [ -f $CFG_TMP ]; then
		$CMD_RM -f $CFG_TMP
	fi
	
	touch $com_tmp_file	
	return $exit_success
}
#------------------------------------------------------------------------------------------
function update_welcomemessage(){
	modify_welcomemessage
	#echo "$cLine" >> $BANNER_TMP	

	$CMD_CP $BANNER_TMP $BANNER_FILE

	if [ -f $BANNER_TMP ]; then
		$CMD_RM -f $BANNER_TMP
	fi
	
	return $exit_success
}
#----------------------------------------------------------------------------------------
function print_border(){
	BDR_LEN=80
	for (( i=0; i< $BDR_LEN; i++ ))
	do 
		echo -n "*" >> $BANNER_TMP
	done    
}
#----------------------------------------------------------------------------------------
function update_badCount(){
	DENY_LINE=$($CMD_CAT $AUTH_FILE | $CMD_GREP -n "deny=[0-9]*" | $CMD_GREP -v "[0-9]*:#" | $AWK -F : '{print $1}')
	OLD_VAL=$($CMD_CAT $AUTH_FILE | $CMD_GREP -v ^# | $CMD_GREP -o "deny=[0-9]*")
	
	if [ -z $OLD_VAL ]; then
		#unlock_config_file
		abort "update_badcount Failed due to fault in $AUTH_FILE"
	else
		NEWROW="deny=$a_lockoutBadCount"
		$($CMD_SED -i "$DENY_LINE s/$OLD_VAL/$NEWROW/" $AUTH_FILE) 2> /dev/null
		rCode=$?
		if [ $rCode -ne $exit_success ]; then
			abort "update_badCount Failed with eCode[$rCode]"
		fi
	fi
	
	return $exit_success
}
#----------------------------------------------------------------------------------------
function update_Duration(){
	LOCKOUT_LINE=$($CMD_CAT $AUTH_FILE | $CMD_GREP -n "unlock_time=[0-9]*" | $CMD_GREP -v "[0-9]*:#" | $AWK -F : '{print $1}')
	OLD_VAL=$($CMD_CAT $AUTH_FILE | $CMD_GREP -v ^# | $CMD_GREP -o "unlock_time=[0-9]*")
	
	if [ -z $OLD_VAL ]; then
		#unlock_config_file
		abort "update_Duration Failed due to fault in $AUTH_FILE"
	else
		duration_in_secs=$(( $a_lockoutDuration * 60 ))
		NEWROW="unlock_time=$duration_in_secs"
		$($CMD_SED -i "$LOCKOUT_LINE s/$OLD_VAL/$NEWROW/" $AUTH_FILE) 2> /dev/null
		rCode=$?
		if [ $rCode -ne $exit_success ]; then
			abort "update_Duration Failed with eCode[$rCode]"
		fi
	fi

	return $exit_success
}
#----------------------------------------------------------------------------------------
function update_pswd_length(){
	MINLEN_LINE=$($CMD_CAT $PSWD_FILE | $CMD_GREP -n "type="| $CMD_GREP -v ^#|$AWK -F : '{print $1}')
	OLD_VAL=$($AWK NR==$MINLEN_LINE $PSWD_FILE | $CMD_GREP -o "minlen=[0-9]*" )
	
	if [ -z $OLD_VAL ]; then
		#unlock_config_file
		abort "update_pswd_length Failed due to fault in $PSWD_FILE"
	else
		NEWROW="minlen=$a_minlen"
		$CMD_SED -i "$MINLEN_LINE s/$OLD_VAL/$NEWROW/" $PSWD_FILE 2> /dev/null
		rCode=$?
		if [ $rCode -ne $exit_success ]; then
			abort "update_pswd_length Failed with eCode[$rCode]"
		fi
	fi

	return $exit_success
}
#----------------------------------------------------------------------------------------
function update_pswd_history(){
	PWDHISTRY_LINE=$($CMD_CAT $PSWD_FILE|$CMD_GREP -n "remember=[0-9]*"|$CMD_GREP -Ev "#|enforce_for_root"|$AWK -F : '{print $1}')
	OLD_VAL=$($AWK NR==$PWDHISTRY_LINE $PSWD_FILE |$CMD_GREP -o "remember=[0-9]*")

	if [ -z $OLD_VAL ]; then
		#unlock_config_file
		abort "update_pswd_history Failed due to fault in $PSWD_FILE"
	else
		NEWROW="remember=$a_history"
		$CMD_SED -i "$PWDHISTRY_LINE s/$OLD_VAL/$NEWROW/" $PSWD_FILE 2> /dev/null
		rCode=$?
		if [ $rCode -ne $exit_success ]; then
			abort "update_pswd_history Failed with eCode[$rCode]"
		fi
	fi

	return $exit_success
}
#----------------------------------------------------------------------------------------
function update_maxpswd_age(){
	# Ckeck if max pswd age <= min pswd age
	min_days=$($CMD_CAT $LOGINDEFS_FILE | $CMD_GREP ^PASS_MIN | awk '{print $2}' | $CMD_GREP -v "#")
	[ $a_maxpswdage -le $min_days ] && exit $exit_pswd_age

	OLD_VAL=$($CMD_CAT $LOGINDEFS_FILE | $CMD_GREP ^PASS_MAX)
	if [ -z "$OLD_VAL" ]; then
		#unlock_config_file
		abort "update_maxpswd_age Failed due to fault in $LOGINDEFS_FILE"
	else
		NEWROW="PASS_MAX_DAYS   -1"
		$CMD_SED -i "s/$OLD_VAL/$NEWROW/g" $LOGINDEFS_FILE 2> /dev/null
		rCode=$?
		if [ $rCode -ne $exit_success ]; then
			abort "update_maxpswd_age Failed with eCode[$rCode]"
		fi
	fi

	echo $a_maxpswdage > $maxpswdage_file
	return $exit_success
}
#---------------------------------------------------------------------------------------
function update_maxacnt_age(){
	echo $a_maxacntage > $maxacntage_file
	if [ $startup_flag -eq 1 ]; then
		update_olduser_expiry
	fi
	
	return $exit_success
                NEWROW="PASS_MAX_DAYS   -1"
}
#---------------------------------------------------------------------------------------
function update_maxacnt_age(){
        echo $a_maxacntage > $maxacntage_file
	if [ $startup_flag -eq 1 ]; then
		update_olduser_expiry
	fi
return $exit_success
}

#----------------------------------------------------------------------------------------
function update_minpswd_age(){
	OLD_VAL=$($CMD_CAT $LOGINDEFS_FILE | $CMD_GREP ^PASS_MIN)
	if [ -z "$OLD_VAL" ]; then
		#unlock_config_file
		abort "update_minpswd_age Failed due to fault in $LOGINDEFS_FILE"
	else
		NEWROW="PASS_MIN_DAYS   0"
		$CMD_SED -i "s/$OLD_VAL/$NEWROW/g" $LOGINDEFS_FILE 2> /dev/null
		rCode=$?
		if [ $rCode -ne $exit_success ]; then
			abort "update_minpswd_age Failed with eCode[$rCode]"
		fi
	fi
	
	echo $a_minpswdage > $minpswdage_file
	return $exit_success
}
#----------------------------------------------------------------------------------------
function get_filename(){
	local word=''
	attr=${!1}
	for w in $audit_policies
	do
		if [ ! -z $(echo $attr | $CMD_GREP -i $w) ]; then
			word=$w
		fi
	done	
	echo $word
}
#----------------------------------------------------------------------------------------
function update_audit_policy(){
	local cnt=$(echo $a_audit | $CMD_GREP -o '~' | $CMD_WC -l)
	cnt=$(( $cnt + 1 ))
	local policy=''
	local attr_name=''
	local attr_val=''
	local word=''
	local file_name
	for i in $(seq 1 $cnt)
	do
		policy=$(echo $a_audit | $CMD_CUT -d~ -f $i)
		attr_name=$(echo $policy | $CMD_CUT -d: -f 1)
		attr_val=$(echo $policy | $CMD_CUT -d: -f 2)
		word=$(get_filename attr_name)
		file_name="$word""_log"
		
		if [ $attr_val == 0 ]; then
			$CMD_LN -s -f /dev/null $var_log_path_1/$file_name 
			$CMD_LN -s -f /dev/null $var_log_path_2/$file_name 
			[[ ! $startup_flag ]] && $CMD_TOUCH $var_log_path_1/$file_name.0
		fi
		
		if [ $attr_val == 1 ]; then
			$CMD_LN -s -f $var_log_path_1/messages $var_log_path_1/$file_name 
			$CMD_LN -s -f $var_log_path_2/messages $var_log_path_2/$file_name 
			[[ ! $startup_flag ]] && $CMD_TOUCH $var_log_path_1/$file_name.1
		fi
	done
	touch $audit_restart_flag
}
#----------------------------------------------------------------------------------------
function abort_audit_policy(){
	echo "abort audit"
	for word in $audit_policies
	do
		file_name="$word""_log"
		if [ -f $var_log_path_1/$file_name.1 ]; then
			$CMD_LN -s -f /dev/null $var_log_path_1/$file_name
			$CMD_LN -s -f /dev/null $var_log_path_2/$file_name
			$CMD_RM $var_log_path_1/$file_name.1
		fi
		
		if [ -f $var_log_path_1/$file_name.0 ]; then
			$CMD_LN -s -f $var_log_path_1/messages $var_log_path_1/$file_name
			$CMD_LN -s -f $var_log_path_2/messages $var_log_path_2/$file_name
			$CMD_RM $var_log_path_1/$file_name.0
		fi
	done
}

# _____________________
#|    _ _   _  .  _    |
#|   | ) ) (_| | | )   |
#|_____________________|
# Here begins the "main" function...

sanity_check

ARGS="a:t:w:b:d:u:m:h:x:e:n:rclsf"
while getopts "$ARGS" option
do
	case "$option" in
		s)	# To set flag for service startup
			startup_flag=1
		;;
		
		a)	#audit policies input in the format word:value~word:value~	
			a_audit="$OPTARG"
			update_audit_policy
		;;
		
		t)	#inactivity timer
			a_timeout=$OPTARG
			#lock_config_file
			update_timeout
		;;
		
		w)	#welcome message
			a_wMsg="$OPTARG"
			update_welcomemessage
		;;
		
		b)	
			a_lockoutBadCount="$OPTARG"
			update_badCount
		;;
		
		d)	
			a_lockoutDuration="$OPTARG"
			update_Duration
		;;
		
		m)
			a_minlen="$OPTARG"
			update_pswd_length
		;;
		
		h)
			a_history="$OPTARG"
			update_pswd_history
		;;
		
		x)
			a_maxpswdage="$OPTARG"
			update_maxpswd_age
		;;
		
		e)	
			a_maxacntage="$OPTARG"
			update_maxacnt_age
		;;
		
		n)	
			a_minpswdage="$OPTARG"
			update_minpswd_age
		;;
		
		r)	# Abort changes
			restore_config_file
		;;
		
		c)	# Commit changes
			unlock_config_file
		;;
		
		l)
			lock_config_file
		;;
		f)
			finalize_audit
		;;		
		?)
			unlock_config_file
			abort "Invalid option received ($1)"
		;;
	esac
done

exit $exit_success
