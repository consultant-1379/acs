#******************************************************************************
#
# NAME
#      acs_ssu_procls.sh
#
# COPYRIGHT Ericsson AB, Sweden 2010.
# All rights reserved.
#
#  The Copyright to the computer program(s) herein 
#  is the property of Ericsson Utvecklings AB, Sweden.
#  The program(s) may be used and/or copied only with 
#  the written permission from Ericsson Utvecklings AB or in 
#  accordance with the terms and conditions stipulated in the 
#  agreement/contract under which the program(s) have been 
#  supplied.
#
# DESCRIPTION
#   This Command Script File packs the appropiate 
#      information for analysing memory usage and keep
#      up til the 10 latest dumps.
#
# DOCUMENT NO
#   
# AUTHOR 
#   2010-07-20 by XSIRKUM
#
#   REV NO      DATE        NAME        DESCRIPTION
#   A          20110704    XSHYCHI     First Version
#
#
#******************************************************************************

if [ -z "$SystemDrive" ];
then
   SystemDrive="/var/log"
fi 

TEMP="$SystemDrive"

if [ ! -d  $TEMP ];
then
    mkdir -p $TEMP
fi

if [ -f "$TEMP/acs_ssu_procdump09.txt" ];
then
    rm "$TEMP/acs_ssu_procdump09.txt"
fi

if [ -f $TEMP/acs_ssu_procdump08.txt ];
then
    mv $TEMP/acs_ssu_procdump08.txt $TEMP/acs_ssu_procdump09.txt
fi

if [ -f  $TEMP/acs_ssu_procdump07.txt ];
then
    mv $TEMP/acs_ssu_procdump07.txt $TEMP/acs_ssu_procdump08.txt
fi

if [ -f  $TEMP/acs_ssu_procdump06.txt ];
then  
   mv $TEMP/acs_ssu_procdump06.txt $TEMP/acs_ssu_procdump07.txt
fi

if [ -f  $TEMP/acs_ssu_procdump05.txt ];
then 
   mv $TEMP/acs_ssu_procdump05.txt  $TEMP/acs_ssu_procdump06.txt
fi

if [ -f  $TEMP/acs_ssu_procdump04.txt ];
then 
   mv $TEMP/acs_ssu_procdump04.txt  $TEMP/acs_ssu_procdump05.txt
fi

if [ -f  $TEMP/acs_ssu_procdump03.txt ];
then
   mv $TEMP/acs_ssu_procdump03.txt $TEMP/acs_ssu_procdump04.txt
fi

if [ -f  $TEMP/acs_ssu_procdump02.txt ];
then
   mv $TEMP/acs_ssu_procdump02.txt $TEMP/acs_ssu_procdump03.txt
fi

if [ -f  $TEMP/acs_ssu_procdump01.txt ];
then mv $TEMP/acs_ssu_procdump01.txt $TEMP/acs_ssu_procdump02.txt
fi

if [ -f  $TEMP/acs_ssu_procdump.txt ];
then 
  mv $TEMP/acs_ssu_procdump.txt  $TEMP/acs_ssu_procdump01.txt
fi

acs_ssu_pstat.sh > $TEMP/acs_ssu_procdump.txt
