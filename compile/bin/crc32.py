import zlib
import sys

buffersize = 65536

with open(sys.argv[1], 'rb') as afile:
    buffr = afile.read(buffersize)
    crcvalue = 0
    while len(buffr) > 0:
        crcvalue = zlib.crc32(buffr, crcvalue)
        buffr = afile.read(buffersize)

print(format(crcvalue & 0xFFFFFFFF, '08x'))
