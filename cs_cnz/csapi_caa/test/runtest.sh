#!/bin/bash

if [ $# -ne 1 ] ; then
        echo "Incorrect usage"
		echo "Usage: \"runtest.sh <filename>\""
        exit 1
fi

FILENAME=$1

if ! [ -x ${FILENAME} ]; then
    echo "${FILENAME} is not an executable file"
fi    

TEMPFILE=/tmp/.${FILENAME}.$$

echo "---------------------------"
echo "Running test $FILENAME"
echo "---------------------------"

./${FILENAME} > ${TEMPFILE}

FAILCOUNT=`grep "#failure" ${TEMPFILE} | wc -l`
SUCCESSCOUNT=`grep "#success" ${TEMPFILE} | wc -l`

if [ -z "${FAILCOUNT}" ]; then
    FAILCOUNT=0
fi

if [ -z "${SUCCESSCOUNT}" ]; then
    SUCCESSCOUNT=0
fi

cat ${TEMPFILE}

echo "---------------------------"
echo "Result from ${FILENAME}"
echo "Failures: ${FAILCOUNT}"
echo "Successes: ${SUCCESSCOUNT}"
echo "---------------------------"

rm -f ${TEMPFILE}