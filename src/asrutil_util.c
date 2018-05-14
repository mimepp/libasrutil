#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include "asrutil.h"
#include "asrutil_util.h"

asrutil_ret asrutil_escape_url(const char *in, char **out, int *out_size)
{
	CURL *curl = curl_easy_init();
	if (curl) {
		char *output = curl_easy_escape(curl, in, 0);
		if (output) {
			*out = output;
			*out_size = strlen(output);
		}
		curl_easy_cleanup(curl);
	} else {
		return ASRUTIL_RET_FAIL;
	}
	return ASRUTIL_RET_OK;
}

static int32_t sign(int32_t a)
{
	if (a > 0)
		return 1;
	else
		return 0;
}

uint8_t asrutil_vad_check(int16_t * data, uint32_t length)
{
	int sum = 0;
	int delta_sum = 0;
	int i = 0;
	for (i = 0; i < length - 1; i++) {
		if (sign(data[i]) != sign(data[i + 1])) {
			delta_sum++;
		}
		//      printf("%d ",data[i] );
		sum += abs(data[i]);
	}
	if (sum > 150000 && delta_sum < 150) {
		return 1;	// find voice content
	} else {
		return 0;
	}
}
