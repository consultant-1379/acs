#!/bin/bash

echo "`date`: $0 $*"

CMD=$1

LOGDIR=/var/opt/apg_log
APG_SSU=/opt/ap/acs/bin/acs_ssumonitord
PIDFILE="/var/run/${SA_AMF_COMPONENT_NAME}".pid

#mkdir -p $RUNDIR
mkdir -p $LOGDIR

cleanup()
{
	if test -f $PIDFILE; then
		PID=`cat $PIDFILE`
		kill $PID 2> /dev/null
		rm $PIDFILE
	fi
}

terminate()
{
	if test -f $PIDFILE; then
		PID=`cat $PIDFILE`
		kill $PID 2> /dev/null
		rm $PIDFILE
	fi
}

instantiate()
{
	if [ ! -x $APG_SSU ]; then
		logger -s -t $0 "ERROR: File $APG_FCSC does not exist or is not executable"
		exit 1
	fi

	cleanup

	$APG_SSU -ha >> ${LOGDIR}/apg_ssu.log 2>&1 &

	echo $! > $PIDFILE
}

case "$CMD" in
	instantiate)
		instantiate $*
		;;
	cleanup)
		cleanup $*
		;;
	terminate)
		terminate $*
		;;
	*)
		echo "Usage: $0 {instantiate|cleanup}"
		exit 1
esac

exit 0

