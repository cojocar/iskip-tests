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

#include "bsearch.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

/* fixed size in bytes of array */
#define ARRAY_NUM_BYTES 1024

/* number of elements from the array */
#define ARRAY_NUM_ELEMS (ARRAY_NUM_BYTES / sizeof (elem_type))

#define uart_rx_char my_uart_rx_char

enum {
	/* Crypto operations */
	CMD_RX_BUF 			= 'B',
	CMD_VERIFY 			= 'V',
	CMD_SEARCH_UNROLL 	= 'S',
	CMD_SEARCH 			= 's',
	CMD_ADD				= 'A',
};

elem_type bytes[ARRAY_NUM_ELEMS];

void
print_sum()
{
	size_t s = 0;
	unsigned i;

	for (i = 0; i < ARRAY_NUM_ELEMS; ++i)
		s += bytes[i];
	PRINTF("%u", s);
}

elem_type rx_elem()
{
	elem_type d = 0;
	size_t j;
	for (j = 0; j < sizeof (d); ++j) {
		d = (d << 8) | (uint8_t)uart_rx_char();
	}

	return d;
}

void
print_verify()
{
	unsigned i;

	for (i = 1; i < ARRAY_NUM_ELEMS; ++i) {
		if (bytes[i-1] > bytes[i]) {
			PRINTF("bad=bytes[%d] %d > %d", i, bytes[i-1], bytes[i]);
			return;
		}
	}
	PRINTS("OK");
}

int my_main(void)
{
	my_uart_init();

	BANNER;
	PRINTS("APP=bsearch\r\n");
	size_t i;
	elem_type x;

	PRINTF("elem_size is: %d", sizeof (elem_type));

	while (1) {
		unsigned command = uart_rx_char();
		switch (command) {
		case CMD_RX_BUF:
			for (i = 0; i < ARRAY_NUM_ELEMS; ++i) {
				bytes[i] = rx_elem();
				PRINTF("[%d]=%x\r\n", i, (uint32_t)bytes[i]);
			}
			print_sum();
			PRINTS("\r\n");
			break;

		case CMD_ADD:
			print_sum();
			PRINTS("\r\n");
			break;

		case CMD_VERIFY:
			print_verify();
			PRINTS("\r\n");
			break;

		case CMD_SEARCH:
			x = rx_elem();
			PRINTF("search(n)=0x%x", x);
			PRINTS("\r\n");
			if (!bsearch_normal(bytes, ARRAY_NUM_ELEMS, x)) {
				PRINTS(":FOUND");
			} else {
				PRINTS(":NOTFD");
			}
			PRINTS("\r\n");
			break;

		case CMD_SEARCH_UNROLL:
			x = rx_elem();
			PRINTF("search(u)=0x%x", x);
			PRINTS("\r\n");
			if (!bsearch_unrolled(bytes, ARRAY_NUM_ELEMS, x)) {
				PRINTS(":FOUND");
			} else {
				PRINTS(":NOTFD");
			}
			PRINTS("\r\n");
			break;

		default:
			PRINTF("Undefined UART command 0x%x\r\n", command);
			break;
		}
	}
	return 0;
}

int cmp(const void *a, const void *b)
{
	return *((elem_type *)a) - *((elem_type *)b);
}

uint8_t bsearch_normal(elem_type a[], uint32_t n, elem_type x)
{
	/*
	void *ret = bsearch(&x, &a[0], n, sizeof (a[0]), cmp);
	if (ret == NULL)
		return 1;
		*/
	int32_t l, r, m;
	l = 0;
	r = n-1;

	while (l <= r) {
		m = (l+r)/2;
		//PRINTF("l=%d r=%d m=%d\r\n", l, r, m);
		if (a[m] == x) {
			PRINTS("O");
			return 0;
		} else if (a[m] < x) {
			l = m+1;
			PRINTS("R");
		} else {
			r = m-1;
			PRINTS("L");
		}
	}
	PRINTS("N");
	return 1;
}
