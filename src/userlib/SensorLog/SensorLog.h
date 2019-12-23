#ifndef USERLIB_SENSORLOG_H_
#define USERLIB_SENSORLOG_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "stm8l15x.h"


/*- Symbolische Konstanten ---------------------------------------------------*/
#define NUM_SENSORLOG_RINGITEMS     8


/*- Typdefinitionen ----------------------------------------------------------*/
typedef struct tag_SensorLogItem {
  struct {
    RTC_DateTypeDef sDate;
    RTC_TimeTypeDef sTime;
  } sTimestamp;
  
  struct {
    int16_t iBME;
    int16_t iCPU;
    int16_t iQMC;
    int16_t iMPU;
  } sTemperature;
  
  uint32_t ulPressure;
  
  uint32_t ulHumidity;
  
  struct {
    uint16_t uiDir;
    uint16_t uiVelo;
  } sWind;
  
  struct {
    uint16_t uiAzimuth;
    int16_t iZenith;
  } sAlignment;
  
  struct {
    int32_t lLat;
    int32_t lLong;
    int16_t iAlt;
  } sPosition;
  
  struct {
    uint16_t uiBatVolt;
    uint16_t uiPanelVolt;
    int16_t iBatCurr;
    int16_t iPanelCurr;
  } sPower;
} SensorLogItem;


/*- Funktionsprototypen ------------------------------------------------------*/
void SensorLog_Init(void);
void SensorLog_Clear(void);

SensorLogItem* SensorLog_Advance(void);
SensorLogItem* SensorLog_Dump(unsigned uOffset);

#endif /* USERLIB_SENSORLOG_H_ */