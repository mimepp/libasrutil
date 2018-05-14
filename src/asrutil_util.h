#ifndef	__ASRUTIL_UTIL_H__
#define	__ASRUTIL_UTIL_H__

#include <stdio.h>
#include <stdint.h>
#include "asrutil.h"

asrutil_ret asrutil_escape_url(const char *in, char **out, int *out_size);
uint8_t asrutil_vad_check(int16_t * data, uint32_t lenght);
#endif
