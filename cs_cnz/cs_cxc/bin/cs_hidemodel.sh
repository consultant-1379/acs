#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       cs_hidemodel.sh
# Description:
#	This script is providing the creation of rules for hiding the HardwareMgmt and CrMgmt MOM in specific environment.
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
# - Mon Sep 14 2015 - Sabyasachi Jha (xsabjha)
#       First version.

AXE_FUNCTIONS="axeFunctionsId=1"

SHELF_ARCHITECTURE_ATTR="apgShelfArchitecture"
NODEA_HOSTNAME_ATTR="nodeAName"
NORTH_BOUND="northBoundId=1,networkConfigurationId=1"

IMM_LIST="immlist"
IMM_CFG="immcfg"
grep='/usr/bin/grep'
rm='/usr/bin/rm'

APOS_COMMON="/opt/ap/apos/conf/apos_common.sh"
source $APOS_COMMON

#SHELF_ARCHITECTURE enumerate
SCB=0
SCX=1
DMX=2
VIRTUALIZED=3
LOG_TAG=cs_hidemodel

#attribute enumerate
RULE_CLASS=Rule
PERMISSION=permission
USERLABEL=userLabel
RULEDATA=ruleData
RULEID=ruleId
ROLEID=roleId
HWMPATH=ManagedElement,SystemFunctions,AxeFunctions,SystemComponentHandling,EquipmentM,HardwareMgmt,*
CRMPATH=ManagedElement,SystemFunctions,AxeFunctions,SystemComponentHandling,EquipmentM,CrMgmt,*
EQMPATH=ManagedElement,Equipment,*
EQM_SCHEMA_PATH=ManagedElement,SystemFunctions,SysM,Schema=AxeComputeResource,*
FDPATH=ManagedElement,SystemFunctions,AxeFunctions,SystemHandling,FunctionDistributionM,*
SHELFARCHPATH=ManagedElement,SystemFunctions,AxeFunctions.apgShelfArchitecture
OAMACCESSPATH=ManagedElement,SystemFunctions,AxeFunctions.apgOamAccess


ACS_BIN_DIR="/cluster/storage/system/config/acs_csbin/"
RULE_FILE="Rule_hidemodel_list"
EQM_RULE_FILE="Equipment_rule_list"
FD_RULE_FILE="Functionhide_rule_list"


function abort() {
        if [ $# -eq 2 ]; then
                log "Aborted ($2)" 
                if [[ $1 =~ [0-9]+ ]]; then
                        exit $1
                else
                        exit 1
                fi
        else
                exit 1
        fi
}
#----------------------------------------------------------------------------------------
# log to system-log
function log(){
        /bin/logger -t "$LOG_TAG" "$*"
}

#----------------------------------------------------------------------------------------

log "checking rule existence"
stat $ACS_BIN_DIR$RULE_FILE > /dev/null  2>&1
hide_rule_file_exist=$?
stat $ACS_BIN_DIR$EQM_RULE_FILE > /dev/null  2>&1
eqm_rule_file_exist=$?
stat $ACS_BIN_DIR$FD_RULE_FILE > /dev/null  2>&1
fd_rule_file_exist=$?
if [[ $hide_rule_file_exist == 0 && $eqm_rule_file_exist == 0 && $fd_rule_file_exist == 0 ]]; then
	log "Installation after restore. Rule object population skipped."
	exit 0
fi

#Get node A hostname
nodea_name=$($IMM_LIST -a $NODEA_HOSTNAME_ATTR $NORTH_BOUND | awk 'BEGIN { FS = "=" } ; {print $2}')

if [ ! $nodea_name ]; then
	abort 1  "Cannot read node A hostname....."
fi

#Execute only on node A
if [ $HOSTNAME != $nodea_name ]; then
	log "This script can be executed only on node A"
	exit 0
fi

#Get shelf_architecture
shelf_architecture=$($IMM_LIST -a $SHELF_ARCHITECTURE_ATTR $AXE_FUNCTIONS | awk 'BEGIN { FS = "=" } ; {print $2}')

if [ ! $shelf_architecture ]; then
	abort 1  "Cannot read APG Shelf Architecture"
fi

#configure rules for hiding HardwareMgmt and CrMgmt MO in different environment.
if [ $shelf_architecture == $VIRTUALIZED ]; then
	log "Populating Rule for Virtualized environment..."

#	if [ $hide_rule_file_exist == 0 ]; then
#		$rm -rf $ACS_BIN_DIR$RULE_FILE
#	fi

	if [ $fd_rule_file_exist != 0 ]; then
		CMD_HW1="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to HardwareMgmt MOM\" -a $RULEDATA="$HWMPATH" $RULEID=HardwareMgmt_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1"
		kill_after_try 5 5 6 $CMD_HW1 2>/dev/null
		ret0=$?
                if [ $ret0 -eq 0 ]; then
                        echo "OK" 2>/dev/null
                elif [ $ret0 -eq 255 ]; then
                        script0=$(script_builder "$CMD_HW1")
                        result0=$($script0 2>&1)
                        if [[ ${result0} =~ 'error' ]]; then
                                if [[ ${result0} =~ 'SA_AIS_ERR_EXIST' ]]; then
                                        echo "OK..Continue" 2>/dev/null
                                else
                                        abort 1 'creation of Rule for hiding HardwareMgmt MOM'
                                fi
                        fi
                else
                        abort 1 'creation of Rule for hiding HardwareMgmt MOM failed'
                fi
		
		CMD_HW2="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to HardwareMgmt MOM\" -a $RULEDATA="$HWMPATH" $RULEID=HardwareMgmt_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1"
                kill_after_try 5 5 6 $CMD_HW2 2>/dev/null
		ret1=$?
                if [ $ret1 -eq 0 ]; then
                        echo "OK" 2>/dev/null
                elif [ $ret1 -eq 255 ]; then
                        script1=$(script_builder "$CMD_HW2")
                        result1=$($script1 2>&1)
                        if [[ ${result1} =~ 'error' ]]; then
                                if [[ ${result1} =~ 'SA_AIS_ERR_EXIST' ]]; then
                                        echo "OK..Continue" 2>/dev/null
                                else
                                        abort 1 'creation of Rule for hiding HardwareMgmt MOM failed'
                                fi
                        fi
                else
                        abort 1 'creation of Rule for hiding HardwareMgmt MOM failed'
                fi


		CMD_FD1="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to FunctionDistribution MOM\" -a $RULEDATA="$FDPATH" $RULEID=FunctionDistribution_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1"

                kill_after_try 5 5 6 $CMD_FD1 2>/dev/null 
		ret2=$?
		if [ $ret2 -eq 0 ]; then
			echo "OK" 2>/dev/null
		elif [ $ret2 -eq 255 ]; then
			script2=$(script_builder "$CMD_FD1")
			result2=$($script2 2>&1)
			if [[ ${result2} =~ 'error' ]]; then
				if [[ ${result2} =~ 'SA_AIS_ERR_EXIST' ]]; then
					echo "OK..Continue" 2>/dev/null
				else
					abort 1 'creation of Rule for hiding FunctionDistribution MOM failed'
				fi
			fi
		else
			abort 1 'creation of Rule for hiding FunctionDistribution MOM failed'
		fi

                CMD_FD2="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to FunctionDistribution MOM\" -a $RULEDATA="$FDPATH" $RULEID=FunctionDistribution_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1"

                kill_after_try 5 5 6 $CMD_FD2 2>/dev/null 
		ret3=$?
		if [ $ret3 -eq 0 ]; then
			echo "OK" 2>/dev/null                    
                elif [ $ret3 -eq 255 ]; then
			script3=$(script_builder "$CMD_FD2")
                        result3=$($script3 2>&1)
                        if [[ ${result3} =~ 'error' ]]; then
                                if [[ ${result3} =~ 'SA_AIS_ERR_EXIST' ]]; then
                                        echo "OK..Continue" 2>/dev/null
                                else
                                        abort 1 'creation of Rule for hiding FunctionDistribution MOM failed'
                                fi
                        fi
                else
                        abort 1 'creation of Rule for hiding FunctionDistribution MOM failed'
                fi

		CMD_CRM1="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to CrMgmt MOM\" -a $RULEDATA=\"$CRMPATH\" $RULEID=CrMgmt_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1"

                kill_after_try 5 5 6 $CMD_CRM1 2>/dev/null
                ret4=$?
                if [ $ret4 -eq 0 ]; then
                        echo "OK" 2>/dev/null
                elif [ $ret4 -eq 255 ]; then
                        script4=$(script_builder "$CMD_CRM1")
                        result4=$($script4 2>&1)
                        if [[ ${result4} =~ 'error' ]]; then
                                if [[ ${result4} =~ 'SA_AIS_ERR_EXIST' ]]; then
                                        echo "OK..Continue" 2>/dev/null
                                else
                                        abort 1 'creation of Rule for hiding FunctionDistribution MOM failed'
                                fi
                        fi
                else
                        abort 1 'creation of Rule for hiding FunctionDistribution MOM failed'
                fi

		CMD_CRM2="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to CrMgmt MOM\" -a $RULEDATA=\"$CRMPATH\" $RULEID=CrMgmt_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1"

                kill_after_try 5 5 6 $CMD_CRM2 2>/dev/null
                ret5=$?
                if [ $ret5 -eq 0 ]; then
                        echo "OK" 2>/dev/null
                elif [ $ret5 -eq 255 ]; then
                        script5=$(script_builder "$CMD_CRM2")
                        result5=$($script5 2>&1)
                        if [[ ${result5} =~ 'error' ]]; then
                                if [[ ${result5} =~ 'SA_AIS_ERR_EXIST' ]]; then
                                        echo "OK..Continue" 2>/dev/null
                                else
                                        abort 1 'creation of Rule for hiding FunctionDistribution MOM failed'
                                fi
                        fi
                else
                        abort 1 'creation of Rule for hiding FunctionDistribution MOM failed'
                fi

                CMD_SHELFARCH1="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to apgShelfArchitecture Attribute\" -a $RULEDATA="$SHELFARCHPATH" $RULEID=AxeFunc_shelfArch_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1"
                kill_after_try 5 5 6 $CMD_SHELFARCH1 2>/dev/null || abort 1 'creation of Rule for hiding apgShelfArchitecture Attribute failed'

                CMD_SHELFARCH2="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to apgShelfArchitecture Attribute\" -a $RULEDATA="$SHELFARCHPATH" $RULEID=AxeFunc_shelfArch_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1"
                kill_after_try 5 5 6 $CMD_SHELFARCH2 2>/dev/null || abort 1 'creation of Rule for hiding apgShelfArchitecture Attribute failed'
                CMD_OAMACCESS1="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to apgOamAccess Attribute\" -a $RULEDATA="$OAMACCESSPATH" $RULEID=AxeFunc_oamAccess_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1"
                kill_after_try 5 5 6 $CMD_OAMACCESS1 2>/dev/null || abort 1 'creation of Rule for hiding apgOamAccess Attribute failed'

                CMD_OAMACCESS2="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to apgOamAccess Attribute\" -a $RULEDATA="$OAMACCESSPATH" $RULEID=AxeFunc_oamAccess_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1"
                kill_after_try 5 5 6 $CMD_OAMACCESS2 2>/dev/null || abort 1 'creation of Rule for hiding apgOamAccess Attribute failed'

        fi

	if [ $eqm_rule_file_exist != 0 ]; then
                CMD_EQM1="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=7 -a $RULEDATA=\"$EQMPATH\" $RULEID=AxeComputeResource_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1"
                kill_after_try 5 5 6 $CMD_EQM1 2>/dev/null || abort 1 'creation of Rule to access Equipment MOM failed'

                CMD_EQM2="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=4 -a $RULEDATA=\"$EQMPATH\" $RULEID=AxeComputeResource_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1"
                kill_after_try 5 5 6 $CMD_EQM2 2>/dev/null || abort 1 'creation of Rule to access Equipment MOM failed'
        fi

	log "creation of Rule for Virtualized environment completed"
else
	#Native environment
	if [ $eqm_rule_file_exist != 0 ]; then
		CMD_EQM3="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $RULEDATA="$EQM_SCHEMA_PATH" $RULEID=AxeComputeResource_3,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1"
		kill_after_try 5 5 6 $CMD_EQM3 2>/dev/null || abort 1 'creation of Rule for hiding AxeComputeResource Schema MO failed'

		CMD_EQM4="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0  -a $RULEDATA="$EQM_SCHEMA_PATH" $RULEID=AxeComputeResource_4,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1"
		kill_after_try 5 5 6 $CMD_EQM4 2>/dev/null || abort 1 'creation of Rule for hiding AxeComputeResource Schema MO failed'
	fi

	if [ $hide_rule_file_exist != 0 ]; then
		log "Populating Rule for hiding CrMgmt MOM in all environment..."

		CMD_CRM1="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to CrMgmt MOM\" -a $RULEDATA=\"$CRMPATH\" $RULEID=CrMgmt_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1"
		kill_after_try 5 5 6 $CMD_CRM1 2>/dev/null || abort 1 'creation of Rule for hiding CrMgmt MOM failed'

		CMD_CRM2="$IMM_CFG -c $RULE_CLASS -a $PERMISSION=0 -a $USERLABEL=\"No access to CrMgmt MOM\" -a $RULEDATA=\"$CRMPATH\" $RULEID=CrMgmt_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1"
		kill_after_try 5 5 6 $CMD_CRM2 2>/dev/null || abort 1 'creation of Rule for hiding CrMgmt MOM failed'

		log "creation of Rule for hiding CrMgmt MOM Completed"
	fi
fi

#creating the directory
if [ ! -d "$ACS_BIN_DIR" ]; then
	mkdir $ACS_BIN_DIR > /dev/null 2>&1
fi
#storing the rules being created in to a file on basis of environment
if [ $shelf_architecture == $VIRTUALIZED ]; then
	$IMM_LIST $RULEID=HardwareMgmt_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1 > $ACS_BIN_DIR$FD_RULE_FILE
	$IMM_LIST $RULEID=HardwareMgmt_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$FD_RULE_FILE

	$IMM_LIST $RULEID=AxeComputeResource_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1 > $ACS_BIN_DIR$EQM_RULE_FILE
	$IMM_LIST $RULEID=AxeComputeResource_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$EQM_RULE_FILE

	$IMM_LIST $RULEID=FunctionDistribution_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$FD_RULE_FILE
        $IMM_LIST $RULEID=FunctionDistribution_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$FD_RULE_FILE

	$IMM_LIST $RULEID=AxeFunc_shelfArch_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$FD_RULE_FILE
        $IMM_LIST $RULEID=AxeFunc_shelfArch_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$FD_RULE_FILE

        $IMM_LIST $RULEID=AxeFunc_oamAccess_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$FD_RULE_FILE
        $IMM_LIST $RULEID=AxeFunc_oamAccess_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$FD_RULE_FILE

	$IMM_LIST $RULEID=CrMgmt_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$FD_RULE_FILE
	$IMM_LIST $RULEID=CrMgmt_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$FD_RULE_FILE

else
	$IMM_LIST $RULEID=CrMgmt_1,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1 > $ACS_BIN_DIR$RULE_FILE
	$IMM_LIST $RULEID=CrMgmt_2,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$RULE_FILE

	$IMM_LIST $RULEID=AxeComputeResource_3,$ROLEID=SystemAdministrator,localAuthorizationMethodId=1 > $ACS_BIN_DIR$EQM_RULE_FILE
	$IMM_LIST $RULEID=AxeComputeResource_4,$ROLEID=SystemReadOnly,localAuthorizationMethodId=1 >> $ACS_BIN_DIR$EQM_RULE_FILE
fi

exit 0
