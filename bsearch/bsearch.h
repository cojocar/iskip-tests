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
#ifndef __BSEARCH_H__
#define __BSEARCH_H__ 1

#include <stdint.h>

#define MY_PRINTS_FOUND PRINTS
#define MY_PRINTS PRINTS
#define MY_PRINTS_NOTFOUND PRINTS

#ifndef elem_type
#define elem_type uint8_t
#endif

uint8_t bsearch_unrolled(elem_type a[], uint32_t n, elem_type x);
uint8_t bsearch_normal(elem_type a[], uint32_t n, elem_type x);

#endif
