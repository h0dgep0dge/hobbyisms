#!/bin/bash

TAP=$(mktemp -u tapXXXXXX)

echo Adding $TAP
ip tuntap add mode tap name $TAP
ip link set $TAP master br0
ip link set $TAP up

socat --experimental FD:10 TUN,iff-no-pi,tun-type=tap,tun-name=$TAP

echo Cleaning up $TAP
ip tuntap del mode tap name $TAP
