#include <stdbool.h>
#include "esp_err.h"

typedef struct {
    bool i2c_operational;
    uint16_t adc_result_mV[4]; 
} pms_adc_results_t;

typedef struct {
    int adc_units_no;
    pms_adc_results_t* results;
} pms_adc_config_t;

esp_err_t pms_adc_init();