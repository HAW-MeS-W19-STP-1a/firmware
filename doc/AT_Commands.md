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
| `<cpu>` | Temperaturmesswert vom On-Die Temperatursensor in 0.01 °C       |
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
| Eingabe      | Ausgabe                          |
|--------------|----------------------------------|
| `AT+CWIND=?` | `+CWIND: <0-13>,<0-999>`<br>`OK` |

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
| Eingabe        | Ausgabe                                                    |
|----------------|------------------------------------------------------------|
| `AT+CGNSPOS=?` | `+CGNSPOS: <-90.0000-90.0000>,<-180.0000-180.0000>,<0-9999.9>`<br>`OK` |

### Read Command
| Eingabe       | Ausgabe                               |
|---------------|---------------------------------------|
| `AT+CGNSPOS?` | `+CGNSPOS: <lat>,<lon>,<alt>`<br>`OK` |

### Write Command
| Eingabe                          | Ausgabe |
|----------------------------------|---------|
| `AT+CGNSPOS=<lat>,<lon>[,<alt>]` | `OK`    |

### Parameter
| Name    | Beschreibung                                                   |
|---------|----------------------------------------------------------------|
| `<lat>` | Breitengrad (Latitude) in 0.0001 °. S mit negativem Vorzeichen |
| `<lon>` | Längengrad (Longitude) in 0.0001 °. W mit negativem Vorzeichen |
| `<alt>` | Höhe über MSL in 0.1 m.                                        |