#!/bin/bash
#
#
# Author : EANBUON, XPHAVAL, TEIGGAM, ESALVES
# Version 2.2
#

# include commonSDP functions
. ../commonSDPfunctions

#
# function:
#
create_campaign()
{
        echo "--------------------------------"
        echo " BlockName is $Bname"
        echo " Ver  is $ver"
        echo " Product No  is $cxp"
        echo "--------------------------------"

        [ ! -d $BPATH ] && mkdir -p $BPATH || :
        for f in $DPATH_FILES_TPL
        do
                if [ -f $f -a -r $f ]
                then
                        # ---------------------
                        # replace bundle tags
                        # ---------------------
                        sed "s/$tagName/$Bname/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagCXP/$cxp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagVer/$ver/g" "$f" > $TFILE && mv -f $TFILE "$f"
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
                        sed "s/$tagSUT/APG_SU_$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagSuFailOver/$suFailOver/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        # ---------------------------
                        # replace CompType tags
                        # ---------------------------
                        sed "s/$tagCompT/$compT$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagHLTCHK/$hltCHK/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagHTPeriodLowerBound/$HCperiod_Lowebound/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagHTMaxDurationLowerBound/$HCmaxDuration_Lowerbound/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagScript/$script/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagDefaultClcCliTimeOut_LowerBound/$defaultClcCliTimeOut_LowerBound/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagDefaultCallbackTimeOut_LowerBound/$defaultCallbackTimeOut_LowerBound/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        #sed "s/$tagRecoveryOnError/$recoveryOnError/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagDisableRestart/$disableRestart/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        # ---------------------------
                        # replace CSType tags
                        # ---------------------------
                        sed "s/$tagCST/$csT$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        # -----------------------------
                        # replace ServiceType tags
                        # -----------------------------
                        sed "s/$tagSVCT/$svcT$ApName/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        # -----------------------------
                        # replace Campaign tags
                        # -----------------------------
                        sed "s/$tagUC/$campaigntype/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagApp/$appT$safApp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagSG/$sgT$safSgNoRed$safApp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagSU1/$suT$safSu1$safApp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagSU2/$suT$safSu2$safApp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagSaAmfSUFailover/$saAmfSUFailover/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagCOMP/$compT$safComp$safApp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        #sed "s/$tagSI/$svcT$safSi$safApp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagSI1/$svcT$safSi1$safApp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagSI2/$svcT$safSi2$safApp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagCSI/$csT$safCsi$safApp/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagPathnamePrefix/$pathnamePrefix/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagRecoveryOnErrorCampaign/$recoveryOnErrorCampaign/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        sed "s/$tagCompVer/$ver/g" "$f" > $TFILE && mv -f $TFILE "$f"
                        # -----------------------------
                        # replace Campaign Upgrade Tags
                        # -----------------------------
                        sed "s/$tagOldName/$OldName/g" "$f" > $TFILE && mv -f $TFILE "$f"
			#sed "s/$tagNewName/$NewName/g" "$f" > $TFILE && mv -f $TFILE "$f"
   			if [ "$campaigntype" == U1 ]
   			 then
    			 if [[ "$f" =~ .*campaign.xml.template$ ]];then
     			  mv "$f" "${f/.xml.template/}.template.xml"
    			 else
     			  mv "$f" "${f/.template/}"
    			 fi
   			else
     			 mv "$f" "${f/.template/}"
   			fi
                else
                        echo "Error: Cannot read $f"
                fi
        done

        # Building the SDP
        pushd $DPATH > /dev/null
        # Validating the campaign xml
        echo "Validating campaign.xml in progress :: "
        validate_ETFfile ETF.xml
	#Not possible to validate campaign with regex
	#validate_Campaignfile campaign.xml
        tar cvzf $Bname-$campaigntype"-"$cxp-$ver.x86_64.sdp -P *.xml
        cp -f $Bname-$campaigntype"-"$cxp-$ver.x86_64.sdp $sdppath
        echo "Campaign Successfully created at $sdppath"

        popd > /dev/null
        rm -rf $DPATH
}

#
# function:
#
function check_number_arguments(){

        if [ $1 -lt 5 ] || [ $1 -gt 6 ]
        then
                echo " usage:"
                echo "   $0 <CXC-Path> <Subsystem_BlockName> <Version> <ProductNumber> <CampaignType>"
                echo "    where:"
                echo "              Argument 1 <CXC-Path> is Block's CXC Path"
                echo "              Argument 2 <Subsystem_BlockName> is Subsystem_BlockName"
                echo "              Argument 3 <Version> is Block Version"
                echo "              Argument 4 <ProductNumber> is Block Product Number "
                echo "              Argument 5 <CampaignType> is Campaign type"
                echo "                           I1 --> Install Campaign (SingleStep Procudure)"
#                echo "                           I2 --> Install Campaign (RollingUpgrade Procudure)"
                echo "                           U1 --> Update  Campaign (RollingUpgrade Procudure)"
                echo "                           R1 --> Remove  Campaign (only test purpose)
                                    Argument 6  <Old-Version> is the old Block Version
                                    Note:
                                            Argument 6 <Old-Version> is Mandatory if <campaignType> is U1"
                exit 1
        fi
}

#Argument $1 <CNZ-Path> is Block's CNZ Path
#Argument $2 <Subsystem_BlockName> is Subsystem_BlockName
#Argument $3 <Version> is Block Version
#Argument $4 <CXPNumber> is Block Product Number
#Argument $5 <Install(I)/Remove(R)/Upgrade(U)> is mode


check_number_arguments $#

#Validate all arguments
echo "Validating Arguments:"
echo "---------------------------------"


sdppath=`get_sdppath $1`
check_exit_value "sdp path is wrong !!!"
rpmpath=`get_rpmpath $1`
check_exit_value "rpm path is wrong !!!"
tmppath=`get_tmppath $1`
check_exit_value "tmp path is wrong !!!"

#--- For debug purposes ----
#echo $sdppath
#echo $rpmpath
#echo $tmppath
# ------------------------
Bname=`validate_blockname $2`
check_exit_value "Block name is wrong !!!"
ver=`validate_r_state $3`
check_exit_value "R-sate is wrong !!!"
cxp=`validate_ProductNr $4`
check_exit_value "Product Number is wrong !!!"

campaigntype=`campaign_getType $5`
check_exit_value "Campaign Type is wrong !!!"

if [ $campaigntype = "I2" ]
then
        echo "I2 Campaign Creation: Disabled"
        exit 0
fi

if [ $campaigntype = "U1" ]
then
        if [ -z $6 ]
        then
                echo " Upgrade Revision tag missing for U1 campaign to continue.."
                exit 1
        else
                OldName=$6
		#NewName=$6
        fi
fi

echo "--->"$campaigntype

DPATH=$tmppath/$Bname"-"$campaigntype"-"$cxp"-"$ver

curdir=`pwd`

SPATH=$curdir/$campaigntype$TEMPLATE_NAME

echo $DPATH
echo $SPATH



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

#include sdp.conf
if [[ -e "$sdppath/sdp.conf" ]]
then
        . $sdppath/sdp.conf
	#. $validatesdpconfpath/validatesdpconf $sdppath/sdp.conf
else
        echo "sdp.conf: Configuration file missing for the block $sdppath"
        exit 1
fi

        create_campaign
        check_exit_value "Impossible to create campaign file !!!"


#END

