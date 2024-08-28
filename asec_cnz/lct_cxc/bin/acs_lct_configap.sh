#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       configap
# Description:
#       A script to manage users and files permissions on node
#
##
# Usage:
#        configap -i : this option is used to create users and groups on APG.
#                  It takes information from the configuration file "acs_lct_apgroup_data.conf"
#
#        configap -a : this option is used to assign the acls to the files
#                  It takes information from the configuration file "acs_lct_acldata.conf"
#
#        configap -d : this option is used
#               - to create directories and assign the acls to these directories.
#                 For this, it takes information from IMM
#
#               - to create directories for FileM functionality
#                 For this, it takes information from IMM
#
##
# Output:
#       Command print-out is not printed to console.
#
##
# Changelog:
# - Wed Mar 29 2023 - Naveen Kumar G (ZGXXNAV)
#   TR IA25179 Fix
# - Fri Feb 11 2021 -Dharma Teja (XDHATEJ)
#   - added fix for TR HZ60448 DNDR failing in GEP5 and GEP7 when raid structure is wrong.
# - Tue Sep 14 2021 Pravalika(zprapxx)
#   TR HZ39945 Fix 
# - Thu Aug 05 2021 Anjali M (xanjali)
#   vBSC: ssh-key management for RP-VM
# - Mon Feb 01 2021 Swapnika Baradi (xswapba) 
#   TR Fix HY81491 - Unable to access "/var/home/tsadmin/" path using tsadmin user
# - Tue Dec 24 2019 Dharma Teja (xdhatej)
#   TR Fix HX80852 -  DNDR failing after both APUB GEP5 boards changed 3.5.0
# - Fri Oct 12 2018 Parameswari Kotha (xparkot)
#   Overwriting 2770 permissions for SoftwareManagement Folder as a workaround for TR HX26631
# - Thu Sep 27 2018 Mahitha Mandalapu (xmahima)
#   Removed 2775 permissions for SoftwareManagement folder(TR HX16884)
# - Thu Apr 26 2018 Swetha Rambathini (xsweram)
#   added sw_package/APG/UPGRADE
# - Wed Feb 21 2018 - Yeswanth Vankayala (xyesvan)
#    TR Fix HW61578
# - Tue Jan 2 2018 - Yeswanth Vankayala (xyesvan)
#    Create swm softwaremanagement Symlink as part of CMW 4.5 PRA 
# - Thu Nov 23 2017 - Swetha Rambathini (xsweram)
#    Changed symlink of sw_package/APG
# - Thu Feb 02 2017 - Pratap Reddy(xpraupp)
#    Changing home directory for ts user and tsadmin users
# - Thu Sep 22 2016 - Neeraj Kasula(xneekas)
#    Impacts for ts_user spillover for vAPG
# - Wed Jan 27 2016 - Alessio Cascone (EALOCAE)
#    Implemented changes for SLES12 adaptation
# - Wed Apr 16 2015 - Suryanarayana Pammi (XPAMSUR)
#    Removed system-adm group functionality
# - Fri Jan 30 2015 - Roni Newatia (XRONNEW)
#	Modified to create the directory in cluster for Integrityap baseline file.
# - Mon Sep 1 2014 - Fabrizio Paglia (XFABPAG)
#       Modified to skip creation of /sw_package/zip path on AP2.
# - Mon May 19 2013 -Greeshmalatha C (XGRECHA)
#       Modified to validate arguments TR HR36404.
# - Tue Nov 23 2011- Sonali Nanda (xsonan)
#       First version.
##

#commnd list to use
###############################
TRUE=$( true; echo $? )
FALSE=$( false; echo $? )
ECHO=/bin/echo
CUT=/usr/bin/cut
USER_MGMT_SCRIPT=/opt/ap/apos/bin/usermgmt/usermgmt
USERADD="${USER_MGMT_SCRIPT} user add --global"
USERMOD="${USER_MGMT_SCRIPT} user modify"
GROUPADD="${USER_MGMT_SCRIPT} group add --global"
GROUPMOD="${USER_MGMT_SCRIPT} group modify"
GROUPLIST="${USER_MGMT_SCRIPT} group list"
PARMTOOL=/opt/ap/apos/bin/parmtool/parmtool
CHOWN=/bin/chown
CHMOD=/bin/chmod
CHGRP=/bin/chgrp
CMD_GETOPT=/usr/bin/getopt
PASSWD=/usr/bin/passwd
CHPASSWD=/usr/sbin/chpasswd
GREP=/usr/bin/grep
AWK=/usr/bin/awk
MKDIR=/bin/mkdir
SETFACL=/usr/bin/setfacl
CHAGE=/usr/bin/chage
TEE=/usr/bin/tee
IMMLIST=/usr/bin/immlist
IMMFIND=/usr/bin/immfind
PRCSTATE=/usr/bin/prcstate
MOUNT=/bin/mount
HEAD=/usr/bin/head
RAIDMGR=/opt/ap/apos/bin/raidmgr
RAIDMGMT=/opt/ap/apos/bin/raidmgmt
TOUCH=/usr/bin/touch
LOTC_API=/usr/share/pso/storage-paths
CMD_CAT=/bin/cat
CMD_LN=/bin/ln
CMD_CP=/bin/cp
# Exit codes
############
exit_success=0
exit_failure=1
exit_usage=2

#setting  global variables
##############################
LOGDIR=/var/log/acs/tra/logging
LOG_FILE=$LOGDIR/configap.log
GROUP_DATA_FILE=acs_lct_apgroup_data.conf 
GROUP_DATA_FILE_PATH=/opt/ap/acs/conf/acs_lct_apgroup_data.conf 
ACL_DATA_FILE=acs_lct_acldata.conf   
ACL_DATA_FILE_PATH=/opt/ap/acs/conf/acs_lct_acldata.conf   
CLUSTER_LCT_DIR="/cluster/storage/system/config/acs_lct"
INPUTFILE="NOINPUTFILE"
print_str="file"
GROUP_CONF=/cluster/etc/group
LOCAL_GROUP_CONF=/etc/group
SSH_LOGIN_FILE=/cluster/etc/login.allow
retval=1
scrsearch="sts_scr"
cpsearch="cp"
licensesearch="license_file"
healthchecksearch="health_check"
cpprintoutssearch="/cp/printouts"
zipsearch="/sw_package/zip"
attr_dirpath="dirPath"
attr_fileowner="fileOwner"
attr_groupowner="fileGroupOwner"
attr_basicPerm="fileBasicPermissions"
attr_splPerm="fileSpecialPermissions"
FileMdn="AxeNbiFoldersnbiFoldersMId=1"
internalroot_attrib="internalRoot"
externalRoot_attrib="externalRoot"
swPackageAPG_attrib="swPackageAPG"
clu_swpkg_location="/storage/no-backup/coremw/SoftwareManagement"
clu_swpkg_apg="/cluster/sw_package/APG"
clu_swpkg_apg_upgrade="/cluster/sw_package/APG/UPGRADE"
clu_storage_location="/cluster/apg/ts_storage"
ts_storage="/ts_storage"
ts_storage_attrib="supportData"
parent_home_dir='/var/home'
tsuser_home_dir="$parent_home_dir/ts_users/"
tsadmin_home_dir="$parent_home_dir/tsadmin/"
a_tsadmin="tsadmin"
group_name="tsgroup"
config_d_flag=0
ap_type=''
LOCTIM_FILE="/etc/localtime"
#cp_files="/cp/files"
source_cpfile="sourceDataForCpFile"
source_cpprintouts="sourceDataForCpPrintouts"
source_healthcheck="sourceDataForHealthCheckFiles"
NBI_USERS_GROUP_NAME="system-nbi-data"
#sourcedata_sts="sourceDataForStsFile"
#hardlimit_user="apgusersvc"
# Message functions
####################
function show_usage() {
  ${ECHO} -e "Usage:  "
  ${ECHO} -e "configap -i"
  ${ECHO} -e "configap -a"
  ${ECHO} -e "configap -d"
  ${ECHO} -e "configap -h"
  ${ECHO} ""
  ${ECHO} -e "where options include:"
  ${ECHO} -e "-i\tcreate groups"
  ${ECHO} -e "-a\tassign acls to files"
  ${ECHO} -e "-d\tcreate directories and assign permissions"
  ${ECHO} -e "-h\thelp information"
  ${ECHO} ""
}

function usage_error() {
  ${ECHO} "$1"
  show_usage
  exit $2
}

function error() {
  ${ECHO} "$@"
  slog "$@"
  ${ECHO} ""
}

function log() {
	${ECHO} "[$(date +'%Y-%m-%d %H:%M:%S')] $@" >>$LOG_FILE
}

function slog() {
  /bin/logger -t configap "$@"
  log "$@"
}

# Usage:
#	create_folder <folder>
#	create_folder <folder> <permissions>
function create_folder() {
	if [ $# -lt 1 ] || [ $# -gt 2 ]; then
		error "Bad usage: $0 <folder> [<permissions>]"
		return $exit_usage
	fi
	
	local folder_to_create=$1
	local folder_permissions=0755
	
	if [ $# -eq 2 ]; then
		folder_permissions=$2
	fi

	if [ ! -d $folder_to_create ]; then
		${MKDIR} -p $folder_to_create
		if [ $? -ne 0 ]; then
			error "ERROR: Failed to create folder '$folder_to_create'!"
			return $exit_failure
		fi
	fi
	
	${CHMOD} $folder_permissions $folder_to_create
	if [ $? -ne 0 ]; then
		error "ERROR: Failed to set the permissions '$folder_permissions' on folder '$folder_to_create'"
		return $exit_failure
	fi
						
	return $exit_success	
}

function is_vapg() {
  local SHELF_ARCH=$(${PARMTOOL} get --item-list shelf_architecture | awk -F '=' {'print $2'})
  local HW_TYPE=$(${PARMTOOL} get --item-list installation_hw | awk -F '=' {'print $2'})
  [[ "$SHELF_ARCH" == 'VIRTUALIZED' && "$HW_TYPE" == 'VM' ]] && return $exit_success
  return $exit_failure
}

# Other Script functions
##############################

# To check other users status
##############################
function check_user_status() {
  user=$1
  ${GREP} "^$user:" /cluster/etc/passwd  1>/dev/null
  if [ $? != 0 ]
  then
    log "INFO: User [$user] does not exist" 
    return $exit_failure
  fi
  return $exit_success  
}

# To create local home directory for tsusers and tsadmin
# also Changing the group ownership of home direcoty to 'tsusers' 
# also granting group write permissions to /var/home/tsusers directory
# home directory path and group names are inputs to this function
function create_home_dir() {
  local home_dir=$1
	local group_name=$2

	# Creation of /var/home directory 
	if [ ! -d $parent_home_dir ]; then 
		create_folder $parent_home_dir 755
		if [ $? -ne 0 ]; then 
			error "ERROR: Failed to create $parent_home_dir directory"
			return $exit_failure
		fi
	fi

	# Create local home directory for all ts users and tsadmin user 
	create_folder $home_dir 755
	if [ $? -eq 0 ];then
		log "INFO: Created common home directory [$home_dir] for tsusres"
		# change the group owner to tsgroup
		${CHGRP} $group_name $home_dir
		if [ $? -ne 0 ];then
			error "ERROR: Failed to change group ownership [$group_name] of [$home_dir]"
			return $exit_failure
		else
			log "INFO: Successfully changed group ownership [$group_name] of [$home_dir]"
		fi   
		configure_home_dir $home_dir $group_name
	fi

  return $exit_success
}

# To configure the common home directory for ts_users
function configure_home_dir() {
  local home_dir=$1
	local group_name=$2
  # copy the file & folders from /etc/skel/ to [$common_local_dir] 
  ${CMD_CP} -Ruf /etc/skel/. $home_dir
  if [ $? -ne 0 ];then
    log "INFO: Failed to copy contents from /etc/skel to [$home_dir]"
  else 
    # permit execute-access to common-home
		if [ $group_name == tsgroup ]; then 
    	${CHMOD} -R 770 $home_dir/
    	if [ $? -ne 0 ];then
      	log "INFO: Failed to permit 770 access to files under the [$home_dir]"
    	fi
        ${CHMOD} 755 /var/home/tsadmin
		fi
    
    # change the group owner to tsgroup and owner as tsadmin       
    ${CHOWN} -R $a_tsadmin:$group_name $home_dir/
    if [ $? -ne 0 ];then
      log "INFO: Failed to change the group owner and owner for files under [$home_dir] "  
    fi   
    
    # change the group owner to tsgroup and owner as root for /ts_user folder
		if [ $group_name == tsgroup ]; then
			${CHOWN} root:$group_name $home_dir
		fi
  fi 
  
	if [ $group_name == tsgroup ]; then
  	if [ ! -d $home_dir/.ssh ];then
    	create_folder $home_dir/.ssh
    	if [ $? -ne 0 ];then
      	error "ERROR: Failed to create directory .ssh under the [$home_dir]"
      	return $exit_failure  
    	else
      	assign_acls $home_dir/.ssh "$a_tsadmin" "$group_name" "770" ""
      	[ ! -f $home_dir/.ssh/known_hosts ] && ${TOUCH} $home_dir/.ssh/known_hosts
      	assign_acls $home_dir/.ssh/known_hosts "$a_tsadmin" "$group_name" "660" ""
    	fi
  	else
    	[ ! -f $home_dir/.ssh/known_hosts ] && ${TOUCH} $home_dir/.ssh/known_hosts 
    	assign_acls $home_dir/.ssh/known_hosts "$a_tsadmin" "$group_name" "660" ""
  	fi
	fi


       # vBSC: SSH key management for RP-VM
       # Default ssh-keys are present in APG which are used for ssh connection 
       # with RP-VM. Assign 640 permission to id_rsa file in ts_users home path
       # id_rsa file in ts_users home path presents only in case of vBSC and permissions
       # are applied only if file exists.
        if [ -f $home_dir/.ssh/id_rsa ];then
           storage_home_dir="/storage/system/config/apos/ssh_keys"
           assign_acls $storage_home_dir/id_rsa "$a_tsadmin" "$group_name" "640" ""
           if [ $? -ne 0 ];then
              error "ERROR: Failed to apply 640 permissions to id_rsa file"
           else
              log "INFO: Applied 640 permissions to id_rsa file"
           fi
        else
             log "INFO: ssh key file does not exists in /var/home/ts_user/.ssh folder"
        fi

  return $exit_success
}

#function used to assign the acls to files/directories
##############################
function assign_acls(){
  src_file_name=$1
  src_owner=$2
  src_group=$3
  src_base_perm=$4
  src_spl_perm=$5
      
  #checking if the owner exists
  if [ ! -z $src_owner ];then
    if [ "$src_owner" != "root" ];then
      ${GREP} "^$src_owner:" /cluster/etc/passwd  1>/dev/null
      if [ $? -ne 0 ]; then
        if [ $config_d_flag -eq 0 ];then
          error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]" 
        fi
        error "ERROR: User [$src_owner],owner of $print_str [$src_file_name] does not exist"
        continue
      fi
    fi
                        
    ${CHOWN} $src_owner $src_file_name
    if [ $? -eq 0 ];then
      log "INFO: [$src_file_name]: Set owner to [$src_owner]"
    else
      if [ $config_d_flag -eq 0 ];then
        error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
      fi
      error "ERROR: Failed to set owner for $print_str [$src_file_name]"
      continue
    fi

  fi

  #checking if the group exists
  if [ ! -z $src_group ];then
    if [ "$src_group" != "root" ];then
      ${GREP} "^$src_group:" /cluster/etc/group 1>/dev/null
      if [ $? -ne 0 ]; then
        if [ $config_d_flag -eq 0 ];then
          error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
        fi
        error "ERROR: Group [$src_group],group owner of $print_str [$src_file_name] does not exist"
        continue
      fi
    fi
    
    ${CHGRP} $src_group $src_file_name
    if [ $? -eq 0 ];then
      log "INFO: [$src_file_name]: Set group owner to [$src_group]"
    else
      if [ $config_d_flag -eq 0 ];then
        error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
      fi
      error "ERROR: Failed to set group owner for $print_str [$src_file_name]"
      continue
    fi
  fi

  #setting the owner and group of the file as per the input file data
  #setting basic permissions of the file
  #${SETFACL} -n -m u::"$src_owner_perm",g::"$src_group_perm",o::"$src_other_perm",m::rwx $src_file_name
  if [ ! -z $src_base_perm ];then
    ${CHMOD} $src_base_perm $src_file_name
    if [ $? -eq 0 ];then
      log "INFO: [$src_file_name]: Basic permission set to [$src_base_perm]"
    else
      if [ $config_d_flag -eq 0 ];then
        error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
      fi
      error "ERROR: Failed to set basic linux permissions for $print_str [$src_file_name]"
      continue
    fi
  fi

  #setting stickybit, suid and sgid permissions if any
  ${CHMOD} -t,u-s,g-s $src_file_name #clearing previous special permission if any
  if [ $? != 0 ]; then
    if [ $config_d_flag -eq 0 ];then
        error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
    fi
    error "ERROR: Failed in processing of sticky bit,suid and sgid permissions for $print_str [$src_file_name]"
    continue
  fi
  
  if [ ! -z $src_spl_perm ]; then
    case $src_spl_perm in
      0) 
        log "INFO: [$src_file_name]: Sticky bit,setuid and sgid permissions are cleared"
      ;;

      1)
        ${CHMOD} +t $src_file_name
        if [ $? -eq 0 ];then
          log "INFO: [$src_file_name]: Sticky bit permission set"
        else
          if [ $config_d_flag -eq 0 ];then
            error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
          fi
          error "ERROR: Failed to set sticky bit for $print_str [$src_file_name]"
          continue
        fi
       ;;

      2)
        ${CHMOD} g+s $src_file_name
        if [ $? -eq 0 ];then 
          log "INFO: [$src_file_name]: Setgid bit permission set"
        else
          if [ $config_d_flag -eq 0 ];then
            error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
          fi
          error "ERROR: Failed to set setgid bit for $print_str [$src_file_name]"
          continue
        fi
      ;;

      3)
        ${CHMOD} +t,g+s $src_file_name
        if [ $? -eq 0 ];then 
          log "INFO: [$src_file_name]: Sticky bit and setgid permissions set"
        else
          if [ $config_d_flag -eq 0 ];then
            error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
          fi
          error "ERROR: Failed to set sticky bit and setgid bit for $print_str [$src_file_name]"
          continue
        fi
      ;;

      4)
        ${CHMOD} u+s $src_file_name
        if [ $? -eq 0 ];then
          log "INFO: [$src_file_name]: Setuid permission set"
        else
          if [ $config_d_flag -eq 0 ];then
            error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
          fi
          error "ERROR: Failed to set setuid bit for $print_str [$src_file_name]"
          continue
        fi
      ;;

      5)
        ${CHMOD} u+s,+t $src_file_name
        if [ $? -eq 0 ];then
          log "INFO:  [$src_file_name]: Setuid and sticky bit permissions set"
        else
          if [ $config_d_flag -eq 0 ];then
            error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
          fi
          error ${ECHO} "ERROR: Failed to set setuid and sticky bit for $print_str [$src_file_name]"
          continue
        fi
      ;;

      6)      
        ${CHMOD} u+s,g+s $src_file_name
        if [ $? -eq 0 ];then
          log "INFO:  [$src_file_name]: Setuid and setgid permissions set" 
        else
          if [ $config_d_flag -eq 0 ];then
            error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
          fi
          error "ERROR: Failed to set setuid and setgid bit for $print_str [$src_file_name]"
          continue
        fi
      ;;

      7)      
        ${CHMOD} +t,u+s,g+s $src_file_name
        if [ $? -eq 0 ];then
          log "INFO:  [$src_file_name]: Stickybit,setuid,setgid permissions set"
        else
          if [ $config_d_flag -eq 0 ];then
            error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
          fi
          error "ERROR: Failed to set sticky bit,setuid and setgid bit for $print_str [$src_file_name]"
          continue
        fi
      ;;

      *)      
        if [ $config_d_flag -eq 0 ];then
          error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
        fi
        error "ERROR: Invalid special permission bit for $print_str [$src_file_name]"
        continue
      ;;
      esac
  else
    log "INFO: No sticky bit, setuid or setgid found for $print_str [$src_file_name] in input file [$INPUTFILE]"
  fi
}

function user_mgmt() {

  # calling the usermanagement function
    if ! is_vapg && [ "$src_u_name" == 'ts_user' ]; then
        log "INFO: ts_user creation during installation only allowed in virtualized"
    else
  	user_add_mod "$src_g_name" "$src_u_name" "$src_u_id" "$src_nologin" 
    fi
  num_grps=$(${ECHO} $src_sec_grp | ${AWK} -F, '{print NF}')
  if [ $num_grps -ge 1 ]; then
    for ((  i = 1 ;  i <= $num_grps ;  i++  ))
    do
      grp=$(${ECHO} $src_sec_grp | ${AWK} -F, '{print $'$i'}')
      if [ ! -z $grp ]; then
        # checking if the secondary group already exists ,if doesnot exist, it returns 1
        ${GREP} "^$grp:"  /cluster/etc/group 1>/dev/null 
        if [ $? -eq 1 ]; then
          # The retrieved secondary group does not exist: create it with a random GID
          ${GROUPADD} --gname=$grp
          if [ $? -eq 0 ]; then
            log "INFO: Added group [$grp] with random gid"
          else
            error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
            error "ERROR: Failed to add group [$grp] with random gid"
            continue
          fi
        fi
      fi
    done
    if ! is_vapg && [ "$src_u_name" == 'ts_user' ]; then
        log "INFO: ts_user creation during installation only allowed in virtualized"
    else
     # Set the list of secondary groups for the user
     ${USERMOD} --secgroups=$src_sec_grp --uname=$src_u_name
      if [ $? -eq 0 ];then
        log "INFO: Added user [$src_u_name] as member of secondary group [$src_sec_grp]"
      else
        error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
        error "ERROR: Failed to add [$src_u_name] as member of secondary group [$src_sec_grp]"
      fi
    fi
  fi
}
      
#function to add or modify user details
##############################
function user_add_mod() {
  src_g_name=$1
  src_u_name=$2
  src_u_id=$3
  src_nologin=$4
  nologin_status=0
  shell_opt=""

  if [ "${src_nologin}" == "n" ]; then
    shell_opt="--shell=/sbin/nologin"
    nologin_status=1
  else
	  shell_opt="--shell=/bin/bash"
  fi

  local homedir_opt="--createhome"
  if [ "$src_u_name" == 'ts_user' ]; then
    homedir_opt="--homedir=$tsuser_home_dir"
    # create common-home for ts users if not already exist
    if [ ! -d $tsuser_home_dir ]; then
      create_home_dir $tsuser_home_dir $group_name
		else
			configure_home_dir $tsuser_home_dir $group_name
    fi
  fi

	if [ "$src_u_name" == "$a_tsadmin" ]; then
		homedir_opt="--homedir=$tsadmin_home_dir"
		if [ ! -d $tsadmin_home_dir ]; then
			create_home_dir $tsadmin_home_dir $group_name
		else
			configure_home_dir $tsadmin_home_dir $group_name
		fi
	fi

  if [ ! -z $src_u_name ]; then
    check_user_status $src_u_name
    retval=$?

    [[ $retval -eq 0 && "$src_u_name" == "$a_tsadmin" ]] && {
      # check for tsadmin groups
      # if tsadmin has tsadmin and tsgroup assigned, then remove tsgroup
      # usermod -P /cluster/etc/ -R tsgroup tsadmin
      ${GROUPLIST} --user=$src_u_name | grep $group_name
      if [ $? -eq 0 ]; then
    	${GROUPMOD} --rmgroup=$group_name --uname=$src_u_name
      fi
    }

    if [ $retval -eq 1 ]; then
      # enter this block if user doesnot exist in APG
      if [ ! -z $src_u_id ]; then #checking if uid field is not empty in input file
        # The user does not already exist in APG: create it with the given UID
        ${USERADD} $shell_opt --gname=$src_g_name --uid=$src_u_id $homedir_opt --uname=$src_u_name
        if [ $? -eq 0 ]; then
            log "INFO: Added user [$src_u_name] with uid [$src_u_id] as member of [$src_g_name]" 
          if [ $nologin_status -eq 1 ]; then
            log "INFO: Nologin set for user [$src_u_name]" 
          fi
        else 
          error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
          error "ERROR: Failed to add user [$src_u_name] with uid [$src_u_id] as member of [$src_g_name]"
          continue
        fi
      else
        # The user does not already exist in APG: create it with a random UID
        ${USERADD} $shell_opt --gname=$src_g_name $homedir_opt --uname=$src_u_name
        if [ $? -eq 0 ]; then
          log  "INFO: Added user [$src_u_name] with random uid as member of [$src_g_name]"
          if [ $nologin_status -eq 1 ]; then
            log  "INFO: Nologin set for user [$src_u_name]" 
          fi
        else
          error  "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
          error  "ERROR:  Failed to add user [$src_u_name] with random uid as member of [$src_g_name]"
          continue
        fi
      fi
      
      #useradd takes a while to reflect user information on busy node.
      sleep 2

      if [ "$src_u_name" == "$a_tsadmin" ]; then
        ${ECHO} "$src_u_name:tsadmin1@" | ${CHPASSWD} 2>/dev/null
        rCode=$?
        if [ $rCode -eq 0 ]; then 
          log "INFO: Password set for [$src_u_name]"

          # Set force password change for tsadmin  
          ${PASSWD} -e $src_u_name 1>/dev/null
          if [ $? -ne 0 ]; then 
            error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
            error "ERROR: Failed to set forced password change on account [$src_u_name]"
            continue
          else
            log "INFO: $src_u_name account set to forced password change" 
          fi 

          # Set tsadmin user as never to expire account
          ${CHAGE} $src_u_name --mindays -1 --maxdays -1 --expiredate -1 --inactive -1 --warndays -1 1>/dev/null
          if [ $? -ne 0 ]; then
            error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
            error "ERROR: Failed to set account [$src_u_name] never to expire"
            continue
          else
            log "INFO: $src_u_name account set never to expire" 
          fi

          #adding the user to ssh configuration file
          str_append="$src_u_name all"
          ${ECHO} "$str_append" >>${SSH_LOGIN_FILE}

        else
          error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
          error "ERROR: Failed to set password for [$src_u_name]"
          continue  
        fi
      fi
    else
      # enter this block if user exists in APG
      log "INFO: User [$src_u_name] already exists" 

      config_nologin=$(${GREP} "^$src_u_name:" /cluster/etc/passwd | ${AWK} -F: '{print $7}')
      if [ "$config_nologin" == "/sbin/nologin" ]; then
        if [ "$src_nologin" != "n" ]; then
          ${USERMOD} $shell_opt --uname=$src_u_name
          if [ $? -eq 0 ]; then
            log "INFO: User [$src_u_name] set to login shell"
          else
            error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
            error "ERROR: Failed to modify user [$src_u_name] to login shell"
            continue
          fi  
        fi
      else
        if [ "$src_nologin" == "n" ]; then
          ${USERMOD} $shell_opt --uname=$src_u_name
          if [ $? -eq 0 ]; then
            log  "INFO: User [$src_u_name] set to no login shell"
          else
            error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
            error "ERROR: Failed to modify user [$src_u_name] to no login shell"
            continue
          fi  
        fi
      fi
        
      #checking if uid field is not empty in input file
      if [ ! -z $src_u_id ]; then 
        config_uid=$(${GREP} "^$src_u_name:" /cluster/etc/passwd | ${AWK} -F: '{print $3}')
        #comparing if uid in input file not matching existing APG uid
        if [ "$config_uid" != "$src_u_id" ]; then 
          # The UID value present into configuration file is not aligned with the existing one: modify it
          ${USERMOD} $shell_opt --uid=$src_u_id --uname=$src_u_name
          if [ $? -eq 0 ];then
            log "INFO: Modified user [$src_u_name] with userid [$src_u_id]" 
          else 
            error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
            error "ERROR: Failed to modify user [$src_u_name] with uid [$src_u_id]"
            continue
          fi
        fi
      fi

      #now we are checking if this existing user is already a member of its corresponding group
      config_user_gid=$(${GREP} "^$src_u_name:" /cluster/etc/passwd | ${AWK} -F: '{print $4}') #retrieving primary gid of this use
      config_group_gid=$(${GREP} "^$src_g_name:"  /cluster/etc/group | ${AWK} -F: '{print $3}') #retrieving gid of the group

      #checking if they donot match
      if [ "$config_user_gid" != "$config_group_gid" ]; then 
    	# The primary group for the current user does not match with the config file: modify it
        ${USERMOD} $shell_opt --gname=$src_g_name --uname=$src_u_name
        if [ $? -eq 0 ];then
          log  "INFO: Modified user [$src_u_name] as member of group [$src_g_name]"
        else 
          error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
          error "ERROR: Failed to modify user [$src_u_name] as member of group [$src_g_name]"
          continue
        fi
      fi
    fi
  fi
}

#function called for configap -i
##############################
function config_i() {

  if [ ! -f $GROUP_DATA_FILE_PATH ]; then
    error "ERROR: File [$GROUP_DATA_FILE],input file for [configap -i] doesnot exist "
    return $exit_failure
  fi

  if [ ! -s $GROUP_DATA_FILE_PATH ]; then
    error "ERROR: File [$GROUP_DATA_FILE],input file for [configap -i] does not contain any data"
    return $exit_failure
  fi

  # reading data line by line from file:
  while read line
  do
    lineno=$(expr $lineno + 1)
    #fetching all field values from the input file
    src_g_name=$(${ECHO} $line | ${AWK} -F: '{print $1}')
    src_g_id=$(${ECHO} $line | ${AWK} -F: '{print $2}')
    src_u_name=$(${ECHO} $line | ${AWK} -F: '{print $3}')
    src_u_id=$(${ECHO} $line | ${AWK} -F: '{print $4}')
    src_nologin=$(${ECHO} $line | ${AWK} -F: '{print $5}')
    src_sec_grp=$(${ECHO} $line | ${AWK} -F: '{print $6}')
  
    if [ "$src_g_name" == "root" ]; then
      user_add_mod $src_g_name $src_u_name "$src_u_id" $src_nologin
      continue
    fi
    # checking if the group already exists ,if doesnot exist, it returns 1
    ${GREP} "^$src_g_name:" /cluster/etc/group 1>/dev/null 
    if [ $? -eq 1 ]; then
      # entered this block if group does not exist in APG
      # checking if group id field is not empty in the input file and add group accordingly
      if [ ! -z $src_g_id ]; then
        # The read GID is not empty: creating group with the read value as GID
        ${GROUPADD} --gid=$src_g_id --gname=$src_g_name
        if [ $? -eq 0 ]; then
          log  "INFO: Added group [$src_g_name] with groupid [$src_g_id]"
        else
          error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
          error "ERROR: Failed to add group [$src_g_name] with given gid [$src_g_id]"
          continue
        fi
      else
        # The read GID is empty: creating group with a random GID
        ${GROUPADD} --gname=$src_g_name
        if [ $? -eq 0 ]; then
          log  "INFO: Added group [$src_g_name] with random gid"
        else
          error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
          error "ERROR: Failed to add group [$src_g_name] with random gid"
          continue
        fi
      fi
      # ts_user creation during installation only allowed in virtualized 
      # environment. Not applicable to Native environment
      #if ! is_vapg && [ "$src_u_name" == 'ts_user' ]; then
        #continue
      #fi
      
      # adding user to APG
      user_mgmt
      
    else
      # entered this block if group already exists in APG
      log "INFO: Group [$src_g_name] already exists"
      # checking if gid field of this group is not null in the input file
      if [ ! -z $src_g_id ]; then
        # finding the existing gid of this group in APG
        config_gid=$(${GREP} "^$src_g_name:"  /cluster/etc/group | ${AWK} -F: '{print $3}')     
        # comparing if the existing gid in APG does not match with gid stated in input fil
        if [ "$config_gid" -ne "$src_g_id" ]; then  
          # The group already exists and its GID is different from the one stored into config file: change the current GID 
          ${GROUPMOD} --gid=$src_g_id --gname=$src_g_name
          if [ $? -eq 0 ]; then
            log "INFO: Modified group [$src_g_name] with groupid [$src_g_id]"
          else
            error "ERROR occured while working on line number [$lineno] of input file [$GROUP_DATA_FILE]"
            error "ERROR: Failed to modify group [$src_g_name] with groupid [$src_g_id]"
            continue
          fi
        fi
      fi
      
      # ts_user creation during installation only allowed in virtualized 
      # environment. Not applicable to Native environment
      #if ! is_vapg && [ "$src_u_name" == 'ts_user' ]; then
        #continue
      #fi
      
      # adding user to APG
      user_mgmt

    fi
  done < $GROUP_DATA_FILE_PATH

  return $exit_success
}

#this will be common to configap -a and -d option
##############################
function config_common() {
  config_option=$1

  if [ $config_option == 'a' ];then
    INPUTFILE=$ACL_DATA_FILE
    print_str="file"

    if [ ! -f $ACL_DATA_FILE_PATH ]
    then
      error "ERROR: File [$INPUTFILE],input file for [configap -$config_option] doesnot exist"
      return $exit_failure
    fi

    if [ ! -s $ACL_DATA_FILE_PATH ]
    then
      error "ERROR: File [$INPUTFILE],input file for [configap -$config_option] does not contain any data"
      return $exit_failure
    fi

    #reading data line by line from file:
    while read line
    do
      # Parse Out comments
      if [[ "$line" =~ ^# ]]; then
        lineno=$(expr $lineno + 1)
        continue
      fi

      lineno=$(expr $lineno + 1)

      #fetching all field values from the input file
      src_file_name=$(${ECHO} $line | ${AWK} -F: '{print $1}')
      src_owner=$(${ECHO} $line | ${AWK} -F: '{print $2}')
      src_group=$(${ECHO} $line | ${AWK} -F: '{print $3}')
      src_base_perm=$(${ECHO} $line | ${AWK} -F: '{print $4}')
      src_spl_perm=$(${ECHO} $line | ${AWK} -F: '{print $5}')
    
      last_chr=$(${ECHO} "$src_file_name" | ${AWK} '{print substr( $0, length($0), 1 ) }')
      last_chr_1=$(${ECHO} "$src_file_name" | ${AWK} '{print substr( $0, length($0) - 1, 1 ) }')
      
      if [ "$last_chr" == "*" ] && [ "$last_chr_1" == "/" ]; then
      
        file_path=$(${ECHO} "$src_file_name" | ${AWK} '{print substr( $0, 0, length($0) - 1 ) }')
        
        for reg_file in $(find $file_path -maxdepth 1 -type f)
        do
          if [ ! -f $reg_file ]; then 
            error "ERROR: [$reg_file] does not exist"
            error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
            #return $exit_failure
            #continue
          else
            assign_acls "$reg_file" "$src_owner" "$src_group" "$src_base_perm" "$src_spl_perm"
          fi
        done

      elif [  "$last_chr" == "*" ] && [ "$last_chr_1" != "/" ]; then
  
        ext_f_name=$(basename "$src_file_name")
        file_start_str=$(${ECHO} "$ext_f_name" | ${AWK} '{print substr( $0, 0 , length($0) - 1 )}')
        len1=$(${ECHO} ${#src_file_name})
        len2=$(${ECHO} ${#ext_f_name})
        pos2=$(expr $len1 - $len2)
        base_f_name=$(${ECHO} "$src_file_name" | cut -c 1-$pos2)
        l=$(expr $len2 - 1)
        for reg_file in $(find $base_f_name -maxdepth 1 -type f)
        do
          a=$(basename "$reg_file")
          start_str=$(${ECHO} "$a" | cut -c 1-$l)
          if [ "$start_str" == "$file_start_str" ];then
            if [ ! -f $reg_file ]; then
              error "ERROR: [$reg_file] does not exist"
              error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
            else
              assign_acls "$reg_file" "$src_owner" "$src_group" "$src_base_perm" "$src_spl_perm"
            fi
          else
            continue
          fi
        done      
      else
        if [ ! -e $src_file_name ];then
          if [[ "$src_file_name" == "/var/log/SC-2-1/security_audit" || "$src_file_name" == "/var/log/SC-2-2/security_audit" ]]; then
            ${TOUCH} $src_file_name
            log "INFO: Creating [$src_file_name] file and assigning required permissions"
            assign_acls "$src_file_name" "$src_owner" "$src_group" "$src_base_perm" "$src_spl_perm"
          elif [[ "$src_file_name" == "/var/log/SC-2-1/messages" || "$src_file_name" == "/var/log/SC-2-2/messages" ]]; then
            ${TOUCH} $src_file_name
            log "INFO: Creating [$src_file_name] file and assigning required permissions"
            assign_acls "$src_file_name" "$src_owner" "$src_group" "$src_base_perm" "$src_spl_perm"
          elif [[ "$src_file_name" == "/var/log/SC-2-1/kernel" || "$src_file_name" == "/var/log/SC-2-2/kernel" ]]; then
            ${TOUCH} $src_file_name
            log "INFO: Creating [$src_file_name] file and assigning required permissions"
            assign_acls "$src_file_name" "$src_owner" "$src_group" "$src_base_perm" "$src_spl_perm"
          elif [[ "$src_file_name" == "/var/log/SC-2-1/auth" || "$src_file_name" == "/var/log/SC-2-2/auth" ]]; then
            ${TOUCH} $src_file_name
            log "INFO: Creating [$src_file_name] file and assigning required permissions"
            assign_acls "$src_file_name" "$src_owner" "$src_group" "$src_base_perm" "$src_spl_perm"
          else
            error "ERROR: [$src_file_name] does not exist"
            error "ERROR occured while working on line number [$lineno] of input file [$INPUTFILE]"
            continue
          fi
        else
          assign_acls "$src_file_name" "$src_owner" "$src_group" "$src_base_perm" "$src_spl_perm"
        fi
      fi
    done < $ACL_DATA_FILE_PATH
    find /var/log/ -type d -exec setfacl -m d:u::rw-,d:g::r--,d:g:tsgroup:r,d:o::--- {} \;
    find /var/log/ -type f ! -group tsgroup ! -name last_reboot_time_stamps ! -name faillog -exec setfacl -m "g:tsgroup:r" {} +
    find /var/log/ -type f -perm /g+wx,o+rwx ! -name last_reboot_time_stamps ! -name faillog -exec chmod g-wx,o-rwx {} +
  fi

  if [ $config_option == 'd' ]; then
    print_str="directory"
    config_d_flag=1

    #stop the services not required here.
    #service cups stop
    #if [ $? == 0 ];then
    #       ${ECHO} "ERROR: Error stoping cups service"
    #fi

    #service smb stop
    #if [ $? == 0 ]; then
    #       ${ECHO} "ERROR: Error stoping cups service"
    #fi

    #AP Node information
    apnodeInfo=$(get_ap_type)
    if [ -z "$apnodeInfo" ]; then
      echo "ERROR: Empty value retrieved for apnodeInfo"
      exit $exit_failure
    elif [[ ! "$apnodeInfo" =~ ^[12]$ ]]; then
      echo "ERROR: Bad value ($apnodeInfo) retrieved for apnodeInfo"
      exit $exit_failure
    fi
      
    log "INFO: [$apnodeInfo] APNODEINFO"

    apdirList=$(${IMMFIND} | ${GREP} lctApdirDataId)
    if [ -z "$apdirList" ]; then
      error "ERROR: Failed to retreive apdirList from IMM"
      exit $exit_failure
    fi

    for obj in $apdirList
    do
      dir_path=$(${IMMLIST} $obj | ${GREP} $attr_dirpath | ${AWK} '{print $3}')
      #echo $dir_path
      dir_owner=$(${IMMLIST} $obj | ${GREP} $attr_fileowner | ${AWK} '{print $3}')
      dir_grpowner=$(${IMMLIST} $obj | ${GREP} $attr_groupowner | ${AWK} '{print $3}')
      dir_basic_perm=$(${IMMLIST} $obj | ${GREP} $attr_basicPerm | ${AWK} '{print $3}')
      dir_spl_perm=$(${IMMLIST} $obj | ${GREP} $attr_splPerm | ${AWK} '{print $3}')

      #check if imm data for all the fieds exists.
      #
      if [ -z "$dir_path" ] || [ -z "$dir_owner" ] || [ -z "$dir_grpowner" ] || [ -z "$dir_basic_perm" ] || [ -z "$dir_spl_perm" ]; then
        error "ERROR: Failed to retrieve IMM data [$apdirList]"
        continue
      fi

      # store the /cp/printouts groups and health_check group owners
      cpprintoutspath=$(${ECHO} "$dir_path" |${GREP} "$cpprintoutssearch")
      healthcheckpath=$(${ECHO} "$dir_path" |${GREP} -w "$healthchecksearch")
      if [ ! -z "$cpprintoutspath" ]; then
         cpprintoutsgroup=$dir_grpowner
      elif [ ! -z "$healthcheckpath" ]; then
         health_checkgroup=$dir_grpowner
      fi

      if [ -e $dir_path ]; then
        log "INFO: [$dir_path] already exists"
      else
        cp_path=$(${ECHO} "$dir_path" |${GREP} -w $cpsearch)
        scrfile_path=$(${ECHO} "$dir_path" | ${GREP} -w $scrsearch)
        licensefile_path=$(${ECHO} "$dir_path" | ${GREP} -w $licensesearch)
        zip_path=$(${ECHO} "$dir_path" | ${GREP} -w $zipsearch)
                               
        if [[ $apnodeInfo -eq 2 ]] && [[ ! -z "$cp_path" || ! -z "$scrfile_path" || ! -z "$licensefile_path" || ! -z "$zip_path" ]] ; then
          log "INFO: [$dir_path] not created"
          continue
        else
          create_folder $dir_path
          if [ $? == 0 ]; then
            log "INFO: [$dir_path] created"
          else
            error "ERROR: Failed to create directory [$dir_path]"
            continue
          fi
        fi
      fi

      assign_acls "$dir_path" "$dir_owner" "$dir_grpowner" "$dir_basic_perm" "$dir_spl_perm"
    done
    
    ${ECHO} >>$LOG_FILE
    
    log "INFO: Creating FileM directory structure"

    FileMPathList=$(${IMMLIST} $FileMdn | ${AWK} '{print $3}')
    internalroot=$(${IMMLIST} $FileMdn | ${GREP} $internalroot_attrib | ${AWK} '{print $3}')
    externalRoot=$(${IMMLIST} $FileMdn | ${GREP} $externalRoot_attrib | ${AWK} '{print $3}')

    if [ -z "$FileMPathList" ] || [ -z  "$internalroot" ] || [ -z "$externalRoot" ]; then
      error "ERROR: Failed to retrieve FileMPathList, internalroot and externalRoot from IMM"
      exit $exit_failure
    fi

    create_folder $internalroot 777 # SUGaR adaptation: Set to 777 file permissions on internalRoot directory
    if [ $? == 0 ]; then
      log "INFO: [$internalroot] created"
    else
      error "ERROR: Failed to create directory [$internalroot]"
      exit $exit_failure  
    fi
    
    create_folder $externalRoot
    if [ $? == 0 ]; then
      log "INFO: [$externalRoot] created"
    else
      error "ERROR: Failed to create directory [$externalRoot]"
    fi  
  
    # Added to create /etc folder under internal_root for TR HQ90420
    create_folder  $internalroot/etc 555

    if [ -f $LOCTIM_FILE ];then
      [ ! -s $internalroot$LOCTIM_FILE ] && $CMD_LN -s $LOCTIM_FILE $internalroot$LOCTIM_FILE 
      log "INFO: Symlink created for file [localtime]"
    else
      log "ERROR: [localtime] file not found"
    fi

    # Additional check to ensure that sw package APG folder is accessible from 
    # active and passive nodes for SW Update by linking the folder to cluster
    swPackageAPG=$(${IMMLIST} $FileMdn | ${GREP} $swPackageAPG_attrib | ${AWK} '{print $3}')

    # To have a predefined folder into export/import area that is reachable
    # by a TS user in Bash (example put folder /cluster/apg/ts_storage
    # under existing folder /support_data/ts_storage)
    ts_storage_path=$(${IMMLIST} $FileMdn | ${GREP} $ts_storage_attrib | ${AWK} '{print $3}')

    # To create folder /data_transfer/source/cp_printouts and /data_transfer/source/cp_files only on AP1
    # Assign /data_transfer/source/cp_printouts user group same as /cp/printouts    
    source_cpfilepath=$(${IMMLIST} $FileMdn | ${GREP} $source_cpfile | ${AWK} '{print $3}')
    source_cpprintoutspath=$(${IMMLIST} $FileMdn | ${GREP} $source_cpprintouts | ${AWK} '{print $3}')

    #To create folder /data_transfer/source/health_check and assign group owner same as /health_check
    source_healthcheckpath=$(${IMMLIST} $FileMdn | ${GREP} $source_healthcheck | ${AWK} '{print $3}')

    for FilePath in $FileMPathList
    do
      first_char=${FilePath:0:1}
      if [ "$first_char" == "/" ];then
        if [[ "$internalroot" != "$FilePath" && "$externalRoot" != "$FilePath" ]]; then
          InternalPath=$internalroot$FilePath
          if [ "$swPackageAPG" == "$FilePath" ]; then

            # Create directory in cluster for sw package APG folder
            [ ! -d $clu_swpkg_location ] && create_folder $clu_swpkg_location
            [ ! -d $clu_swpkg_apg ] && create_folder $clu_swpkg_apg
            [ ! -s $InternalPath ] && $CMD_LN -s $clu_swpkg_apg $InternalPath
            [ ! -L $clu_swpkg_apg_upgrade ] && $CMD_LN -s $clu_swpkg_location $clu_swpkg_apg_upgrade
            continue
          fi

          if [ "$ts_storage_path" == "$FilePath" ]; then
            # Create directory in cluster for supportdata ts_storage folder
            [ ! -d $clu_storage_location ] && {
              create_folder $clu_storage_location 775
              if [ $? == 0 ];then
                log "INFO: Created shared directory [$clu_storage_location] for tsusers"
                # change the group owner to tsgroup
                ${CHGRP} $group_name $clu_storage_location
                if [ $? -ne 0 ];then
                  error "ERROR: Failed to change group ownership [$group_name] of [$clu_storage_location]"
                  return $exit_failure
                else
                  log "INFO: Successfully changed group ownership [$group_name] of [$clu_storage_location]"
                fi
              fi
            }
            InternalPath=$internalroot$ts_storage_path$ts_storage
            [ ! -s $InternalPath ] && $CMD_LN -s $clu_storage_location $InternalPath
                                                
            # Create directory for IPT
            IptinternalPath=$internalroot$FilePath/protocol_tracer/
            if [ $apnodeInfo -eq 2 ] && [ ! -z "$IptinternalPath" ]; then
              log "INFO: [$IptinternalPath] not created"
              continue
            else
              create_folder $IptinternalPath
              if [ $? == 0 ]; then
                log "INFO: [$IptinternalPath] created"
              else
                error "ERROR: Failed to create directo_ro[$IptinternalPath]"
              fi
            fi
            if [ -d $IptinternalPath ]; then
              InternalPath_IPT_LOGS=$internalroot$FilePath/protocol_tracer/logs
              InternalPath_IPT_DATA=$internalroot$FilePath/protocol_tracer/data
              [ ! -s $InternalPath_IPT_LOGS ] && $CMD_LN -s /data/IPT/LOGS $InternalPath_IPT_LOGS
              [ ! -s $InternalPath_IPT_DATA ] && $CMD_LN -s /data/IPT/DATA $InternalPath_IPT_DATA
            fi
            continue
          fi
          cp_path=$(${ECHO} "$FilePath" | ${GREP} -wi $cpsearch)
          scrfile_path=$(${ECHO} "$FilePath" | ${GREP} -w $scrsearch)
          licensefile_path=$(${ECHO} "$FilePath" | ${GREP} -w $licensesearch)
          zip_path=$(${ECHO} "$FilePath" | ${GREP} -w $zipsearch)

          if [[ $apnodeInfo -eq 2 ]] &&  [[ ! -z "$cp_path"  || ! -z "$scrfile_path" || ! -z "$licensefile_path" || ! -z "$zip_path" ]]; then
            log "INFO: [$InternalPath] not created"
            continue
          else
            if [[ $apnodeInfo -eq 2 ]] && [[ "$source_cpfilepath" == "$FilePath" || "$source_cpprintoutspath" == "$FilePath" ]];then       
              log "INFO: [$InternalPath] not created"
              continue
            else 
              create_folder $InternalPath 775
              if [ $? == 0 ]; then
                 log "INFO: [$InternalPath] created"
                if [[ $apnodeInfo -ne 2 && "$source_cpprintoutspath" == "$FilePath" && ! -z $cpprintoutsgroup ]]; then
                  $CHGRP $cpprintoutsgroup $InternalPath
                    if [ $? != 0 ];then
                      error "ERROR: Failed to change group ownership of [$InternalPath]"
                    else
                      log "INFO: Successfully changed group ownership to [$cpprintoutsgroup] of [$InternalPath]"
                    fi
    
                fi       
                                                           
                if [[ $source_healthcheckpath == $FilePath && ! -z $health_checkgroup ]]; then
                  $CHGRP $health_checkgroup $InternalPath
                  if [ $? != 0 ];then
                    error "ERROR: Failed to change group ownership of [$InternalPath]"
                  else
                    log "INFO: Successfully changed group ownership to [$health_checkgroup] of [$InternalPath]"
                  fi
                fi    
              else
                error "ERROR: Failed to create directory [$InternalPath]"
                continue
              fi
            fi
          fi
        fi
      fi
    done

    # SUGaR adaptation: Set to 777 file permissions on externalRoot directory (com_fuse mount point)
    ${CHMOD} 777 $externalRoot
    if [ $? == 0 ]; then
      log "INFO: [$externalRoot] file permissions set"
    else
      error "ERROR: Failed to set permissions on directory [$externalRoot]"
      exit $exit_failure  
    fi
  fi

   # SUGar adaptatioin: Create and Set 777 folder permissions to '/storage/clear/acs_usafm'
    storage_clear_acs_usafm="$(<$LOTC_API/clear)"/acs_usafm
    create_folder $storage_clear_acs_usafm 777
    if [ $? == 0 ]; then
      log "INFO: [$storage_clear_acs_usafm] created"
    else
      error "ERROR: Failed to create directory [$storage_clear_acs_usafm]"
    fi

	#Creation of directory under cluster for integrityap Baseline file.
	[ ! -d $CLUSTER_LCT_DIR ] && {
		${MKDIR} -p $CLUSTER_LCT_DIR
		if [ $? == 0 ]; then
			log "INFO: [$CLUSTER_LCT_DIR] created"
		else
			error "ERROR: Failed to create directory [$CLUSTER_LCT_DIR]"
		fi
	}

	return $exit_success
}

function get_rep_type() {
  # Check the data disk replication type
  DD_REP_TYP=$(immlist -a dataDiskReplicationType axeFunctionsId=1 2>/dev/null | awk -F'=' '{print $2}')
  if [[ -z "$DD_REP_TYP" || "$DD_REP_TYP" -eq 1 ]]; then
    raid_cmd="${RAIDMGMT}"
  elif [ $DD_REP_TYP -eq 2 ]; then
    raid_cmd="${RAIDMGR} -s"
  fi
}

function get_ap_type() {
  AP_NODE=$(${PARMTOOL} get --item-list ap_type)
  if [ $? -ne 0 ]; then
    abort "Failure while retrieving AP node from parmtool"	
  fi
  echo ${AP_NODE} | awk -F'=' '{print $2}' | awk -F"AP" '{print $2}' 2> /dev/null
}

#function called for configap -a
##############################
function config_a() {
  config_common "a"
  if [ $? == 0 ];then

    # touch file /storage/clear/acs-lct/configap 
    # touch file /tmp/configap_a 
    # LCT daemon will look for existence of this file
    # to know if it has to launch configap -a.
    # Apparently this file is cleared during restore and update 
    STORAGE_PATH=$(${CMD_CAT} $LOTC_API/clear)
    SUBSYS_NAME=acs
    BLOCK_NAME=lct
    NODE_ID=$(</etc/cluster/nodes/this/id)
    TMP_FILE="/tmp/configap_a"
    dnr_with_new_board=$FALSE   
    [ ! -d $STORAGE_PATH/$SUBSYS_NAME-$BLOCK_NAME ] && create_folder $STORAGE_PATH/$SUBSYS_NAME-$BLOCK_NAME  
    $(${TOUCH} $STORAGE_PATH/$SUBSYS_NAME-$BLOCK_NAME/configap.$NODE_ID)
    
    [ ! -f $TMP_FILE ] && $(${TOUCH} $TMP_FILE)

    # Creating directory for sw_package to make it available after B&R
	create_folder $clu_swpkg_location 2770 
    if [ $? == 0 ]; then
      log "INFO: [$clu_swpkg_location] file permissions set"
    else
      error "ERROR: Failed to set permissions on directory [$clu_swpkg_location]"
    fi

	${CHGRP} $NBI_USERS_GROUP_NAME $clu_swpkg_location
    if [ $? == 0 ]; then
      log "INFO: [$clu_swpkg_location] group correctly changed to $NBI_USERS_GROUP_NAME"
    else
      error "ERROR: Failed to change group on directory [$clu_swpkg_location]"
    fi

    create_folder $clu_swpkg_apg 2774
    if [ $? == 0 ]; then
      log "INFO: [$clu_swpkg_apg] file permissions set"
    else
      error "ERROR: Failed to set permissions on directory [$clu_swpkg_apg]"
    fi

    ${CHGRP} $NBI_USERS_GROUP_NAME $clu_swpkg_apg
    if [ $? == 0 ]; then
      log "INFO: [$clu_swpkg_apg] file permissions set"
    else
      error "ERROR: Failed to set permissions on directory [$clu_swpkg_apg]"
    fi  

    # Check the data disk replication type
    if [ -f /cluster/home/board_type ]; then
     	board_type=$( cat /cluster/home/board_type)
     	[ $board_type == 'new' ] && dnr_with_new_board=$TRUE
    fi
    if [ $dnr_with_new_board -eq $FALSE ]; then   
	get_rep_type
    	# create AXE Roles text file if not already present.
    	status=$( $raid_cmd);status=$( echo $status)
        if [ "$status" != "DOWN" ]; then
           create_axe_roles
        fi
    else 
        create_axe_roles
    fi
    # create home directory for ts users if not already exist
    if [ ! -d $tsuser_home_dir ] ; then
      create_home_dir $tsuser_home_dir tsgroup
    else
      configure_home_dir $tsuser_home_dir tsgroup
    fi

    # create home directory for tsadmin if not already exist
    if [ ! -d $tsadmin_home_dir ] ; then
      create_home_dir $tsadmin_home_dir tsadmin
    else
      configure_home_dir $tsadmin_home_dir tsadmin
    fi

    # create nbi_root/softwaremanagement softlink if not exist
    create_swm_softlink 'SoftwareManagement;cmw-swm:cmw-swm'

    # create nbi_root/sw_package/APG softlink if not exist
    create_swm_softlink 'sw_package/APG;root:SWPKGGRP'

    # create nbi_root/sw_package/APG softlink if not exist
    create_swm_softlink 'sw_package/APG/UPGRADE;root:system-nbi-data'

    return $exit_success
  else
    return $exit_failure
  fi
}

function create_swm_softlink(){
  local NAME="$1"
  local NBI_ROOT='/data/opt/ap/internal_root'
  local STORAGE_SOFTWARE_MGMT='/storage/no-backup/coremw/SoftwareManagement'
  local CLUSTER_SWPKG_APG='/cluster/sw_package/APG'
  local CLUSTER_SWPKG_APG_UPGRADE="$NBI_ROOT/sw_package/APG/UPGRADE"
  local NBI_SOFTWARE_MGMT="$NBI_ROOT/$NAME"
  local USER_GROUP=$( echo $NBI_SOFTWARE_MGMT | awk -F ';' '{print $2}')
  local NBI_SOFTWARE_MGMT_FOLDER

  if [ -z "$USER_GROUP" ]; then 
    NBI_SOFTWARE_MGMT_FOLDER=${NBI_SOFTWARE_MGMT%/*}
  else
    NBI_SOFTWARE_MGMT=$( echo $NBI_SOFTWARE_MGMT | awk -F ';' '{print $1}')
    NBI_SOFTWARE_MGMT_FOLDER=${NBI_SOFTWARE_MGMT%/*}
  fi

  if [[ "$NBI_SOFTWARE_MGMT_FOLDER" == "$NBI_ROOT" ]]; then
    if [ ! -d $NBI_SOFTWARE_MGMT_FOLDER ]; then
      slog "creating folder: [$NBI_SOFTWARE_MGMT_FOLDER]"
      mkdir -p $NBI_SOFTWARE_MGMT_FOLDER
      ${CHMOD} 777 $NBI_SOFTWARE_MGMT_FOLDER
    fi
    if [ ! -L $NBI_SOFTWARE_MGMT ];then
      if [ -d $NBI_SOFTWARE_MGMT ]; then
        log "$NBI_SOFTWARE_MGMT folder found.. removing it."
        rm -rf $NBI_SOFTWARE_MGMT
      fi
    fi
  elif [[ "$NBI_SOFTWARE_MGMT_FOLDER" == "$NBI_ROOT/sw_package" ]]; then
    if [ ! -d $NBI_SOFTWARE_MGMT_FOLDER ]; then
      slog "creating folder: [$NBI_SOFTWARE_MGMT_FOLDER]"
      mkdir -p $NBI_SOFTWARE_MGMT_FOLDER
      ${CHMOD} 2775 $NBI_SOFTWARE_MGMT_FOLDER
      [ ! -z "$USER_GROUP" ] && ${CHOWN} -h "$USER_GROUP" $NBI_SOFTWARE_MGMT_FOLDER
    fi
    STORAGE_SOFTWARE_MGMT="$CLUSTER_SWPKG_APG"
  fi
  
  if [ ! -L $NBI_SOFTWARE_MGMT ];then
    if [ -d $NBI_SOFTWARE_MGMT ]; then
      slog "$NBI_SOFTWARE_MGMT folder found.. skipping symbolic creation"
    else
      local PRINTOUT="creating symbolic link: [$NBI_SOFTWARE_MGMT -> $STORAGE_SOFTWARE_MGMT]..."
      slog "$PRINTOUT"
      $CMD_LN -s $STORAGE_SOFTWARE_MGMT $NBI_SOFTWARE_MGMT &>/dev/null
      if [ $? -ne 0 ]; then
        slog "$PRINTOUT failed"
      else
        slog "$PRINTOUT success"
        if [ ! -z "$USER_GROUP" ]; then
          ${CHOWN} -h  "$USER_GROUP" $NBI_SOFTWARE_MGMT
          [ $? -ne 0 ] && log "INFO: Failed to change the group owner and owner for files under [$NBI_SOFTWARE_MGMT] "
        fi
      fi
    fi
  else
    # remove recursive links found if any.
    pushd $NBI_SOFTWARE_MGMT &>/dev/null
    local LINK_NAME=${NBI_SOFTWARE_MGMT##*/}
    if [ -L $LINK_NAME ]; then
      local PRINTOUT="recursive link found in [$NBI_SOFTWARE_MGMT], removing..."
      slog "$PRINTOUT"
      rm -r $LINK_NAME &>/dev/null
      if [ $? -ne 0 ]; then
        slog "$PRINTOUT failed"
      else
        slog "$PRINTOUT success"
      fi
    fi
  fi

  # this case is to retain the permissions of /cluster/sw_package/APG and nbi_root/SoftwareManagement
  # after the upgrade, while applying the permissions to only /cluster/sw_package/APG/UPGRADE
  if [[ ! -z "$USER_GROUP" && "$NBI_SOFTWARE_MGMT" == "$CLUSTER_SWPKG_APG_UPGRADE" ]]; then
    local E_USER_GROUP=$( stat -c '%U:%G' $NBI_SOFTWARE_MGMT)
    if [[ ! -z "$E_USER_GROUP" && "$E_USER_GROUP" != "$USER_GROUP" ]]; then
      slog "user group [$NBI_SOFTWARE_MGMT], found:[$E_USER_GROUP], expecting:[$USER_GROUP], applying"
      ${CHOWN} -h  "$USER_GROUP" $NBI_SOFTWARE_MGMT
      [ $? -ne 0 ] && log "INFO: failed to apply [$USER_GROUP] to [$NBI_SOFTWARE_MGMT]"
    fi
  fi
}

# function to create axe roles
function create_axe_roles() {
  AXEROLES=Node_Role_Export.txt
  SUPPRT_DATA=''

  local internal_root=$( ${IMMLIST} -a $internalroot_attrib $FileMdn)
  internal_root=$( echo $internal_root | $CUT -d = -f2)

  local support_data=$( ${IMMLIST} -a supportData $FileMdn)
  support_data=$( echo $support_data | $CUT -d = -f2)

  SUPPRT_DATA=$internal_root/$support_data

  if [[ -f $SUPPRT_DATA/$AXEROLES  && ! -n $SUPPRT_DATA/$AXEROLES ]]; then
    #echo  "$SUPPRT_DATA/$AXEROLES already exist"
    slog  "$SUPPRT_DATA/$AXEROLES already exist"
  else
    #echo "Creating $SUPPRT_DATA/$AXEROLES"
    slog "Creating $SUPPRT_DATA/$AXEROLES:"

    (
      echo 'ROLE SystemAdministrator,SystemSecurityAdministrator,SystemReadOnly,EricssonSupport,CpRole0,CpRole1,CpRole2,CpRole3,CpRole4,CpRole5,CpRole6,CpRole7,CpRole8,CpRole9,CpRole10,CpRole11,CpRole12,CpRole13,CpRole14,CpRole15'
    ) &> $SUPPRT_DATA/$AXEROLES

    chmod 444 $SUPPRT_DATA/$AXEROLES
    #echo '..Success'
    slog '..Success'
  fi
  return $exit_success
}

#function called for configap -d
##############################
function config_d(){
 dnr_with_new_board=$FALSE 
 # Check the data disk replication type
 if [ -f /cluster/home/board_type ]; then
    board_type=$( cat /cluster/home/board_type)
    [ $board_type == 'new' ] && dnr_with_new_board=$TRUE
 fi
 if [ $dnr_with_new_board -eq $FALSE ]; then 
    get_rep_type
    status=$( $raid_cmd);status=$( echo $status)
    if [ "$status" == "DOWN" ]; then 
       error "ERROR: Data disks are not mounted: Command cannot be executed"
       return $exit_failure
    fi
 fi   
 config_common "d" || return $exit_failure
 create_axe_roles
 # create nbi_root/softwaremanagement softlink if not exist
 create_swm_softlink 'SoftwareManagement;cmw-swm:cmw-swm'
 return $exit_success
}

function logrotate() {
  #Check if LOGDIR exists, if not create one
  if [ -d $LOGDIR ]; then
    if [ ! -f $LOG_FILE ]; then
      ${TOUCH} $LOG_FILE
    fi  
  else
    ${MKDIR} -p $LOGDIR
    ${TOUCH} $LOG_FILE
  fi  
}

function validate_args() {
  if [[ $# != 1 ]] ;then
    usage_error "Incorrect usage" $exit_usage
  fi

  # fetching all command line arguments
  ##############################
  str=$(echo $1)

  # parsing command line arguments
  ##############################
  arg_err_status=0 #this would get set to 1 if argument is invalid
  arg_num=$(${ECHO} $#)
  #arg_1=$1

  while [ $# -gt 0 ]
  do
    arg=$(echo "$1")
    arg_char_1=$(echo ${arg:0:1})
    if [ "$arg_char_1" != '-' ];then
      #${ECHO} "invalid option : $arg"
      arg_err_status=1
    fi
    shift
  done

  # exit from code if invalid argument format
  ##############################
  if [ $arg_err_status == 1 ];then
    usage_error "Incorrect usage" $exit_usage
  fi

  # Check command line arguments and set up internal variables
  #
  #if [[ $arg_num -gt 0 ]];then
  #  case "$str" in
  #    -h)
  #      if [ $arg_num -gt 2 ];then
  #        usage_error "Incorrect usage." $exit_usage
  #      fi
  #      
  #      show_usage
  #      exit $exit_success
  #      ;;
  #  esac
  #fi

  # sending arguments to getopt if valid argument format
  ##############################
  #TEMP=$(getopt ida $str  2> /dev/null 2>&1)
  #TEMP=$CMD_GETOPT --quiet --quiet-output --options="$OPTIONS" -- "$@"

  # exit from code if invalid arguments 
  ##############################
  if [ $? != 0 ]; then
    usage_error "Incorrect Usage" $exit_usage 
  fi
}

function parse_args() {
  if [[ $# != 1 ]] ;then
    usage_error "Incorrect usage" $exit_usage
  fi
  #TRUE=$( true; echo $? )
  #FALSE=$( false; echo $? )

  #OPT_USER_ARG=$FALSE

  #local rCode=$FALSE

  #  local PARAMETERS=$*
  #  local OPTIONS='idah'

    
  #  $CMD_GETOPT --quiet --quiet-output  --options="$OPTIONS" -- "$@"
  #  rCode=$?
  #  [ $rCode -ne $TRUE ] && {
  #    usage_error "Incorrect usage" $exit_usage
  #  }  
    
  #  local ARGS="$@"
  #  eval set -- "$ARGS"
    
    # Make sure to handle the cases for all the options listed in OPTIONS
    
  #  while [ $# -gt 0 ]; do    
  case "$1" in
    -i)        
      slog "INFO: configap -i Invoked"
      lineno=0
      config_i
      retval=$?
      ${ECHO} >>$LOG_FILE
      #shift
    ;;
    -a)        
      slog "INFO: configap -a Invoked"
      lineno=0
      config_a
      retval=$?
      ${ECHO} >>$LOG_FILE
      #shift
    ;;
    -d)        
      slog "INFO: configap -d Invoked"
      lineno=0
      config_d
      retval=$?
      ${ECHO} >>$LOG_FILE
      #shift
    ;;
    -h)
      show_usage
      exit $exit_success
    ;;
  
    *)
      usage_error "Incorrect usage" $exit_usage
    ;;
  esac    
#  done
#shift  
  exit $retval
}
# _____________________ _____________________ 
#|    _ _   _  .  _    |    _ _   _  .  _    |
#|   | ) ) (_| | | )   |   | ) ) (_| | | )   |   
#|_____________________|_____________________|
# Here begins the "main" function...

# Set the interpreter to exit if a non-initialized variable is used.
#set -x  # Trace executed lines, useful for debugging script
set -u  # Unset variables is an error
logrotate
#validate_args "$@"
parse_args "$@"
#exit $exit_success
