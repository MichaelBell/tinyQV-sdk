import sys

if len(sys.argv) < 3:
    sys.exit(1)

in_file = open(sys.argv[1], "rb")
out_file = open(sys.argv[2], "wb")

while b := in_file.read(1):
    byte = b[0]
    out_byte = (((byte & 0x01) << 7) | ((byte & 0x02) << 5) | ((byte & 0x04) << 3) | ((byte & 0x08) << 1) |
                ((byte & 0x10) >> 1) | ((byte & 0x20) >> 3) | ((byte & 0x40) >> 5) | ((byte & 0x80) >> 7))
    out_file.write(bytes((out_byte,)))
