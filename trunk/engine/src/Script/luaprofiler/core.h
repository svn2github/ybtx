
typedef   struct tagGlobalState   GlobalState;
typedef   struct tagProfState     ProfState;


ProfState* InitCoreProfiler(const char *FileName, int HaveHeader, GlobalState* pState);

void CallHookIn(ProfState* S, char *FuncName, char *file, int LineDefined, int CurrentLine);

int CallHookOut(ProfState *S, GlobalState* pState);

void CloseCoreProfiler(ProfState* S, GlobalState* pState);


