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

#define AES_ECB_enc AES128_ECB_encrypt
#define AES_ECB_dec AES128_ECB_decrypt

#include "aes.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define LEN(array) ((sizeof(array))/(sizeof(array[0])))
#define uart_tx_char my_uart_tx_char
#define uart_rx_char my_uart_rx_char

#define uart_printf PRINTF

//#define AES_DEBUG
#ifdef AES_DEBUG
#define uart_printf_debug uart_printf
#define uart_tx_buf_debug(...) uart_tx_buf
#else
#define uart_printf_debug(...) do {} while (0)
#define uart_tx_buf_debug(...) do {} while (0)
#endif

static void uart_rx_buf(uint8_t *data, size_t len);
static void uart_tx_buf(const uint8_t *data, size_t len);

static void dump_aes_buffers();
static void incr_aes_buf_enc_in();

// key size
// AES-128 4 words = 16 bytes
// AES-256 8 words = 32 bytes
#define AES_KEY_BYTES     32
#define AES_BUF_IN_BYTES  16
#define AES_BUF_OUT_BYTES  16

#define BOARD_GPIO_SET_PIN(p, dir) my_signal((((dir) & 0x1) << 4) | ((p) & 0xf))
#define BOARD_GPIO_UP(p) BOARD_GPIO_SET_PIN(p, 1)
#define BOARD_GPIO_DOWN(p) BOARD_GPIO_SET_PIN(p, 0)

static uint8_t aes_key[AES_KEY_BYTES] = 
    { 0xCA, 0xFE, 0xBA, 0xBE, 0xDE, 0xAD, 0xBE, 0xEF,
      0xFA, 0xCE, 0xB0, 0x0C, 0xDE, 0xFE, 0xA7, 0xED,
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0xBA, 0xAD, 0xF0, 0x0D, 0x60, 0x0D, 0xAB, 0xBA}; 

static uint8_t aes_buf_enc_in[AES_BUF_IN_BYTES] = 
    { 'A','A','A','A','A','A','A','A',
      'A','A','A','A','A','A','A','A'};
static uint8_t aes_buf_enc_out[AES_BUF_OUT_BYTES] =
    { 'B','B','B','B','B','B','B','B',
      'B','B','B','B','B','B','B','B' };
static uint8_t aes_buf_dec_in[AES_BUF_IN_BYTES] =
    { 'C','C','C','C','C','C','C','C',
      'C','C','C','C','C','C','C','C' };
static uint8_t aes_buf_dec_out[AES_BUF_OUT_BYTES] =
    { 'D','D','D','D','D','D','D','D',
      'D','D','D','D','D','D','D','D' };

enum {
	// Crypto operation commands
	CMD_AES_SET_KEY    = ':', // define key
	CMD_AES_EDIN       = '*', // data in for encryption
	CMD_AES_EDIN_INC   = '+', // increment encoder input data buffer entries
	CMD_AES_DDIN       = '@', // data in for decryption
	CMD_AES_EDOUT_DDIN = '%', // copy encrypt out data buffer to decrypt in buffer
	CMD_AES_ENCODE     = '>', // encode input data
	CMD_AES_DECODE     = '<', // decode data from encoder output
	CMD_AES_CHECK      = '=', // check dec(enc(din)) == din
	CMD_AES_DUMP       = '!', // display state of key and data buffers
	CMD_AES_SET_MULTI  = 'F', // set the factor
	CMD_AES_ENCODE_MULTI= 'M', // do multiple rounds of AES
	CMD_HELP           = '?',
};
#define FACTOR 100

static uint32_t rounds_cnt;

#define __WAIT __wait
void __wait(void)
{
	unsigned __cnt;
	//for (__cnt = 0; __cnt < 10000; ++__cnt) {
	for (__cnt = 0; __cnt < 100; ++__cnt) {
		asm volatile ("nop");
	}
}

int my_main(void)
{
	my_uart_init();
	unsigned i;
	rounds_cnt = FACTOR;

	BANNER;
	PRINTS("APP=aes\r\n");
	while (1) {
		// get command character from UART receiver
		unsigned command = uart_rx_char();
		switch (command) {
		case CMD_AES_DUMP:
			dump_aes_buffers();
			break;
		case CMD_AES_SET_KEY:
			uart_printf_debug("\nenter 128-bit key:\n");
			uart_rx_buf(aes_key, 16);  // get AES-128 16 byte key from UART
#ifdef PROTO_DEBUG
			/* XXX: always echo back the key */
			uart_tx_buf(aes_key, 16); // echo
#endif
			break;
		case CMD_AES_EDIN:
			uart_printf_debug("\nenter input encode data:\n");
			uart_rx_buf(aes_buf_enc_in, 16);
#ifdef PROTO_DEBUG
			uart_tx_buf_debug(aes_buf_enc_in, 16); // echo
#endif
			break;
		case CMD_AES_DDIN:
			uart_printf_debug("\nenter input decode data:\n");
			uart_rx_buf(aes_buf_dec_in, 16);
#ifdef PROTO_DEBUG
			uart_tx_buf_debug(aes_buf_dec_in, 16);
#endif
			break;
		case CMD_AES_EDIN_INC:
			uart_printf_debug("\nincrement encode data\n");
			incr_aes_buf_enc_in();
			break;
		case CMD_AES_EDOUT_DDIN:
			uart_printf_debug("\ncopy encode data out to decode data in buffer\n");
			memcpy(aes_buf_dec_in, aes_buf_enc_out, 16);
			break;

		case CMD_AES_SET_MULTI:
			uart_printf_debug("\nset multi\n");
			command = uart_rx_char();
			rounds_cnt = FACTOR * command;
			uart_tx_buf((const uint8_t *)&rounds_cnt, sizeof (rounds_cnt));
			break;

		case CMD_AES_ENCODE_MULTI:
			uart_printf_debug("\nECB encrypt - multi\n");
			for (i = 0; i < rounds_cnt; ++i) {
				AES_ECB_enc(aes_buf_enc_in, aes_key, aes_buf_enc_out);
			}
			uart_tx_buf(aes_buf_enc_out, 16);
			break;

		case CMD_AES_ENCODE:
			uart_printf_debug("\nECB encrypt - start\n");

			//__WAIT;
			BOARD_GPIO_UP(2);
			AES_ECB_enc(aes_buf_enc_in, aes_key, aes_buf_enc_out);
			BOARD_GPIO_DOWN(2);

			uart_tx_buf(aes_buf_enc_out, 16);  // dump output buffer
			uart_printf_debug("ECB encrypt - done \n");
			break;
		case CMD_AES_DECODE:
			uart_printf_debug("\nECB decrypt - start\n");

			//__WAIT;
			BOARD_GPIO_UP(2);
			AES_ECB_dec(aes_buf_dec_in, aes_key, aes_buf_dec_out);
			BOARD_GPIO_DOWN(2);

			uart_tx_buf(aes_buf_dec_out, 16);  // dump output buffer

			uart_printf_debug("ECB decrypt - done\n");
			break;
		case CMD_AES_CHECK:
			uart_printf("\nenter input encode data for check:\n");
			uart_rx_buf(aes_buf_enc_in, 16);
			uart_tx_buf(aes_buf_enc_in, 16);
			uart_printf("\nECB check encrypt - decrypt sequence\n");

			AES_ECB_enc(aes_buf_enc_in, aes_key, aes_buf_enc_out);
			AES_ECB_dec(aes_buf_enc_out, aes_key, aes_buf_dec_out);

			if (0 == memcmp((uint8_t *) aes_buf_enc_in, 
						(uint8_t *) aes_buf_dec_out, 16)) {
				uart_printf("PASS\n");
			} else {
				uart_printf("FAIL\n");
			}
			break;
		case CMD_HELP:
			uart_printf(": define key\
					\n* data in for encryption\
					\n+ increment encoder input data buffer entries\
					\n@ data in for decryption\
					\n% copy encrypt out data buffer to decrypt in buffer\
					\n> encode input data\
					\n< decode data from encoder output\
					\n= check dec(enc(din)) == din\
					\n! dump state of key and data buffers\
					\n? help\n");
			break;
		default:
			uart_printf("Error: Undefined UART command %x\n", command);
			break;
		}
		uart_printf_debug("Done. Enter next command\n");
	}

	return 0;
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


static void dump_aes_buffers()
{
	uart_printf("aes_key: ");
	uart_tx_buf(aes_key, 16);
	uart_printf("\n");
	uart_printf("aes_buf_enc_in: ");
	uart_tx_buf(aes_buf_enc_in, 16);
	uart_printf("\n");
	uart_printf("aes_buf_enc_out: ");
	uart_tx_buf(aes_buf_enc_out, 16);
	uart_printf("\n");
	uart_printf("aes_buf_dec_in: ");
	uart_tx_buf(aes_buf_dec_in, 16);
	uart_printf("\n");
	uart_printf("aes_buf_dec_out: ");
	uart_tx_buf(aes_buf_dec_out, 16);
	uart_printf("\n");
}

static void incr_aes_buf_enc_in()
{
	int i;
	for (i=0; i<16; i++) {
		aes_buf_enc_in[i]++;
	}
}

