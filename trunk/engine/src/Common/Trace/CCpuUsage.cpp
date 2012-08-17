#include "stdafx.h"
#include "CCpuUsage.h"
#include "ThreadHelper.h"

#ifndef _WIN32
#include <unistd.h>

int32 GetNumCores()
{
	FILE* fp = fopen("/proc/stat", "r");
	if (!fp)
	{
		return -1;
	}
	// Skip first line
	char line[100]; 
	fgets(line, sizeof(line), fp);
	int numCores = -1;
	char firstWord[100];
	do
	{
		numCores++;
		if (fgets(line, 100, fp))
		{
			sscanf(line, "%s ", firstWord);
		} else {
			break;
		}
	} while (strncmp(firstWord, "cpu", 3) == 0);
	fclose(fp);
	return numCores;
}
#else
int32 GetNumCores()
{
	return 0;
}
#endif

CSystemCpuUsage::CSystemCpuUsage()
:m_oldBusyTime(0)
,m_oldIdleTime(0)
,m_numCores(0)
{
    m_numCores = GetNumCores();
    m_oldBusyTimeMulti = new int64[m_numCores];
    m_oldIdleTimeMulti = new int64[m_numCores];
    m_idleArray = new int64[m_numCores];
    m_busyArray = new int64[m_numCores];
    m_resultArray = new uint32[m_numCores];

    GetData(m_oldBusyTime, m_oldIdleTime, m_busyArray, m_idleArray);
}

CSystemCpuUsage::~CSystemCpuUsage()
{
	delete [] m_oldBusyTimeMulti;
	delete [] m_oldIdleTimeMulti;
	delete [] m_idleArray;
	delete [] m_busyArray;
	delete [] m_resultArray;
}

CProcessCpuUsage::CProcessCpuUsage()
:m_oldProcessCpuTime(0)
,m_oldTotelCpuTime(0)
,m_numCores(0)
{
	 m_numCores = GetNumCores();
	GetData(m_oldProcessCpuTime, m_oldTotelCpuTime);
}

CThreadCpuUsage::CThreadCpuUsage(HTHREADID tid)
:m_oldThreadCpuTime(0)
,m_oldTotelCpuTime(0)
,m_numCores(0)
{
	 m_numCores = GetNumCores();
	GetData(m_oldThreadCpuTime, m_oldTotelCpuTime,tid);
}

CWatchCpuThreadID::CWatchCpuThreadID()
{
	CreateLock(&m_hLock);
}

CWatchCpuThreadID::~CWatchCpuThreadID()
{
	DestroyLock(&m_hLock);
}
void CWatchCpuThreadID::AddWatchCpuThreadID(const char* szThreadName, HTHREADID tid)
{
	Lock(&m_hLock);
	m_vecThreadID.push_back(make_pair(szThreadName,tid));
	Unlock(&m_hLock);
}

#ifndef _WIN32
int32 CSystemCpuUsage::CpuUsage()
{
    uint32 dummy = 0;
    uint32* dummyArray = NULL;
    return CpuUsageMultiCore(dummy, dummyArray);
}

int32 CSystemCpuUsage::CpuUsageMultiCore(uint32& numCores,uint32*& coreArray)
{
	coreArray = m_resultArray;
	numCores = m_numCores;
	int64 busy = 0;
	int64 idle = 0;
	GetData(busy, idle, m_busyArray, m_idleArray);

	int64 deltaBusy = busy - m_oldBusyTime;
	int64 deltaIdle = idle - m_oldIdleTime;
	m_oldBusyTime = busy;
	m_oldIdleTime = idle;

	int retVal = -1;
	if (deltaBusy + deltaIdle == 0)
	{
		retVal = 0;
	}
	else
	{
		retVal = (int)(100 * (deltaBusy) / (deltaBusy + deltaIdle));
	}

	if (coreArray == NULL)
	{
		return retVal;
	}

	for (uint32 i = 0; i < m_numCores; i++)
	{
		deltaBusy = m_busyArray[i] - m_oldBusyTimeMulti[i];
		deltaIdle = m_idleArray[i] - m_oldIdleTimeMulti[i];
		m_oldBusyTimeMulti[i] = m_busyArray[i];
		m_oldIdleTimeMulti[i] = m_idleArray[i];
		if(deltaBusy + deltaIdle == 0)
		{
			coreArray[i] = 0;
		}
		else
		{
			coreArray[i] = (int)(100 * (deltaBusy) / (deltaBusy+deltaIdle));
		}
	}
	return retVal;
}

void CSystemCpuUsage::GetData(int64& busy, int64& idle, int64*& busyArray,int64*& idleArray)
{
	FILE* fp = fopen("/proc/stat", "r");
    if (!fp)
    {
        return;
    }

    char line[100];
	fgets(line, sizeof(line), fp);
    char firstWord[100];
    sscanf(line, "%s ", firstWord);
    if(strncmp(firstWord, "cpu", 3)!=0)
    {
		fclose(fp);
        return;
    }
    char sUser[100];
    char sNice[100];
    char sSystem[100];
    char sIdle[100];
    sscanf(line, "%s %s %s %s %s ", firstWord, sUser, sNice, sSystem, sIdle);
    int64 luser = atoll(sUser);
    int64 lnice = atoll(sNice);
    int64 lsystem = atoll(sSystem);
    int64 lidle = atoll (sIdle);

    busy = luser + lnice + lsystem;
    idle = lidle;
    for (uint32 i = 0; i < m_numCores; i++)
    {
        fgets(line, sizeof(line), fp);
        sscanf(line, "%s %s %s %s %s ", firstWord, sUser, sNice, sSystem,
               sIdle);
        luser = atoll(sUser);
        lnice = atoll(sNice);
        lsystem = atoll(sSystem);
        lidle = atoll (sIdle);
        busyArray[i] = luser + lnice + lsystem;
        idleArray[i] = lidle;
    }
    fclose(fp);
    return;
}

int32 CProcessCpuUsage::CpuUsageCurProcess()
{
	int64 process = 0;
	int64 totel = 0;
	GetData(process, totel);
	int64 deltaProcess = process - m_oldProcessCpuTime;
	int64 deltaTotel = totel - m_oldTotelCpuTime;
	m_oldProcessCpuTime = process;
	m_oldTotelCpuTime = totel;

	int retVal = -1;
	if (deltaTotel == 0)
	{
		retVal = 0;
	}
	else
	{
		retVal = (int)(10000 * (deltaProcess) / deltaTotel);
	}
	return retVal*m_numCores;
}

void CProcessCpuUsage::GetData(int64& processTime, int64& totelTime)
{
	string file = "/proc/";
	HPID pid = getpid();
	char szPid[32];
	sprintf( szPid, "%d", pid );
	file = file+szPid+"/stat";

	FILE* fp = fopen( "/proc/stat", "r");
	if (!fp)
	{
		return;
	}

	char line[100];
	fgets(line, sizeof(line), fp);
	char firstWord[100];
	sscanf(line, "%s ", firstWord);
	if(strncmp(firstWord, "cpu", 3)!=0)
	{
		fclose(fp);
		return;
	}
	char sUser[100];
	char sNice[100];
	char sSystem[100];
	char sIdle[100];
	sscanf(line, "%s %s %s %s %s ", firstWord, sUser, sNice, sSystem, sIdle);
	int64 luser = atoll(sUser);
	int64 lnice = atoll(sNice);
	int64 lsystem = atoll(sSystem);
	int64 lidle = atoll (sIdle);

	totelTime = luser + lnice + lsystem+lidle;
	fclose(fp);

	FILE* fp2 = fopen( file.c_str(), "r");
	if (!fp2)
	{
		return;
	}
	char line2[200];
	fgets(line2, sizeof(line2), fp2);
	char Temp[100];
	char uTime[100];
	char sTime[100];
	char cuTime[100];
	char csTime[100];
	sscanf(line2, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s ", 
		Temp,Temp,Temp,Temp,Temp,Temp,Temp,
		Temp,Temp,Temp,Temp,Temp,Temp,
		uTime, sTime, cuTime, csTime);
	int64 luTime = atoll(uTime);
	int64 lsTime = atoll(sTime);
	int64 lcuTime = atoll(cuTime);
	int64 lcsTime = atoll (csTime);
	processTime = luTime+lsTime+lcuTime+lcsTime;
	fclose(fp2);
}


int32 CThreadCpuUsage::CpuUsageCurProcessbyTID(HTHREADID tid)
{
	int64 process = 0;
	int64 totel = 0;

	GetData(process, totel,tid);
	int64 deltaProcess = process - m_oldThreadCpuTime;
	int64 deltaTotel = totel - m_oldTotelCpuTime;
	m_oldThreadCpuTime = process;
	m_oldTotelCpuTime = totel;

	int retVal = -1;
	if (deltaTotel == 0)
	{
		retVal = 0;
	}
	else
	{
		retVal = (int)(10000 * (deltaProcess) / deltaTotel);
	}
	return retVal*m_numCores;;
}

void CThreadCpuUsage::GetData(int64& processTime, int64& totelTime,HTHREADID tid)
{
	string file = "/proc/";
	HPID pid = getpid();
	char szPid[32];
	sprintf( szPid, "%d", pid );
	char szTid[32];
	sprintf( szTid, "%d", tid );
	file = file+szPid+"/task/"+szTid+"/stat";

	FILE* fp = fopen( "/proc/stat", "r");
	if (!fp)
	{
		return;
	}

	char line[100];
	fgets(line, sizeof(line), fp);
	char firstWord[100];
	sscanf(line, "%s ", firstWord);
	if(strncmp(firstWord, "cpu", 3)!=0)
	{
		fclose(fp);
		return;
	}
	char sUser[100];
	char sNice[100];
	char sSystem[100];
	char sIdle[100];
	sscanf(line, "%s %s %s %s %s ", firstWord, sUser, sNice, sSystem, sIdle);
	int64 luser = atoll(sUser);
	int64 lnice = atoll(sNice);
	int64 lsystem = atoll(sSystem);
	int64 lidle = atoll (sIdle);

	totelTime = luser + lnice + lsystem+lidle;
	fclose(fp);

	FILE* fp2 = fopen( file.c_str(), "r");
	if (!fp2)
	{
		return;
	}
	char line2[200];
	fgets(line2, sizeof(line2), fp2);
	char Temp[100];
	char uTime[100];
	char sTime[100];
	char cuTime[100];
	char csTime[100];
	sscanf(line2, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s ", 
		Temp,Temp,Temp,Temp,Temp,Temp,Temp,
		Temp,Temp,Temp,Temp,Temp,Temp,
		uTime, sTime, cuTime, csTime);
	int64 luTime = atoll(uTime);
	int64 lsTime = atoll(sTime);
	int64 lcuTime = atoll(cuTime);
	int64 lcsTime = atoll (csTime);
	processTime = luTime+lsTime+lcuTime+lcsTime;
	fclose(fp2);
}

#else

int32 CSystemCpuUsage::CpuUsage()
{
	return 0;
}

int32 CSystemCpuUsage::CpuUsageMultiCore(uint32& numCores,uint32*& coreArray)
{
	return 0;
}

void CSystemCpuUsage::GetData(int64& busy, int64& idle, int64*& busyArray,int64*& idleArray)
{
}

int32 CProcessCpuUsage::CpuUsageCurProcess()
{
	return 0;
}

void CProcessCpuUsage::GetData(int64& processTime, int64& totelTime)
{
}


int32 CThreadCpuUsage::CpuUsageCurProcessbyTID(HTHREADID tid)
{
	return 0;
}

void CThreadCpuUsage::GetData(int64& processTime, int64& totelTime,HTHREADID tid)
{
}

#endif

