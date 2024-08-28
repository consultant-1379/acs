#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       ipsecls.sh
# Description:
#       A script to wrap the invocation of ipsecls from the COM CLI.
##
# Changelog:
#     - Monday 9th of September 2013 - by Fabrizio Paglia (xfabpag)
#       First version.
##

ikeVersion=`cat /cluster/etc/ikeversion`

if [ "$1" == "-v" ] && [ "$#" -eq 1 ] ; then
        echo -e "\nIKE Protocol: $(cat /cluster/etc/ikeversion)\n"
else
        if [[ $ikeVersion == "IKEV1" ]]
        then
                /usr/bin/sudo /opt/ap/acs/bin/ipsecls "$@"
        else
                /usr/bin/sudo /opt/ap/acs/bin/ipsecls_strongswan "$@"
        fi
fi

exit $?

