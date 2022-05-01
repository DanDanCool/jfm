#include "jfm.h"

#include <stdlib.h>
#include <string.h>

char* jfm_strcpy(const char* str)
{
	char* buf = (char*)malloc(strlen(str) + 1);
	return strcpy(buf, str);
}
