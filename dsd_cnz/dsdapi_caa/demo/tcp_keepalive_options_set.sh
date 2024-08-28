#!/bin/bash

echo 2 > /proc/sys/net/ipv4/tcp_keepalive_intvl
echo 2 > /proc/sys/net/ipv4/tcp_keepalive_probes
echo 5 > /proc/sys/net/ipv4/tcp_keepalive_time
