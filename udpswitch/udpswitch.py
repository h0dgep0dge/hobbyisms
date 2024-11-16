import random
from socket import *
from scapy.all import Ether

serverSocket = socket(AF_INET, SOCK_DGRAM)

serverSocket.bind(('', 31337))

macs = {}
endpoints = []

while True:
    message, address = serverSocket.recvfrom(1600)
    e = Ether(message)
    src_mac = e.src
    dst_mac = e.dst
    print(src_mac,"->",dst_mac)
    
    if src_mac not in macs:
        macs[src_mac] = address
    
    if address not in endpoints:
        endpoints.append(address)


    if dst_mac not in macs:
        for addr in endpoints:
            if addr != address:
                #print("packet from",address,"sending to",addr)
                serverSocket.sendto(message,addr)
    else:
        serverSocket.sendto(message,macs[dst_mac])