#include <stdint.h>
#include "esp_err.h"

esp_err_t i2c_init_interface(uint8_t i2c_port_num, int scl_gpio, int sda_gpio, int i2c_clk);
esp_err_t i2c_write(uint8_t i2c_port_num, uint8_t i2c_address, uint8_t* data, size_t data_len);
esp_err_t i2c_read(uint8_t i2c_port_num, uint8_t i2c_address, uint8_t* data, size_t data_len);
esp_err_t i2c_check(uint8_t i2c_port_num, uint8_t i2c_address);