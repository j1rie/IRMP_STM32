#! /bin/sh
./dfu-util -d 1209:4443 -a 0 -w -U $1
exit
