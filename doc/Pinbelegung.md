# Pin-Beschreibungen
In diesem Dokument wird für die in [Pinbeschreibung.pdf](Pinbeschreibung.pdf) aufgezeichneten Pins die Funktion kurz beschrieben.

## Analogeingänge
* `ADC1_UPV_IN0`: Solarzellenspannung
* `ADC1_IBAT_IN1`: Ladestrom an der Batterie
* `ADC1_IPV_IN2`: Strom von der Solarzelle
* `ADC1_UBAT_IN3`: Batteriespannung
* `ADC1_TBAT_IN7`: Batterietemperatur über einen NTC-Spannungsteiler
* `ADC1_WV_IN16`: Windrichtung als Analogwert an der Windfahne
* `SYS_PVD_IN`: 3.3V Versorgungsspannung (Überwachung)

## Digitale I/O
* `GPIO_SD_DET`: Erkennungsschalter für SD-Kartenslot
* `GPIO_MOT1_A`: Motor 1 Drehrichtungs-Signal A
* `GPIO_MOT1_B`: Motor 1 Drehrichtungs-Signal B
* `GPIO_MOT2_A`: Motor 2 Drehrichtungs-Signal A
* `GPIO_MOT2_B`: Motor 2 Drehrichtungs-Signal B
* `GPIO_BTN1_BLUE`: Eingang für blauen Taster am Nucleo-Board
* `BT_STATUS`: Statussignal vom Bluetooth-Modul
* `BT_CMD`: Auswahl Command-/Communication-Mode am Bluetooth-Modul
* `BT_PWREN`: Aktivierung der 5V-Spannungsversorgung zum Bluetooth-Modul
* `GPS_PWREN`: Aktivierung der 5V-Spannungsversorgung zum GPS-Modul
* `MOT_PWREN`: Aktivierung der 12V-Spannungsversorgung zum Motortreiber
* `LED_BT`: Bluetooth Status-LED
* `LED_SYS`: Systemstatus-LED
* `TIM3_ANEM_CH1`: Timer 3 Zählereingang für Anemometersignal
* `SYS_SWIM`: ST-Link Debugger- und Programmierschnittstelle

## Kommunikationsschnittstellen
* `USART1_...`: Serial-Over-Bluetooth Schnittstelle
* `USART2_...`: Debug-Schnittstelle
* `USART3_...`: GPS NMEA-out / Command-In Schnittstelle
* `I2C1_...`: I²C-Bus für Sensoren
* `SPI2_...`: SPI-Schnittstelle für SD-Karte