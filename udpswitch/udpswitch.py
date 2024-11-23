import random
from socket import *

serverSocket = socket(AF_INET, SOCK_DGRAM)

serverSocket.bind(('', 31337))

macs = {}
endpoints = []

while True:
    message, address = serverSocket.recvfrom(1600)

    src_mac = message[6:12]
    dst_mac = message[0:6]

    #print(src_mac,"->",dst_mac)
    

    if src_mac not in macs:
        macs[src_mac] = address
    
    if address not in endpoints:
        endpoints.append(address)


    if dst_mac not in macs or dst_mac == "ff:ff:ff:ff:ff:ff":
        for addr in endpoints:
            if addr != address:
                serverSocket.sendto(message,addr)
    else:
        serverSocket.sendto(message,macs[dst_mac])