#ifndef	__ASRUTIL_TEXT_TRANSLATE_H__
#define	__ASRUTIL_TEXT_TRANSLATE_H__

asrutil_ret asrutil_text_translate_init();
asrutil_ret asrutil_text_translate_uninit();

// convert voice to string, to_data will store the result
asrutil_ret asrutil_text_translate_convert(void *from_data, int len, char **to_data, int *to_data_len, asrutil_text_translate_mode mode);
#endif
