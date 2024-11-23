### UDPSwitch

Virtualbox has a cool feature called the UDPTunnel network driver, which wraps up ethernet frames in a udp packet and sends it off to some endpoint. This is really cool! However, it's strictly point-to-point (well almost, there's no reason a single machine couldn't recieve packets from more than one other, but each can only send to one other). So my idea was to write a program to recieve these packets, and handle them similarly to an ethernet switch, forwarding them to the correct endpoint.

But wait! An ethernet frame wrapped in a udp packet? That sounds like that thing you can do by calling socat with a udp address and a tuntap address, so turns out you even get networking between VMs and real computers for free, using socat.

```bash
socat udp:1.2.3.4:31337 TUN,iff-no-pi,tun-type=tap
```

or if you prefer to let socat configure the interface for you

```bash
socat udp:1.2.3.4:31337 TUN:192.168.0.10/24,up,iff-no-pi,tun-type=tap
```

As often happens, a problem you thought needed to be solved with a custom piece of software can be solved with a socat one-liner. The following line creates a udp listener, and each new connection spawns a tap interface.

```bash
sudo socat -4 --experimental udp-listen:31337,fork tun,up,iff-no-pi,tun-type=tap,netns=bridge
```

These interfaces can be bridged together, allowing the kernel to take care of STP, forwarding tables, and all the rest of it. At the time of writing this isn't done automatically, but wouldn't that be something