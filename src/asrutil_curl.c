#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include "asrutil.h"
#include "asrutil_curl.h"

typedef struct asrutil_curl_data_t asrutil_curl_data;

struct asrutil_curl_data_t {
	char *buf;
	off_t count;
};

static size_t write_func(void *data, size_t size, size_t nmemb, void *stream)
{
	asrutil_curl_data *param = (asrutil_curl_data *) stream;
	memcpy((param->buf + param->count), data, size * nmemb);
	param->count += size * nmemb;
	return size * nmemb;
}

char *asrutil_curl_fetch_url(const char *url)
{
	char *buf = calloc(ASRUTIL_MAX_BUF, 1);
	CURL *curl = NULL;
	CURLcode ret = -1;
	asrutil_curl_data data;

	curl = curl_easy_init();
	if (NULL == curl)
		return NULL;

	data.buf = buf;
	data.count = 0;

	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);
	ret = curl_easy_perform(curl);
	printf("curl_easy_perform return value: %d (%s)\n", ret, curl_easy_strerror(ret));
	curl_easy_cleanup(curl);
	return buf;
}

char *asrutil_curl_post_data(const char *url, char *voice_data, int len)
{
	CURL *curl = NULL;
	CURLcode res = -1;
	struct curl_slist *headerlist = NULL;
	char tmp[ASRUTIL_MAX_BUF] = { 0 };
	char *buf = calloc(ASRUTIL_MAX_BUF * 4, 1);
	asrutil_curl_data data;

	data.buf = buf;
	data.count = 0;

	snprintf(tmp, sizeof(tmp), "%s%s", "Content-Type: voice/pcm; rate=", "8000");
	headerlist = curl_slist_append(headerlist, tmp);

	snprintf(tmp, sizeof(tmp), "Content-Length: %d", len);
	headerlist = curl_slist_append(headerlist, tmp);

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, voice_data);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		printf("perform curl error:%d.\n", res);
		return NULL;
	}
	curl_slist_free_all(headerlist);
	curl_easy_cleanup(curl);

	return buf;
}

char *asrutil_curl_post_data_chunked(const char *url, read_callback_f read_callback, void *readdata)
{
	CURL *curl = NULL;
	CURLcode res = -1;
	struct curl_slist *headerlist = NULL;
	char tmp[ASRUTIL_MAX_BUF] = { 0 };
	char *buf = calloc(ASRUTIL_MAX_BUF, 1);
	asrutil_curl_data data;

	data.buf = buf;
	data.count = 0;

	snprintf(tmp, sizeof(tmp), "%s%s", "Content-Type: voice/pcm; rate=", "8000");
	headerlist = curl_slist_append(headerlist, tmp);
	headerlist = curl_slist_append(headerlist, "Transfer-Encoding: chunked");

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

	curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
	curl_easy_setopt(curl, CURLOPT_READDATA, readdata);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		printf("perform curl error:%d.\n", res);
		return NULL;
	}
	curl_slist_free_all(headerlist);
	curl_easy_cleanup(curl);

	return buf;
}
