#ifndef _ADC_READ_H_
#define _ADC_READ_H_

#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/adc.h"

#define TAG                         ADC_READ

#define TIMES                       256
#define ADC_CTX_INIT                1
#define ADC_FREQ_HIGH               SOC_ADC_SAMPLE_FREQ_THRES_HIGH
#define ADC_FREQ_LOW                SOC_ADC_SAMPLE_FREQ_THRES_LOW
#define ADC_READ_DELAY              ADC_MAX_DELAY

/**
 * @brief   ADC read Errors
 * 
 */
typedef enum 
{
    ADC_READ_OK                     = 0,
    ADC_READ_E_PTR_NULL             = -1,
    ADC_READ_E_NOT_INIT             = -2,
    ADC_READ_E_CTRLR_CFG            = -3,
    ADC_READ_E_DATA_INVALID         = -4,
    ADC_READ_E_UNKNOW               = -15,
} adc_read_err;
/**
 * @brief   ADC read CTX Struct
 * 
 */
typedef struct __attribute__((__packed__))
{
    uint16_t            msk_channel_1;
    uint16_t            msk_channel_2;
    adc_channel_t       *channel;
    uint8_t             channel_num;
    uint8_t             init;
} adcReadChannel;

#endif /* _ADC_READ_H_ */