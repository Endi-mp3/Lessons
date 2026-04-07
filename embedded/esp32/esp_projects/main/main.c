#include "my_network.h"
#include "my_ir.h"


static ir_raw_packet_t lvIR_Recv_Packet = { 0 };
static rmt_channel_handle_t lvIR_Recv_Hdlr;

void app_main(void)
{
	esp_err_t lvError;
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
//    ESP_ERROR_CHECK(example_connect());

	lvIR_Recv_Hdlr = ir_raw_rx_init(2);

	while(1 && lvIR_Recv_Hdlr) {
		lvError = ir_raw_receive(lvIR_Recv_Hdlr, &lvIR_Recv_Packet);
		if (lvError == ESP_OK && lvIR_Recv_Packet.length) {
			ESP_LOGI("app_main", "recv:");
			for(int i = 0; i < lvIR_Recv_Packet.length; i++)
				printf("%x ", (unsigned int)lvIR_Recv_Packet.symbols[i].val);
			printf("\n");
		}
	}
	/*
#ifdef CONFIG_EXAMPLE_TCP_SERVER
    SemaphoreHandle_t server_ready = xSemaphoreCreateBinary();
    assert(server_ready);
    xTaskCreate(tcp_server_task, "tcp_server", 4096, &server_ready, 5, NULL);
    xSemaphoreTake(server_ready, portMAX_DELAY);
    vSemaphoreDelete(server_ready);
#endif // CONFIG_EXAMPLE_TCP_SERVER

#ifdef CONFIG_EXAMPLE_TCP_CLIENT
    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
#endif // CONFIG_EXAMPLE_TCP_CLIENT
	*/
}
