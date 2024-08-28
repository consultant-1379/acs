#!/bin/bash
# Install command

RPM_CMD=cmw-rpm-config-add

# File containing RPM file/name list
RPM_LIST_FILE=rpm_list.txt
# Field in RPM_LIST_FILE to use when installing
RPM_LIST_FIELD=1

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

