#include "i2c_driver.h"
#include "driver/i2c.h"

#define I2C_WRITE               (uint8_t)0x00
#define I2C_READ                (uint8_t)0x01

esp_err_t i2c_init_interface(uint8_t i2c_port_num, int scl_gpio, int sda_gpio, int i2c_clk)
{
    i2c_port_t i2c_port;
    switch (i2c_port_num) {
    case 0:
        i2c_port = I2C_NUM_0;
        break;

    case 1:
        i2c_port = I2C_NUM_1;
        break;

    default:
        return ESP_FAIL;
        break;
    }

    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_gpio,
        .scl_io_num = scl_gpio,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = i2c_clk,
    };

    esp_err_t result = i2c_param_config(i2c_port, &i2c_config);
    if (result != ESP_OK) {
        return result;
    }

    result = i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0);

    return result;
}

esp_err_t i2c_check(uint8_t i2c_port_num, uint8_t i2c_address)
{
    i2c_port_t i2c_port;
    switch (i2c_port_num) {
    case 0:
        i2c_port = I2C_NUM_0;
        break;

    case 1:
        i2c_port = I2C_NUM_1;
        break;

    default:
        return ESP_FAIL;
        break;
    }
    i2c_address = (i2c_address << 1) | I2C_WRITE;
    i2c_cmd_handle_t i2c_command = i2c_cmd_link_create();
    i2c_master_start(i2c_command);
    i2c_master_write_byte(i2c_command, i2c_address, true);
    i2c_master_stop(i2c_command);
    esp_err_t result = i2c_master_cmd_begin(i2c_port, i2c_command, 0);
    i2c_cmd_link_delete(i2c_command);

    return result;
}

esp_err_t i2c_write(uint8_t i2c_port_num, uint8_t i2c_address, uint8_t* data, size_t data_len)
{
    i2c_port_t i2c_port;
    switch (i2c_port_num) {
    case 0:
        i2c_port = I2C_NUM_0;
        break;

    case 1:
        i2c_port = I2C_NUM_1;
        break;

    default:
        return ESP_FAIL;
        break;
    }
    i2c_address = (i2c_address << 1) | I2C_WRITE;
    i2c_cmd_handle_t i2c_command = i2c_cmd_link_create();
    i2c_master_start(i2c_command);
    i2c_master_write_byte(i2c_command, i2c_address, true);
    i2c_master_write(i2c_command, data, data_len, true);
    i2c_master_stop(i2c_command);
    esp_err_t result = i2c_master_cmd_begin(i2c_port, i2c_command, 0);
    i2c_cmd_link_delete(i2c_command);

    return result;
}

esp_err_t i2c_read(uint8_t i2c_port_num, uint8_t i2c_address, uint8_t* data, size_t data_len)
{
    i2c_port_t i2c_port;
    switch (i2c_port_num) {
    case 0:
        i2c_port = I2C_NUM_0;
        break;

    case 1:
        i2c_port = I2C_NUM_1;
        break;

    default:
        return ESP_FAIL;
        break;
    }
    i2c_address = (i2c_address << 1) | I2C_READ;
    i2c_cmd_handle_t i2c_command = i2c_cmd_link_create();
    i2c_master_start(i2c_command);
    i2c_master_write_byte(i2c_command, i2c_address, true);
    i2c_master_read(i2c_command, data, data_len, I2C_MASTER_ACK);
    i2c_master_stop(i2c_command);
    esp_err_t result = i2c_master_cmd_begin(i2c_port, i2c_command, 0);
    i2c_cmd_link_delete(i2c_command);

    return result;
}