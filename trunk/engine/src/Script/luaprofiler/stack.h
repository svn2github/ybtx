
typedef long clock_t;


// 数据节点
typedef struct tagProfData {
	sqr::uint64 MarkerFunctionLocalTime; // 函数运行所打的时间标志
	sqr::uint64 MarkerFunctionTotalTime; // 整体运行所打的时间标志
	char*   FileDefined;  // 所在文件
	char*   FunctionName; // 函数名
	long    LineDefined;  // 定义所在行
	long    CurrentLine;  // 当前所在行
	double   LocalTime;    // 调用GetSeconds后获取时间返回值，即运行时间
	double   TotalTime;    // 同上
	struct tagProfData* next; // 链表
} ProfData;


// 链表
typedef struct tagProfState {
   int level; // 当前栈等级
   ProfData *top;
} ProfState;



//// 内存统计模式下的节点
//typedef struct tagCountData {
//	int   exec_count;   // 该函数执行次数
//	char* FileDefined;  // 所在文件
//	char* FunctionName; // 函数名
//	long  LineDefined;  // 定义所在行
//	long  CurrentLine;  // 当前所在行
//	float LocalTime;    // 调用GetSeconds后获取时间返回值，即运行时间
//	float TotalTime;    // 同上
//} CountData;


void StackPush(ProfState* S, ProfData r);
ProfData StackPop(ProfState* S);
ProfData* StackGet(ProfState* S, int idx);


