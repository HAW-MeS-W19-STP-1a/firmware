/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "commlib.h"
#include "SolarTracking.h"
#include "BlinkSequencer.h"
#include "BTHandler.h"
#include "ATCmd.h"
#include "GPSHandler.h"
#include "sensorlib.h"
#include "motorlib.h"
#include "diskio.h"
#include "ff.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


/*- Globale Variablen --------------------------------------------------------*/
bool bDir;

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
  ATCmd_Init();
  
  /* NMEA Input, Binary CMD output                        */
  GPIO_Init(GPIOG, GPIO_Pin_0, GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOG, GPIO_Pin_1, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow); // GPS Power Enable
  GPSHandler_Init();
  UART3_Init();
  UART3_ReceiveUntilTrig('$', '\r', COMMLIB_UART3RX_MAX_BUF);
  
  /* Motor controller                                     */
  GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_IT); // Nothalt
  GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_PU_IT); // Limit A
  GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_In_PU_IT); // Limit B
  EXTI_SetPortSensitivity(EXTI_Port_B, EXTI_Trigger_Falling);
  EXTI_SelectPort(EXTI_Port_B);
  EXTI_SetHalfPortSelection(EXTI_HalfPort_B_LSB, ENABLE);
  EXTI_SetHalfPortSelection(EXTI_HalfPort_B_MSB, ENABLE);
  GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Slow); // Motor Power Enable
  GPIO_Init(GPIOF, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Slow); // Motor 2, Dir A
  GPIO_Init(GPIOF, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Slow); // Motor 2, Dir B
  GPIO_Init(GPIOF, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Slow); // Motor 1, Dir A
  GPIO_Init(GPIOF, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow); // Motor 1, Dir B
  Motor_Init();
  Motor_Cmd(true);
  
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
  GPIO_Init(GPIOG, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(GPIOG, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(GPIOG, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOG, GPIO_Pin_3, GPIO_Mode_Out_PP_High_Fast);
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
  
  //printf("Powerup wait\r\n");
  //while (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4) != 0);
  
  Motor_SetTiltRef(0);
  Motor_SetTurnRef(0);
  Motor_SetTurn(450);
  bDir = true;
  
  
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
  printf(" OK\r\n");
  I2CMaster_DeInit();
  
  #ifdef FATFS_DEMO
  printf("SD-Card init...");
  {
    FATFS fs;
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
      
      Wind_Update(&sSensorWind);
      //printf("Pos Tilt=%d, Turn=%d\r\n", Motor_GetTilt(), Motor_GetTurn());
      
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
      
      if (Motor_IsTurnReached())
      {
        Motor_SetTurn(bDir ? 0 : 450);
      }
      
      I2CMaster_Init();
      BME280_Update(&sSensorBME280);   
      QMC5883_Update(&sSensorQMC5883);
      MPU6050_Update(&sSensorMPU6050);
      CPUTemp_Update(&sSensorCPUTemp);
      I2CMaster_DeInit();
      
      BTHandler_TakeWakeup();
      GPSHandler_TaskWakeup();
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