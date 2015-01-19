#! /bin/sh
while (true); do
#if  [ 
/z/dfu-util/src/dfu-util -d 1eaf:0003 -a 1 -D $1 -s 0x8000000
# ]; then break; fi
if [ $? -eq 0 ]
then break
fi
sleep 1
done
exit
