#ifndef APP_IOMAP_H_
#define APP_IOMAP_H_

#define BTN_BLUE_PORT   GPIOG
#define BTN_BLUE_PIN    GPIO_Pin_4

#define BTN_STOP_PORT   GPIOB
#define BTN_STOP_PIN    GPIO_Pin_0

#define LIM_PORT        GPIOB
#define LIM_A_PIN       GPIO_Pin_3
#define LIM_B_PIN       GPIO_Pin_4

#define MOT_PWREN_PORT  GPIOD
#define MOT_PWREN_PIN   GPIO_Pin_4

#define MOT_PORT        GPIOF
#define MOT2_B_PIN      GPIO_Pin_4
#define MOT2_A_PIN      GPIO_Pin_5
#define MOT1_B_PIN      GPIO_Pin_6
#define MOT1_A_PIN      GPIO_Pin_7

#define USART2_PORT     GPIOE
#define USART2_TX_PIN   GPIO_Pin_4
#define USART2_RX_PIN   GPIO_Pin_3

#define I2C1_PORT       GPIOC
#define I2C1_SDA_PIN    GPIO_Pin_0
#define I2C1_SCL_PIN    GPIO_Pin_1

#define USART3_GPS_PORT GPIOG
#define USART3_GPS_RX_PIN   GPIO_Pin_0
#define USART3_GPS_TX_PIN   GPIO_Pin_1

#define GPS_PWREN_PORT  GPIOD
#define GPS_PWREN_PIN   GPIO_Pin_2

#define BT_PWREN_PORT   GPIOD
#define BT_PWREN_PIN    GPIO_Pin_3
#define BT_STAT_PORT    GPIOD
#define BT_STAT_PIN     GPIO_Pin_1
#define BT_CMD_PORT     GPIOC
#define BT_CMD_PIN      GPIO_Pin_4

#define USART2_BT_PORT      GPIOC
#define USART2_BT_RX_PIN    GPIO_Pin_3
#define USART3_BT_TX_PIN    GPIO_Pin_2

#define TIM3_ANEM_IN_PORT   GPIOB
#define TIM3_ANEM_IN_PIN    GPIO_Pin_1

#define ADC1_WV_IN16_PORT   GPIOB
#define ADC1_WV_IN16_PIN    GPIO_Pin_2

#define ADC1_IPV_IN0_PORT   GPIOA
#define ADC1_IPV_IN0_PIN    GPIO_Pin_6
#define ADC1_IPV_IN0_CH     ADC_Channel_0
#define ADC1_UBAT_IN1_PORT  GPIOA
#define ADC1_UBAT_IN1_PIN   GPIO_Pin_5
#define ADC1_UBAT_IN1_CH    ADC_Channel_1
#define ADC1_UPV_IN2_PORT   GPIOA
#define ADC1_UPV_IN2_PIN    GPIO_Pin_4
#define ADC1_UPV_IN2_CH     ADC_Channel_2
#define ADC1_IBAT_IN3_PORT  GPIOC
#define ADC1_IBAT_IN3_PIN   GPIO_Pin_7
#define ADC1_IBAT_IN3_CH    ADC_Channel_3

#define LED_PORT        GPIOB
#define LED_SYS_PIN     GPIO_Pin_5
#define LED_MOT_PIN     GPIO_Pin_6
#define LED_BT_PIN      GPIO_Pin_7

#define SPI2_PORT       GPIOG
#define SPI2_SCK_PIN    GPIO_Pin_5
#define SPI2_MOSI_PIN   GPIO_Pin_6
#define SPI2_MISO_PIN   GPIO_Pin_7

#define SD_CS_PORT      GPIOE
#define SD_CS_PIN       GPIO_Pin_7

#endif /* APP_IOMAP_H_ */