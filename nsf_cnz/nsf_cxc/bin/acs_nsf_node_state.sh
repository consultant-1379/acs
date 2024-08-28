#!/bin/bash

# CoreMW SU Status is considered as node status.

SLOT_ID=`cat /etc/opensaf/slot_id`

case $SLOT_ID in
        1)
        echo "`amf-state siass ha "safSISU=safSu=SC-1\,safSg=2N\,safApp=OpenSAF,safSi=SC-2N,safApp=OpenSAF" |grep saAmfSISUHAState | cut -d"=" -f2 | cut -d "(" -f1`"
                ;;
        2)
        echo "`amf-state siass ha "safSISU=safSu=SC-2\,safSg=2N\,safApp=OpenSAF,safSi=SC-2N,safApp=OpenSAF" |grep saAmfSISUHAState | cut -d"=" -f2 | cut -d "(" -f1`"
                ;;
        *)
                echo "UNDEFINED"
                exit 1
esac
exit 0

