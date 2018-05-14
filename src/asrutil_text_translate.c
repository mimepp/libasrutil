#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <errno.h>
#include <openssl/md5.h>

#define __STRICT_ANSI__
#include <json-c/json.h>

#include "asrutil.h"
#include "asrutil_curl.h"
#include "asrutil_util.h"

asrutil_ret asrutil_text_translate_init()
{
	return ASRUTIL_RET_OK;
}

asrutil_ret asrutil_text_translate_uninit()
{
	return ASRUTIL_RET_OK;
}

asrutil_ret asrutil_text_translate_convert(char *from_data, int len, char **to_data, int *to_data_len, asrutil_text_translate_mode mode)
{
	char url[ASRUTIL_MAX_BUF] = { 0 };
	char tmp[ASRUTIL_MAX_BUF] = { 0 };
	char *response = NULL;
	json_object *jobj = NULL;
	json_object *tmp_jobj;
	char sign[ASRUTIL_MAX_BUF] = { 0 };
	char salt[ASRUTIL_MAX_BUF] = { 0 };
	int a = rand();
	unsigned char md[16] = { 0 };
	int i = 0;
	char buf[33] = { 0 };
	char *from_data_escaped = NULL;
	int from_data_escaped_len = 0;
	char *from_data_new = NULL;

	if (NULL == from_data || NULL == to_data || NULL == to_data_len) {
		ASRUTIL_LOG("%s:%d, bad parameter.\n", __FUNCTION__, __LINE__);
		return ASRUTIL_RET_FAIL;
	}

	from_data_new = calloc(len, 1);
	snprintf(from_data_new, len, "%s", from_data);
	ASRUTIL_LOG("from_data_new: %s, len: %d\n", from_data_new, len);

	if (ASRUTIL_MODE_EN_2_ZH == mode) {
		asrutil_escape_url(from_data_new, &from_data_escaped, &from_data_escaped_len);
		ASRUTIL_LOG("from_data_escaped: %s, %d\n", from_data_escaped, from_data_escaped_len);
	} else {
		from_data_escaped = from_data_new;
		from_data_escaped_len = len;
	}
	snprintf(salt, sizeof(salt), "%d", a);
	strcat(sign, ASRUTIL_TEXT_TRANSLATE_KEY);
	strncat(sign, from_data_new, len);
	strcat(sign, salt);
	strcat(sign, ASRUTIL_TEXT_TRANSLATE_SECRET);
	ASRUTIL_LOG("sign string: %s\n", sign);

	MD5((const unsigned char *)sign, strlen(sign), md);
	for (i = 0; i < 16; i++) {
		snprintf(tmp, sizeof(tmp), "%2.2x", md[i]);
		strcat(buf, tmp);
	}
	ASRUTIL_LOG("sign result: %s\n", buf);
	snprintf(url, sizeof(url), "%s", ASRUTIL_API_TEXT_TRANSLATE_URL_BASE);
	strcat(url, "appid=");
	strcat(url, ASRUTIL_TEXT_TRANSLATE_KEY);
	strcat(url, "&q=");
	strncat(url, from_data_escaped, from_data_escaped_len);
	if (ASRUTIL_MODE_EN_2_ZH == mode) {
		ASRUTIL_SAFE_FREE(from_data_escaped);
	}
	ASRUTIL_SAFE_FREE(from_data_new);

	strcat(url, "&from=");
	if (ASRUTIL_MODE_ZH_2_EN == mode) {
		strcat(url, ASRUTIL_TEXT_TRANSLATE_ID_ZH);
	} else {
		strcat(url, ASRUTIL_TEXT_TRANSLATE_ID_EN);
	}
	strcat(url, "&to=");
	if (ASRUTIL_MODE_ZH_2_EN == mode) {
		strcat(url, ASRUTIL_TEXT_TRANSLATE_ID_EN);
	} else {
		strcat(url, ASRUTIL_TEXT_TRANSLATE_ID_ZH);
	}
	strcat(url, "&salt=");
	strcat(url, salt);
	strcat(url, "&sign=");
	strcat(url, buf);

	ASRUTIL_LOG("%s:%d, url: %s\n", __FUNCTION__, __LINE__, url);

	response = asrutil_curl_fetch_url(url);

	if (NULL == response) {
		ASRUTIL_LOG("%s:%d, return error.\n", __FUNCTION__, __LINE__);
		return ASRUTIL_RET_FAIL;
	}
	ASRUTIL_LOG("response: %s\n", response);

	jobj = json_tokener_parse(response);
	if (NULL == jobj || is_error(jobj)) {
		ASRUTIL_LOG("%s:%d, json_tokener_parse return error.\n", __FUNCTION__, __LINE__);
		ASRUTIL_SAFE_FREE(response);
		return ASRUTIL_RET_FAIL;
	}
	json_object_object_foreach(jobj, key, val) {
		if (!strcmp(key, ASRUTIL_ID_TRANS_RESULT)) {
			if (json_type_array == json_object_get_type(val)) {
				int j = 0, array_len = 0;

				array_len = json_object_array_length(val);
				for (j = 0; j < array_len; j++) {
					tmp_jobj = json_object_array_get_idx(val, j);
					if (tmp_jobj && !is_error(tmp_jobj)) {
						json_object_object_foreach(tmp_jobj, key2, val2) {
							if (!strcmp(key2, ASRUTIL_ID_TRANS_DST)) {
								if (json_type_string == json_object_get_type(val2)) {
									char
									*p = strdup(json_object_get_string(val2));
									int p_len = strlen(p) + 1;

									*to_data = p;
									*to_data_len = p_len;
									ASRUTIL_LOG
									    ("%s:%d, to_data: %s, to_data_len: %d\n",
									     __FUNCTION__, __LINE__, *to_data, *to_data_len);
								}
							}
						}

					}
				}
			}
		}
	}
	json_object_put(jobj);
	ASRUTIL_SAFE_FREE(response);

	return ASRUTIL_RET_OK;
}
