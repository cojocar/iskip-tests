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
#ifndef __BANNER_H__
#define __BANNER_H__ 1

#define STR(s) #s
#define STR_VALUE(s) STR(s)

#ifdef __arm__
#define PROFILE_STR "arm " STR_VALUE(__ARM_ARCH_PROFILE) "." STR_VALUE(__ARM_ARCH) "." STR_VALUE(__ARM_ARCH_7M__)
#else
#define PROFILE_STR "not arm"
#endif

#ifdef __clang__
#define BANNER do {                                      \
	PRINTS("Clang version: "__clang_version__ "\r\n");   \
	PRINTF("Build date: " __DATE__ " " __TIME__ "\r\n"); \
	PRINTS("Profile: " PROFILE_STR "\r\n");              \
} while (0)
#else
#define BANNER do {                                      \
	PRINTS("GCC version: " __VERSION__ "\r\n");          \
	PRINTF("Build date: " __DATE__ " " __TIME__ "\r\n"); \
} while (0)
#endif

#endif
