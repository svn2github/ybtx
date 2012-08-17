

#define  STRING_LEN        256
#define  STRING_POOL_SIZE  2000000
#define  MAX_BUFFER_COUNT  5000


// 普通模式下的节点
typedef struct tagBufferData {
	int   level;        // 当前栈级别
	char* FileDefined;  // 所在文件
	char* FunctionName; // 函数名
	long  LineDefined;  // 定义所在行
	long  CurrentLine;  // 当前所在行
	double LocalTime;    // 调用GetSeconds后获取时间返回值，即运行时间
	double TotalTime;    // 同上
} BufferData;


// 该结构存储各文件之间需要共享的变量
typedef struct tagGlobalState {
	int   CurrentLine;  // 记录当前函数所在行
	double FuncCallTime; // Lua函数调用所需时间
	int   HookMark;     // 是否继续记录函数调用标识
	char  auxs[STRING_LEN]; // 记录log文件名

	int BufCount;
	BufferData* Buffer;
} GlobalState;


#ifdef DEBUG
#define ASSERT(e, msg)                           \
	if (!e) {                                    \
		printf("Assertion failed: %s\n", msg);   \
		exit(1);                                 \
	}

#define AstFile(e, msg)                          \
	if (!e) {                                    \
		printf("Can't open file: %s\n", msg);    \
		exit(1);                                 \
	}

#else
#define ASSERT(e, msg)
#define AstFile(e, msg)

#endif

