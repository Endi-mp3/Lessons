#include "irw.h"
#include "esp_log.h"
#include "driver/rmt_rx.h"

static const char *TAG = "IR_FILTER";

#define IR_GLITCH_FILTER_NS  (200000)   /* Игнорировать импульсы короче 200 мкс (типичный мусор) */
#define MIN_SIGNAL_SYMBOLS   (10)       /* Игнорировать пакеты короче 10 перепадов (случайные всплески) */

rmt_channel_handle_t ir_raw_rx_init(int gpio_num)
{
    rmt_channel_handle_t rx_chan = NULL;

    rmt_rx_channel_config_t rx_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1us
        .gpio_num = gpio_num,
        .mem_block_symbols = 128,
        .flags.invert_out = true, // Для VS1838B
    };
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_config, &rx_chan));

    rmt_rx_tool_config_t filter_config = {
        .filter_threshold_ns = IR_GLITCH_FILTER_NS,
    };
    ESP_ERROR_CHECK(rmt_apply_rx_filter(rx_chan, &filter_config));

    ESP_ERROR_CHECK(rmt_enable(rx_chan));
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

esp_err_t ir_raw_receive_clean(rmt_channel_handle_t rx_chan, ir_raw_packet_t *out_data)
{
    rmt_rx_done_event_data_t rx_data;
    // signal_range_max_ns = 10мс (если тишина дольше - считаем пакет завершенным)
    rmt_receive_config_t recv_cfg = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 10000000
    };

    if (rmt_receive(rx_chan, out_data->symbols, sizeof(out_data->symbols), &recv_cfg) == ESP_OK) {

        if (out_data->length < MIN_SIGNAL_SYMBOLS) {
            ESP_LOGD(TAG, "Noize suppression (len: %d)", out_data->length);
            return ESP_ERR_INVALID_SIZE;
        }

        ESP_LOGI(TAG, "Signal recv: %d symbols", out_data->length);
        return ESP_OK;
    }

    return ESP_FAIL;
}

esp_err_t ir_raw_send(rmt_channel_handle_t tx_chan, ir_raw_packet_t *data)
{
    rmt_transmit_config_t tx_cfg = {
		.loop_count = 0
	};
    return rmt_transmit(tx_chan, data->symbols, data->length, &tx_cfg);
}
