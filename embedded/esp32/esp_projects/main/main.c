#include "my_network.h"
#include "my_ir.h"
#include "my_socket_proto.h"


static ir_raw_packet_t lvIR_Recv_Packet = { 0 };
static rmt_channel_handle_t lvIR_Recv_Hdlr;

int callback_on_receive(char* buffer, uint32_t* length)
{
	ESP_LOGI("app_main", "recv:");
	for(int i = 0; i < *length; i++)
		printf("%02x ", buffer[i]);
	printf("\n");
	struct Packet *pkt = (struct Packet *)buffer;

	ESP_LOGI("app_main", "Here %i", __LINE__);
	switch (pkt->header.cmd)
	{
	case my_sock_cmd_slot_assign:

	ESP_LOGI("app_main", "Here %i", __LINE__);
		// pseudo code
		// form success answer package
		// form error answer package
		//
		break;
	case my_sock_cmd_slot_clean_all:
	ESP_LOGI("app_main", "Here %i", __LINE__);
		pkt->data[0] = my_sock_err_ok;
	ESP_LOGI("app_main", "Here %i", __LINE__);
		*length = sizeof(struct Packet) + 1;
	ESP_LOGI("app_main", "Here %i", __LINE__);
		break;
	default:
		break;
	}

	ESP_LOGI("app_main", "Here %i", __LINE__);

	// обработка сигналов  (switch)
	return 0;
}
SemaphoreHandle_t server_ready;
TcpServerParameters params;
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
    ESP_ERROR_CHECK(example_connect());

#if 0
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
#endif

	server_ready = xSemaphoreCreateBinary();
	assert(server_ready);
	params.callback = callback_on_receive;
	params.other = &server_ready;
	xTaskCreate(tcp_server_task, "tcp_server", 4096, &params, 5, NULL);
	xSemaphoreTake(server_ready, portMAX_DELAY);
	vSemaphoreDelete(server_ready);
/*
#ifdef CONFIG_EXAMPLE_TCP_CLIENT
    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
#endif // CONFIG_EXAMPLE_TCP_CLIENT
*/
}
