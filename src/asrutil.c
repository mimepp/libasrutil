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

int main(int argc, char *argv[])
{
	printf("%s: %d\n", argv[0], argc);
	return 0;
}
