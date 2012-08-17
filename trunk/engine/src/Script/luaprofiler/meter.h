
typedef   struct tagProfState     ProfState;
typedef   struct tagProfData      ProfData;


ProfState* InitStack(void);

void CallFunc(ProfState* S, char* FuncName, char* FileDefined, long LineDefined, long CurrentLine);

void PauseLocalTime(ProfState* S);

void PauseTotalTime(ProfState* S);

void ResumeLocalTime(ProfState* S);

void ResumeTotalTime(ProfState* S);

void ResumeAllTime(ProfState* S);

ProfData RetFunc(ProfState* S, int IsResume);

