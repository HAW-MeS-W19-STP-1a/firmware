/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "commlib.h"
#include "SolarTracking.h"
#include "BlinkSequencer.h"
#include "BTHandler.h"
#include "GPSHandler.h"
#include "sensorlib.h"
#include "pff.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


/*- Globale Variablen --------------------------------------------------------*/
/*! Timer-Zähler für 1s-Task                                                  */
volatile int iTimer1s = 10;

/*! Statusflag für Ausführung des 1s-Task                                     */
volatile bool bTask1sFlag = false;

/*! Statusflag für Ausführung des Wakeup-Task                                 */
volatile bool bTaskWakeupFlag = false;

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
  
  /* Timer 2 for 200ms Task trigger                         */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  TIM2_InternalClockConfig();
  TIM2_TimeBaseInit(TIM2_Prescaler_128, TIM2_CounterMode_Down, 12500);
  TIM2_ITConfig(TIM2_IT_Update, ENABLE);
  TIM2_Cmd(ENABLE);
  
  /* Initialise GPIO pins                                 */
  GPIO_Init(GPIOA, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow); // PWM
  GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_PU_No_IT); // Anemometer In
  GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_FL_No_IT); // Wind Vane Analog In
  GPIO_Init(GPIOG, GPIO_Pin_4, GPIO_Mode_In_FL_No_IT); // Button
   
  /* Blink Module init                                    */
  Blink_Init();
  Blink_SetPattern(Blink_Led_SYS, 0x0001);
  
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
  GPIO_Init(GPIOE, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);
  UART2_Init();
  
  /* Initialise Pins for I2C Master Mode                  */
  GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_OD_HiZ_Fast);
  GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Fast);
  
  /* Bluetooth UART TX and RX                             */
  BTHandler_Init();
  
  /* NMEA Input, Binary CMD output                        */
  GPIO_Init(GPIOG, GPIO_Pin_0, GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOG, GPIO_Pin_1, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow); // GPS Power Enable
  GPSHandler_Init();
  UART3_Init();
  UART3_ReceiveUntilTrig('$', '\r', COMMLIB_UART3RX_MAX_BUF);
  
  /* RTC init                                             */
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
  RTC_StructInit(&sRtcInit);
  sRtcInit.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&sRtcInit);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_SetWakeUpCounter(10);
  RTC_WakeUpCmd(ENABLE);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  
  printf("Powerup wait\r\n");
  while (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4) != 0);
  
  /* Enable interrupt execution                           */
  enableInterrupts();
  printf("Program started\r\n");
  
  I2CMaster_Init();
  printf("BME280 init...");
  BME280_Init(&sSensorBME280, 0x76);
  printf(" OK\r\nQMC5883 init...");
  QMC5883_Init(&sSensorQMC5883, 0x0D);
  QMC5883_SetRefTemp(&sSensorQMC5883, 3400);
  printf(" OK\r\nMPU6050 init...");
  MPU6050_Init(&sSensorMPU6050, 0x68, false);
  printf(" OK\r\nWind Timer + ADC init...");
  Wind_Init(&sSensorWind, 1000); 
  printf(" OK\r\nCPU Temp init...");
  CPUTemp_Init(&sSensorCPUTemp);
  printf(" OK\r\nDone.\r\n");
  I2CMaster_DeInit();
  
  while (1)
  {
    if (bTask1sFlag)
    {
      bTask1sFlag = false;
      printf("Task1s\r\n");
      
      Wind_Update(&sSensorWind);
      
      BTHandler_Task1s();
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
      
      I2CMaster_Init();
      BME280_Update(&sSensorBME280);   
      QMC5883_Update(&sSensorQMC5883);
      MPU6050_Update(&sSensorMPU6050);
      CPUTemp_Update(&sSensorCPUTemp);
      I2CMaster_DeInit();
      
      BTHandler_TakeWakeup();
      GPSHandler_TaskWakeup();
    }
    
    BTHandler_Poll();
    
    /* User Interface                                     */
    if (UART1_IsRxReady())
    {
      if (BTHandler_IsActive() && (UART1_GetRxCount() > 0))
      {
        printf("Bluetooth RX\r\n");
        UART1_FlushTx();
        switch (aucUart1RxBuf[0])
        {
          case '?':
            sprintf((volatile char*)aucUart1TxBuf, "HELP__:\r\n\tt = Temperature\r\n\tw = Wind\r\n\tp = Pressure\r\n\th = Humidity\r\n");
            UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
            while(!UART1_IsTxReady());
            UART1_FlushTx();
            sprintf((volatile char*)aucUart1TxBuf, "\tg = GPS data\r\n\td = Date/Time from RTC\r\n\tz = Sun Zenith/Azimuth\r\n");
            UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
            while(!UART1_IsTxReady());
            UART1_FlushTx();
            sprintf((volatile char*)aucUart1TxBuf, "\ts = SR/SS\r\n\tr = Reset\r\n");
            break;
            
          case 't':
            sprintf((volatile char*)aucUart1TxBuf, "TEMP__: BME=%d MCU=%d QMC=%d MPU=%d\r\n", sSensorBME280.sMeasure.iTemperature, (int16_t)sSensorCPUTemp.sMeasure.cTemp, sSensorQMC5883.sMeasure.iTemperature, sSensorMPU6050.sMeasure.iTemperature);
            break;
            
          case 'h':
            sprintf((volatile char*)aucUart1TxBuf, "HUMID_: BME=%ld\r\n", sSensorBME280.sMeasure.ulHumidity);
            break;
            
          case 'p':
            sprintf((volatile char*)aucUart1TxBuf, "PRESS_: BME=%ld\r\n", sSensorBME280.sMeasure.ulPressure);
            break;
            
          case 'w':
            sprintf((volatile char*)aucUart1TxBuf, "WIND__: SPD=%d DIR=%d\r\n", sSensorWind.sMeasure.uiVelocity, (int)sSensorWind.sMeasure.eDirection);
            break;
            
          case 'a':
            sprintf((volatile char*)aucUart1TxBuf, "ALIGN_: AZM=%d ZEN_XZ=%d ZEN_YZ=%d\r\n", sSensorQMC5883.sMeasure.uiAzimuth, sSensorMPU6050.sMeasure.sAngle.iXZ, sSensorMPU6050.sMeasure.sAngle.iYZ);
            break;
            
          case 'g':
            sprintf((volatile char*)aucUart1TxBuf, "GPS___: LAT=%ld LON=%ld ALT=%d\r\n", sSensorGPS.sPosition.lLat, sSensorGPS.sPosition.lLong, sSensorGPS.sPosition.iAlt);
            break;
            
          case 'd':
            RTC_GetDate(RTC_Format_BIN, &sDate);
            RTC_GetTime(RTC_Format_BIN, &sTime);
            sprintf((volatile char*)aucUart1TxBuf, "RTC___: DATE=%02d.%02d.%02d TIME=%02d:%02d:%02d\r\n", (unsigned int)sDate.RTC_Date, (unsigned int)sDate.RTC_Month, (unsigned int)sDate.RTC_Year, (unsigned int)sTime.RTC_Hours, (unsigned int)sTime.RTC_Minutes, (unsigned int)sTime.RTC_Seconds);
            break;
            
          case 'z':
          {
            double zenith, azimuth;
            int iZenith, iAzimuth;
            RTC_GetDate(RTC_Format_BIN, &sDate);
            RTC_GetTime(RTC_Format_BIN, &sTime);
            calculate_current_sun_position(
              2000+sDate.RTC_Year,
              sDate.RTC_Month,
              sDate.RTC_Date,
              sTime.RTC_Hours + (sTime.RTC_Minutes / 60.0) + (sTime.RTC_Seconds / 3600.0),
              2,
              53.55,
              9.99,
              &azimuth,
              &zenith
            );
            iZenith = zenith;
            iAzimuth = azimuth;
            sprintf((volatile char*)aucUart1TxBuf, "TRACK_: ZEN=%d AZM=%d\r\n", iZenith, iAzimuth);
            break;
          }
          
          case 's':
          {
            double sunrise, sunset;
            int iSunriseHour, iSunsetHour, iSunriseMin, iSunsetMin;
            RTC_GetDate(RTC_Format_BIN, &sDate);
            RTC_GetTime(RTC_Format_BIN, &sTime);
            calculate_sunrise_and_fall(
              2000+sDate.RTC_Year,
              sDate.RTC_Month,
              sDate.RTC_Date,
              2,
              53.55,
              9.99,
              90,
              90,
              &sunrise,
              &sunset
            );
            iSunriseHour = sunrise;
            iSunriseMin = (sunrise - iSunriseHour) * 60;
            iSunsetHour = sunset;
            iSunsetMin = (sunset - iSunsetHour) * 60;
            sprintf((volatile char*)aucUart1TxBuf, "TRACK_: SR=%02d:%02d SS=%02d:%02d\r\n", iSunriseHour, iSunriseMin, iSunsetHour, iSunsetMin);
            break;
          }
          break;
          
          case 'r':
          {
            typedef void (*funcptr)(void);
            funcptr vReset = (funcptr)0x08000;
            printf("ResetCmd");
            sprintf((volatile char*)aucUart1TxBuf, "RESET_: OK\r\n");
            UART1_SendUntil('\r', COMMLIB_UART1_MAX_BUF);
            while (!UART1_IsTxReady());
            vReset();
          }
          break;
            
          default:
            sprintf((volatile char*)aucUart1TxBuf, "ERROR_: Invalid input. Type '?' for help\r\n");
        }
        UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
      }
      UART1_FlushRx();
      UART1_Receive(1);
    }
    
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
    wfi();
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