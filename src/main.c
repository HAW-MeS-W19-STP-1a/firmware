/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "commlib.h"
#include "SolarTracking.h"
#include "BlinkSequencer.h"
#include "BTHandler.h"
#include "ATCmd.h"
#include "GPSHandler.h"
#include "SensorLog.h"
#include "sensorlib.h"
#include "motorlib.h"
#include "diskio.h"
#include "ff.h"
#include "io_map.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*- Globale Variablen --------------------------------------------------------*/
bool bDir;

/*! Timer-Zähler für 1s-Task                                                  */
volatile int iTimer1s = 10;

/*! Statusflag für Ausführung des 1s-Task                                     */
volatile bool bTask1sFlag = true;

/*! Statusflag für Ausführung des Wakeup-Task                                 */
volatile bool bTaskWakeupFlag = true;

/*! Konfigurationsdaten für die interne Echtzeituhr                           */
RTC_InitTypeDef sRtcInit;

/*! Datumsangabe für/aus interner Echtzeituhr                                 */
RTC_DateTypeDef sDate;

/*! Zeitangabe für/aus interner Echtzeituhr                                   */
RTC_TimeTypeDef sTime;

/*! Sensordaten vom BME280 Klimasensor                                        */
BME280_Sensor sSensorBME280;

/*! Sensordaten von der Windfahne und vom Anemometer                          */
Wind_Sensor sSensorWind;

/*! Sensordaten vom CPU-Temperatursensor                                      */
CPUTemp_Sensor sSensorCPUTemp;

/*! Sensordaten vom QMC5883 Magnetometer                                      */
QMC5883_Sensor sSensorQMC5883;

/*! Sensordaten vom MPU6050 Accelerometer / Gyro                              */
MPU6050_Sensor sSensorMPU6050;

/*! Handle für Dateisystem                                                    */
FATFS fs;


/*- Funktionsprototypen ------------------------------------------------------*/
void SaveSensors(void);


/*!****************************************************************************
 * @brief
 * Hauptprogramm
 *
 * @date 15.10.2019
 ******************************************************************************/
void main(void)
{  
  /* 16 MHz System Clock                                  */
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  
  /* External 32.768 kHz Clock for RTC                    */
  CLK_LSEConfig(CLK_LSE_ON);
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1);
  
  /* Timer 2 für 100ms-Task Trigger                       */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  TIM2_InternalClockConfig();
  TIM2_TimeBaseInit(TIM2_Prescaler_128, TIM2_CounterMode_Down, 12500);
  TIM2_ITConfig(TIM2_IT_Update, ENABLE);
  TIM2_Cmd(ENABLE);
  
  /* GPIO Button                                          */
  GPIO_Init(BTN_BLUE_PORT, BTN_BLUE_PIN, GPIO_Mode_In_FL_No_IT);
   
  /* Blink Module init                                    */
  Blink_Init();
  Blink_SetPattern(Blink_Led_SYS, 0x5555);
  
  /* ADC on PB2: Wind Vane                                */
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
  ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_1);
  ADC_Cmd(ADC1, ENABLE);
  
  /* Timer 5 as PWM output                                */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, ENABLE);
  TIM5_TimeBaseInit(TIM5_Prescaler_128, TIM5_CounterMode_Up, 1250);
  TIM5_OC1Init(TIM5_OCMode_PWM1, TIM5_OutputState_Enable, 3000, TIM5_OCPolarity_High, TIM5_OCIdleState_Reset);
  TIM5_CtrlPWMOutputs(ENABLE);
  TIM5_Cmd(ENABLE);
  
  /* Debug UART TX only                                   */
  GPIO_Init(USART2_PORT, USART2_TX_PIN, GPIO_Mode_Out_PP_High_Fast);
  UART2_Init();
  
  /* Initialise Pins for I2C Master Mode                  */
  GPIO_Init(I2C1_PORT, I2C1_SCL_PIN, GPIO_Mode_Out_OD_HiZ_Fast); // SDA
  GPIO_Init(I2C1_PORT, I2C1_SDA_PIN, GPIO_Mode_Out_OD_HiZ_Fast);
  
  /* Bluetooth UART TX and RX                             */
  BTHandler_Init();
  ATCmd_Init();
  
  /* NMEA Input, Binary CMD output                        */
  GPSHandler_Init();
  
  /* Messwerteprotokoll initialisieren                    */
  SensorLog_Init();
  
  /* Motor controller                                     */
  Motor_Init();
  Motor_Cmd(false);
  
  /* RTC init                                             */
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
  RTC_StructInit(&sRtcInit);
  sRtcInit.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&sRtcInit);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_SetWakeUpCounter(10);
  RTC_WakeUpCmd(ENABLE);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  
  /* SPI und SD-Karte                                     */
  GPIO_Init(SPI2_PORT, SPI2_SCK_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(SPI2_PORT, SPI2_MOSI_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(SPI2_PORT, SPI2_MISO_PIN, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(SD_CS_PORT, SD_CS_PIN, GPIO_Mode_Out_PP_High_Fast);
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI2, ENABLE);
  SPI_Init(SPI2, 
    SPI_FirstBit_MSB, 
    SPI_BaudRatePrescaler_64, 
    SPI_Mode_Master, 
    SPI_CPOL_Low,
    SPI_CPHA_1Edge, 
    SPI_Direction_2Lines_FullDuplex,
    SPI_NSS_Soft,
    0
  );
  SPI_Cmd(SPI2, ENABLE);
  
  /* RTC initialisieren                                   */
  #ifdef STATIC_INIT_RTC
  RTC_TimeStructInit(&sTime);
  sTime.RTC_Hours = 8;
  sTime.RTC_Minutes = 0;
  sTime.RTC_Seconds = 0;
  RTC_SetTime(RTC_Format_BIN, &sTime);
  RTC_DateStructInit(&sDate);
  sDate.RTC_Date = 26;
  sDate.RTC_Month = 11;
  sDate.RTC_Year = 19;
  RTC_SetDate(RTC_Format_BIN, &sDate);
  #endif /* STATIC_INIT_RTC */

  #ifdef MOTORLIB_DEMO
  Motor_Cmd(true);
  Motor_SetTurnRef(0);
  Motor_SetTurn(450);
  Motor_SetTilt(900);
  bDir = true;
  #endif /* MOTORLIB_DEMO */
  
  /* Sun Tracking                                         */
  Tracking_Init();
   
  /* Enable interrupt execution                           */
  enableInterrupts();
  printf("Program started\r\n");
  
  I2CMaster_Init();
  printf("BME280 init...");
  BME280_Init(&sSensorBME280, 0x76);
  printf(" OK\r\nQMC5883 init...");
  QMC5883_Init(&sSensorQMC5883, 0x0D);
  QMC5883_SetRefTemp(&sSensorQMC5883, 3400);
  QMC5883_Update(&sSensorQMC5883);
  printf(" OK\r\nMPU6050 init...");
  MPU6050_Init(&sSensorMPU6050, 0x68, false);
  printf(" OK\r\nWind Timer + ADC init...");
  Wind_Init(&sSensorWind, 1000); 
  printf(" OK\r\nCPU Temp init...");
  CPUTemp_Init(&sSensorCPUTemp);
  printf(" OK\r\nMotor init...");
  I2CMaster_DeInit();
  Motor_SetTurnRef(sSensorQMC5883.sMeasure.uiAzimuth);
  Motor_SetTurn(sSensorQMC5883.sMeasure.uiAzimuth);
  printf(" OK\r\n");
  
  printf("SD-Card init...");
  if (f_mount(&fs, "", 0) == FR_OK)
  {
    printf(" OK\r\n");
  }
  else
  {
    printf(" FAIL\r\n");
  }
  
  Blink_SetPattern(Blink_Led_SYS, 0x0001);
  
  #ifdef FATFS_DEMO
  printf("SD-Card init...");
  {
    DIR dir;
    FILINFO fno;
    FIL fil;
    char rc;
    unsigned bw;
    
    if (f_mount(&fs, "", 0) == FR_OK)
    {
      printf(" OK\r\n  f_open...");
      
      if (f_open(&fil, "TEST.CSV", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK)
      {
        printf(" OK\r\n  f_write...");
        f_write(&fil, "Hello World!\r\n", 14, &bw);
        if ((f_close(&fil) == FR_OK) && (bw == 14))
        {
          printf(" OK\r\n");
        }
        else
        {
          printf(" FAIL\r\n");
        }
      }
      else
      {
        printf(" FAIL\r\n");
      }
    }
    else
    {
      printf(" FAIL\r\n");
    }
  }
  #endif /* FATFS_DEMO */
  
  while (1)
  {
    if (bTask1sFlag)
    {
      bTask1sFlag = false;
      printf("Task1s\r\n");
      
      /* Wind-Mittelwert und -Böen auswerten              */
      Wind_UpdateSpd(&sSensorWind);
      
      /* Winkel aktualisieren                             */
      I2CMaster_Init();
      MPU6050_Update(&sSensorMPU6050);
      QMC5883_Update(&sSensorQMC5883);
      I2CMaster_DeInit();
      
      /* Tracking                                         */
      Tracking_Task1s();
      printf("Align: %d, %d\r\n", sSensorQMC5883.sMeasure.uiAzimuth, sSensorMPU6050.sMeasure.sAngle.iXZ);
      
      /* Blauen Taster für Bluetooth-Weckfunktion         */
      if (!GPIO_ReadInputDataBit(BTN_BLUE_PORT, BTN_BLUE_PIN))
      {
        BTHandler_TakeWakeup();
      }
      BTHandler_Task1s();
      
      /* Blink Pattern für 1s-Task                        */
      if (Blink_Ready(Blink_Led_SYS))
      {
        Blink_SetPattern(Blink_Led_SYS, 0x0001);
      }
    }
    
    if (bTaskWakeupFlag)
    {      
      bTaskWakeupFlag = false;
      printf("TaskWakeup\r\n");
      Blink_SetPattern(Blink_Led_SYS, 0x005F);
      
      #ifdef MOTORLIB_DEMO
      if (Motor_IsTurnReached())
      {
        Motor_SetTurn(bDir ? 0 : 450);
        if (Motor_IsTiltReached())
        {
          Motor_SetTilt(bDir ? 0 : 900);
          bDir = !bDir;
        }
      }
      #endif /* MOTORLIB_DEMO */
      
      /* Sensormessswerte abrufen                         */
      I2CMaster_Init();
      BME280_Update(&sSensorBME280);   
      QMC5883_Update(&sSensorQMC5883);
      MPU6050_Update(&sSensorMPU6050);
      Wind_UpdateDir(&sSensorWind);
      CPUTemp_Update(&sSensorCPUTemp);
      I2CMaster_DeInit();
      
      /* Bluetooth / GPS aufwecken                        */
      BTHandler_TakeWakeup();
      GPSHandler_TaskWakeup();
      
      /* Ausrichtung starten                              */
      Tracking_TaskWakeup();
      
      /* Sensordaten speichern                            */
      SaveSensors();
    }
    
    /* Bluetooth Verbindung und AT-Commands verarbeiten   */
    BTHandler_Poll();
    ATCmd_Poll();
    
    /* NMEA-Sentences vom GPS-Modul parsen                */
    if (GPSHandler_Poll())
    {
      if (sSensorGPS.sInfo.bTimeValid)
      {
        /* Aktualisierung der Echtzeituhr                 */
        RTC_TimeStructInit(&sTime);
        sTime.RTC_Hours = sSensorGPS.sTime.ucHour;
        sTime.RTC_Minutes = sSensorGPS.sTime.ucMin;
        sTime.RTC_Seconds = sSensorGPS.sTime.ucSec;
        RTC_SetTime(RTC_Format_BIN, &sTime);
        printf("GpsTime\r\n");
        sSensorGPS.sInfo.bTimeValid = false;
      }
      
      if (sSensorGPS.sInfo.bDateValid)
      {
        /* Aktualisierung des Datums                      */
        RTC_DateStructInit(&sDate);
        sDate.RTC_Date = sSensorGPS.sDate.ucDay;
        sDate.RTC_Month = sSensorGPS.sDate.ucMonth;
        sDate.RTC_Year = sSensorGPS.sDate.ucYear;
        RTC_SetDate(RTC_Format_BIN, &sDate);
        printf("GpsDate\r\n");
        sSensorGPS.sInfo.bDateValid = false;
      }
    }
        
    /* Fertig - auf nächsten Interrupt warten             */
    //wfi();
  }
}

void SaveSensors(void)
{
  SensorLogItem* pLog;
  uint16_t uiWindDir;
  FIL fil;
  
  /* Abs. Windrichtung über Azimuth bestimmen             */
  uiWindDir = 6300 + sSensorQMC5883.sMeasure.uiAzimuth - (sSensorWind.sMeasure.eDirection * 225);
  while (uiWindDir >= 3600) 
  {
    uiWindDir -= 3600;
  }
  
  /* Eintrag im Ringspeicher                              */
  pLog = SensorLog_Advance();
  RTC_GetDate(RTC_Format_BIN, &(pLog->sTimestamp.sDate));
  RTC_GetTime(RTC_Format_BIN, &(pLog->sTimestamp.sTime));
  pLog->sTemperature.iBME = sSensorBME280.sMeasure.iTemperature;
  pLog->sTemperature.iCPU = sSensorCPUTemp.sMeasure.cTemp * 100;
  pLog->sTemperature.iQMC = sSensorQMC5883.sMeasure.iTemperature;
  pLog->sTemperature.iMPU = sSensorMPU6050.sMeasure.iTemperature;
  pLog->ulPressure = sSensorBME280.sMeasure.ulPressure;
  pLog->ulHumidity = sSensorBME280.sMeasure.ulHumidity;
  pLog->sWind.uiDir = uiWindDir;
  pLog->sWind.uiVelo = sSensorWind.sMeasure.uiAvgVelocity;
  pLog->sAlignment.uiAzimuth = sSensorQMC5883.sMeasure.uiAzimuth;
  pLog->sAlignment.iZenith = sSensorMPU6050.sMeasure.sAngle.iYZ;
  pLog->sPosition.lLat = sSensorGPS.sPosition.lLat;
  pLog->sPosition.lLong = sSensorGPS.sPosition.lLong;
  pLog->sPosition.iAlt = sSensorGPS.sPosition.iAlt;
  pLog->sPower.uiBatVolt = 0;
  pLog->sPower.uiPanelVolt = 0;
  pLog->sPower.iBatCurr = 0;
  pLog->sPower.iPanelCurr = 0;
  
  /* Auf SD-Karte schreiben                               */
  printf("WriteLog...");
  if (f_open(&fil, "LOG.TXT", FA_WRITE | FA_OPEN_APPEND) == FR_OK)
  {
    printf(" OK\r\n");
    f_printf(&fil, "%04d-%02d-%02dT%02d:%02d:%02dZ,",
      (int)pLog->sTimestamp.sDate.RTC_Year + 2000,
      (int)pLog->sTimestamp.sDate.RTC_Month,
      (int)pLog->sTimestamp.sDate.RTC_Date,
      (int)pLog->sTimestamp.sTime.RTC_Hours,
      (int)pLog->sTimestamp.sTime.RTC_Minutes,
      (int)pLog->sTimestamp.sTime.RTC_Seconds
    );
    f_printf(&fil, "%d,%d,%d,%d,", 
      pLog->sTemperature.iBME,
      pLog->sTemperature.iCPU,
      pLog->sTemperature.iQMC,
      pLog->sTemperature.iMPU
    );
    f_printf(&fil, "%l,%l,",
      pLog->ulPressure,
      pLog->ulHumidity
    );
    f_printf(&fil, "%d,%d,%d,%d,",
      pLog->sWind.uiDir,
      pLog->sWind.uiVelo,
      pLog->sAlignment.uiAzimuth,
      pLog->sAlignment.iZenith
    );
    f_printf(&fil, "%l,%l,%d,",
      pLog->sPosition.lLat,
      pLog->sPosition.lLong,
      pLog->sPosition.iAlt
    );
    f_printf(&fil, "%d,%d,%d,%d\r\n",
      pLog->sPower.uiBatVolt,
      pLog->sPower.uiPanelVolt,
      pLog->sPower.iBatCurr,
      pLog->sPower.iPanelCurr
    );
    f_close(&fil);
  }
  else
  {
    printf(" FAIL\r\n");
  }
}

/*!**************************************************************************** 
 * @brief
 * Interrupthandler für 100ms-Task. Behandelt 
 *
 * @date  15.10.2019
 ******************************************************************************/
@far @interrupt void Timer2Interrupt(void)
{
  TIM2_ClearFlag(TIM2_FLAG_Update);
    
  /* Blink Pattern                                        */
  Blink_Poll();
  
  /* Lageregelung                                         */
  Motor_Task100ms();
      
  /* Timer-Zähler und Flags für 1s-Task behandeln         */
  if (iTimer1s > 0)
  {
    --iTimer1s;
  }
  else
  {
    iTimer1s = 10;
    bTask1sFlag = true;
  }
}

/*!****************************************************************************
 * @brief
 * Interrupthandler für 15min-Task
 * 
 * @date 27.10.2019
 ******************************************************************************/
@far @interrupt void RTC_InterruptHandler(void)
{
  RTC_ClearFlag(RTC_FLAG_WUTF);
  bTaskWakeupFlag = true;
}