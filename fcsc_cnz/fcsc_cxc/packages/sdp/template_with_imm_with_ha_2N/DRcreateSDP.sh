#!/bin/bash
#
# Author : XPHAVAL, TEIGGAM, ESALVES, EANBUON, EANFORM, XFABPAG
# Version 2.4
#

# include commonSDP functions
. ../commonSDPfunctions

create_model_files()
{
        #Declare all necessary variables-----Start

        echo "---------------------------------------"
        echo " Block SDP Details "
        echo "---------------------------------------"
        echo " Block Name is $Bname"
        echo " Block Version  is $ver"
        echo " Product Number  is $cxp"


        [ ! -d $BPATH ] && mkdir -p $BPATH || :
                pushd $DPATH > /dev/null
                mkdir models
                popd > /dev/null

        confpath=$blockpath/conf
        #echo $confpath

	##############################################
	# START: MODELS.INFO
	if [ -s "$confpath/models.info" ] ; then
		cat "$confpath/models.info" | grep -e '^VERSION:[ ]*'$cxp'-'$ver'$' &> /dev/null
		if [ $? -ne 0 ] ; then
			echo "ERROR: Current version $cxp-$ver not found in models.info"
			rm -rf $DPATH
			exit 2
		fi
		cp "$confpath/models.info" "$DPATH"
	fi
	## END: MODELS.INFO
	#############################################

        for file in `ls $confpath |  grep -e ".*_imm_.*[.]xml$"`
        do
                cp $confpath/$file "$DPATH/models"
		chmod +x $DPATH/models/$file
        done
	
	for file in `ls $confpath | grep -e ".*_*_mp[.]xml$" | grep -v "DWAXE\|Instances"`
	do	
		cp $confpath/$file "$DPATH/models"
		chmod +x $DPATH/models/$file
	done
#-------------------------------------------------------------------------------
         #Create IMM model configuration file
        pushd $DPATH > /dev/null
        if [ -e $IMM_R1_MODEL_CONF_FILE ]
        then
                rm $IMM_R1_MODEL_CONF_FILE; touch $IMM_R1_MODEL_CONF_FILE
        fi

        ls -1 models | grep ".*_imm_.*[.]xml" |grep -v ".Roles_Rules_imm_.*[.]xml" | awk '{print "models/"$1}' >> $IMM_R1_MODEL_CONF_FILE
	cp $IMM_R1_MODEL_CONF_FILE $OLD_IMM_R1_MODEL_CONF_FILE	

        #Create COM model configuration file
        pushd $DPATH > /dev/null
        if [ -e $COM_MODEL_CONF_FILE ]
        then
                rm $COM_MODEL_CONF_FILE; touch $COM_MODEL_CONF_FILE
        fi
        ls -1 models | grep ".*_*_mp[.]xml" | awk '{print "models/"$1}' >> $COM_MODEL_CONF_FILE

        pushd $DPATH > /dev/null
        #Create IMM Local Auth model configuration file
        file=$(ls -1 models | grep ".Roles_Rules_imm_.*[.]xml")
        if [ "$file" != "" ] 
        then 
           if [ -e $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE ]
           then
                rm $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE; touch $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE
           fi
           ls -1 models | grep ".Roles_Rules_imm_.*[.]xml" | awk '{print "models/"$1}' >> $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE
       fi	
}

create_rpm_list()
{

        ls $rpmpath/*.rpm > /dev/null
        if [ $? -ne 0 ]
        then
                echo "ERROR: No rpms found in $rpmpath path"
                rm -rf $DPATH
                exit 1
        fi

                                                #Copy all rpm to SDP from vobs
        ls $rpmpath | grep ".*[.]rpm" > /dev/null
                                                #Copy rpm files to SDP folder
                                                #echo "rpm copy: $dest_folder"
                                                #Create rpm_list.txt file
        #ls $rpmpath | grep $ver >> /dev/null
        #if [ $? -gt 0 ]
        #then
        #        echo "No rpms found with version: $ver for block: $Bname"
        #        rm -rf $DPATH
        #        exit 1
        #fi
        #for f in `ls $rpmpath | grep $ver`
	for f in `ls $rpmpath/*.rpm`
        do
                /bin/cp -r $f $DPATH
                f=${f#$rpmpath/}
                subrpmname=${f%.x86_64.rpm}
                echo "$f $subrpmname" >> $TFILE
        done

        mv -f $TFILE "$DPATH/rpm_list.txt"
}

fill_template_validate()
{

        for f in $DPATH_FILES_TPL
        do
	 if [ -f $f -a -r $f ]; then
		# ---------------------
		# replace bundle tags
		# ---------------------
		sed "s/$tagName/$Bname/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagVer/$ver/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagCXP/$cxp/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagSafVer/$safVer/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# ---------------------------
		# replace AppType tags
		# ---------------------------
		sed "s/$tagAppT/$appT$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# ---------------------------
		# replace SGType tags
		# ---------------------------
		sed "s/$tagSgT/$sgT$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagSuProbPeriod/$suProbation_period/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagSuProbCounterMax/$suProbation_counterMax/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagCompProbPeriod/$compProbation_period/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagCompProbCounterMax/$compProbation_counterMax/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagAutoAdjustPeriod/$autoAdjust_period/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# -----------------------
		# replace SUType tags
		# -----------------------
		sed "s/$tagSUT/$suT$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagSuFailOver/$suFailOver/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# ---------------------------
		# replace CompType tags
		# ---------------------------
		sed "s/$tagCompT/$compT$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagHLTCHK/$hltCHK/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# Added for More than one service 
		sed "s/$tagHLTCHK1/$hltCHK1/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagHLTCHK2/$hltCHK2/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# Added for More than one service -- ends
		sed "s/$tagHTPeriodLowerBound/$HCperiod_Lowebound/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagHTMaxDurationLowerBound/$HCmaxDuration_Lowerbound/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagScript/$script/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# Added for more than one service
		sed "s/$tagScript1/$script1/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagScript2/$script2/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# Added for more than one service - Ends
		sed "s/$tagDefaultClcCliTimeOut_LowerBound/$defaultClcCliTimeOut_LowerBound/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagDefaultCallbackTimeOut_LowerBound/$defaultCallbackTimeOut_LowerBound/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagRecoveryOnError/$recoveryOnError/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagDisableRestart/$disableRestart/g" "$f" > $TFILE && mv -f $TFILE "$f"
		sed "s/$tagCompVer/$ver/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# ---------------------------
		# replace CSType tags
		# ---------------------------
		sed "s/$tagCST/$csT$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
		# -----------------------------
		# replace ServiceType tags
		# -----------------------------
		sed "s/$tagSVCT/$svcT$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
           mv -f "$f" "${f/.template/}"
          else
                echo "Error: Cannot read $f"
                exit 1
          fi
        done

# Need to validate xml code here..
echo "---------------------------------------"
echo "Validating ETF.xml .."
echo "---------------------------------------"
validate_ETFfile ETF.xml


}

create_bundle()
{

        pushd $DPATH > /dev/null
	echo "---------------------------------------"
        echo "Creating SDP Bundle for $Bname....."
	echo " "
        if [ $? -eq 0 ]
        then
                chmod +x *.sh
                tar czvf $Bname-$cxp-$ver.x86_64.sdp -P *
                cp $Bname-$cxp-$ver.x86_64.sdp $sdppath
                if [ $? -eq 0 ]
                then
			echo " " 
                        echo "$Bname-$cxp-$ver.x86_64.sdp created successfully at location $sdppath"
			echo " "
                else
                        echo "Error:: Copying failed from $DPATH"
                        exit 1
                fi
        else
                echo "Validation failed"
                exit 1
        fi

        popd $DPATH > /dev/null

}

#
# function: 
#
function check_number_arguments(){

	if [ $1 -ne 4 ]
	then
		echo " usage:"
		echo "   $0 <CXC-Path> <Subsystem_BlockName> <Version> <ProductNumber>" 
		echo "    where:"
		echo "              Argument 1 <CXC-Path> is Block's CXC Path"
		echo "              Argument 2 <Subsystem_BlockName> is Subsystem_BlockName"
		echo "              Argument 3 <Version> is Block Version"
		echo "              Argument 4 <ProductNumber> is Block Product Number "
                echo ""
		exit 1
	fi

}

check_number_arguments $#


#validate all arguments
echo "Validating Arguments:"
echo "---------------------------------"

blockpath=$1
sdppath=`get_sdppath $1`
check_exit_value "sdp path is wrong !!!"
rpmpath=`get_rpmpath $1`
check_exit_value "rpm path is wrong !!!"
tmppath=`get_tmppath $1`
check_exit_value "tmp path is wrong !!!"

#echo $sdppath
#echo $rpmpath
#echo $tmppath

Bname=`validate_blockname $2`
check_exit_value "Block name is wrong !!!"
ver=`validate_r_state $3`
check_exit_value "R-sate is wrong !!!"
cxp=`validate_ProductNr $4`
check_exit_value "Product Number is wrong !!!"



DPATH=$tmppath/$Bname"-"$cxp"-"$ver

curdir=`pwd`

#if [ $Bname == "ACS_CHB" ]
#then
#	SPATH=$curdir/$TEMPLATE_PATH_3	
#else
#	SPATH=$curdir/$TEMPLATE_PATH
#fi

        SPATH=$curdir/$TEMPLATE_PATH

#echo $DPATH
echo $SPATH
#sleep 100


if [ ! -d $DPATH ]
then
  #echo " Source template path is $SPATH"
  #echo " Destination path is $DPATH"
  mkdir $DPATH
  check_exit_value "Impossible to create $DPATH directory"
  /bin/cp -R $SPATH/* $DPATH/.
  check_exit_value "Impossible to copy from $SPATH to $DPATH"
fi

DPATH_FILES_TPL=$DPATH$ext

#include info file if exists
INFO_FILE_CHECK=$(ls -1 $sdppath/*_info.txt 2> /dev/null| wc -l)

if [[ $INFO_FILE_CHECK -eq 1 ]]
then
	/bin/cp $sdppath/*_info.txt $DPATH/
	check_exit_value "Impossible to copy info file from $sdppath to $DPATH"
	echo "Info file succesfully copied"
fi

#include sdp.conf
if [[ -e "$sdppath/sdp.conf" ]]
then
	. $sdppath/sdp.conf
else
	echo "sdp.conf: Configuration file missing for the block $sdppath"
	exit 1
fi

create_model_files
create_rpm_list
fill_template_validate
create_bundle

rm -rf $DPATH
exit 0

# END FILE

