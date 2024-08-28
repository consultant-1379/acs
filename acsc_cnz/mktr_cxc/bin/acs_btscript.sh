#!/bin/bash -u

CORE_FILES=$(ls -rt /var/log/core|grep -i "core-")
CMD_TOUCH='/usr/bin/touch'
CMD_RM='/usr/bin/rm -f'
CORE_PATH='/var/log/core/'
TMP_FILE='/tmp/temp.txt'
OPT_AP_FOLDERS='acs|aes|apos|cphw|cps|cqs|ext|fixs|fms|mas|mcs|ocs|pes|sgs|sts'

$($CMD_RM $TMP_FILE)
$($CMD_TOUCH $TMP_FILE)

echo "echo "---------- bt ----------" \n
bt
echo "----------  bt full ----------" \n
bt full
echo "---------- t a a bt ----------" \n
t a a bt
quit $@" >> $TMP_FILE

echo "***************************************************"
echo "Core files present on node are : "
echo "$CORE_FILES"
echo "***************************************************"
for core in $CORE_FILES
do
	echo "******************************************************************************"
	echo "Collecting BT for core dump $core"
	echo "******************************************************************************"
	temp1=$(echo $core | grep -bo "-"|sed 's/:.*$//')	
	s1=$(echo $temp1|awk '{print $1}')
	s2=$(echo $temp1|awk '{print $2}')
	
	temp2=$(echo $core | grep -bo "_"|sed 's/:.*$//')
	
	subsystem=$(echo $core | cut -c $((s1+2))-$temp2)
	binaryname=$(echo $core | cut -c $((s1+2))-$s2)
	echo "Subsystem is $subsystem"
	echo "Binary is $binaryname"
	
	if [[ "$subsystem" =~ ^($OPT_AP_FOLDERS)$ ]]
	then
        	echo "Subsystem $subsystem is in /opt/ap/ folder"
		gdb --command=$TMP_FILE /opt/ap/$subsystem/bin/$binaryname $CORE_PATH/$core
	else
	        echo "Subsystem $subsystem is not in /opt/ap/ folder"
		echo "Skip the BT collection procedure"
	fi	
done
$($CMD_RM $TMP_FILE)
exit $?
