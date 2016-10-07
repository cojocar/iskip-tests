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

#include <stdint.h>

int my_main(void)
{
	int echo_enabled = 1;
	my_uart_init();

	BANNER;
	PRINTS("APP=hello-test\r\n");

	while (1) {
		uint32_t x;
		PRINTS("MAGIC=");

		x = my_uart_rx_char();
		if (echo_enabled)
			my_uart_tx_char(x);

		if (x == 'X' || x == 'x')  {
			PRINTS("MAGIC=Bye!\r\n");
			break;
		} else if (x == 'B') {
			PRINTS("MAGIC=B\r\n");
		} else if (x == 'D')  {
			PRINTS("MAGIC=D\r\n");
		} else if (x <= '9'  && x >= '0') {
			PRINTS("MAGIC=number\r\n");
		} else if (x <= 'z'  && x >= 'a') {
			PRINTS("MAGIC=lowercase\r\n");
		} else if (x <= 'Z'  && x >= 'A') {
			PRINTS("MAGIC=uppercase\r\n");
		} else if (x == '@') {
			echo_enabled = !echo_enabled;
			PRINTF("ECHO_ENABLED=%d\r\n", echo_enabled);
		} else {
			PRINTS("MAGIC=?\r\n");
		}
	}
	PRINTS("DONE\r\n");
	return 0;
}
