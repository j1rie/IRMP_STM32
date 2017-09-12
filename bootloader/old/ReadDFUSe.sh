#! /bin/sh
while (true); do
./dfu-util -d 1eaf:0003 -a 1 -U $1
if [ $? -eq 0 ]
then break
fi
sleep 1
done
exit
