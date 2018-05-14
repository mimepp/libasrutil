#ifndef	__ASRUTIL_VOICE_ENGINE_H__
#define	__ASRUTIL_VOICE_ENGINE_H__

asrutil_ret asrutil_voice_engine_init();
asrutil_ret asrutil_voice_engine_uninit();

// need to get the token of REST API
asrutil_ret asrutil_voice_engine_register();

asrutil_ret asrutil_voice_engine_set_lang(asrutil_voice_engine_lang lang);

// convert voice to string, utf8_data will store the result
asrutil_ret asrutil_voice_engine_convert(void *voice_data, int len, char **utf8_data, int *utf8_data_len);

asrutil_ret asrutil_voice_engine_convert_chunked(read_callback_f read_callback, void *readdata, char **utf8_data, int *utf8_data_len);
#endif
