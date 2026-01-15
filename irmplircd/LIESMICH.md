## Automatisches starten und stoppen von irmplircd beim Booten oder beim Anschliessen/Entfernen des Geräts
Man holt irmplircd von https://github.com/realglotzi/irmplircd.  
Kopiere die udev Regel '70-irmp.rules' in das udev rules Verzeichnis (/etc/udev/rules.d/) und  
kopiere den systemd service 'irmplircd.service' in das services Verzeichnis (/etc/systemd/system/).  
Kopiere die systemd-tmpfiles Konfigurations Datei 'irmplircd.conf' in das tmpfiles Verzeichnis (/etc/tmpfiles.d/)  
und führe einmal "systemctl enable irmplircd.service" aus.

## Wurde der Computer vom Empfänger gestartet?
Man kann protokollieren, wann der Empfänger den Computer gestartet hat.  
Bei jedem Start durch den Empfänger sendet er eine konfigurierbare Zeit lang sekündlich KEY_REFRESH. Der erste wird in die Logdatei /var/log/started_by_IRMP_STM32 geschrieben. Dazu wird von irexec oder triggerhappy log_KEY_REFRESH_IRMP_STM32.sh aufgerufen.  
Wenn kurz nach den Bootmeldungen (je nach Distribution/var/log/boot.msg o.ä.) ein Eintrag im Logfile (/var/log/started_by_IRMP_STM32) landet, weiß man, dass der Computer vom Empfänger gestartet wurde.  
Wenn der Eintrag im Logfile älter ist als die Bootmeldungen, wurde per Einschalter am Computer oder per Timer gestartet.  
Konfiguration über stm32IRconfig -> s -> x -> 90.  
Zum Testen muss der PC aus sein und vom Empfänger gestartet werden.

Man kann dies im Shutdown-Skript von VDR verwenden, um zu verhindern, dass der Computer beim ersten Drücken der Power-Taste heruntergefahren wird, wenn er bereits läuft. Siehe Beispielskript.  
Das ist nützlich, wenn eine Logitech Fernbedienung mit Makros auf einen Tastendruck hin den VDR und alle anderen Geräte einschaltet. Wenn der VDR durch einen Timer gestartet wurde, würde er sonst aus gehen, wenn man alle anderen Geräte einschalten will. Durch eine Abfrage im shutdown-Skript kann man das vermeiden (siehe das Beispielskript vdrshutdown).  

Kopiere den systemd service 'irmpexec.service' in das services Verzeichnis (/etc/systemd/system/) und führe einmal "systemctl enable irmpexec.service" aus.

irmp_stm32.map must contain a line "ff0000000000 KEY_REFRESH"

stm32kbd2uinput schreibt direkt in /var/log/started_by_IRMP_STM32.

## Mit irmpexec Kommandos ausführen
Zum Beispiel eine irmplircd.map Zeile "070000000100 KEY_1" und eine  
irmpexec.map Zeile "KEY_1 ls -l" führt "ls -l" aus, wenn 070000000100 kommt.
