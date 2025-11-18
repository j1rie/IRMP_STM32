## Automatical start and stop of irmplircd at boot or on device dis/reconnect
get irmplircd from https://github.com/realglotzi/irmplircd
put the udev rule '70-irmp.rules' in your udev rules directory (/etc/udev/rules.d/) and
put the systemd service 'irmplircd.service' into your services directory (/etc/systemd/system/)
put the systemd-tmpfiles config file 'irmplircd.conf' into your tmpfiles directory (/etc/tmpfiles.d/)
and run "systemctl enable irmplircd.service" once

## Was the computer started by the receiver?
You can log when the receiver has started the computer.  
At each start by the receiver, it sends ff0000000000 every second for a configurable period of time. The first one is written to the log file /var/log/started_by_IRMP_STM32. To do this, log_KEY_REFRESH.sh is called by irmpexec.  
If an entry appears in the log file (/var/log/started_by_IRMP_STM32) shortly after the boot messages (depending on the distribution/var/log/boot.msg or similar), you know that the computer was started by the receiver.  
If the entry in the log file is older than the boot messages, it was started by switching on the computer or by timer.  
Configuration via stm32IRconfig -> s -> x -> 90.  
To test, the PC must be turned off and started by the receiver.

You can use this in VDR's shutdown script to avoid shutting down on the first power key press, if the computer is already running. See the example script.
This is handy if you have a remote control with macros like the Logitech and you want to use one button to switch on not only VDR but also other devices. If the VDR was started by a timer, it would otherwise go off when you want to switch on all other devices. This can be avoided by a query in the shutdown script (see the example script vdrshutdown).  

Put the systemd service 'irmpexec.service' in your services directory (/etc/systemd/system/) and run "systemctl enable irmpexec.service" once
irmp_stm32.map must contain a line "ff0000000000 KEY_REFRESH"

vdr-plugin-irmplirc writes into /var/log/started_by_IRMP_STM32 directly.

## Execute commands with irmpexec
For example an irmplircd.map line "070000000100 KEY_1" and an
irmpexec.map line "KEY_1 ls -l" will execute "ls -l" on 070000000100.
