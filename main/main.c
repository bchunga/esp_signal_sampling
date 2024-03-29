/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/**
 * Considerations:
 * 
 * 1) Target Device should be specify into the SDKConfig to use the correct configuration for
 *    ESP32 and ESP32C3 devices.
 * 
 * 2) To allow use the DMA Controller on ESP32C3, ESP-IDF v4.4 or higher environment is needed.
 * 
 * 3) Into the components/adc_read folder is a adc read library, but it uses ESP-IDF v4.2 
 *    environment. The code above is based on ESP-IDF v4.4 so in faq to use de adc library 
 *    the function have to be change to the functions used on v4.4.
 * 
 * 
 */

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/adc.h"

#if CONFIG_IDF_TARGET_ESP32C3
#define TIMES              512
#endif

#if CONFIG_IDF_TARGET_ESP32
#define TIMER               256
#endif

#define GET_UNIT(x)        ((x>>3) & 0x1)

#if CONFIG_IDF_TARGET_ESP32
#define ADC_RESULT_BYTE     2
#define ADC_CONV_LIMIT_EN   1                       /* For ESP32, this should always be set to 1 */
#define ADC_CONV_MODE       ADC_CONV_SINGLE_UNIT_1  /* ESP32 only supports ADC1 DMA mode */
#define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE1
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_RESULT_BYTE     4
#define ADC_CONV_LIMIT_EN   0
#define ADC_CONV_MODE       ADC_CONV_ALTER_UNIT     /* ESP32C3 only supports alter mode */
#define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE2    /* ESP32C3 supports TYPE2 outputs */
#endif

#if CONFIG_IDF_TARGET_ESP32C3
static uint16_t adc1_chan_mask = BIT(2) | BIT(3);
static uint16_t adc2_chan_mask = BIT(0);
static adc_channel_t channel[3] = {ADC1_CHANNEL_2, ADC1_CHANNEL_3, (ADC2_CHANNEL_0 | 1 << 3)};
#endif

#if CONFIG_IDF_TARGET_ESP32
static uint16_t adc1_chan_mask = BIT(7);
static uint16_t adc2_chan_mask = 0;
static adc_channel_t channel[1] = {ADC1_CHANNEL_0};
#endif

static const char *TAG = "ADC DMA";

static void continuous_adc_init(uint16_t adc1_chan_mask, uint16_t adc2_chan_mask, adc_channel_t *channel, uint8_t channel_num)
{
    adc_digi_init_config_t adc_dma_config = {
        .max_store_buf_size = 2048,
        .conv_num_each_intr = TIMES,
        .adc1_chan_mask = adc1_chan_mask,
        .adc2_chan_mask = adc2_chan_mask,
    };
    ESP_ERROR_CHECK(adc_digi_initialize(&adc_dma_config));

    adc_digi_configuration_t dig_cfg = {
        .conv_limit_en = ADC_CONV_LIMIT_EN,
        .conv_limit_num = 250,
        .sample_freq_hz = 80 * 1000,
        .conv_mode = ADC_CONV_MODE, // ESP32C3 only supports Alter Mode
        .format = ADC_OUTPUT_TYPE,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        uint8_t unit = GET_UNIT(channel[i]);
        uint8_t ch = channel[i] & 0x7;
        adc_pattern[i].atten = ADC_ATTEN_DB_11;
        adc_pattern[i].channel = ch;
        adc_pattern[i].unit = unit;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    }
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_digi_controller_configure(&dig_cfg));
}

#if !CONFIG_IDF_TARGET_ESP32
static bool check_valid_data(const adc_digi_output_data_t *data)
{
    const unsigned int unit = data->type2.unit;
    if (unit > 2) return false;
    if (data->type2.channel >= SOC_ADC_CHANNEL_NUM(unit)) return false;

    return true;
}
#endif

void app_main(void)
{
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint8_t result[TIMES] = {0};
    memset(result, 0xcc, TIMES);

    continuous_adc_init(adc1_chan_mask, adc2_chan_mask, channel, sizeof(channel) / sizeof(adc_channel_t));
    adc_digi_start();

    while(1) {
        ret = adc_digi_read_bytes(result, TIMES, &ret_num, ADC_MAX_DELAY);
        if (ret == ESP_OK || ret == ESP_ERR_INVALID_STATE) {

            for (int i = 0; i < ret_num; i += ADC_RESULT_BYTE) {
                adc_digi_output_data_t *p = (void*)&result[i];
    /**
     * Print if used to send data to the Serial Ploter (UART)
     * 
     */
    #if CONFIG_IDF_TARGET_ESP32
                printf("$%s;\n", p->type1.data);
                
    #else /* CONFIG_IDF_TARGET_ESP32C3 */
                if (ADC_CONV_MODE == ADC_CONV_BOTH_UNIT || ADC_CONV_MODE == ADC_CONV_ALTER_UNIT) {
                    if (check_valid_data(p)) {
                        printf("$%s;\n", p->type2.data);
                    } else {
                        ESP_LOGI(TAG, "Invalid data [%d_%d_%x]", p->type2.unit+1, p->type2.channel, p->type2.data);
                    }
                }
    #endif
            }
            

            /**
             * To avoid Watchdog Timeout error.
             * 
             */
            vTaskDelay(1);
        } else if (ret == ESP_ERR_TIMEOUT) {
            /**
             * ``ESP_ERR_TIMEOUT``: If ADC conversion is not finished until Timeout, you'll get this return error.
             * 
             */
            
            vTaskDelay(1000);
        }

    }

    adc_digi_stop();
    ret = adc_digi_deinitialize();
    assert(ret == ESP_OK);
}