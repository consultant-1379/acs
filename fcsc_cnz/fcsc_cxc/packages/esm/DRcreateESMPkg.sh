#!/bin/bash
#
# Author :XANJDIT/ZGXXNAV
#
#
# include commonESM functions
. commonESMfunctions
##
# [Description: Creation of runtime package 
# [Parameters:Input as sdp package
#
# 
# [Return:
#  <$blkname>-<$ver>-runtime-<$product_number>.tar.gz         
#  ex:
#  apg_lct-8.0.0-000-runtime-CXC1371491.tar.gz
#
##
tooldir=$1/$7/../../IO_Developments/Tools/linux_conf_file

create_runtime()
{

        pushd $RUNPATH > /dev/null
	echo "---------------------------------------"
        echo "creating runtime pacakge for $Bname....."
	echo " "
        if [ $? -eq 0 ]
        then
             version=$(csm_version $cxp $ver)
 	     if [ $? -eq 1 ]
             then 
    	          echo "error: $version failed"
 	 	  exit 1
             fi
	      file=`ls $sdppath |  grep -e ".*[-]$cxp[-]$ver[.]x86_64[.]sdp$"` 
               if [ $? = 0 ]
               then
                 cp -rfp $sdppath/$file "$RUNPATH"
               else
                  echo "file not found"
                  exit 1 
               fi  
              
               tar czvf $Bname-$version-runtime-$cxp_esm.tar.gz -P *
		
              if [ $? -eq 0 ]
              then  
                cp -rfp $Bname-$version-runtime-$cxp_esm.tar.gz  $esmpath
               if [ $? -eq 0 ]
                 then
			echo " " 
                        echo "$Bname-$version-runtime-$cxp_esm.tar.gz runtitme created successfully at location $esmpath"
			echo " "
                else
                        echo "error:: copying failed from $RUNPATH"
                        exit 1
  	        fi
             else
                echo "validation failed"
		exit 1
	     fi	
	 else
                echo "validation failed"
                exit 1
        fi

        popd $RUNPATH > /dev/null

}

##
# [Description: Creation of Deployment package
# [Parameters:Input as yml,python script package and scripts of block
#
#
# [Return:
#  <$blkname>-<$ver>-deployment-<$product_number>.tar.gz
#  ex:
#  apg_lct-8.0.0-000-deployment-CXC1371491.tar.gz 
#
##

create_deployment()
{

        pushd $DEPPATH > /dev/null
        echo $DEPPATH
        echo "---------------------------------------"
        echo "creating Deployment pacakge for $Bname....."
        echo " "
        mkdir -p $SCRIPATH
        mkdir -p $PHYPATH

        version=$(csm_version $cxp $ver)
        if [ $? -eq 1 ]
        then
            echo "error: $version failed"
            exit 1
        fi

        confpath=$blockpath/conf
        coremw=$confpath/coremw_plugin
        scripts=$confpath/coremw_plugin/scripts 
        
        if [ -e $CSM_METADATA ]
        then
                rm $CSM_METADATA; touch $CSM_METADATA
        fi
        echo -e "csm-metadata-version=1.0.0\ncsm-root=./" >> $CSM_METADATA
        if [ -d $coremw ]
        then 
          for file in `ls $coremw |  grep -e ".*[.]py$"`
          do
               cp -rfp "$coremw/$file" "$DEPPATH/$PHYPATH/"
               chmod +x "$DEPPATH/$PHYPATH/$file"
          done
          if [ -d $scripts ]
          then 
             if [ "$(ls -A $scripts)" ] 
              then
               cp -rfp $scripts/* "$DEPPATH/$SCRIPATH/"
               chmod +x $DEPPATH/$SCRIPATH/*
             fi
          fi
       fi

       
        file=`ls $confpath |  grep -e ".*[.]yml$"`
        echo $file
        if [ "$file" != "" ]
        then
        ct_checkout $confpath/$file        
       
         ret=$(python $tooldir/apg_csm_update.py -p $confpath -c $file -v $version -d $Bname-$version-deployment-$cxp_esm.tar.gz -r $Bname-$version-runtime-$cxp_esm.tar.gz -b ERIC-$cxcname-$cxp-$ver -f $cxcname-$cxp-$ver.x86_64.sdp)
                if [ $? -eq 0 ] 
                then  
                  cp -rfp "$confpath/$file" "$DEPPATH/csm"
                  chmod +x "$DEPPATH/csm/$file"
                else
                 echo "csm fragment update failed"
		 python $tooldir/apg_csm_update.py -h
		 ct_uncheckco $confpath/$file
                 exit 1
	        fi
        else
             echo "yaml file not found "
             exit 1        
        fi
        if [ $? -eq 0 ]
        then
              echo $DEPPATH
              tar czvf $Bname-$version-deployment-$cxp_esm.tar.gz -P *
              if [ $? -eq 0 ]
                 then
                        cp -rfp $Bname-$version-deployment-$cxp_esm.tar.gz  $esmpath
                        echo " "
                        echo "$Bname-$version-deployment-$cxp_esm.tar.gz deployment created successfully at location $esmpath"
                else
                        echo "error:: copying failed from $DEPPATH"
                        exit 1
              fi
         else
                echo "validation failed"
                exit 1
        fi
        popd $DEPPATH > /dev/null

}


ct_checkout()
{
 echo $CT_LSCO $1 > /dev/null 2>&1
  if [ $? != 0 ]  
   then echo "$1 file is not found !!!"
   exit 2
  fi
  nfiles=$($CT_LSCO $1  | wc -l)
  if [ $nfiles != 0 ]
   then echo "$1 file is already in Checked out"
  else echo "$1 file is in Checkin ... I'm performing checked-out action"
     $CT_CO $1
  fi
}

ct_uncheckco()
{
 $CT_LSCO $1 > /dev/null 2>&1
 if [ $? != 0 ] 
   then echo "$1 file is not found !!!"
   exit 2
 fi
 nfiles=$($CT_LSCO $1  | wc -l)
 if [ $nfiles != 0 ]
  then echo "$1 file is in Checkout ... I'm performing unchecked-out action!!!"
        $CT_UNCO $1
 else echo "$1 file is already in Checkin ..."
 fi
}

#
# function: 
#
function check_number_arguments(){

	if [ $1 -ne 7 ]
	then
		echo " usage:"
		echo "   $0 <CXC-Path> <Subsystem_BlockName> <Version> <Base versio> <ProductNumber><CXC_NAME>" 
		echo "    where:"
		echo "              Argument 1 <CXC-Path> is Block's CXC Path"
		echo "              Argument 2 <Subsystem_BlockName> is Subsystem_BlockName"
		echo "              Argument 3 <Version> is Block Version"
		echo "              Argument 4 <Base Version> is Block Base Version"
		echo "              Argument 5 <ProductNumber> is Block Product Number "
		echo "              Argument 6 <CX Name> is CXC NAME "
                echo "              Argument 7 <ntacs PATH> is path to ntacs "
                echo ""
		exit 1
	fi

}

check_number_arguments $#

CLEARTOOL="cleartool"
CT_CO="$CLEARTOOL co -nc -unr"
CT_LSCO="$CLEARTOOL lsco -cview"
CT_UNCO="$CLEARTOOL unco -rm"

#Validate all arguments
echo "Validating Arguments:"
echo "---------------------------------"
echo $1
blockpath=$1
sdppath=`get_sdppath $1`
check_exit_value "sdp path is wrong !!!"
esmpath=`get_esmpath $1`
check_exit_value "sdp path is wrong !!!"
tmppath=`get_tmppath $1`
check_exit_value "tmp path is wrong !!!"

Bname=`validate_blockname $2`
check_exit_value "Block name is wrong !!!"
ver=`validate_r_state $3`
check_exit_value "R-sate is wrong !!!"
cxp=`validate_ProductNr $5`
check_exit_value "Product Number is wrong !!!"
cxcname=`validate_cxcname $6`
check_exit_value "cxc name is wrong !!!"
baseswver=`echo $4 | cut -d'-' -f2`
echo $baseswver


RUNPATH=$tmppath/$Bname"-"$cxp"-"$ver"-"runtime
DEPPATH=$tmppath/$Bname"-"$cxp"-"$ver"-"deployment
CSM_METADATA=csm.metadata
Blkname=`echo $Bname | sed 's/\_/\./g'`
curdir=`pwd`
SCRIPATH="csm/plugin/$Blkname/scripts"
PHYPATH="csm/plugin/$Blkname/lib/python2.7"

cxp_esm=$(echo $cxp | cut -d'_' -f1)

curdir=`pwd`


if [ ! -d $RUNPATH ]
then
  mkdir $RUNPATH
  echo $RUNPATH	
  check_exit_value "Impossible to create $RUNPATH directory"
fi

if [ ! -d $DEPPATH ]
then
  mkdir $DEPPATH
  echo $DEPPATH
  check_exit_value "Impossible to create $RUNPATH directory"
fi

create_runtime
create_deployment
rm -rf $RUNPATH
rm -rf $DEPPATH
exit 0

# END FILE

