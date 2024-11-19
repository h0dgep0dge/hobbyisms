### UDPSwitch

Virtualbox has a cool feature called the UDPTunnel network driver, which wraps up ethernet frames in a udp packet and sends it off to some endpoint. This is really cool! However, it's strictly point-to-point (well almost, there's no reason a single machine couldn't recieve packets from more than one other, but each can only send to one other). So my idea was to write a program to recieve these packets, and handle them similarly to an ethernet switch, forwarding them to the correct endpoint.

But wait! An ethernet frame wrapped in a udp packet? That sounds like that thing you can do by calling socat with a udp address and a tuntap address, so turns out you even get networking between VMs and real computers for free, using socat.

socat udp:1.2.3.4:31337 TUN,iff-no-pi,tun-type=tap

or if you prefer to let socat configure the interface for you

socat udp:1.2.3.4:31337 TUN:192.168.0.10/24,up,iff-no-pi,tun-type=tap