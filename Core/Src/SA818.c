#include "SA818.h"

extern UART_HandleTypeDef huart1;

void SA818_Init(SA818 *dev) {
    HAL_GPIO_WritePin(dev->PTT_GPIO_Port, dev->PTT_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(dev->PD_GPIO_Port, dev->PD_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(dev->HL_GPIO_Port, dev->HL_Pin, GPIO_PIN_RESET);
    
    if (dev->has_amp) {
        HAL_GPIO_WritePin(dev->AMP_GPIO_Port, dev->AMP_Pin, GPIO_PIN_RESET);
    }
}

uint8_t SA818_ReadSerialTimeout(SA818 *dev) {
    uint8_t received;
    uint32_t startTick = HAL_GetTick();

    while (HAL_GetTick() - startTick < 500) {
        if (HAL_UART_Receive(dev->huart, &received, 1, 100) == HAL_OK) {
            // printf("recieved..\n");
            HAL_UART_Transmit(dev->huart, &received, strlen(received), 100); // for debugging
            return 1;
        }
    }
#ifdef DEBUG==1
    const char *msg = "Read timeout\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
#endif
    return 0;
}

uint8_t SA818_Begin(SA818 *dev) {
    const char *cmd = "AT\r\n";
    HAL_UART_Transmit(dev->huart, (uint8_t*)cmd, strlen(cmd), 100);
    HAL_Delay(500);

    for (uint8_t r = 0; r < 5; r++) {
        const char *connect = "AT+DMOCONNECT\r\n";
        HAL_UART_Transmit(dev->huart, (uint8_t*)connect, strlen(connect), 100);
        if (SA818_ReadSerialTimeout(dev)) return 1;
    }
    return 0;
}

uint8_t SA818_SetPower(SA818 *dev, uint8_t is_high) {
    if (is_high) {
        // Simulate floating pin by setting as input
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = dev->HL_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(dev->HL_GPIO_Port, &GPIO_InitStruct);
        // HAL_GPIO_WritePin(dev->HL_GPIO_Port, dev->HL_Pin, SET);

    } else {
        HAL_GPIO_WritePin(dev->HL_GPIO_Port, dev->HL_Pin, GPIO_PIN_RESET);
    }
    return 1;
}

uint8_t SA818_SetConfig(SA818 *dev, uint8_t bw, const char* tx_f, const char* rx_f, const char* tx_ctcss, const char* rx_ctcss, uint8_t squelch) {
    HAL_GPIO_WritePin(dev->PTT_GPIO_Port, dev->PTT_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+DMOSETGROUP=%d,%s,%s,%s,%d,%s\r\n", bw, tx_f, rx_f, tx_ctcss, squelch, rx_ctcss);

    for (uint8_t r = 0; r < 5; r++) {
        HAL_UART_Transmit(dev->huart, (uint8_t*)cmd, strlen(cmd), 100);
        if (SA818_ReadSerialTimeout(dev)) return 1;
    }

    return 0;
}

uint8_t SA818_SetVolume(SA818 *dev, uint8_t volume) {
    HAL_GPIO_WritePin(dev->PTT_GPIO_Port, dev->PTT_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    if (volume > 8) volume = 8;

    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+DMOSETVOLUME=%d\r\n", (int)volume);
    HAL_UART_Transmit(dev->huart, (uint8_t*)cmd, strlen(cmd), 100);

    return SA818_ReadSerialTimeout(dev);
}

uint8_t SA818_SetFilters(SA818 *dev, bool preemph, bool highpass, bool lowpass) {
    HAL_GPIO_WritePin(dev->PTT_GPIO_Port, dev->PTT_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+SETFILTER=%d,%d,%d\r\n", preemph, highpass, lowpass);
    HAL_UART_Transmit(dev->huart, (uint8_t*)cmd, strlen(cmd), 100);

    return SA818_ReadSerialTimeout(dev);
}

void SA818_PowerDown(SA818 *dev, uint8_t powerdown) {
    HAL_GPIO_WritePin(dev->PTT_GPIO_Port, dev->PTT_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(dev->PD_GPIO_Port, dev->PD_Pin, powerdown ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void SA818_SetModeTX(SA818 *dev) {
    HAL_GPIO_WritePin(dev->PTT_GPIO_Port, dev->PTT_Pin, SA_MODE_TX);
    if (dev->has_amp) HAL_GPIO_WritePin(dev->AMP_GPIO_Port, dev->AMP_Pin, GPIO_PIN_SET);
}

void SA818_SetModeRX(SA818 *dev) {
    HAL_GPIO_WritePin(dev->PTT_GPIO_Port, dev->PTT_Pin, SA_MODE_RX);
    if (dev->has_amp) HAL_GPIO_WritePin(dev->AMP_GPIO_Port, dev->AMP_Pin, GPIO_PIN_RESET);
}