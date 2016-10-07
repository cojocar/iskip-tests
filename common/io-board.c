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
//#include "uart.h"

extern unsigned uart_rx_char(void);
extern void uart_tx_char(char);
extern void uart_init(void);

#include <stdint.h>

#define PRINTF_BUFFER_LEN 512

uint32_t printf_buffer_len = PRINTF_BUFFER_LEN;
char printf_buffer[PRINTF_BUFFER_LEN];

unsigned my_uart_rx_char(void)
{
	return uart_rx_char();
}

void my_uart_tx_char(char c)
{
	uart_tx_char(c);
}

void my_uart_init()
{
	uart_init();
}

extern void write_gpio_bit(unsigned whichgpio, unsigned whichbit, unsigned value);

void my_signal(unsigned s)
{
	unsigned pin = s & 0xf;
	unsigned is_up = !! (s & 0x10);
	write_gpio_bit(0, pin, is_up);
}
