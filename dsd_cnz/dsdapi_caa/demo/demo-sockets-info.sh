#!/bin/bash

while true ; do
	clear
	netstat --inet -t -pavnee | grep -Ei "(active)|(proto)|(demo-)"
	echo
	netstat --unix -pavnee | grep -Ei "(active)|(proto)|(demo-)"
	usleep $1
done
