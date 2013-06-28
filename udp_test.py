#!/usr/bin/env python2
'''UDP echo to test communication
'''

from socket import *
import sys

local_addr = ('', 1337)
arduino_addr = ('192.168.0.116', 1337)

def main():
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind(local_addr)
    print("listening at: %s" % (s.getsockname(), ))
    print("arduino addr: %s" % (arduino_addr, ))
    s.sendto('hello', arduino_addr)
    
    while(1):
        (recv_data, addr) = s.recvfrom(2048)
        if not recv_data:
            time.sleep(0.1)
            continue
        if addr != arduino_addr:
            print("Got %s bytes from %s, ignoring" % (len(recv_data), addr))
            continue
        print("Got %s bytes, replying" % (len(recv_data), ))
        s.sendto(recv_data, addr)
        
if __name__ == '__main__':
    main()