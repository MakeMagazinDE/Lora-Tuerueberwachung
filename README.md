![GitHub Logo](http://www.heise.de/make/icons/make_logo.png)

Maker Media GmbH

***

# Türüberwachung mit LoRa

### Alarm- und Überwachungskontakt mit Batteriebetrieb und drahtloser Übertragung

![Picture](https://github.com/MakeMagazinDE/Lora-Tuerueberwachung/blob/master/aufm_gh_k.JPG) 

Sie suchen eine Möglichkeit, Türen und Tore netzunabhängig zu überwachen? Sie möchten informiert werden, wenn die Tür zu einer bestimmten Zeit nicht den erwarteten Zustand hat? Dann könnte sie dieses kleine Projekt interessieren. 

Den vollständigen Artikel mit der Bauanleitung gibt es in der [Make-Ausgabe 4/21 ab Seite 104](https://www.heise.de/select/make/2021/4).

**Nachtrag:** Das System läuft mittlerweile seit gut 18 Monaten problemlos und zuverlässig. Alle Hühner sind lebendig und glücklich. Allerdings zeigen sich Probleme beim OLED-Display der Empfangseinheit. Da das Display permanent eingeschaltet ist, geht die Leuchtstärke der einzelnen Pixel teilweise sehr stark zurück. Wie bei OLED-Displays üblich sich die Pixel am stärksten betroffen, die am häufigsten aktiv eingeschaltet waren.

In der Softwareversion V1.2 ist die Anzeige ist jetzt nur noch aktiv:
*	1 Minute nach dem Start der Empfangseinheit
*	1 Minute nachdem der Taster SW1 gedrückt wurde
*	1 Minute nachdem die Erinnerung aktiviert wurde, dass die Tür des Hühnerstalles noch geöffnet ist

Hierfür ist ein Taster bei der Empfangeinheit nachzurüsten (siehe Schaltung_Taster_neu.png), der die Anzeige aktiviert.

### Material Sendemodul:
* STM32L031 Nucleo-32 Evalboard
* AI-Thinker Ra-02 -Modulmit SX1278
* Antenne
* Antennenkabel
* Kupferlackdraht 0,3mm
* Lochrasterplatine
* Reedschalter
* Gehäuse
* Ein-/Ausschalter
* Abstandsbolzen
* Kabel zum Reed-Kontakt
* Kunststoffschrauben
* Buchsenleiste
* Batteriehalterung 2xAA
* Schrauben zur Montage (je nach Material auf dem das Modul montiert wird)
* Silikon oder 2-Komponentenkleber zum Abdichten

### Material Empfangsmodul:
* ESP32-S WiFi Bluetooth Development Board 0,96 OLED
* AI-Thinker Ra-02-Modul mit SX1278
* Antenne
* Antennenkabel
* Seeed-Studio-Modul Grove-Speaker
* USB-Netzteil
* Gehäuse
* Abstandsbolzen
* Kunststoffschrauben
* Buchsenleiste
* Lochrasterplatine
* Kupferlackdraht 0,2mm zur Verdrahtung

Das Software-Projekt für den Sender befindet sich im Verzeichnis ChickenDoor, das für den Empfänger im Verzeichnis ChickenHouse.

Das PDF ChickenHouseWoNucleo.pdf enthält einen Schaltplan ohne Nucleo-Board, falls der Mikrocontroller direkt eingesetzt werden soll (optional).
