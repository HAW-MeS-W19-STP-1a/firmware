# AT Command Reference
Befehlsreferenz für die Bluetooth-Kommunikation mit der Wetterstation. **Zeilenenden werden mit `\r\n` signalisiert.** Gültige Befehle werden am Ende mit `OK\r\n` durch den Controller quittiert. Bei Fehlern wird `ERROR\r\n` ausgegeben.

## Befehlsübersicht
1. [`AT+CTEMP` Temperatur](#atctemp-temperatur)
2. [`AT+CPRES` Luftdruck](#atcpres-luftdruck)
3. [`AT+CHUM` Luftfeuchtigkeit](#atchum-luftfeuchtigkeit)
4. [`AT+CTIME` Echtzeituhr](#atctime-echtzeituhr)
5. [`AT+CWIND` Wind](#atcwind-wind)
6. [`AT+CALIGN` Ausrichtung des Solarpanels](#atcalign-ausrichtung-des-solarpanels)
7. [`AT+CGNSPOS` GPS Position](#atcgnspos-gps-position)
8. [`AT+CPWR` Leistungswerte](#atcpwr-leistungswerte)
9. [`AT+CINTV` Messintervall](#atcintv-messintervall)
10. [`AT+CGUI` Datensatz für UI ausgeben](#atcgui-datensatz-fur-ui-ausgeben)
11. [`AT+CWKUP` Wakeup Task](#atcwkup-wakeup-task)

## `AT+CTEMP` Temperatur
* Read-only

### Test Command
| Eingabe      | Ausgabe |
|--------------|---------|
| `AT+CTEMP=?` | `OK`    |

### Read Command
| Eingabe     | Ausgabe                                   |
|-------------|-------------------------------------------|
| `AT+CTEMP?` | `+CTEMP: <bme>,<cpu>,<qmc>,<mpu>`<br>`OK` |

### Parameter
| Name    | Beschreibung                                                    |
|---------|-----------------------------------------------------------------|
| `<bme>` | Temperaturmesswert vom BME280 in 0.01 °C. Bevorzugter Messwert. |
| `<cpu>` | Temperaturmesswert vom On-Die Temperatursensor in 1 °C          |
| `<qmc>` | Temperaturmesswert vom QMC5883 in 0.01 °C                       |
| `<mpu>` | Temperaturmesswert vom MPU6050 in 0.01 °C. Unzuverlässig        |

## `AT+CPRES` Luftdruck
* Read-only

### Test Command
| Eingabe      | Ausgabe |
|--------------|---------|
| `AT+CPRES=?` | `OK`    |

### Read Command
| Eingabe     | Ausgabe                 |
|-------------|-------------------------|
| `AT+CPRES?` | `+CPRES: <bme>`<br>`OK` |

### Parameter
| Name    | Beschreibung                                    |
|---------|-------------------------------------------------|
| `<bme>` | Luftdruckmesswert vom BME280 in 1 Pa (0.01 hPa) |

## `AT+CHUM` Luftfeuchtigkeit
* Read-only

### Test Command
| Eingabe     | Ausgabe |
|-------------|---------|
| `AT+CHUM=?` | `OK`    |

### Read Command
| Eingabe    | Ausgabe                |
|------------|------------------------|
| `AT+CHUM?` | `+CHUM: <bme>`<br>`OK` |

### Parameter
| Name    | Beschreibung                                   |
|---------|------------------------------------------------|
| `<bme>` | Luftfeuchtigkeitsmesswert vom BME280 in 0.01 % |

## `AT+CWIND` Wind
* Read-only

### Test Command
| Eingabe      | Ausgabe |
|--------------|---------|
| `AT+CWIND=?` | `OK`    |

### Read Command
| Eingabe     | Ausgabe                       |
|-------------|-------------------------------|
| `AT+CWIND?` | `+CWIND: <dir>,<spd>`<br>`OK` |

### Parameter
| Name    | Beschreibung                 |
|---------|------------------------------|
| `<dir>` | Windrichtung in 0.01 °       |
| `<spd>` | Windgeschwindigkeit in 1 m/s |

## `AT+CTIME` Echtzeituhr

### Test Command
| Eingabe      | Ausgabe                                         |
|--------------|-------------------------------------------------|
| `AT+CTIME=?` | `+CTIME: 0-99,0-12,0-31,0-23,0-59,0-59`<br>`OK` |

### Read Command
| Eingabe     | Ausgabe                                         |
|-------------|-------------------------------------------------|
| `AT+CTIME?` | `+CTIME: <YY>,<MM>,<DD>,<hh>,<mm>,<ss>`<br>`OK` |

### Write Command
Legt das aktuelle Datum und die Uhrzeit in UTC fest.

| Eingabe                                  | Ausgabe |
|------------------------------------------|---------|
| `AT+CTIME=<YY>,<MM>,<DD>,<hh>,<mm>,<ss>` | `OK`    |

### Parameter
| Name   | Beschreibung              |
|--------|---------------------------|
| `<YY>` | Jahr `0` bis `99`         |
| `<MM>` | Monat `1` bis `12`        |
| `<DD>` | Tag `1` bis `31`          |
| `<hh>` | Stunde `0` bis `23` (UTC) |
| `<mm>` | Minute `0` bis `59`       |
| `<ss>` | Sekunde `0` bis `59`      |

## `AT+CALIGN` Ausrichtung des Solarpanels
* Read-only

### Test Command
| Eingabe       | Ausgabe |
|---------------|---------|
| `AT+CALIGN=?` | `OK`    |

### Read Command
| Eingabe      | Ausgabe                        |
|--------------|--------------------------------|
| `AT+CALIGN?` | `+CALIGN: <azm>,<zen>`<br>`OK` |

### Parameter
| Name    | Beschreibung                                      |
|---------|---------------------------------------------------|
| `<azm>` | Azimut (Kompassrichtung missweisend) in 0.01 °    |
| `<zen>` | Zenit in 0.01 °                                   |

## `AT+CGNSPOS` GPS Position

### Test Command
| Eingabe        | Ausgabe                                        |
|----------------|------------------------------------------------|
| `AT+CGNSPOS=?` | `+CGNSPOS: <-90.0000-90.0000>,<-180.0000-180.0000>,<-3276.8-3276.7>`<br>`OK` |

### Read Command
| Eingabe       | Ausgabe                               |
|---------------|---------------------------------------|
| `AT+CGNSPOS?` | `+CGNSPOS: <lat>,<lon>,<alt>`<br>`OK` |

### Write Command
Überschreibt die GPS-Position für Demo-Anwendungen.

| Eingabe                          | Ausgabe |
|----------------------------------|---------|
| `AT+CGNSPOS=<lat>,<lon>[,<alt>]` | `OK`    |

### Parameter
| Name    | Beschreibung                                                   |
|---------|----------------------------------------------------------------|
| `<lat>` | Breitengrad (Latitude) in 0.0001 °. S mit negativem Vorzeichen |
| `<lon>` | Längengrad (Longitude) in 0.0001 °. W mit negativem Vorzeichen |
| `<alt>` | Höhe über MSL in 0.1 m.                                        |

## `AT+CPWR` Leistungswerte
* Read-only

### Test Command
| Eingabe     | Ausgabe |
|-------------|---------|
| `AT+CPWR=?` | `OK`    |

### Read Command
| Eingabe    | Ausgabe                                                      |
|------------|--------------------------------------------------------------|
| `AT+CPWR?` | `+CPWR: <v_bat>,<i_bat>,<v_solar>,<i_solar>,<v_sys>`<br>`OK` |

### Parameter
| Name        | Beschreibung                      |
|-------------|-----------------------------------|
| `<v_bat>`   | Spannung an der Bleizelle in 1 mV |
| `<i_bat>`   | Lade-/Entladestrom in 1 mA        |
| `<v_solar>` | Solarzellenspannung in 1 mV       |
| `<i_solar>` | Strom von Solarzelle in 1 mA      |
| `<v_sys>`   | System-Versorgungsspannung in mV  |

## `AT+CINTV` Messintervall
* Write-only

### Test Command
| Eingabe      | Ausgabe                   |
|--------------|---------------------------|
| `AT+CINTV=?` | `+CINTV: 10-3600`<br>`OK` |

### Write Command
Setzt den Wakeup-Timer auf die angegebene Dauer zurück.

| Eingabe          | Ausgabe |
|------------------|---------|
| `AT+CINTV=<int>` | `OK`    |

### Parameter
| Name    | Beschreibung         |
|---------|----------------------|
| `<int>` | Messintervall in 1 s |

## `AT+CGUI` Datensatz für UI ausgeben
* Read-only

### Test Command
| Eingabe     | Ausgabe |
|-------------|---------|
| `AT+CGUI=?` | `OK`    |

### Read Command
Gibt alle Datensätze seit dem letzten Aufruf von `AT+CGUI` aus.

| Eingabe    | Ausgabe                                                        |
|------------|----------------------------------------------------------------|
| `AT+CGUI?` | `+CGUI: <yy>,<MM>,<dd>,<hh>,<mm>,<ss>,<t_bme>,<t_cpu>,<t_qmc>,<t_mpu>,<w_dir>,<w_spd>,<pres>,<hum>,<zen>,<azm>,<lat>,<lon>,<alt>,<v_bat>,<i_bat>,<v_solar>,<i_solar>,<v_sys>`<br>`+CGUI: ...`<br>`OK` |

## `AT+CWKUP` Wakeup Task

### Test Command
| Eingabe      | Ausgabe |
|--------------|---------|
| `AT+CWKUP=?` | `OK`    |

### Execute Command
Messwerte werden neu eingelesen und das Panel neu ausgerichtet.

| Eingabe    | Ausgabe |
|------------|---------|
| `AT+CWKUP` | `OK`    |
