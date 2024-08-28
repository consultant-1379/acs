#!/bin/bash

echo -e "TCP/IP Keep alive interval time ==" `cat /proc/sys/net/ipv4/tcp_keepalive_intvl`
echo -e "TCP/IP Keep alive probes ==" `cat /proc/sys/net/ipv4/tcp_keepalive_probes`
echo -e "TCP/IP Keep alive timeout in seconds ==" `cat /proc/sys/net/ipv4/tcp_keepalive_time`
