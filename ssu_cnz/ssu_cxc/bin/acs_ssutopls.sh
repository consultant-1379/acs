#******************************************************************************
#
# NAME
#      ACS_SSU_Topls.sh
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
#   A          20100720    XSIRKUM     First Version
#
#
#******************************************************************************

if [ -z "$SystemDrive" ];
then
   SystemDrive="/var/log"
fi 

if [ -z "$AP_HOME" ];
then
   AP_HOME="/opt/AP"
fi

TEMP="$SystemDrive"

if [ ! -d  $TEMP ];
then
    mkdir -p $TEMP
fi

if [ -f "$TEMP/ACS_SSU_Procdump09.txt" ];
then
    rm "$TEMP/ACS_SSU_Procdump09.txt"
fi

if [ -f $TEMP/ACS_SSU_Procdump08.txt ];
then
    mv $TEMP/ACS_SSU_Procdump08.txt $TEMP/ACS_SSU_Procdump09.txt
fi

if [ -f  $TEMP/ACS_SSU_Procdump07.txt ];
then
    mv $TEMP/ACS_SSU_Procdump07.txt $TEMP/ACS_SSU_Procdump08.txt
fi

if [ -f  $TEMP/ACS_SSU_Procdump06.txt ];
then  
   mv $TEMP/ACS_SSU_Procdump06.txt $TEMP/ACS_SSU_Procdump07.txt
fi

if [ -f  $TEMP/ACS_SSU_Procdump05.txt ];
then 
   mv $TEMP/ACS_SSU_Procdump05.txt  $TEMP/ACS_SSU_Procdump06.txt
fi

if [ -f  $TEMP/ACS_SSU_Procdump04.txt ];
then 
   mv $TEMP/ACS_SSU_Procdump04.txt  $TEMP/ACS_SSU_Procdump05.txt
fi

if [ -f  $TEMP/ACS_SSU_Procdump03.txt ];
then
   mv $TEMP/ACS_SSU_Procdump03.txt $TEMP/ACS_SSU_Procdump04.txt
fi

if [ -f  $TEMP/ACS_SSU_Procdump02.txt ];
then
   mv $TEMP/ACS_SSU_Procdump02.txt $TEMP/ACS_SSU_Procdump03.txt
fi

if [ -f  $TEMP/ACS_SSU_Procdump01.txt ];
then mv $TEMP/ACS_SSU_Procdump01.txt $TEMP/ACS_SSU_Procdump02.txt
fi

if [ -f  $TEMP/ACS_SSU_Procdump.txt ];
then 
  mv $TEMP/ACS_SSU_Procdump.txt  $TEMP/ACS_SSU_Procdump01.txt
fi

top -b -n1 > $TEMP/ACS_SSU_Procdump.txt
