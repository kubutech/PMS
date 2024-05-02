#include <stdio.h>
#include <string.h>
#include <rom/ets_sys.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_timer.h"
#include "driver/twai.h"
#include "driver/gpio.h"
#include "i2c_driver.h"
#include "pms_adc.h"

#define MSG_SIZE                64
#define TEST_MSG_ID             0xF893E
#define TEST_PATTERN            {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0xFF}
#define CAN_TX                  33
#define CAN_RX                  34
#define GPIO_MUX_0              12
#define GPIO_MUX_1              13
#define ESP_INTR_FLAG_DEFAULT   0
#define BLOCK_DURATION_MS       100  

#define SCL1                    22
#define SDA1                    21
#define LDAC                    15
#define I2C_1_PORT              0
#define MCP4728_ADDR            0x61
#define VREF_CONFIG             0x80
#define MEAS_RES                2

typedef struct {
    uint16_t command;
    uint16_t measured;
    uint8_t counter;
} analogue_data_t;

void app_main(void)
{   
    nvs_flash_init();
    esp_err_t result;

    /*Initialise I2C*/

    result = i2c_init_interface(I2C_1_PORT, SCL1, SDA1, 400000);
    printf("I2C_1 config: %s\n", result == ESP_OK ? "Success" : "Failure");

    /*I2C self test*/
    result = i2c_check(I2C_1_PORT, MCP4728_ADDR);
    printf("I2C_1 self test result: %s\n", result == ESP_OK ? "Success" : "Failure");
    
    result = pms_adc_init();
    printf("ADC init: %s\n", result == ESP_OK ? "Success" : "Failure");

    /*Configure MCP4728*/
    uint8_t config = VREF_CONFIG;
    result = i2c_write(I2C_1_PORT, MCP4728_ADDR, &config, 1);
    printf("I2C_1 VREF result: %s\n", result == ESP_OK ? "Success" : "Failure");

    TickType_t last_wake_time = xTaskGetTickCount();

    gpio_reset_pin(LDAC);
    gpio_set_direction(LDAC, GPIO_MODE_OUTPUT);

    uint32_t counter = 0;
    uint16_t analogue_command = 0;

    vTaskDelayUntil(&last_wake_time, BLOCK_DURATION_MS / portTICK_PERIOD_MS);

    analogue_data_t analogue_data[3];

    for (int i = 0; i < 3; i++) {
        analogue_data[i].command = 0;
        analogue_data[i].measured = 0;
        analogue_data[i].counter = 8;
    }
    
    while (1) {

        /*Block 1 - Verify expected state & set faults (2 Periods)*/
        
        /*Read from ADS1015*/

        //ads1015_cfg[1] = (ads1015_cfg[1] | (input_select + 4) << 4);

        //printf("%x- ", ads1015_cfg[1]);

        //int64_t start_time = esp_timer_get_time();

        //do {
        //    result = i2c_read(I2C_2_PORT, ADS1015_ADDR, data_rx, 2);
        //} while (data_rx[0] != ads1015_cfg[1] && (int64_t)esp_timer_get_time() - start_time < 7000);

        //uint8_t pointer_register = 0x00;
        //result = i2c_write(I2C_2_PORT, ADS1015_ADDR, &pointer_register, 1);

        //start_time = esp_timer_get_time();

        for (int i = 0; i < 4; i++) {

            //vTaskDelay(5 / portTICK_PERIOD_MS);
            
            //uint8_t data_rx[2] = {0};
            //result = i2c_read(I2C_2_PORT, ADS1015_ADDR, data_rx, 2);

            //data_rx[1] = data_rx[1] >> 4;
            //data_rx[1] = (data_rx[0] << 4) | data_rx[1];
            //data_rx[0] = data_rx[0] >> 4;

            //memcpy(&analogue_data[i].measured, data_rx, 2);
            //analogue_data[i].measured = ntohs(analogue_data[i].measured) * 2;

            //printf("%d. Command: %d, Measured: %d\n", i, analogue_data[i].command, analogue_data[i].measured);

            //result = pms_adc_measure();
            //if (result != ESP_OK) {
            //    printf("Error reading ADC!\n");
            //}
            //if (abs(analogue_data[i].command - analogue_data[i].measured) > 33 && analogue_data[i].counter > 0) {
            //    analogue_data[i].counter -= 1;
            //} else if (abs(analogue_data[i].command - analogue_data[i].measured) > 33 && analogue_data[i].counter == 0) {
            //    printf("Error on channel: %d\n", i);
            //} else {
            //    analogue_data[i].counter = 4;
            //}
            //printf("%d. Counter: %d\n", i, analogue_data[i].counter);
        }
        //printf("-----------------------------\n");

        //printf("%d\n", (analogue_data[input_select].measured));

        //ads1015_cfg[1] = 0x82;
        //input_select += 1;
        //if (input_select > 3) {
        //    input_select = 0;
        //}
        

        //vTaskDelay(1 / portTICK_PERIOD_MS);

        vTaskDelayUntil(&last_wake_time, 2 * BLOCK_DURATION_MS / portTICK_PERIOD_MS);

        /*Block 2 - CAN Processing (5 MS)*/

        vTaskDelayUntil(&last_wake_time, BLOCK_DURATION_MS / portTICK_PERIOD_MS);

        /*Block 3 - SET GPIO (5 MS)*/
        //analogue_command = (counter * 10) % 4096;
        analogue_command = 4095;
        uint16_t analogue_command_list[4] = {4095, 2048, 1024, 0};
        uint8_t configuration[8];
        for (int i = 0; i < 8; i++) {
            //if (i % 2 == 0) {
            //    analogue_command = i * 200;
            //}
            analogue_command = analogue_command_list[i / 2];
            configuration[i] = *((uint8_t*)(&analogue_command) + (i + 1) % 2);
        }
        i2c_write(I2C_1_PORT, MCP4728_ADDR, configuration, 8);

        gpio_set_level(LDAC, 0);
        gpio_set_level(LDAC, 1);

        for (int i = 0; i < 4; i++) {
            analogue_data[i].command = (analogue_command * 3300) / 4096;
        }

        vTaskDelayUntil(&last_wake_time, BLOCK_DURATION_MS / portTICK_PERIOD_MS);

        /*Block 4 - Wait till EOF*/

        counter++;
        vTaskDelayUntil(&last_wake_time, BLOCK_DURATION_MS / portTICK_PERIOD_MS);
    }
}
