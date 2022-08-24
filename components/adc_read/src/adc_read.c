
// #include "../include/adc_read.h"

// #ifdef CONFIG_ENABLE_ADC_READ_LOGS
// #include "esp_log.h"
// #define printSerialLogInf(Tag, Format, ...)     ESP_LOGI(tag, format, ...)
// #define printSerialLogErr(Tag, Format, ...)     ESP_LOGE(tag, format, ...)
// #else 
// #define printSerialLogInf(Tag, Format, ...)
// #define printSerialLogErr(Tag, Format, ...)
// #endif

// // TODO: add logs 

// static adc_read_err check_valid_data(const adc_digi_output_data_t *data);

// /**
//  * @brief   Check integrity of the ADC Read contex structure
//  *  
//  */
// adc_read_err adc_read_check_ctx_integrity(adcReadChannel * _ctx)
// {
//     if (_ctx->channel == NULL)          return ADC_READ_E_PTR_NULL;
//     if (_ctx->init != ADC_CTX_INIT)      return ADC_READ_E_NOT_INIT;
//     return ADC_READ_OK;
// }
// /**
//  * @brief   Initialice CTX config for a continuous ADC reading task 
//  *  
//  */
// adc_read_err adc_read_cont_init(adcReadChannel * _ctx)
// {
//     int8_t res = ADC_READ_E_UNKNOW;

//     res = adc_read_check_ctx_integrity(_ctx);
//     if (res != ADC_READ_OK)
//         return res;

//     adc_digi_init_config_t adc_dma_config = {
//         .max_store_buf_size = 1024,
//         .conv_num_each_intr = 256,
//         .adc1_chan_mask = _ctx->msk_channel_1,
//         .adc2_chan_mask = _ctx->msk_channel_2,
//     };

//     adc_digi_initialize(&adc_dma_config);

//     adc_digi_pattern_config_t adc_pattern[10] = {0};
    
//     adc_digi_configuration_t digi_cfg = {
//         .conv_limit_en = 0,
//         .conv_limit_num = 250,
//         .sample_freq_hz = 620,
//     };

//     // digi_cfg.adc_pattern = &(_ctx->channel_num);   // TODO: FIX should use memset here
//     memset(digi_cfg.adc_pattern, _ctx->channel_num, sizeof(uint8_t));

//     for (int i = 0; i < (_ctx->channel_num); i++) {
//         uint8_t unit = ((_ctx->channel[i] >> 3) & 0x1);
//         uint8_t ch = _ctx->channel[i] & 0x7;
//         adc_pattern[i].atten = ADC_ATTEN_DB_0;
//         adc_pattern[i].channel = ch;
//         adc_pattern[i].unit = unit;
//     }

//     digi_cfg.adc_pattern = adc_pattern;
//     res = adc_digi_controller_configure(&digi_cfg);
//     if (res != ADC_READ_OK)
//         return ADC_READ_E_CTRLR_CFG;

//     res = ADC_READ_OK;
//     return res;
// }
// /**
//  * @brief   Initialice the ADC Read contex
//  *  
//  */
// adc_read_err adc_ctx_init(adcReadChannel * _ctx)
// {
//     if (_ctx->channel == NULL)          return ADC_READ_E_PTR_NULL;

//     _ctx->msk_channel_1 = BIT(0)|BIT(1);
//     _ctx->msk_channel_2 = BIT(0);
//     adc_channel_t channelCfg[3] = {ADC1_CHANNEL_0, ADC1_CHANNEL_1, (ADC2_CHANNEL_0 | 1 << 3)};
//     memset(_ctx->channel,channelCfg, (sizeof(adc_channel_t)*3));
//     _ctx->channel_num = (sizeof(adc_channel_t)*3) / sizeof(adc_channel_t);
//     _ctx->init = ADC_CTX_INIT;
    
//     return ADC_READ_OK;
// }
// /**
//  * @brief   Start a continuous reading task
//  * 
//  */
// adc_read_err adc_read_cont()
// {
//     int8_t res = ADC_READ_E_UNKNOW;
//     uint32_t RetNum = 0;

//     uint8_t output[TIMES] = {0};
    
//     adcReadChannel adc_ctx = {0};
//     res = adc_ctx_init(&adc_ctx);
//     if (res != ADC_READ_OK)
//         return res;

//     res = adc_read_cont_init(&adc_ctx);
//     if (res != ADC_READ_OK)
//         return res;

//     adc_digi_start();

//     uint n = 20;
//     while(n--)
//     {
//         res = adc_digi_read_bytes(output, TIMES, &RetNum, ADC_READ_DELAY);
//         for (int i=0; i<RetNum; i+=4)
//         {
//             adc_digi_output_data_t *pOutputData = (void*)&output[i];
//             if (check_valid_data(pOutputData) != ADC_READ_OK) {
//                 printSerialLogInf(TAG,"ADC%d_CH%d: %x", p->type2.unit+1, p->type2.channel, p->type2.data);
//             } else {
//                 printSerialLogErr(TAG,"Invalid data!");    
//             }
//         }
//     }

//     adc_digi_stop();
//     res = adc_digi_deinitialize();
//     if (res != ADC_READ_OK)
//         return ADC_READ_E_UNKNOW;

//     res = ADC_READ_OK;
//     return res;
// }
// /**
//  * @brief   Trigger a single ADC Read task 
//  * 
//  */
// adc_read_err adc_read_single()
// {
//     int8_t res = ADC_READ_E_UNKNOW;

//     int adc1_reading[3] = {0xcc};
//     int adc2_reading[1] = {0xcc};
//     // const char TAG_CH[][10] = {"ADC1_CH2", "ADC1_CH3","ADC1_CH4", "ADC2_CH0"};

//     adc1_config_width(ADC_WIDTH_BIT_DEFAULT);
//     adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_0);
//     adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_6);
//     adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_0);
//     adc2_config_channel_atten(ADC2_CHANNEL_0, ADC_ATTEN_DB_0);

//     int n = 20;
//     while (n--) {

//         adc1_reading[0] = adc1_get_raw(ADC1_CHANNEL_2);
//         adc1_reading[1] = adc1_get_raw(ADC1_CHANNEL_3);
//         adc1_reading[2] = adc1_get_raw(ADC1_CHANNEL_4);
//         res = adc2_get_raw(ADC2_CHANNEL_0, ADC_WIDTH_BIT_12, &adc2_reading[0]);

//         // for (int i = 0; i < 3; i++) {
//             // ESP_LOGI(TAG_CH[i], "%x", adc1_reading[i]);
//             printSerialLogInf(TAG,"%x %x %x %x",adc1_reading[0],adc1_reading[1],adc1_reading[2],adc2_reading[0]);
//         // }

//         res = adc2_get_raw(ADC2_CHANNEL_0, ADC_WIDTH_BIT_12, &adc2_reading[0]);
//         // ESP_LOGI(TAG_CH[3], "%x", adc2_reading[0]);
//         // printSerialLogInf(TAG_CH[3], "%x", adc2_reading[0]);
//     }

//     res = ADC_READ_OK;
//     return res;
// }
// /**
//  * @brief   Verifity the ADC data to be a valid reading 
//  * 
//  */
// static adc_read_err check_valid_data(const adc_digi_output_data_t *data)
// {
//     const unsigned int unit = data->type2.unit;
//     if (unit > 2) return ADC_READ_E_DATA_INVALID;
//     if (data->type2.channel >= SOC_ADC_CHANNEL_NUM(unit)) return ADC_READ_E_DATA_INVALID;

//     return ADC_READ_OK;
// }
