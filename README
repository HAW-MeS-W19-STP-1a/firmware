# STP_1a_Wetterstation Firmware
Hauptprogramm für die Recheneinheit der Wetterstation (Projekt 1a STP WiSe 2019/2020).

### Software
* ST Visual Develop Entwicklungsumgebung
* COSMIC C Compiler for STM8 (ST Special Edition)
* ST-Link V2 Programmer- und Debugger-Treiber
* PuTTy, hTerm o.Ä. Serial Terminal

### Hardware
* STM8L152R8 Nucleo64 Entwicklungsboard
* Serial-Over-Bluetooth Umsetzer: Generic HC-05
* GPS-Receiver: uBlox NEO-5 oder NEO-6
* Temperatur-, Luftdruck- und Luftfeuchtesensor: Bosch Sensortec BME280
* Magnetometer: QST Corp. QMC5883L
* Accelerometer und Gyroscope: InvenSense MPU6050
* Breakout-Board für einen microSD-Kartenslot
* n- und p-MOSFETs zur Schaltung von +5V und für Level-Shifting
* LEDs für Benutzeranzeige

## Firmwareabbild erzeugen
1. ST Visual Develop starten und Workspace-Datei `STP_1a_Wetterstation.stp` über `File -> Open Workspace` laden
2. Compiler-Installationspfad unter `Project -> Settings -> Toolset Info -> STM8 Cosmic` anpassen
3. Build- und Programmiervorgang über `Debug -> Start Debugging`
4. Nach erfolgreichem Build Programmausführung über `Debug -> Continue (F5)` starten

## Bedienung
Nach dem Erstanlauf wartet das Hauptprogramm darauf, dass der blaue Taster auf dem Nucleo-Board betätigt wird, ehe die Wetterstation aktiviert wird.

### Bluetooth
Das Bluetooth-Modul wird zeitweise deaktiviert, da die Stromaufnahme im Standby-Betrieb bei bis zu 80mA liegen kann. Es wird in regelmäßigen Zeitabständen (siehe Wakeup-Task) für einige Sekunden aktiviert. Wenn eine Verbindung zu einem Rechner besteht, bleibt das Modul eingeschaltet.

Die blaue LED (an Pin B7) zeigt den Zustand des Bluetooth-Moduls an:
* ausgeschaltet: Bluetooth-Modul deaktiviert
* 2x kurz blinkend: Bluetooth-Modul aktiv, aber nicht verbunden
* leuchtend: Bluetooth-Modul aktiv und Verbindung aktiv

**Verbindungsparameter:**
* Baudrate: 9600 (ohne Bedeutung, da es sich um einen VCP handelt)
* Parität: keine
* Stoppbits: 1
* Datenbits: 8

**Benutzer-Interface:**
Über die Bluetooth-Verbindung kann auf ein textbasiertes Interface zugegriffen werden. Es werden derzeit Einzelzeichen-Befehle ausgewertet. Eine Übersicht mit unterstützten Befehlen wird bei Eingabe von `?` ausgegeben:
```
HELP__:
        t = Temperature
        w = Wind
        p = Pressure
        h = Humidity
        g = GPS data
        d = Date/Time from RTC
        z = Sun Zenith/Azimuth
        s = SR/SS
        r = Reset
```

### Debug-Schnittstelle
Zusätzliche Debug-Meldungen werden über die serielle Schnittstelle des ST-Link Programmers ausgegeben. Eine Eingabe von Daten über die Debug-Schnittstelle ist nicht vorgesehen.

**Verbindungsparameter:**
* Baudrate: 1000000 (1 MBaud)
* Parität: keine
* Stoppbits: 1
* Datenbits: 8