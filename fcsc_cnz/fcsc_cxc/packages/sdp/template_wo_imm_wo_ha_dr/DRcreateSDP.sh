#!/bin/bash
#
#
# Author : XPHAVAL, TEIGGAM
# Version 2.1
#

# include commonSDP functions
. /vobs/cm4ap/ntacs/fcsc_cnz/fcsc_cxc/packages/sdp/commonSDPfunctions

create_bundle()
{
echo "-------------creating SDP bundle--------------------"

#-------------------------------------------------
#Process rpms & create rpm_list-----Start
#echo " RPM Path is "$rpmpath

#Check if there is any rpm
ls $rpmpath/*.rpm > /dev/null
check_exit_value "ERROR: No rpms found in $rpmpath path"


#ls $rpmpath | grep ".*[.]rpm"
#echo "rpm copy: "$DPATH


#Copy rpm files to SDP folder
#Create rpm_list.txt file
for f in `ls $rpmpath/*.rpm`
do
                /bin/cp -r $f $DPATH
                f=${f#$rpmpath/}
                subrpmname=${f%.x86_64.rpm}
                echo "$f $subrpmname" >> $TFILE
done
mv -f $TFILE "$DPATH/rpm_list.txt"
#Process rpms & create rpm_list-----End
#---------------------------------------------------------
#Create all necessary templates, scripts & validate------End

#echo "DPATH_FILES_TPL= "$DPATH_FILES_TPL

for f in $DPATH_FILES_TPL
do
  if [ -f $f -a -r $f ]; then
   sed "s/$tagName/$Bname/g" "$f" > $TFILE && mv -f $TFILE "$f"
   sed "s/$tagVer/$ver/g" "$f" > $TFILE && mv -f $TFILE "$f"
   sed "s/$tagCXP/$cxp/g" "$f" > $TFILE && mv -f $TFILE "$f"
   mv -f "$f" "${f/.template/}"
  else
   echo "Error: Cannot read $f"
  fi
done

pushd $DPATH > /dev/null
                                                                
# Validating XML File
validate_ETFfile ETF.xml
if [ $? -eq 0 ]
then
        chmod 777 *.sh
        sdpFileName=$Bname-$cxp-$ver
        create_and_copy_sdpFile $sdpFileName $sdppath
        check_exit_value "Impossible to create file  $sdpFileName !!!"
else
        echo "Validation Failed, Please check manually"
fi

popd > /dev/null

echo "-------------terminating SDP bundle--------------------"

echo ""

}



#Argument $1 <CXC-Path> is Block's CXC Path
#Argument $2 <Subsystem_BlockName> is Subsystem_BlockName
#Argument $3 <Version> is Block Version
#Argument $4 <ProductNumber> is Block Product Number 

sdp_check_number_arguments $#
#Validate all arguments
#echo "Validating Arguments:"
#echo "---------------------------------"

sdppath=`get_sdppath $1`
check_exit_value "sdp path is wrong !!!"
rpmpath=`get_rpmpath $1`
check_exit_value "rpm path is wrong !!!"
tmppath=`get_tmppath $1`
check_exit_value "tmp path is wrong !!!"

Bname=`validate_blockname $2`
check_exit_value "Block name is wrong !!!"
ver=`validate_r_state $3`
check_exit_value "R-sate is wrong !!!"
cxp=`validate_ProductNr $4`
check_exit_value "Product Number is wrong !!!"

DPATH=$tmppath/$Bname"-"$cxp"-"$ver
DPATH_FILES_TPL=$DPATH$ext

curdir=`pwd`

SPATH=$curdir/$TEMPLATE_PATH

create_and_copy_template_to_dest_path $DPATH $SPATH

#include info file if exists
INFO_FILE_CHECK=$(ls -1 $sdppath/*_info.txt 2> /dev/null| wc -l)

if [[ $INFO_FILE_CHECK -eq 1 ]]
then
	/bin/cp $sdppath/*_info.txt $DPATH/
	check_exit_value "Impossible to copy info file from $sdppath to $DPATH"
	echo "Info file succesfully copied"
fi

create_bundle

#END
