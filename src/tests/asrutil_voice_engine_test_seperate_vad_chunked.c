#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <errno.h>
#include <stdint.h>

#include "asrutil.h"
#include "asrutil_curl.h"
#include "asrutil_util.h"
#include "asrutil_voice_engine.h"

#define MIN(a, b) ((a) > (b) ? (b) : (a))

asrutil_ret test_read_voice_data(char *path, char **data, int *len)
{
	FILE *fp = NULL;
	int content_len = 0;
	int ret = 0;

	if (NULL == path || NULL == data || NULL == len) {
		ASRUTIL_LOG("%s:%d, bad parameter.\n", __FUNCTION__, __LINE__);
		return ASRUTIL_RET_FAIL;
	}
	fp = fopen(path, "r");
	if (NULL == fp) {
		return ASRUTIL_RET_FAIL;
	}
	fseek(fp, 0, SEEK_END);
	content_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	*data = (char *)calloc(content_len, 1);
	ret = fread(*data, content_len, sizeof(char), fp);
	if (0 == ret) {
		ASRUTIL_LOG("%s:%d, fread ret: %d.\n", __FUNCTION__, __LINE__, ret);
	}
	*len = content_len;
	fclose(fp);
	return ASRUTIL_RET_OK;
}

typedef struct asrutil_curl_data_chunked_t asrutil_curl_data_chunked;

struct asrutil_curl_data_chunked_t {
	char *buf;
	off_t len;
	char *p;
	int p_len;
};

static uint8_t vad_cnt = 0;

size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	int16_t *value = NULL;
	int vad_block_size = 0;
	int src_len = 0;

	asrutil_curl_data_chunked *param = (asrutil_curl_data_chunked *) userp;

	if ((size * nmemb < 1) || (param->p_len > param->len))
		return 0;

	value = (int16_t *) param->p;

	vad_block_size = MIN(param->len - param->p_len, ASRUTIL_VAD_BLOCK_SIZE);
	if (asrutil_vad_check(value, vad_block_size) == 0)
		vad_cnt++;
	else
		vad_cnt = 0;
	if (vad_cnt > ASRUTIL_VAD_COUNT_THRESHOLD) {
		ASRUTIL_LOG("%s:%d. vad_cnt reach threshold: %d.\n", __FUNCTION__, __LINE__, ASRUTIL_VAD_COUNT_THRESHOLD);
		vad_cnt = 0;
		return 0;
	}

	src_len = vad_block_size * 2;

	memcpy(ptr, param->p, src_len);
	param->p += src_len;
	param->p_len += src_len;
	return src_len;
}

int main(int argc, char *argv[])
{
	char *voice_data = NULL;
	int len = 0;
	char *zh_data = NULL;
	int zh_len = 0;

	if (argc < 3) {
		printf("Usage: %s voicefile language\n", argv[0]);
		printf("    language: 0, zh; 1, en\n");
		return ASRUTIL_RET_FAIL;
	}
	asrutil_voice_engine_init();
	asrutil_voice_engine_register();
	asrutil_voice_engine_set_lang(atoi(argv[2]));

	test_read_voice_data(argv[1], &voice_data, &len);
	ASRUTIL_LOG("%s:%d, voice_data: %p, len: %d\n", __FUNCTION__, __LINE__, voice_data, len);

	asrutil_curl_data_chunked readdata;

	readdata.buf = voice_data;
	readdata.len = len;
	readdata.p = voice_data;
	readdata.p_len = 0;

	while (readdata.len > readdata.p_len) {
		ASRUTIL_LOG("%s:%d, p: %p, p_len: %d\n", __FUNCTION__, __LINE__, readdata.p, readdata.p_len);
		asrutil_voice_engine_convert_chunked(read_callback, &readdata, &zh_data, &zh_len);
		ASRUTIL_LOG("%s:%d, zh_data: %s, zh_len: %d\n", __FUNCTION__, __LINE__, zh_data, zh_len);
		ASRUTIL_SAFE_FREE(zh_data);
	}
	ASRUTIL_SAFE_FREE(voice_data);
	asrutil_voice_engine_uninit();
	return 0;
}
