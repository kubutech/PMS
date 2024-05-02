#include "pms_adc.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <arpa/inet.h>
#include "i2c_driver.h"

#define I2C_SCL                 19
#define I2C_SDA                 18
#define I2C_PORT                1
#define ADS1015_ADDRESS(x)      (0x48 + x)

void adc_measurement_task(void* pvParameters);
void convert_results(uint16_t* register_value);

esp_err_t pms_adc_init()
{
    esp_err_t result;

    result = i2c_init_interface(I2C_PORT, I2C_SCL, I2C_SDA, 400000);
    if (result != ESP_OK) {
        return result;
    }

    for (int i = 0; i < 1; i++) {
        result = i2c_check(I2C_PORT, ADS1015_ADDRESS(i));
        if (result != ESP_OK) {
            return result;
        }
    }

    if (xTaskCreate(adc_measurement_task, "ADC", 4096, NULL, 5, NULL) != pdPASS) {
            return ESP_FAIL;
    }

    return ESP_OK;
}

void adc_measurement_task(void* pvParameters)
{
    uint8_t ads1015_cfg[] = {0x01, 0x83, 0xE3};
    
    while(1) {
        int64_t measurement_start_time = esp_timer_get_time();
        for (int input_select = 0; input_select < 3; input_select++) {
            ads1015_cfg[1] = 0x83;
            ads1015_cfg[1] = (ads1015_cfg[1] | (input_select + 4) << 4);
            i2c_write(I2C_PORT, ADS1015_ADDRESS(0), ads1015_cfg, 3);
            i2c_write(I2C_PORT, ADS1015_ADDRESS(0), ads1015_cfg, 3);
            i2c_write(I2C_PORT, ADS1015_ADDRESS(0), ads1015_cfg, 3);
            
            uint16_t register_value = 0;
            int64_t start_time = esp_timer_get_time();
            do {
                i2c_read(I2C_PORT, ADS1015_ADDRESS(0), (uint8_t*)&register_value, sizeof(register_value));
                i2c_read(I2C_PORT, ADS1015_ADDRESS(0), (uint8_t*)&register_value, sizeof(register_value));
                i2c_read(I2C_PORT, ADS1015_ADDRESS(0), (uint8_t*)&register_value, sizeof(register_value));
            } while (*((uint8_t*)&register_value) != ads1015_cfg[1] && (int64_t)esp_timer_get_time() - start_time < 7000);
            
            uint8_t pointer_register = 0x00;
            i2c_write(I2C_PORT, ADS1015_ADDRESS(0), &pointer_register, 1);
            i2c_write(I2C_PORT, ADS1015_ADDRESS(0), &pointer_register, 1);
            i2c_write(I2C_PORT, ADS1015_ADDRESS(0), &pointer_register, 1);
            i2c_read(I2C_PORT, ADS1015_ADDRESS(0), (uint8_t*)&register_value, sizeof(register_value));
            i2c_read(I2C_PORT, ADS1015_ADDRESS(0), (uint8_t*)&register_value, sizeof(register_value));
            i2c_read(I2C_PORT, ADS1015_ADDRESS(0), (uint8_t*)&register_value, sizeof(register_value));
            convert_results(&register_value);
            convert_results(&register_value);
            convert_results(&register_value);
            //printf("Result: %d\n", register_value);

        }
        printf("Measure time: %lld\n", (int64_t)esp_timer_get_time() - measurement_start_time);
    }
}

void convert_results(uint16_t* register_value)
{
    *((uint8_t*)register_value + 1) = *((uint8_t*)register_value + 1) >> 4;
    *((uint8_t*)register_value + 1) = (*((uint8_t*)register_value) << 4) | *((uint8_t*)register_value + 1);
    *((uint8_t*)register_value) = *((uint8_t*)register_value) >> 4;

    *register_value = ntohs(*register_value);
}