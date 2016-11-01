import sys
import os
import socket

def main(ip, port, size):
    data = 's'*size
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect((ip, port))
    while True:
        s.sendall(data)

if __name__ == "__main__":
    argc = len(sys.argv)
    if argc<3:
        print 'usage:%s <ip> <port> [size]'%sys.argv[0]
        sys.exit(-1)
    ip   = sys.argv[1]
    port = int(sys.argv[2])
    size = 100
    if argc>3:
        size = int(sys.argv[3])
    main(ip, port, size)
