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
#include "asrutil_text_translate.h"

asrutil_ret test_read_from_data(char *path, char **data, int *len)
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
	char *from_data = NULL;
	int from_len = 0;
	char *to_data = NULL;
	int to_len = 0;
	asrutil_text_translate_mode mode = ASRUTIL_MODE_ZH_2_EN;

	if (argc < 3) {
		printf("Usage: %s mode textfile\n", argv[0]);
		printf("    mode: 0, zh to en; 1, en to zh\n");
		return ASRUTIL_RET_FAIL;
	}
	mode = (asrutil_text_translate_mode) atoi(argv[1]);

	asrutil_text_translate_init();

	test_read_from_data(argv[2], &from_data, &from_len);
	ASRUTIL_LOG("%s:%d, from_data: %s, from_len: %d, mode: %d\n", __FUNCTION__, __LINE__, from_data, from_len, mode);

	asrutil_text_translate_convert(from_data, from_len - 1, &to_data, &to_len, mode);
	ASRUTIL_LOG("%s:%d, to_data: %s, to_len: %d\n", __FUNCTION__, __LINE__, to_data, to_len);

	ASRUTIL_SAFE_FREE(from_data);
	ASRUTIL_SAFE_FREE(to_data);

	asrutil_text_translate_uninit();
	return 0;
}
