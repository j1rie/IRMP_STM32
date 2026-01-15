#!/bin/sh
LANG=de_DE.UTF-8
if [ /var/log/started_by_IRMP_STM32 -ot /var/log/boot.msg ];
then date >> /var/log/started_by_IRMP_STM32; fi;
