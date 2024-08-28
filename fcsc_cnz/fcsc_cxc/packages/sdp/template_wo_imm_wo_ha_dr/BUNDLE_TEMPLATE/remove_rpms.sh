#! /bin/sh
##
## Copyright (c) Ericsson AB, 2010.
##
## All Rights Reserved. Reproduction in whole or in part is prohibited
## without the written consent of the copyright owner.
##
## ERICSSON MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
## SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING
## BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. ERICSSON
## SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A
## RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
## DERIVATIVES.
##
##

# Remove command
RPM_CMD=cmw-rpm-config-delete
# File containing RPM file/name list
RPM_LIST_FILE=rpm_list.txt
# Field in RPM_LIST_FILE to use when removing
RPM_LIST_FIELD=2

# Assume RPM files located in same directory as script
cd $(dirname $0)
RPM_FILES=$(cat $RPM_LIST_FILE | cut -d' ' -f${RPM_LIST_FIELD})

for rpm in $RPM_FILES
do
    $RPM_CMD $rpm
    if [ $? -ne 0 ]; then
        exit 1
    fi
done

exit 0


