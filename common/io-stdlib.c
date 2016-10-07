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

#include <stdio.h>
#include <stdint.h>

static FILE *out_file;
static FILE *in_file;

#define PRINTF_BUFFER_LEN 1024

uint32_t printf_buffer_len = PRINTF_BUFFER_LEN;
char printf_buffer[PRINTF_BUFFER_LEN];

unsigned my_uart_rx_char(void)
{
	return getc(in_file);
}

void my_uart_tx_char(char c)
{
	putc(c, out_file);
	fflush(out_file);
}

void my_uart_init()
{
	out_file = stdout;
	in_file = stdin;
}

void my_signal(unsigned s)
{
	/* do nothing */
}
