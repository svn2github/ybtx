

FILE* trace_open(const wchar_t* file, const wchar_t* mode, const char* source, int line);
size_t trace_read(void* buf, size_t ElementSize, size_t count, FILE* fp, const char* source, int line);
size_t trace_write(void* buf, size_t ElementSize, size_t count, FILE* fp, const char* source, int line);
int trace_seek(FILE* fp, long offset, int whence, const char* source, int line);
long trace_tell(FILE* fp, const char* source, int line);
int trace_close(FILE* fp, const char* source, int line);
void trace_error(const char* msg, const char* source, int line);
void trace_init();



#define  _wfopen(file, mode)                                         \
	trace_open(file, mode, __FILE__, __LINE__)
#define  fread(buf, elementSize, conut, fp)                        \
	trace_read(buf, elementSize, conut, fp, __FILE__, __LINE__)
#define  fwrite(buf, elementSize, conut, fp)                       \
	trace_write(buf, elementSize, conut, fp, __FILE__, __LINE__)
#define  fseek(fp, offset, whence)                                 \
	trace_seek(fp, offset, whence, __FILE__, __LINE__)
#define  ftell(fp)                                                 \
	trace_tell(fp, __FILE__, __LINE__)
#define  fclose(fp)                                                \
	trace_close(fp, __FILE__, __LINE__)
#define  echo_error(msg)                                           \
	trace_error(msg, __FILE__, __LINE__)


