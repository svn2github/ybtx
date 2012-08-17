
#include "stdafx.h"


#define  logFile  L"ybtxerr.txt"


void trace_init()
{
	fclose(_wfopen(logFile, L"w"));
}


FILE* trace_open(const wchar_t* file, const wchar_t* mode, const char* source, int line)
{
	FILE* fp = _wfopen(file, mode);
	FILE* __fLog = _wfopen(logFile, L"a");
	fwprintf(__fLog, L"=============== %s ===============\n", file);
	if ( !fp )
		fwprintf(__fLog, L"open file error: %s\nsource: %s\nline: %d\n\n", file, source, line);
	fclose(__fLog);

	return fp;
}

size_t trace_read(void* buf, size_t ElementSize, size_t count, FILE* fp, const char* source, int line)
{
	size_t __result = fread(buf, ElementSize, count, fp);
	if (__result == -1)
	{
		FILE* __fLog = _wfopen(logFile, L"a");
		fprintf(__fLog, "read file error\nsource: %s\nline: %d\n\n",
			source, line);
		fclose(__fLog);
	}
	else if (__result == 0 || __result < count)
	{
		FILE* __fLog = _wfopen(logFile, L"a");
		fprintf(__fLog, "read max then file\nsource: %s\nline: %d\n",
			source, line);
		fprintf(__fLog, "read: %d, expect: %d\n\n", __result, count);
		fclose(__fLog);
	}

	return __result;
}

size_t trace_write(void* buf, size_t ElementSize, size_t count, FILE* fp, const char* source, int line)
{
	size_t __result = fwrite(buf, ElementSize, count, fp);
	if (__result == -1)
	{
		FILE* __fLog = _wfopen(logFile, L"a");
		fprintf(__fLog, "write file error\nsource: %s\nline: %d\n\n",
			source, line);
		fclose(__fLog);
	}
	else if (__result == 0 || __result < count)
	{
		FILE* __fLog = _wfopen(logFile, L"a");
		fprintf(__fLog, "write max then file\nsource: %s\nline: %d\n",
			source, line);
		fprintf(__fLog, "write: %d, expect: %d\n\n", __result, count);
		fclose(__fLog);
	}

	return __result;
}

int trace_seek(FILE* fp, long offset, int whence, const char* source, int line)
{
	int __result = fseek(fp, offset, whence);
	if (__result != 0)
	{
		FILE* __fLog = _wfopen(logFile, L"a");
		fprintf(__fLog, "seek error\nsource: %s\nline: %d\n\n",
			source, line);
		fclose(__fLog);
	}

	return __result;
}

long trace_tell(FILE* fp, const char* source, int line)
{
	long __result = ftell(fp);
	if (__result == -1)
	{
		FILE* __fLog = _wfopen(logFile, L"a");
		fprintf(__fLog, "ftell error\nsource: %s\nline: %d\n\n",
			source, line);
		fclose(__fLog);
	}

	return __result;
}

int trace_close(FILE* fp, const char* source, int line)
{
	int __result = fclose(fp);
	FILE* __fLog = _wfopen(logFile, L"a");
	if (__result != 0)
		fprintf(__fLog, "close error\nsource: %s\nline: %d\n\n", source, line);
	fprintf(__fLog, "=============== Close ===============\n");
	fclose(__fLog);

	return __result;
}

void trace_error(const char* msg, const char* source, int line)
{
	FILE* __fLog = _wfopen(logFile, L"a");
	fprintf(__fLog, "error occur: %s\nsource: %s\nline: %d\n\n",
		msg, source, line);
	fclose(__fLog);
}


