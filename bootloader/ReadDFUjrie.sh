#! /bin/sh
./dfu-util -d 1209:4443 -a 0 -L -U $1
exit
