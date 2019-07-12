#!/bin/sh
if [ "$(stat -c %Y /var/log/started_by_IRMP_STM32)" -lt "$(stat -c %Y /var/log/boot.msg)" ];
then date >> /var/log/started_by_IRMP_STM32; fi;
