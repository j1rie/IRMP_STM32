# IRMP auf STM32 - ein USB-IR-Empfänger/Sender/Einschalter mit Wake-up Timer

Ein Fernbedienungsempfänger mit vielen Funktionen basierend auf billiger Hardware.

![Stick](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/Stick.jpg)
 
![Stick am Board](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/Stick_am_Board.jpg)

## Einleitung
Für 2 € kann man bei ebay einen ST-Link Emulator oder ein STM32F103-Entwicklungsboard, meist aus chinesischer Produktion, kaufen. Auf den STM32F103 Mikrocontroller darauf wird eine Open-Source-Firmware mit vielen Funktionen geflasht.

## Merkmale
* Anschluss über USB 
* registriert sich als HID-Gerät, erscheint als /dev/hidraw oder „Custom HID device“, keine Treiber erforderlich
* überträgt die Daten über USB2 in Hardware mit Fullspeed 
* IR-Empfänger (ca. 40 Protokolle in Hardware dekodiert) 
* Einschalten des PCs per Fernbedienung von S3 (STR) und S4 (STD) über USB oder von S3, S4 und S5 (SoftOff) über den Einschalter  auf der Hauptplatine (+5V erforderlich, über USB oder vom Netzteil oder der Hauptplatine) 
* Einschalten des PCs über den eingebauten Timer von S3 (STR) und S4 (STD) über USB oder von S3, S4 und S5 (SoftOff) über den Einschalter auf der Hauptplatine (+5V erforderlich, über USB oder vom Netzteil oder der Hauptplatine) 
* IR-Sender (etwa 40 Protokolle) 
* die Konfiguration wird im emulierten Eeprom gespeichert
* Bootloader für einfaches Firmware Update
* Makro Unterstützung

Mit einem Makro kann eine Folge von IR-Befehlen über die Sendediode gesendet werden, wenn ein Trigger empfangen wird.
macroslot0 ist der Auslöser, macroslot1 ... macroslot7 sind die zu sendenden Befehle. 

## Software Linux
* GUI-Konfigurationswerkzeug stm32IRconfig_gui für die Konfiguration: Wakeup-Codes, Makros, Alarmzeit einstellen, abrufen und zurücksetzen, IR senden und den empfangenen IR-Code anzeigen. Es ist möglich, Wakeups und Makros per Fernbedienung zu programmieren. Erstellen, Testen und Bearbeiten der Übersetzungstabelle per Fernbedienung und Maus. Umfassende Debug-Meldungen. Firmware-Aktualisierung.
* Interaktives Kommandozeilenprogramm stm32IRconfig zur Konfiguration: Setzen, Abrufen und Zurücksetzen von Wakeups, Makros, Alarmzeit, Senden von IR und Anzeigen des empfangenen IR-Codes. Es ist möglich, Wakeups und Makros per Fernsteuerung zu programmieren.
* stm32IRalarm zum Setzen und Auslesen der Alarmzeit per Skript 
* stm32FWupgrade für das Firmware Upgrade per Kommandozeile
* irmplircd ist ein Daemon, der im Hintergrund als unabhängiger lirc-Server läuft und die IR-Codes/Ereignisse an die Anwendung weitergibt https://github.com/realglotzi/irmplircd
* irctl für die Konfiguration: https://github.com/olebowle/irctl
* Software verfügbar für Ubuntu, yaVDR, easyVDR, MLD.

## Software Windows
* GUI-Konfigurationsprogramm stm32IRconfig_gui (wie bei Linux)
* interaktives Kommandozeilenprogramm stm32IRconfig (wie bei Linux)
* stm32IRalarm zum Setzen und Auslesen der Alarmzeit per Skript
* stm32FWupgrade für das Firmware-Upgrade per Kommandozeile
* MediaPortal-Plugin: https://github.com/pikim/HIDIRT-host/tree/master/hidirt.MePo
* EventGhost mit generischer HID

## Erstellen einer Keymap
Das stm32IRconfig_gui kann verwendet werden, um eine Keymap mit der Fernsteuerung zu erstellen.  
Es gibt zwei Methoden.  
  
(1)  
![write_IR](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/write_IR.jpg)  
Erste Methode: Drücken Sie „receive mode“ und öffnen Sie die Vorlagen Keymap.
Klicken Sie auf die Zeile mit dem Namen der Taste, drücken Sie die Taste auf der Fernbedienung, drücken Sie „write IR“ - wiederholen Sie dies für alle Tasten.  
  
(2)  
![append](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/append.jpg)  
Zweite Methode: Drücken Sie „receive mode“.
Drücken Sie die Taste auf der Fernbedienung, geben Sie den Namen der Taste in das Textfeld Key ein und drücken Sie „append“ - wiederholen Sie diesen Vorgang für alle Tasten.

Sie können nun die Tastenbelegung testen: Nachdem Sie die Taste auf der Fernbedienung gedrückt haben, sollte der rote Cursor in der entsprechenden Zeile stehen.
Wenn Sie fertig sind, drücken Sie „save“, um die Tastenbelegung zu speichern.

## Wakeup anlernen
Wenn das erste Wakeup leer ist, werden die ersten empfangenen IR-Daten im Wakeup gespeichert.  
Wakeup mit stm32IRconfig_gui ändern: set by remote - wakeup drücken, Taste auf der Fernbedienung drücken.  
Wichtig zum Testen: Wakeup findet nur statt, wenn der PC ausgeschaltet ist.  
Wenn der PC eingeschaltet ist, wird nur die Taste an den PC gesendet (man kann also die gleiche Taste zum Ein- und Ausschalten verwenden).  

## EventGhost
![EG1](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/EG1.jpg)  

![EG2](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/EG2.jpg)  

![EG7](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/EG7.jpg)  

Ein Keycode im linken Ereignisfenster wird in das rechte Konfigurationsfenster gezogen und der gewünschten Aktion eines Plugins zugewiesen. 

## Aus den Quellen bauen
Die Quellen sind erst nach dem Ausführen des Prepare-Skripts vollständig, das Skript lädt die IRMP-Quellen herunter, extrahiert und patcht die Quellen. Die ST-Quellen müssen Sie manuell herunterladen, da Sie sich leider registrieren müssen.  
Dadurch ist es möglich, die Patches unter die GPL zu stellen, ohne in die Originallizenzen einzugreifen.  
Es ist auch einfacher zu sehen, was geändert wurde.  
Kompilieren mit arm-none-eabi-gcc.  

## Pin-Out
Siehe /STM32Fxxxxx/src/config.h.

## Flashen des Bootloaders mit einem ST-Link
Verbinden Sie  
Programmiergerät SWDIO -> IO  
Programmiergerät SWCLK -> CLK  
Programmiergerät GND -> G  
Programmiergerät 3V3 -> V3  
Windows:  
Holen Sie sich das STM32 ST-Link Utility.  
File - Open File - boot.blackDev.bin (https://github.com/j1rie/STM32F103-bootloader/tree/master/binaries)  
Target - Program & Verify (bestätigen Sie eine Meldung über den Ausleseschutz, falls sie erscheint)  
Linux:  
openocd -f ocd-program.cfg  
Wenn der Flash gesperrt ist, führe zuerst openocd -f ocd-unlock.cfg aus, trenne das Gerät ab und schließe es wieder an, und 
führen Sie openocd -f ocd-program.cfg erneut aus.  
Die Dateien befinden sich in den STM32Fxxx(yy)-bootloader Repos.  
https://github.com/j1rie/IRMP_STM32_KBD/wiki/Herstellungsprozess-Schwarze#bootloader-flashen  

## Flashen der Firmware
Trennen Sie die Verbindung zum USB.  
Linux: Start ./stm32FWupgrade ./2019-07-06_16-32_blackDev_BL_SC_jrie.bin  
Windows: Start stm32FWupgrade.exe 2020-02-15_00-29_blueDev_BL_SC_jrie.bin  
An den USB anschließen.  
Anstelle von stm32FWupgrade können Sie auch stm32IRconfig_gui verwenden.  
https://github.com/j1rie/IRMP_STM32_KBD/wiki/Herstellungsprozess-Schwarze#firmware-flashen  

## Andere Möglichkeiten zum Flashen
Flashen mit einem USB-serial-TTL ist auf den Entwicklerboards möglich.  
Flashen über USB ist auf den Boards möglich, die es unterstützen.  
Siehe https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_ein_USB_IR_Empf%C3%A4nger/Sender/Einschalter_mit_Wakeup-Timer#Firmware_Flashing  

## irmplircd, eventlircd, udev und systemd
Siehe /irmplircd/README.

## Dank an
Frank Meyer für IRMP. [1]  
Uwe Becker für die Anpassung von IRMP an den STM32F4xx und sein USB-HID für den STM32F4xx. [2]  
Seine Arbeit hat mir beim Einstieg in den STM32F105 geholfen.  
Andrew Kambaroff für sein USB-HID für den STM32F103. [3]  
Seine Arbeit half mir bei den ersten Schritten mit dem STM32F103.  
Ole Ernst für die Code-Überprüfung, das Linux-Makefile und das Linux-Download-Extract-Patch-Skript und das neue Protokoll. [4]  
Manuel Reimer für den gcc-4.9 Fix, SimpleCircuit und die Überprüfung, ob der Host läuft. [5]  

[1] https://www.mikrocontroller.net/articles/IRMP  
[2] http://mikrocontroller.bplaced.net/wordpress/?page_id=744  
[3] https://habr.com/post/208026/  
[4] https://github.com/olebowle  
[5] https://github.com/M-Reimer  

## Diskussion und weitere Informationen
Kommentare, Fragen und Anregungen können an https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/ gesendet werden.

## Bauanleitungen
https://github.com/j1rie/IRMP_STM32_KBD/wiki/Herstellungsprozess-Schwarze  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_Bauanleitung  
Die letzten beiden sind auch in /doc/old/Deutsch.  

Viel Spaß mit IRMP auf STM32!

Copyright (C) 2014-2024 Jörg Riechardt
