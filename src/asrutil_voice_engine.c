#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <errno.h>

#define __STRICT_ANSI__
#include <json-c/json.h>

#include "asrutil.h"
#include "asrutil_curl.h"
#include "asrutil_util.h"

static char *token = NULL;
static asrutil_voice_engine_lang language = ASRUTIL_VOICE_ENGINE_ZH;

asrutil_ret asrutil_voice_engine_init()
{
	token = NULL;
	return ASRUTIL_RET_OK;
}

asrutil_ret asrutil_voice_engine_uninit()
{
	ASRUTIL_SAFE_FREE(token);
	return ASRUTIL_RET_OK;
}

asrutil_ret asrutil_voice_engine_register()
{
	char token_url[ASRUTIL_MAX_BUF] = { 0 };
	char *response = NULL;
	json_object *jobj = NULL;
	int len = 512;

	snprintf(token_url, sizeof(token_url), ASRUTIL_API_REQUEST_TOKEN, ASRUTIL_CONSUMER_KEY, ASRUTIL_CONSUMER_SECRET);
	ASRUTIL_LOG("token_url: %s\n", token_url);
	response = asrutil_curl_fetch_url(token_url);

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
		if (!strcmp(key, ASRUTIL_ID_ACCESS_TOKEN)) {
			if (json_type_string == json_object_get_type(val)) {
				ASRUTIL_SAFE_FREE(token);
				token = calloc(len, 1);
				snprintf(token, len, "%s", json_object_get_string(val));
				ASRUTIL_LOG("%s:%d, token: %s\n", __FUNCTION__, __LINE__, token);
			}
		}
	}
	json_object_put(jobj);
	ASRUTIL_SAFE_FREE(response);

	return ASRUTIL_RET_OK;
}

asrutil_ret asrutil_voice_engine_set_lang(asrutil_voice_engine_lang lang)
{
	language = lang;
	return ASRUTIL_RET_OK;
}

static asrutil_ret asrutil_voice_engine_convert_internal(char *voice_data,
							 int len,
							 char **utf8_data, int *utf8_data_len, int chunked, read_callback_f read_callback, void *readdata)
{
	char url[ASRUTIL_MAX_BUF] = { 0 };
	char tmp[ASRUTIL_MAX_BUF] = { 0 };
	char *response = NULL;
	json_object *jobj = NULL;
	json_object *tmp_jobj;
	char *json_start = NULL;

	if ((0 == chunked)
	    && (NULL == voice_data || NULL == utf8_data || NULL == utf8_data_len || NULL == token)) {
		ASRUTIL_LOG("%s:%d, bad parameter.\n", __FUNCTION__, __LINE__);
		return ASRUTIL_RET_FAIL;
	}
	if ((1 == chunked)
	    && (NULL == read_callback || NULL == readdata || NULL == utf8_data || NULL == utf8_data_len || NULL == token)) {
		ASRUTIL_LOG("%s:%d, bad parameter.\n", __FUNCTION__, __LINE__);
		return ASRUTIL_RET_FAIL;
	}

	snprintf(url, sizeof(url), "%s", ASRUTIL_API_URL_BASE);
	if (ASRUTIL_VOICE_ENGINE_ZH == language) {
		snprintf(tmp, sizeof(tmp), ASRUTIL_API_URL_TOKEN, ASRUTIL_TEXT_TRANSLATE_ID_ZH, ASRUTIL_CONSUMER_CUID, token);
	} else {
		snprintf(tmp, sizeof(tmp), ASRUTIL_API_URL_TOKEN, ASRUTIL_TEXT_TRANSLATE_ID_EN, ASRUTIL_CONSUMER_CUID, token);
	}
	strcat(url, tmp);

	ASRUTIL_LOG("%s:%d, url: %s\n", __FUNCTION__, __LINE__, url);

	if (0 == chunked) {
		response = asrutil_curl_post_data(url, voice_data, len);
	} else {
		response = asrutil_curl_post_data_chunked(url, read_callback, readdata);
	}

	if (NULL == response) {
		ASRUTIL_LOG("%s:%d, return error.\n", __FUNCTION__, __LINE__);
		return ASRUTIL_RET_FAIL;
	}
	ASRUTIL_LOG("response: %s\n", response);

	if (0 == chunked) {
		json_start = response;
	} else {
		json_start = strstr(response, ASRUTIL_RESPONSE_JSON_START);
		if (NULL == json_start) {
			ASRUTIL_LOG("%s:%d, could not find json start.\n", __FUNCTION__, __LINE__);
			ASRUTIL_SAFE_FREE(response);
			return ASRUTIL_RET_FAIL;
		}
	}

	jobj = json_tokener_parse(json_start);
	if (NULL == jobj || is_error(jobj)) {
		ASRUTIL_LOG("%s:%d, json_tokener_parse return error.\n", __FUNCTION__, __LINE__);
		ASRUTIL_SAFE_FREE(response);
		return ASRUTIL_RET_FAIL;
	}
	json_object_object_foreach(jobj, key, val) {
		if (!strcmp(key, ASRUTIL_ID_CONVERT_RESULT)) {
			if (json_type_array == json_object_get_type(val)) {
				int j = 0, array_len = 0;

				array_len = json_object_array_length(val);
				for (j = 0; j < array_len; j++) {
					tmp_jobj = json_object_array_get_idx(val, j);
					if (tmp_jobj && !is_error(tmp_jobj)) {
						const char *p = json_object_to_json_string(tmp_jobj);
						int p_len = 0;
						if ((ASRUTIL_VOICE_ENGINE_ZH == language)
						    && p && (strlen(p) > 5)) {
							p_len = strlen(p) - 1 - 3;
							*utf8_data = calloc(p_len, 1);
							snprintf(*utf8_data, p_len, "%s", p + 1);	// remove quotation marks (2) and chinese dot (3)
							*utf8_data_len = p_len;
						} else if ((ASRUTIL_VOICE_ENGINE_EN == language) && p && (strlen(p) > 3)) {
							p_len = strlen(p) - 1 - 1;
							*utf8_data = calloc(p_len, 1);
							snprintf(*utf8_data, p_len, "%s", p + 1);	// remove quotation marks (2) and english dot (1)
							*utf8_data_len = p_len;
						} else {
							p_len = 2;
							*utf8_data = calloc(p_len, 1);
							snprintf(*utf8_data, p_len, "%s", "");
							*utf8_data_len = p_len;
						}
						ASRUTIL_LOG("%s:%d, utf8_data: %s, utf8_data_len: %d\n", __FUNCTION__, __LINE__, *utf8_data, *utf8_data_len);
					}
					break;
				}
			}
		}
	}
	json_object_put(jobj);
	ASRUTIL_SAFE_FREE(response);

	return ASRUTIL_RET_OK;
}

asrutil_ret asrutil_voice_engine_convert(char *voice_data, int len, char **utf8_data, int *utf8_data_len)
{
	return asrutil_voice_engine_convert_internal(voice_data, len, utf8_data, utf8_data_len, 0, NULL, NULL);
}

asrutil_ret asrutil_voice_engine_convert_chunked(read_callback_f read_callback, void *readdata, char **utf8_data, int *utf8_data_len)
{
	return asrutil_voice_engine_convert_internal(NULL, 0, utf8_data, utf8_data_len, 1, read_callback, readdata);
}
