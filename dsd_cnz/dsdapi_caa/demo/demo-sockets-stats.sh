#!/bin/bash

while true ; do
	clear
	netstat --interfaces -e | grep -Ei "(inet)|(encap)|(packets)|(bytes)"
	usleep $1
done
