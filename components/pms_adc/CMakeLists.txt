idf_component_register(SRCS "pms_adc.c"
			INCLUDE_DIRS "include"
                        PRIV_REQUIRES esp_timer i2c_driver freertos)
