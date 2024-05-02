idf_component_register(SRCS "pms_main.c"
                        PRIV_REQUIRES driver i2c_driver pms_adc nvs_flash esp_timer)
