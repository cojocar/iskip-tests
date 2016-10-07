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
#include "mini-printf.h"

#include <stdint.h>

extern char printf_buffer;
extern uint32_t printf_buffer_len;

void my_uart_prints(const char *s)
{
	while (*s) {
		my_uart_tx_char(*s);
		++s;
	}
}

void my_uart_printf(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	mini_vsnprintf(&printf_buffer, printf_buffer_len, (char *)fmt, va);
	va_end(va);
	my_uart_prints(&printf_buffer);
}
