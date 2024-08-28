#!/bin/bash
for i in {1..500}
do
   echo "Run number $i"
   acs_csd --run-foreground &
   pid="$!"
   
   echo "Pid = $pid"
   sleep 10
   
   if [ $? -eq 0 ]; then
   	echo "Killing $pid"
   	kill -9 $pid
   else 
   	echo "RESULT NOT OK!!!!!!!!!!"
   	break   	
   fi
   
done