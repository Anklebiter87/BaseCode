import socket
import struct
import sys


class data:
    def __init__(self):
        self.size = 0
        self.data = 0
        self.packedmsg = None

    def decode_packet(self, rawdata):
        self.packedmsg = rawdata
        self.size = struct.unpack('>q', rawdata[0:8])[0]
        self.data = struct.unpack('>{}s'.format(len(rawdata[8:])), rawdata[8:])[0]

    def make_packet(self, mesg):
        junk = struct.pack('>q', 1)
        self.size = len(junk) + len(mesg)
        self.data = mesg
        self.packedmsg = struct.pack('<q', self.size)
        self.packedmsg += struct.pack('<{}s'.format(len(self.data)), self.data)


def main():
    sock = socket.socket()
    sock.bind(("127.0.0.1", 1111))
    sock.listen(10)
    if len(sys.argv) < 2:
        print("supply a binary")
        exit(1)
    while 1:
        clientsocket = None
        try:
            packet = data()
            (clientsocket, address) = sock.accept()
            f = open(sys.argv[1], 'br')
            buff = f.read()
            packet.make_packet(buff)
            print("sending")
            clientsocket.sendall(packet.packedmsg)
        except KeyboardInterrupt:
            if clientsocket:
                clientsocket.close()
            break
        except Exception as e:
            print(e)
            clientsocket.close()
            break
    sock.close()

if __name__ == "__main__":
    main()


