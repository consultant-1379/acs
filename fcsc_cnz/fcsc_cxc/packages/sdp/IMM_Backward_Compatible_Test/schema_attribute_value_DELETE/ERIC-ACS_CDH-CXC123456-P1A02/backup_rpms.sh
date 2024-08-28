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

# This script is executed when a backup operation is performed on the node.

AP_HOME_DIR="/home/ap"
AP_ETC_DIR="${AP_HOME_DIR}/etc"
AP_BACKUP_DIR="${AP_HOME_DIR}/backup"
AP_BACKUP_NAME=$2
AP_BACKUP_FPATH="${AP_BACKUP_DIR}/${AP_BACKUP_NAME}.tar.gz"


print_usage () {
    echo "Invalid command: $0 $*"
    echo $"Usage: $0 {create <label> | restore <label> | remove <label> | list}"

}

# Check the exit value from the last command and return 1 if it failed.
check_exit_value () {
    if [ $? -ne 0 ]; then
        echo "$1"
        exit 1
    fi
}

# Create a new directory
create_directory () {
    mkdir -p "$1"
    check_exit_value "Could not create "$1""
    #echo ""$1" have been created"
}

# Remove a file or a directory
remove_file () {
    rm -rf "$1"
    check_exit_value "Could not remove "$1""
    #echo ""$1" have been removed"
}

create () {

    # Create the backup directory if it does not exist
    if [[ ! -e ${AP_BACKUP_DIR} ]]; then
        create_directory $AP_BACKUP_DIR
    fi

    # The label can not exist
    if [[ ! -e "${AP_BACKUP_FPATH}" ]]; then

        cd $AP_ETC_DIR
        tar -pczf "$AP_BACKUP_FPATH" *
        check_exit_value "Could not create backup for $AP_BACKUP_NAME"
        cd - >> /dev/null
        #echo "Backup $AP_BACKUP_FPATH created"

    else
        echo "The label $AP_BACKUP_NAME already exist"
        exit 1
    fi

}

restore () {

    # Check that the label exist
    if [ -e "${AP_BACKUP_FPATH}" ]; then

        # Remove the etc directory if exist
        if [ -e ${AP_ETC_DIR} ]; then
            remove_file $AP_ETC_DIR
        fi

        create_directory $AP_ETC_DIR
        tar -xzf "$AP_BACKUP_FPATH" -C $AP_ETC_DIR
        check_exit_value "Could not extract backup for label $AP_BACKUP_NAME"
        #echo "Backup $AP_BACKUP_FPATH have been restored"

    else

        echo "Not found Com backup with label $AP_BACKUP_NAME"
        exit 1

    fi
}

remove () {

    # Check that the label exist
    if [ -e "${AP_BACKUP_FPATH}" ]; then

        remove_file "$AP_BACKUP_FPATH"
        #echo "Backup with label $AP_BACKUP_NAME have been removed"

    else

        echo "Not found Com backup with label $AP_BACKUP_NAME"
        exit 1

    fi
}

list () {

    if [ -e ${AP_BACKUP_DIR} ]; then

        for x in $AP_BACKUP_DIR/*.tar.gz;
        do
            echo $x | sed "s|${AP_BACKUP_DIR}/||g" | sed "s|.tar.gz||g"
        done

       ## BACKUP_LIST=`ls -C $AP_BACKUP_DIR/*.tar.gz`

       ## LABELS=`echo ${BACKUP_LIST} | sed "s|${AP_BACKUP_DIR}/||g" | sed 's|.tar.gz||g' | sed 's| |\n|g'`
       ## echo -e "$LABELS"

    fi

}


case "$1" in
    create)
        if [ "$#" = 2 ]; then

            create

        else
            print_usage
            exit 1
        fi
        ;;

    restore)
        if [ "$#" = 2 ]; then
            restore
        else
            print_usage
            exit 1
        fi
        ;;

    remove)
        if [ "$#" = 2 ]; then
            remove
        else
            print_usage
            exit 1
        fi
        ;;

    list)
        if [ "$#" = 1 ]; then
            list
        else
            print_usage
            exit 1
        fi
        ;;
    *)
        print_usage
        exit 1

esac

exit 0
