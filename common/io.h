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
#ifndef __IO_H__

unsigned my_uart_rx_char(void) __attribute__((noinline));
void my_uart_tx_char(char c) __attribute__((noinline));
void my_uart_init() __attribute__((noinline));
void my_signal(unsigned) __attribute__((noinline));

/* common stuff */
void my_uart_printf(const char* fmt, ...);
void my_uart_prints(const char *s);

#define PRINTF(...) my_uart_printf(__VA_ARGS__)
#define PRINTS(s) my_uart_prints(s)

#endif
