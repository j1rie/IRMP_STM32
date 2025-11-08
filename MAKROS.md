## Makros
Ein Makro ermöglicht das Senden einer Befehlsfolge über die IR-Sendediode mit einem Tastendruck.
macroslot0 ist der Auslöser, macroslot1 … macroslot7 sind die Befehle, die bei Empfang des Auslösers ausgesendet werden.

Beispiel:  
In macro0-macroslot0 ist Taste A konfiguriert.  
In macro0-macroslot1 ist Taste B konfiguriert.  
In macro0-macroslot2 ist Taste C konfiguriert.  
In macro0-macroslot3 ist Taste D konfiguriert.  
In macro0-macroslot4 ist Taste E konfiguriert.  
Wenn nun A gedrückt und empfangen wird, werden nacheinander B, C, D und E über die IR-Sendediode ausgesendet.

Praktische Anwendung:  
Auf „Power“ soll der VDR angehen und der TV und der Receiver und …  
„Power“ kommt in wakeupslot0 und in macro0-macroslot0.  
„TV an“ kommt in macro0-macroslot1.  
„Receiver an“ kommt in macro0-macroslot2.  
…

## Makros mit stm32kbdIRconfig_gui eingeben
Mit "select" macro und macroslot auswählen. Der Auslöser kommt in macroslot0, z.B. mit der Fernbedienung. Die zu sendenden Befehle kommen in macroslot1 und folgende, auch z.B. mit der Fernbedienung.  
Wenn ein Auslöser ffffffffffff ist, wir das Senden beendet, und wenn ein zu sendender IR code ffffffffffff ist, wird beim nächsten Makro weiter gemacht.

## Technisch
In einem neuen Eeprom und einem Eeprom nach Reset sind alle Bytes '0xff'. Das liegt an den technischen Gründen, wie das Löschen von Flash funktioniert.  
'0xff' bedeutet also noch unbeschrieben.

Nach dem IR-Empfang wird der empfangene IR-Code nacheinander mit allen Makro-Triggern verglichen. Wenn ein Trigger 'ffffffffffffff' ist, wird das Parsen beendet.  
Wenn der empfangene IR-Code und der Auslöser übereinstimmen, werden die zugehörigen IR-Codes gesendet, aber wiederum bei 'ffffffffffff' wird das Senden beendet.  
ffffffffffffff' ist also die Stoppmarke sowohl für Trigger als auch für zu sendende IR-Codes.  
Dies ist praktisch, da die 'ffffffffffff's aufgrund der Eeprom-Eigenschaften bereits von Anfang an vorhanden sind.

## Spezialfall letztes Makro
Hier ist der Auslöser ein abgelaufener Alarm Timer.
Gesendet wird ab macro7-macroslot1.
