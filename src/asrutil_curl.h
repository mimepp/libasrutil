#ifndef	__ASRUTIL_CURL_H__
#define	__ASRUTIL_CURL_H__

char *asrutil_curl_fetch_url(const char *url);
char *asrutil_curl_post_data(const char *url, char *voice_data, int len);
char *asrutil_curl_post_data_chunked(const char *url, read_callback_f read_callback, void *readdata);
#endif
