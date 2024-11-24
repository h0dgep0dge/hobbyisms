#!/bin/bash

ip netns add bridge
ip -n bridge link add br0 type bridge
ip -n bridge link set br0 up

socat --experimental UDP4-LISTEN:31337,reuseaddr,fork EXEC:./handle_client.sh,fdin=10,fdout=11,nofork,netns=bridge
