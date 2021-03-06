import os,sys,re,struct

def bin2src(s,name):
    s=bytearray(s)
    o=""
    o+=("static const unsigned char %s[]={" % name)
    for i in range(0,len(s)):
        if (i%32==0):
            o+=("\n")
        o+=("0x%2.2X," % s[i])
    o+=("\n};\n")
    return o

l=len(sys.argv)
if l<3:
    print "Error: invalid argument\npython bin2src.py file.bin name file.c"
    sys.exit()
f=open(sys.argv[1],"rb")
data=f.read()
f.close()

name=sys.argv[2]
fout=sys.argv[3]

data=bin2src(data,name)
f=open(fout,"wb+")
f.write(data)
f.close()
print "Done"