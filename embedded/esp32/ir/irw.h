#pragma once


#include "driver/rmt_rx.h"
#include "driver/rmt_tx.h"

#define MAX_RAW_SYMBOLS (512U)

typedef struct
{
    rmt_symbol_word_t symbols[MAX_RAW_SYMBOLS];
    size_t length;
} ir_raw_packet_t;

rmt_channel_handle_t ir_raw_rx_init(int gpio_num);
rmt_channel_handle_t ir_raw_tx_init(int gpio_num);

esp_err_t ir_raw_receive(rmt_channel_handle_t rx_chan, ir_raw_packet_t *out_data, TickType_t wait_ticks);
esp_err_t ir_raw_send(rmt_channel_handle_t tx_chan, ir_raw_packet_t *data);

