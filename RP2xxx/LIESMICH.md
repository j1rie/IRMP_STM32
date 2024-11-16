## IRMP auf RP2xxx

<img src="https://www.vdr-portal.de/index.php?attachment/48154-20230825-130009-jpg" width="33%"> [1]  
[Empfänger im Thin Client](https://www.vdr-portal.de/index.php?attachment/49235-ir-sensor-1-jpg) [2] 
[Empfänger im Thin Client](https://www.vdr-portal.de/index.php?attachment/49236-ir-sensor-2-jpg) [2]

Für Boards mit dem RP2xxx, z.B. den Raspberry Pi Pico, Pico2 und viele andere. 
Dies sind zusätzliche Informationen, grundlegende Informationen unter https://github.com/j1rie/IRMP_STM32#readme.

## Flashen der Firmware
Trennen Sie die Verbindung zum USB.
Schließen Sie das Gerät an USB an, während Sie die BOOTSEL-Taste gedrückt halten, und lassen Sie sie dann los. Das Gerät wird als Massenspeicher angezeigt.
Ziehen Sie die Datei firmware.uf2 per Drag & Drop auf den Speicher. Das Gerät wird neu gestartet und startet als IRMP HID Gerät.

Durch das Senden des Befehls „reboot“ wird das Gerät ebenfalls in den Massenspeichermodus versetzt.

## Erster Test
Drücken Sie die BOOTSEL-Taste im Suspend-Modus, und der PC sollte aufwachen. 
Für die Verkabelung siehe https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_Bauanleitung#Minimalistic_assembly_for_experienced_users 
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung#Solder_Cables 
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung#Connect_Cables

## Emuliertes Eeprom
Jede Konfiguration, die mit einem der Konfigurationsprogramme vorgenommen wird, wird zunächst nur im Cache gespeichert. Um 
diese Änderungen dauerhaft im Flash zu speichern, müssen Sie einen Eeprom-Commit durchführen.
Ausnahme: Das erste Aufwachen wird aus Gründen der Abwärtskompatibilität von der Firmware übertragen.

## Aus den Quellen bauen
Siehe [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started) 
-> Holen Sie sich das SDK und die Beispiele 
-> Installieren Sie die Toolchain 
-> Erstelle „Blink“ 
-> Laden und Ausführen von „Blink“ 

## Pin-out
Siehe ./src/config.h.

##
\
[1] Waveshare RP2040-One mit TSOP von clausmuus, siehe https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/&postID=1361220#post1361220 
[2] Waveshare RP2040-One mit TSOP von FireFly, siehe https://www.vdr-portal.de/forum/index.php?thread/132289-irmp-auf-stm32-ein-usb-hid-keyboard-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-t/&postID=1371419#post1371419