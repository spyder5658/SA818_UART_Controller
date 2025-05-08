#ifndef SA818_H
#define SA818_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define SA_BANDWIDTH_12_5KHZ	0
#define SA_BANDWIDTH_25KHZ		1

#define SA_CTCSS_OFF	"0000"
#define SA_SQUELCH_OFF	0

#define SA_POWER_LOW	0
#define SA_POWER_HIGH	1

#define SA_VOLUME_MIN	0
#define SA_VOLUME_DEFAULT	4
#define SA_VOLUME_MAX	8

#define SA_FILTER_OFF	0
#define SA_FILTER_ON	1

#define SA_POWER_OFF	0
#define SA_POWER_ON		1

#define SA_MODE_TX GPIO_PIN_RESET
#define SA_MODE_RX GPIO_PIN_SET

typedef struct {
    UART_HandleTypeDef *huart;
    GPIO_TypeDef* PTT_GPIO_Port;
    uint16_t PTT_Pin;
    GPIO_TypeDef* PD_GPIO_Port;
    uint16_t PD_Pin;
    GPIO_TypeDef* HL_GPIO_Port;
    uint16_t HL_Pin;
    GPIO_TypeDef* AMP_GPIO_Port;
    uint16_t AMP_Pin;
    bool has_amp;
} SA818;

void SA818_Init(SA818 *dev);
uint8_t SA818_ReadSerialTimeout(SA818 *dev);
uint8_t SA818_Begin(SA818 *dev);
uint8_t SA818_SetPower(SA818 *dev, uint8_t is_high);
uint8_t SA818_SetConfig(SA818 *dev, uint8_t bw, const char* tx_f, const char* rx_f, const char* tx_ctcss, const char* rx_ctcss, uint8_t squelch);
uint8_t SA818_SetVolume(SA818 *dev, uint8_t volume);
uint8_t SA818_SetFilters(SA818 *dev, bool preemph, bool highpass, bool lowpass);
void SA818_PowerDown(SA818 *dev, uint8_t powerdown);
void SA818_SetModeTX(SA818 *dev);
void SA818_SetModeRX(SA818 *dev);

#endif