![GitHub Logo](http://www.heise.de/make/icons/make_logo.png)

Maker Media GmbH

***

# Türüberwachung mit LoRa

### 


![Picture](https://github.com/MakeMagazinDE/Lora-Tuerueberwachung/blob/master/aufm_gh_k.JPG) 

Sie suchen eine Möglichkeit, Türen und Tore netzunabhängig zu überwachen? Sie möchten informiert werden, wenn die Tür zu einer bestimmten Zeit nicht den erwarteten Zustand hat? Dann könnte sie dieses kleine Projekt interessieren. 

Den vollständigen Artikel mit der Bauanleitung gibt es in der [Make-Ausgabe 5/19 ab Seite 118](https://www.heise.de/select/make/2019/5/1571592996373573).

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
