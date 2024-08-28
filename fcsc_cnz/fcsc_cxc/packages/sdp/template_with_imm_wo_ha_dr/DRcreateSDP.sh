#!/bin/bash
#
# Author : XPHAVAL, TEIGGAM, XFABPAG
# Version 2.2
#

# include commonSDP functions
. ../commonSDPfunctions
#
# function: 
#
create_model_files()
{
[ ! -d $BPATH ] && mkdir -p $BPATH || :
pushd $DPATH > /dev/null
   mkdir models
if [ "$Bname" == "APOS_OSCONFBIN" ]; then
   mkdir enm_models
fi
   check_exit_value "Impossible to create models directory"
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

	if [ "$Bname" == "APOS_OSCONFBIN" ]; then
	  for file in `ls $confpath |  grep -e ".*_ENM_.*[.]xml$"`
          do
                cp $confpath/$file "$DPATH/enm_models"
                chmod +x $DPATH/enm_models/$file
          done
	  if [ -s "$confpath/offline-consumer.config" ] ; then
		cp $confpath/offline-consumer.config "$DPATH"
                chmod +x $DPATH/offline-consumer.config
	  fi
	fi
        for file in `ls $confpath | grep -e ".*_*_mp[.]xml$" | grep -v "DWAXE\|Instances"`
        do
                cp $confpath/$file "$DPATH/models"
                chmod +x $DPATH/models/$file
        done
#-------------------------------------------------------------------------------
       #Create IMM model configuration file
    if [ "$Bname" != "APOS_OSCONFBIN" ]; then
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

	#Create IMM Local Auth model configuration file
        pushd $DPATH > /dev/null
        file=$(ls -1 models | grep ".Roles_Rules_imm_.*[.]xml")
        if [ "$file" != "" ]
        then
           if [ -e $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE ]
           then
              rm $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE; touch $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE
           fi
           ls -1 models | grep ".Roles_Rules_imm_.*[.]xml" | awk '{print "models/"$1}' >> $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE
        fi
      else
        pushd $DPATH > /dev/null
        if [ -e $IMM_R1_MODEL_APOS_CONF_FILE ]
        then
                rm $IMM_R1_MODEL_APOS_CONF_FILE; touch $IMM_R1_MODEL_APOS_CONF_FILE
        fi

        ls -1 models | grep ".*_imm_.*[.]xml" |grep -v ".Roles_Rules_imm_.*[.]xml" | awk '{print "models/"$1}' >> $IMM_R1_MODEL_APOS_CONF_FILE
	cp $IMM_R1_MODEL_APOS_CONF_FILE $OLD_IMM_R1_MODEL_CONF_FILE
        #Create COM model configuration file
        pushd $DPATH > /dev/null
        if [ -e $COM_MODEL_APOS_CONF_FILE ]
        then
                rm $COM_MODEL_APOS_CONF_FILE; touch $COM_MODEL_APOS_CONF_FILE
        fi
        ls -1 models | grep ".*_*_mp[.]xml" | awk '{print "models/"$1}' >> $COM_MODEL_APOS_CONF_FILE

        #Create IMM Local Auth model configuration file
        pushd $DPATH > /dev/null
        file=$(ls -1 models | grep ".Roles_Rules_imm_.*[.]xml")
        if [ "$file" != "" ]
        then
           if [ -e $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE ]
           then
              rm $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE; touch $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE
           fi
           ls -1 models | grep ".Roles_Rules_imm_.*[.]xml" | awk '{print "models/"$1}' >> $IMM_I_LOCAL_AUTH_R1_MODEL_CONF_FILE
        fi
		
      fi	
			
}


#
# function: 
#
create_rpm_list()
{

 #echo " RPM Path is "$rpmpath
 #echo " Des Path is "$DPATH

 #Check if there is any rpm

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
	 #     echo "No rpms found with version: $ver for block: $BName"
	 #     rm -rf $DPATH
	 #     exit 1
	 #fi

 for f in `ls $rpmpath/*.rpm`
 do
      /bin/cp -r $f $DPATH
      f=${f#$rpmpath/}
      subrpmname=${f%.x86_64.rpm}
      echo "$f $subrpmname" >> $TFILE
 done

 mv -f $TFILE "$DPATH/rpm_list.txt"
}


#
# function: 
#
fill_template_validate()
{

 for f in $DPATH_FILES_TPL
 do
   if [ -f $f -a -r $f ]; then
      sed "s/$tagName/$Bname/g" "$f" > $TFILE && mv -f $TFILE "$f"
      sed "s/$tagVer/$ver/g" "$f" > $TFILE && mv -f $TFILE "$f"
      sed "s/$tagCXP/$cxp/g" "$f" > $TFILE && mv -f $TFILE "$f"
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

#
# function: 
#
create_bundle()
{

 pushd $DPATH > /dev/null
 echo "---------------------------------------"
 echo "Creating SDP Bundle for $Bname....."
 echo " "
 if [ $? -eq 0 ]
 then
     chmod +x *.sh

     sdpFileName=$Bname-$cxp-$ver
     create_and_copy_sdpFile $sdpFileName $sdppath
     check_exit_value "Impossible to create file  $sdpFileName !!!"
 else
     echo "Validation failed"
     exit 1
 fi

 popd $DPATH > /dev/null
}



#Argument $1 <CXC-Path> is Block's CXC Path
#Argument $2 <Subsystem_BlockName> is Subsystem_BlockName
#Argument $3 <Version> is Block Version
#Argument $4 <ProductNumber> is Block Product Number 


sdp_check_number_arguments $#


#Validate all arguments
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

SPATH=$curdir/$TEMPLATE_PATH

#echo $DPATH
#echo $SPATH


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


create_model_files

create_rpm_list

fill_template_validate

#include info file if exists
INFO_FILE_CHECK=$(ls -1 $sdppath/*_info.txt 2> /dev/null| wc -l)

if [[ $INFO_FILE_CHECK -eq 1 ]]
then
	/bin/cp $sdppath/*_info.txt $DPATH/
	check_exit_value "Impossible to copy info file from $sdppath to $DPATH"
	echo "Info file succesfully copied"
fi

create_bundle

rm -rf $DPATH
exit 0

# END FILE
