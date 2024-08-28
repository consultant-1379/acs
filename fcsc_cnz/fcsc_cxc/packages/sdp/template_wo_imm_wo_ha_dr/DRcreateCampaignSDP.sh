#!/bin/bash
#
#
# Author : EANBUON, XPHAVAL, TEIGGAM
# Version 2.1
#

# include commonSDP functions
. /vobs/cm4ap/ntacs/fcsc_cnz/fcsc_cxc/packages/sdp/commonSDPfunctions

create_campaign()
{
echo "-------------create_campaign--------------------"
[ ! -d $BPATH ] && mkdir -p $BPATH || :
for f in $DPATH_FILES_TPL
do
  if [ -f $f -a -r $f ]; then
   sed "s/$tagName/$Bname/g" "$f" > $TFILE && mv -f $TFILE "$f"
   sed "s/$tagVer/$ver/g" "$f" > $TFILE && mv -f $TFILE "$f"
   sed "s/$tagCXP/$cxp/g" "$f" > $TFILE && mv -f $TFILE "$f"
   if [ "$campaigntype" == U1 ]
   then
     sed "s/$tagOldName/$oldver/g" "$f" > $TFILE && mv -f $TFILE "$f"
   fi
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

validate_ETFfile ETF.xml
#Not possible to validate campaign with regex
#validate_Campaignfile campaign.xml
campaignFileName=$Bname-$campaigntype"-"$cxp-$ver
create_and_copy_sdpFile $campaignFileName $sdppath
check_exit_value "Impossible to create file  $campaignFileName !!!"
popd > /dev/null
echo "-------------terminating SDP campaign--------------------"

}

# -------------------------------------------
# Start script execution

#Argument $1 <CNZ-Path> is Block's CNZ Path
#Argument $2 <Subsystem_BlockName> is Subsystem_BlockName
#Argument $3 <Version> is Block Version
#Argument $4 <CXPNumber> is Block Product Number 
#Argument $5 <Install(I)/Remove(R)/Upgrade(U)> is mode
#Argument 6  <Old-Version> is the old Block Version. It is mandatory if Argument $5 is equal to Upgrade(U) 

campaign_check_number_arguments $#


#Validate all arguments
echo "Validating Arguments:"
echo "---------------------------------"


sdppath=`get_sdppath $1`
check_exit_value "sdp path is wrong !!!"
rpmpath=`get_rpmpath $1`
check_exit_value "rpm path is wrong !!!"
tmppath=`get_tmppath $1`
check_exit_value "tmp path is wrong !!!"

echo $sdppath
echo $rpmpath
echo $tmppath

Bname=`validate_blockname $2`
check_exit_value "Block name is wrong !!!"
ver=`validate_r_state $3`
check_exit_value "R-sate is wrong !!!"
cxp=`validate_ProductNr $4`
check_exit_value "Product Number is wrong !!!"

campaigntype=`campaign_getType $5`
check_exit_value "Campaign Type is wrong !!!"

# for debug purpose !!
echo "Campaign Type is "$campaigntype
if [ $campaigntype = "I2" ]
then
        echo "I2 Campaign Creation: Disabled"
        exit 0
fi

if [ "$campaigntype" == U1 ]
then
    if [ $# -eq 6 ]
    then
        #oldver=`validate_r_state $6`
	oldver=$6
        #check_exit_value "Old R-sate is wrong !!!"
    else
        check_exit_value "Old R-sate is missing !!!"
    fi
fi

DPATH=$tmppath/$Bname"-"$campaigntype"-"$cxp"-"$ver

curdir=`pwd`

SPATH=$curdir/$campaigntype$TEMPLATE_NAME

# for debug purpose !!
echo $DPATH
echo $SPATH

create_and_copy_template_to_dest_path $DPATH $SPATH

DPATH_FILES_TPL=$DPATH$ext

create_campaign
check_exit_value "Impossible to create campaign file !!!"

#END
