#!/bin/bash
# Author : xphaval
#

args_stub()
{
	# This is created because, the regular expressions are validating on this suse 11.2;; Reason Unknown.
	# Remove this stub later
	specfile=$1
	echo $specfile	

	#blockpath=$1
	#BName=`echo $2 | tr '[:lower:]' '[:upper:]'`
	#ver=`echo $3 | tr '[:lower:]' '[:upper:]'`
	#cxp=`echo $4 | tr '[:lower:]' '[:upper:]'`

	#echo "$blockpath ;; $BName ;; $ver ;; $cxp "
}

validate_arguments()
{
#Validating the first design rule path
        if [[ $1 =~ "[/].*[a-zA-Z0-9][_][c][n][z]" ]]
        then
                blockpath=$1
                echo "$1: OK"
        else
                echo "$1: Violates Design Rule of Block Path"
                exit
        fi

#Validating the block Name
        if [[ $2 =~ "^[a-zA-Z0-9]*_[a-zA-Z0-9]" ]]
        then
                BName=`echo $2 | tr '[:lower:]' '[:upper:]'`
                echo "$BName: OK"
        else
                echo "$2: Violates Design Rule of Block Name"
                exit
        fi
# Validating the Version
        if [[ $3 =~ "^[R,P][0-9 A-Z]*$" ]]
        then
                ver=`echo $3 | tr '[:lower:]' '[:upper:]'`
                echo "$ver: OK"
        else
                echo "$3: Violates the Design Rule of Version"
                exit
        fi
# Validating the CXC Number
        if [[ $4 =~ "^CXP[0-9_]" ]]
        then
                cxp=`echo $4 | tr '[:lower:]' '[:upper:]'`
                echo "$cxp: OK"
        else
                echo "$4: Violates the Design Rule of CXC Number"
                exit
        fi
}

set_temp_path()
{
	path="/tmp/$USER/"
	rm -rf $path > /dev/null
	mkdir $path
	if [ $? -ne 0 ]
	then
		echo "ERROR: "$path" can not be created"
		exit 1
	fi
	dest_folder=$path
}

get_block_details()
{
	if [[ $1 =~ "^[a-zA-Z0-9]*_[a-zA-Z0-9]" ]]
        then
                BName=`echo $1 | tr '[:lower:]' '[:upper:]'`
        else
                echo "$1: Violates Design Rule of Block Name"
                exit
        fi

        blockname=`echo $1 | awk -F_ '{print $2}'`
	blockname_lower=`echo $blockname | tr '[:upper:]' '[:lower:]'`

	blockpath=/vobs/cm4ap/ntacs/"$blockname_lower"_cnz/	
	sdppath=/vobs/cm4ap/ntacs/"$blockname_lower"_cnz/"$blockname_lower"_cxc/packages/sdp/
	makefilepath=/vobs/cm4ap/ntacs/"$blockname_lower"_cnz/"$blockname_lower"_cxc/Makefile

	if [ -e $makefilepath ]
	then
		ver=`cat $makefilepath | grep ^CXC_VER | awk -F'?=' '{split($2,a," "); print a[1]}'`
		cxp=`cat $makefilepath | grep ^CXC_NR | awk -F'?=' '{split($2,a," "); print a[1]}'`
	else
		echo "Error: Block Specific Details Not Found."
		exit 1
	fi
		
}

configure_source_path()
{
	# Source path
	# SPATH="/vobs/cm4ap/ntacs/fcsc_cnz/fcsc_cxc/packages/sdp/template_with_imm_with_ha_dr/proto_template"
	SPATH="/vobs/cm4ap/ntacs/fcsc_cnz/fcsc_cxc/packages/sdp/template_create_sdp/proto_template"
	#echo "Source Path of template:: $SPATH"

	#BPATH="/vobs/cm4ap/ntacs/fcsc_cnz/fcsc_cxc/packages/sdp/template_with_imm_wo_ha_dr/SDPbakup/"
	BPATH="/vobs/cm4ap/ntacs/fcsc_cnz/fcsc_cxc/packages/sdp/template_create_sdp/SDPbackup"
	
	rm -rf $BPATH > /dev/null
	mkdir $BPATH
	if [ $? -ne 0 ]
	then
		echo "ERROR: "$BPATH" can not be created"
		exit 1
	fi
}

create_model_files()
{
	set_temp_path
	#Declare all necessary variables-----Start
	tagName="#TAG_NAME#"
	tagVer="#TAG_REV#"
	tagCXP="#TAG_CXP#"

	echo " Block Name is $BName"
	echo " Block Version  is $ver"
	echo " Block CXP  is $cxp"
	
	ext="/*.template"
	# Destination Path
	DPATH=$dest_folder/$BName"-"$cxp"-"$ver
	
	DPATH_FILES_TPL="$DPATH$ext"
	TFILE="/tmp/out.tmp.$$"
	if [ ! -d $DPATH ]
	then
	  #echo " Destination path is $DPATH"
	  #echo " Source template path is $SPATH"
	  mkdir $DPATH
	  /bin/cp -R $SPATH/* $DPATH/.
	fi

	[ ! -d $BPATH ] && mkdir -p $BPATH || :		
		pushd $DPATH > /dev/null
		mkdir models
		popd > /dev/null
		
	confpath=$blockpath/"$blockname_lower"_cxc/conf

	for file in `ls $confpath |  grep -e ".*_imm_.*[.]xml"`
	do
		cp $confpath/$file "$DPATH/models"
	done
#-------------------------------------------------------------------------------
	#Create imm-model.config file
	pushd $DPATH > /dev/null
	if [ -e imm-model.config ]
	then
		rm imm-model.config; touch imm-model.config
	fi
	
	ls -1 models >> imm-model.config
}

create_rpm_list()
{

	rpmpath=$blockpath/"$blockname_lower"_cxc/packages/rpm/
	#echo " RPM Path is "$rpmpath

	#Check if there is any rpm

	ls $rpmpath/*.rpm > /dev/null
	if [ $? -ne 0 ]
	then
		echo "ERROR: No rpms found in $rpmpath path"
		rm -rf $DPATH
		exit 1
	fi

	#Copy all rpm to SDP from vobs
	#Copy rpm files to SDP folder
	#echo "rpm copy: $dest_folder"
	#Create rpm_list.txt file

	#for f in `ls $rpmpath/*.rpm`
	for f in `ls $rpmpath | grep $ver`
	do
		/bin/cp -r $rpmpath/$f $DPATH
		f=${f#$rpmpath/}
		subrpmname=${f%.x86_64.rpm}
		echo "$f $subrpmname" >> $TFILE
	done

	mv -f $TFILE "$DPATH/rpm_list.txt"
}

fill_template_validate()
{

	ext="/*.template"
	DPATH_FILES_TPL="$DPATH$ext"
	#echo "DPATH_FILES_TPL= $DPATH_FILES_TPL"

	for f in $DPATH_FILES_TPL
	do
	  if [ -f $f -a -r $f ]; then
	   /bin/cp -f $f $BPATH
	   sed "s/$tagName/$BName/g" "$f" > $TFILE && mv -f $TFILE "$f"
	   sed "s/$tagVer/$ver/g" "$f" > $TFILE && mv -f $TFILE "$f"
	   sed "s/$tagCXP/$cxp/g" "$f" > $TFILE && mv -f $TFILE "$f"
	   mv -f "$f" "${f/.template/}"
	  else
	   echo "Error: Cannot read $f"
	  fi
	done

# Need to validate xml code here..
echo "-------------------------------------------------------------------------------------"
echo "Validating ETF.xml .."
echo "-------------------------------------------------------------------------------------"
xmllint --schema /vobs/IO_Developments/MIDDLEWARE/schemas/etf.xsd --output /dev/null ETF.xml
if [ $? -gt 0 ]
then
        echo "Error: Validation Failed for xml file"
        exit 1
fi
}

create_sdp_bundle()
{
	pushd $DPATH > /dev/null
	if [ $? -eq 0 ]
	then
		chmod +x *.sh
		echo "--------------------------------------------------------------------------"
		echo "..Creating SDP Bundle with the files .. "
		echo "--------------------------------------------------------------------------"
		tar czvf $BName-$cxp-$ver.x86_64.sdp -P *
		cp $BName-$cxp-$ver.x86_64.sdp $sdppath
		if [ $? -eq 0 ]
		then
			echo "-------------------------------------------------------------------"
		        echo $BName-$cxp-$ver.x86_64.sdp created successfully at location $sdppath
			echo "-------------------------------------------------------------------"
		else
			echo "*******************************************************************"
		        echo "Copying failed from $DPATH, Please do a manual copy of SDP"
			echo "*******************************************************************"
		        exit 1
		fi
	else
		echo "Validation failed"
		exit 1
	fi
	popd $DPATH > /dev/null
}

if [ $# -ne 1 ]
then
  echo " "
  echo " usage:"
  echo "   ./createsdp.sh <BlockName> "
  echo " "
  echo " "
  echo "where:"
  echo "              Argument 1 <Subsystem_BlockName>"
  exit 1
fi

#validate_arguments $1 $2 $3 $4 ; *STUB*
#args_stub $1 ; *STUB*
get_block_details $1
configure_source_path
create_model_files
create_rpm_list
fill_template_validate
create_sdp_bundle

rm -rf $DPATH
exit 0
#/bin/rm $TFILE

