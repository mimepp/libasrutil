#ifndef	__ASRUTIL_H__
#define	__ASRUTIL_H__

#define ASRUTIL_APP_NAME			"asrutil"
#define ASRUTIL_VERSION				"0.1"

#define ASRUTIL_CONSUMER_CUID			"10195875"
#define ASRUTIL_CONSUMER_KEY			"OVEEDTraL23mgWG23w2vNTAo"
#define ASRUTIL_CONSUMER_SECRET			"65781a6e179787d8d72dd4e528f8c4ef"

#define ASRUTIL_API_REQUEST_TOKEN		"https://openapi.baidu.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s"

#define ASRUTIL_MAX_PATH			(512)
#define ASRUTIL_MAX_BUF				(4096)
#define ASRUTIL_CURL_LOW_SPEED_LIMIT		(1)
#define ASRUTIL_CURL_LOW_SPEED_TIMEOUT		(60)

#define ASRUTIL_HTTP_GET			"GET"
#define ASRUTIL_HTTP_POST			"POST"

#define ASRUTIL_API_URL_BASE			"http://vop.baidu.com/server_api"
#define ASRUTIL_API_URL_TOKEN			"?lan=%s&cuid=%s&token=%s"
#define ASRUTIL_API_LAN_CH			"zh"
#define ASRUTIL_API_LAN_EN			"en"

#define ASRUTIL_ID_ACCESS_TOKEN			"access_token"
#define ASRUTIL_ID_CONVERT_RESULT		"result"

#define ASRUTIL_DEFAULT_WRITABLE_TMP_PATH	"/tmp"
#define ASRUTIL_DEFAULT_OAUTH_JSON_FILE		"asrutil_oauth.json"

#define ASRUTIL_TEXT_TRANSLATE_KEY		"20170929000085943"
#define ASRUTIL_TEXT_TRANSLATE_SECRET		"XKEg8gfFOJkbhIAg6QlH"

#define ASRUTIL_API_TEXT_TRANSLATE_URL_BASE	"http://api.fanyi.baidu.com/api/trans/vip/translate?"
#define ASRUTIL_TEXT_TRANSLATE_ID_ZH		"zh"
#define ASRUTIL_TEXT_TRANSLATE_ID_EN		"en"

#define ASRUTIL_ID_TRANS_RESULT			"trans_result"
#define ASRUTIL_ID_TRANS_DST			"dst"

#define ASRUTIL_VAD_BLOCK_SIZE			512
#define ASRUTIL_VAD_COUNT_THRESHOLD		3

#define ASRUTIL_RESPONSE_JSON_START		"{\"corpus_no\""

int asrutil_file_log(const char *fmt, ...);

#define ASRUTIL_DEBUG

#ifdef ASRUTIL_DEBUG
#define ASRUTIL_LOG(format,args...)	fprintf(stdout, format, ## args)
#define ASRUTIL_FILE_LOG	//asrutil_file_log
#else
#define ASRUTIL_LOG(format,args...)
#define ASRUTIL_FILE_LOG
#endif

#define ASRUTIL_SAFE_FREE(a) { if (a) {free(a); a = NULL;} }

#define ASRUTIL_LOG_FILE_NAME			"asrutil.log"

#define ASRUTIL_COOKIE_FILE_NAME		"cookie.txt"

typedef enum {
	ASRUTIL_RET_FAIL = -1,
	ASRUTIL_RET_OK = 0,
} asrutil_ret;

typedef enum {
	ASRUTIL_VOICE_ENGINE_ZH = 0,
	ASRUTIL_VOICE_ENGINE_EN,
} asrutil_voice_engine_lang;

typedef enum {
	ASRUTIL_MODE_ZH_2_EN = 0,
	ASRUTIL_MODE_EN_2_ZH,
} asrutil_text_translate_mode;

typedef size_t(*read_callback_f) (void *ptr, size_t size, size_t nmemb, void *userp);

#endif
