#pragma once

#include <cstddef>
#include <cstdio>
#include <stdarg.h>
#include <stdlib.h>
#include <switch.h>
#include <pthread.h>
int vasprintf(char** ret, const char* format, va_list args)
{
	va_list copy;
	va_copy(copy, args);

	/* Make sure it is determinate, despite manuals indicating otherwise */
	*ret = NULL;

	int count = vsnprintf(NULL, 0, format, args);
	if (count >= 0)
	{
		char* buffer = (char*)malloc(count + 1);
		if (buffer == NULL)
			count = -1;
		else if ((count = vsnprintf(buffer, count + 1, format, copy)) < 0)
			free(buffer);
		else
			*ret = buffer;
	}
	va_end(copy);  // Each va_start() or va_copy() needs a va_end()

	return count;
}

int asprintf(char** ret, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int count = vasprintf(ret, format, args);
	va_end(args);
	return(count);
}