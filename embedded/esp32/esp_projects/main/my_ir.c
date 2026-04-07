#include "my_ir.h"
#include "esp_log.h"
#include "driver/rmt_rx.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"
#include "driver/rmt_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "IR_FILTER";

#define IR_GLITCH_FILTER_NS  (200000)   /* Игнорировать импульсы короче 200 мкс (типичный мусор) */
#define MIN_SIGNAL_SYMBOLS   (10)       /* Игнорировать пакеты короче 10 перепадов (случайные всплески) */

static rmt_rx_done_event_data_t rx_data;
static QueueHandle_t receive_queue;

static bool rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    // send the received RMT symbols to the parser task
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

rmt_channel_handle_t ir_raw_rx_init(int gpio_num)
{
    rmt_channel_handle_t rx_chan = NULL;

    rmt_rx_channel_config_t rx_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1us
        .gpio_num = gpio_num,
        .mem_block_symbols = 128,
        .flags.invert_in = true, // Для VS1838B
        .flags.with_dma = false, // Для VS1838B
    };
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_config, &rx_chan));

    receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    if (receive_queue == NULL) {
		ESP_LOGE(TAG, "Receive queue creation failed\n");
		while(1) { }
	}
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = rmt_rx_done_callback,
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_chan, &cbs, receive_queue));
	ESP_LOGI(TAG, "register RX done callback");
    ESP_ERROR_CHECK(rmt_enable(rx_chan));
	ESP_LOGI(TAG, "rmt_enable rx_chan done");
    return rx_chan;
}

rmt_channel_handle_t ir_raw_tx_init(int gpio_num)
{
    rmt_channel_handle_t tx_chan = NULL;
    rmt_tx_channel_config_t tx_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = gpio_num,
        .mem_block_symbols = 128,
        .resolution_hz = 1000000,
        .trans_queue_depth = 4,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_config, &tx_chan));
    rmt_carrier_config_t carrier_cfg = {
		.duty_cycle = 0.33,
		.frequency_hz = 38000
	};

    ESP_ERROR_CHECK(rmt_apply_carrier(tx_chan, &carrier_cfg));
    ESP_ERROR_CHECK(rmt_enable(tx_chan));
    return tx_chan;
}

esp_err_t ir_raw_receive(rmt_channel_handle_t rx_chan, ir_raw_packet_t *out_data)
{
	static int recv_started = 0;
    // signal_range_max_ns = 10мс (если тишина дольше - считаем пакет завершенным)
    rmt_receive_config_t recv_cfg = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 10000000
    };
    if (recv_started == 0 && rmt_receive(rx_chan, out_data->symbols, sizeof(out_data->symbols), &recv_cfg) == ESP_OK) {
		recv_started = 1;
    }

	if (recv_started) {
		if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(100)) == pdPASS) {
			memcpy(out_data->symbols, rx_data.received_symbols, rx_data.num_symbols);
			out_data->length = rx_data.num_symbols;
			ESP_LOGI(TAG, "Signal recv: %d symbols", out_data->length);
			recv_started = 0;
			return ESP_OK;
		}
		else {
			ESP_LOGI(TAG, "Empty");
		}
	}
    return ESP_FAIL;
}

esp_err_t ir_raw_send(rmt_channel_handle_t tx_chan, ir_raw_packet_t *data)
{
    rmt_transmit_config_t tx_cfg = {
		.loop_count = 0
	};
    return rmt_transmit(tx_chan, NULL, data->symbols, data->length, &tx_cfg);
}
