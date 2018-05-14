## Process this file with automake to produce Makefile.in
SUBDIRS = src #docs

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = asrutil.pc

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = autogen.sh asrutil.pc.in gtk-doc.make

asrutildir = $(includedir)/asrutil
asrutil_HEADERS = config.h

indent:
	indent -npro -kr -i8 -ts8 -sob -l160 -ss -ncs -cp1 -il0 src/*.c src/*.h src/tests/*.c
	rm -rf src/*~ src/tests/*~

run_voice_zh_1:
	./src/tests/asrutil_voice_engine_test ./media_files/xinwen.0929.10s.male.1ch.16bit.8khz.pcm 0

run_voice_zh_2:
	./src/tests/asrutil_voice_engine_test_seperate ./media_files/xinwen.0929.1m.female.1ch.16bit.8khz.pcm 0

run_voice_zh_3:
	./src/tests/asrutil_voice_engine_test_seperate_vad ./media_files/xinwen.0929.1m.female.1ch.16bit.8khz.pcm 0

run_voice_zh_4:
	./src/tests/asrutil_voice_engine_test_seperate_vad_chunked ./media_files/xinwen.0929.1m.female.1ch.16bit.8khz.pcm 0

run_voice_zh_5:
	./src/tests/asrutil_voice_engine_test_chunked ./media_files/xinwen.0929.1m.female.1ch.16bit.8khz.pcm 0

run_voice_en_1:
	./src/tests/asrutil_voice_engine_test_seperate ./media_files/cnn10.0821.1m.male.1ch.16bit.8khz.pcm 1

run_text_1:
	./src/tests/asrutil_text_translate_test 0 ./media_files/zh_2_en.txt

run_text_2:
	./src/tests/asrutil_text_translate_test 1 ./media_files/en_2_zh.txt

run_voice_text_zh_2_en:
	./src/tests/asrutil_voice_engine_test_seperate_zh_2_en ./media_files/xinwen.0929.1m.female.1ch.16bit.8khz.pcm 0

run_voice_text_en_2_zh:
	./src/tests/asrutil_voice_engine_test_seperate_en_2_zh ./media_files/cnn10.0821.1m.male.1ch.16bit.8khz.pcm 1

run_all: run_voice_zh_1 run_voice_zh_2 run_voice_zh_3 run_voice_zh_4 run_voice_zh_5 run_voice_en_1 run_text_1 run_text_2 run_voice_text_zh_2_en run_voice_text_en_2_zh
