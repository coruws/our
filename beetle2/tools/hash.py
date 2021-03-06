import pefile,struct,zlib

def gethash(name):
    hash=0
    tmp=0
    l=len(name)
    for c in name:
        tmp=(tmp & 0xFFFFFF00)| ord(c)
        tmp=(tmp>>7)|(tmp<<(32-9))
        tmp=tmp&0xFFFFFFFF
        hash=hash^tmp
    return hash

dll='kernel32.dll'
pe =  pefile.PE(dll)
name=dll.replace(".","_").upper()
print name+" = "+hex(zlib.crc32(dll,-1)&0xFFFFFFFF)
for entry in pe.DIRECTORY_ENTRY_EXPORT.symbols:
    print "_"+entry.name+" = "+ str(hex(zlib.crc32(entry.name,-1)&0xFFFFFFFF))
