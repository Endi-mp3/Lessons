idf_component_register(SRCS "main.c" "my_ir.c" "my_ir.h" "my_network.c" "my_network.h"
					REQUIRES esp_driver_rmt
					REQUIRES esp_event
					REQUIRES nvs_flash
					REQUIRES esp_netif
                    INCLUDE_DIRS ".")
