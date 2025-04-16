## IRMP auf RP2xxx

<img src="https://www.vdr-portal.de/index.php?attachment/48154-20230825-130009-jpg" width="33%"> [1]  
<img src="https://www.vdr-portal.de/index.php?attachment/49235-ir-sensor-1-jpg" width="100%">
<img src="https://www.vdr-portal.de/index.php?attachment/49236-ir-sensor-2-jpg" width="60%"> [2]  


Für Boards mit dem RP2xxx, z.B. den Raspberry Pi Pico, Pico2 und viele andere.  
Dies sind zusätzliche Informationen, grundlegende Informationen unter https://github.com/j1rie/IRMP_STM32#readme.

## Firmware flashen
USB Verbindung trennen.
Halten Sie die BOOTSEL-Taste gedrückt und steckenn Sie das Gerät an den USB Anschluss. Das Gerät wird als Massenspeicher angezeigt.
Ziehen Sie die Datei firmware.uf2 per Drag & Drop auf den Massenspeicher. Das Gerät wird neu gestartet und startet als IRMP HID Gerät.

Durch das Senden des Befehls „reboot“ wird das Gerät ebenfalls in den Massenspeichermodus versetzt.

Wenn sich bereits eine ältere Firmware auf dem Gerät befindet, versetzt stm32IRconfig - b das Gerät in den Massenspeichermodus, und
picotool load -v -x firmware.uf2 flasht die Firmware, verifiziert und startet sie.

## Erster Test
Drücken Sie im Suspend-Modus die BOOTSEL-Taste, und der PC sollte aufwachen.  
Für die Verkabelung siehe https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_Bauanleitung#Minimalistic_assembly_for_experienced_users  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung#Solder_Cables  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung#Connect_Cables

## Emuliertes Eeprom
Jede Konfiguration, die mit einem der Konfigurationsprogramme vorgenommen wird, wird zunächst nur im Cache gespeichert. Um
diese Änderungen dauerhaft im Flash zu speichern, muss ein Eeprom-Commit durchgeführt werden. Dies betrifft Makros und mehrere Wakeups.  
Ausnahme: Der erste wakeup wird aus Gründen der Abwärtskompatibilität von der Firmware übertragen.

## Aus den Quellen bauen
Siehe [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started)  
-> SDK und die Beispiele herunterladen  
-> Toolchain Installieren  
-> „Blink“ erstellen  
-> „Blink“ hochladen und ausführen  

## Pin-Out
Siehe ./src/config.h.

## 5 V vom Netzteil statt über USB
Wenn das Gerät vom Netzteil versorgt werden muss, darf es nicht zusätzlich über USB versorgt werden, um Querströme zu vermeiden.
Eine Möglichkeit ist es, die Leiterbahn auf dem PCB zu durchtrennen:  

![VBUS trennen](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/RP2040-One_VBUS_trennen.jpg)

## Signale von den LEDs

Der Pico(2) hat eine normale LED, der One und der Zero haben eine RGB LED, und der XIAO-RP2350 hat eine RGBW LED.
Zusätzlich kann eine externe LED oder RGB LED angeschlossen werden.
Diese zeigen an, was in der Firmware passiert.

| Empfänger             |  RGB-LED                                     |
|-----------------------|----------------------------------------------|
| stromlos              |  aus                                         |
| USB eingehängt        |  weiß (oder custom)                          |
| USB suspend           |  orange                                      |
| IR Empfang            |  flackert blau                               |
| Wakeup speichern      |  blinkt schnell rot                          |
| Wakeup                |  blinkt schnell rot                          |
| Reboot                |  blinkt schnell rot                          |
| IR senden             |  kurz gelb                                   |
| VDR läuft             |  rot(*)                                      |
| VDR nimmt auf         |  blinkt rot entsprechend Anzahl Aufnahmen(*) |
| Konfigurationsbefehl  |  kurz grün                                   |

(*) mit vdr-plugin-statusleds [3]

Dann gibt es noch die Status LED (über hidraw angesteuert), die den Status des vdr-plugin-statusleds anzeigt und beim Einschalten, beim Speichern des ersten Wakeups und bei Reboot blinkt.

## Externe RGB-LEDs
Es wird empfohlen, zwei Widerstände im Datenkabel zu verwenden, siehe http://stefanfrings.de/ws2812/.

##
 \
[1] Waveshare RP2040-One mit TSOP von clausmuus, siehe https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/&postID=1361220#post1361220  
[2] Waveshare RP2040-One mit TSOP von FireFly, siehe https://www.vdr-portal.de/forum/index.php?thread/132289-irmp-auf-stm32-ein-usb-hid-keyboard-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-t/&postID=1371419#post1371419  
[3] https://github.com/j1rie/vdr-plugin-statusleds  
