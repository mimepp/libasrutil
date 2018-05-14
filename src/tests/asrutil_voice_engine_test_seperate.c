#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <errno.h>

#include "asrutil.h"
#include "asrutil_curl.h"
#include "asrutil_util.h"
#include "asrutil_voice_engine.h"

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

int main(int argc, char *argv[])
{
	char *voice_data = NULL;
	int len = 0;
	char *zh_data = NULL;
	int zh_len = 0;
	int block_size = 40960;	// 40k, about 3 seconds
	int p_len = 0;
	char *p = NULL;
	int src_len = 0;

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
	p = voice_data;
	while (p_len < len) {
		src_len = (len - p_len) > block_size ? block_size : (len - p_len);
		asrutil_voice_engine_convert(p, src_len, &zh_data, &zh_len);
		ASRUTIL_LOG("%s:%d, zh_data: %s, zh_len: %d\n", __FUNCTION__, __LINE__, zh_data, zh_len);
		ASRUTIL_SAFE_FREE(zh_data);
		p += block_size;
		p_len += block_size;
	}

	ASRUTIL_SAFE_FREE(voice_data);
	asrutil_voice_engine_uninit();
	return 0;
}
