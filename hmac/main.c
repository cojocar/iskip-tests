/*
 * Copyright 2016 The IskipTests Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "io.h"
#include "banner.h"
#include "cryptoc/hmac.h"

#include <stddef.h>
#include <stdint.h>

#define STR(s) #s
#define STR_VALUE(s) STR(s)

#define HMAC_SHA256_KEY_BYTES     32 /* Key is 32 bytes (8 words) */
#define HMAC_SHA256_DIGEST_BYTES  32 /* Digest is 32 bytes (8 words) */
#define HMAC_SHA256_MESSAGE_BYTES 32 /* Max message is fixed at 32 bytes */

#define LEN(array) ((sizeof(array))/(sizeof(array[0])))
#define uart_tx_char my_uart_tx_char
#define uart_rx_char my_uart_rx_char

static uint8_t g_hmac_key[HMAC_SHA256_KEY_BYTES];
static uint8_t g_hmac_msg[HMAC_SHA256_MESSAGE_BYTES];
/* static uint8_t g_hmac_out[HMAC_SHA256_DIGEST_BYTES]; */

static void uart_rx_buf(uint8_t *data, size_t len);
static void uart_tx_buf(const uint8_t *data, size_t len);

enum {
	/* Crypto operations */
	CMD_SET_HMAC_SHA256_KEY = 0x3A, /* ascii ':' */
	CMD_HMAC_SW_SHA256      = 0x3B,
	CMD_HMAC_SW_START       = 0x3C,
	CMD_HMAC_SW_SHA256_MULTI = '>',
	CMD_HMAC_SW_SHA256_SET_MULTI = '^',
};
#define FACTOR 100

static uint32_t rounds_cnt;

#define BOARD_GPIO_SET_PIN(p, dir) my_signal((((dir) & 0x1) << 4) | ((p) & 0xf))
#define BOARD_GPIO_UP(p) BOARD_GPIO_SET_PIN(p, 1)
#define BOARD_GPIO_DOWN(p) BOARD_GPIO_SET_PIN(p, 0)

int my_main(void)
{
	my_uart_init();
	unsigned i;
	rounds_cnt = FACTOR;

	BANNER;
	PRINTS("APP=hmac\r\n");

	LITE_HMAC_CTX ctx;
	const uint8_t *digest;

	while (1) {
		unsigned command = uart_rx_char();
		switch (command) {
		case CMD_SET_HMAC_SHA256_KEY:
			uart_rx_buf(g_hmac_key, LEN(g_hmac_key));
			break;

		case CMD_HMAC_SW_SHA256:
			uart_rx_buf(g_hmac_msg, LEN(g_hmac_msg));
			break;

		case CMD_HMAC_SW_START: /* 3c */

			BOARD_GPIO_UP(2);
			HMAC_SHA256_init(&ctx, g_hmac_key, LEN(g_hmac_key));

			BOARD_GPIO_UP(1);
			HMAC_update(&ctx, g_hmac_msg, LEN(g_hmac_msg));

			BOARD_GPIO_UP(0);
			digest = HMAC_final(&ctx);

			BOARD_GPIO_DOWN(0);
			BOARD_GPIO_DOWN(1);
			BOARD_GPIO_DOWN(2);

			uart_tx_buf(digest, HMAC_SHA256_DIGEST_BYTES);
			break;

		case CMD_HMAC_SW_SHA256_MULTI:
			uart_rx_buf(g_hmac_msg, LEN(g_hmac_msg));

			for (i = 0; i < rounds_cnt; ++i) {
				HMAC_SHA256_init(&ctx, g_hmac_key, LEN(g_hmac_key));
				HMAC_update(&ctx, g_hmac_msg, LEN(g_hmac_msg));
				digest = HMAC_final(&ctx);
			}

			uart_tx_buf(digest, HMAC_SHA256_DIGEST_BYTES);
			break;

		case CMD_HMAC_SW_SHA256_SET_MULTI:
			command = uart_rx_char();
			rounds_cnt = FACTOR * command;
			uart_tx_buf((const uint8_t *)&rounds_cnt, sizeof (rounds_cnt));
			break;

		default:
			PRINTF("Undefined UART command %d\r\n", command);
			break;
		}
	}
}

static void uart_rx_buf(uint8_t *data, size_t len)
{
	while (len-- > 0) {
		*data = uart_rx_char();
		data++;
	}
}

static void uart_tx_buf(const uint8_t *data, size_t len)
{
	while (len-- > 0) {
		uart_tx_char(*data);
		data++;
	}
}


void *__aeabi_memcpy(void *dst, void *src, size_t len)
{
	size_t i;
	for (i = 0; i < len; ++i) {
		((char *)dst)[i] = ((char *)src)[i];
	}
	return dst;
}

