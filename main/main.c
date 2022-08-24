/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/adc.h"

#define TIMES              256
#define GET_UNIT(x)        ((x>>3) & 0x1)

static const char *TAG = "ADC DMA";


void app_main(void)
{
    static char buffer[10] = { 0 };

    while(1) {

        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);
        int val = adc1_get_raw(ADC1_CHANNEL_0);

        sprintf(&buffer, "$%d;", val);
        printf("%s\n", buffer);
        vTaskDelay(1);

  

    }

}