#include "stdafx.h"
#include "CScriptDebugerMgr.h"
#include "CScriptDebuger.h"
#include "ThreadHelper.h"
#include "CScriptData.h"
#include "ErrLogHelper.h"
#include "CodeCvs.h"
#ifndef _WIN32
#	define _countof(array)	(sizeof(array)/sizeof(array[0]))
#endif

namespace sqr
{
	enum EDebugerType
	{
		eDebugerRunning = 0,
		eDebugerSuspended = 1,
		eDebugerNone = 2,
	};

	class CScriptDbContainer
	{
	public:
		CScriptDbContainer();
		~CScriptDbContainer();
		
		list<CScriptDbContainer*>::iterator*	m_pItContainer;
		HSEMAPHORE			m_smContainer;
		EDebugerType		m_Type;
		uint32				m_uId;
	};
}

CScriptDbContainer::CScriptDbContainer()
: m_pItContainer(NULL)
{
	CreateSemaphore(&m_smContainer, 0,INT_MAX);
}

CScriptDbContainer::~CScriptDbContainer()
{
	DestroySemaphore(&m_smContainer);
}

//--------------------------------------------------------------------

CScriptDebugerMgr::CScriptDebugerMgr()
: m_bRunning(false)
, m_bDbFileLoad(false)
, m_bHasBreak(false)
{
	CreateLock(&m_lock);
	CreateLock(&m_BreakLock);
}

CScriptDebugerMgr::~CScriptDebugerMgr()
{
	DestroyLock(&m_BreakLock);
	DestroyLock(&m_lock);
}

CScriptDebugerMgr& CScriptDebugerMgr::Inst()
{
	static CScriptDebugerMgr ls_Inst;
	return ls_Inst;
}

CScriptDbContainer* CScriptDebugerMgr::GetContainer(CScriptDebuger* pDebuger)
{
	return pDebuger->m_pDbContainer;
}

void CScriptDebugerMgr::AddDebuger(CScriptDebuger* pDebuger)
{
	if(GetContainer(pDebuger))
		GenErr("该debuger的container已经存在，不能再次添加");

	CScriptDbContainer* pContainer = new CScriptDbContainer;
	pContainer->m_Type = eDebugerNone;
	
	Lock(&m_lock);
	static uint32 uId = 0;
	pContainer->m_uId = uId;
	m_mapId2Container.insert(make_pair(uId, pContainer));
	++uId;
	Unlock(&m_lock);

	pDebuger->m_pDbContainer = pContainer;
}

void CScriptDebugerMgr::RemoveDebuger(CScriptDebuger* pDebuger)
{
	CScriptDbContainer* pContainer = GetContainer(pDebuger);
	if(!pContainer)
		GenErr("该debuger的container已经删除，不能再次删除");

	uint32 uId = pContainer->m_uId;

	Lock(&m_lock);
	m_mapId2Container.erase(uId);
	Unlock(&m_lock);

	Ast(!pContainer->m_pItContainer);
	
	delete pContainer;
	pDebuger->m_pDbContainer = NULL;
}

void CScriptDebugerMgr::WaitForDebuger(CScriptDebuger* pDebuger)
{
	bool bWaited = false;
	Lock(&m_lock);
	if(!m_bRunning)
	{
		m_bRunning = true;
		bWaited = true;
	}
	
	CScriptDbContainer* pContainer = GetContainer(pDebuger);

	//如果等到了，就把debuger的type设置为running，直接退出
	//否则type为suspended，然后加入一个等待队列里面，
	if(bWaited)
	{
		pContainer->m_Type = eDebugerRunning;
		Unlock(&m_lock);
		return;
	}
	else
	{
		pContainer->m_Type = eDebugerSuspended;
	}

	Ast(!pContainer->m_pItContainer);
	pContainer->m_pItContainer = new list<CScriptDbContainer*>::iterator;
	
	m_lstWaitingContainer.push_back(pContainer);
	*pContainer->m_pItContainer = --m_lstWaitingContainer.end();

	Unlock(&m_lock);

	GetSemaphore(&pContainer->m_smContainer);

	Lock(&m_lock);
	pContainer->m_Type = eDebugerRunning;
	Unlock(&m_lock);
}

void CScriptDebugerMgr::CancelBlock(CScriptDebuger* pDebuger)
{
	CScriptDbContainer* pContainer = GetContainer(pDebuger);
	
	Lock(&m_lock);
	pContainer->m_Type = eDebugerNone;
	//如果为空，则表明没有等待需要运行的db了
	if(m_lstWaitingContainer.empty())
	{
		m_bRunning = false;
		Unlock(&m_lock);
		return;
	}

	CScriptDbContainer* pOtherContainer= m_lstWaitingContainer.front();
	m_lstWaitingContainer.pop_front();

	Ast(pOtherContainer->m_pItContainer);
	delete pOtherContainer->m_pItContainer;
	pOtherContainer->m_pItContainer = NULL;
	
	pOtherContainer->m_Type = eDebugerRunning;

	Unlock(&m_lock);

	PutSemaphore(&pOtherContainer->m_smContainer);
}

void CScriptDebugerMgr::PrintDebugerInfo()
{
	Lock(&m_lock);
	printf("ID\t\t\tDebuger State\n");

	const char* szType[3] = {"调试器正在运行", "调试器已被挂起", "调试器没有运行"};

	map<uint32, CScriptDbContainer*>::iterator iter = m_mapId2Container.begin();
	while(iter != m_mapId2Container.end())
	{
		CScriptDbContainer* pContainer = iter->second;
		printf("%d\t\t\t%s\n", pContainer->m_uId, szType[(uint32)pContainer->m_Type]);
		++iter;
	}
	Unlock(&m_lock);
}

void CScriptDebugerMgr::SwitchDebuger(uint32 uId, CScriptDebuger* pDebuger)
{
	CScriptDbContainer* pContainer = GetContainer(pDebuger);
	//如果切换的线程id本来就是当前的，什么都不做，直接退出
	if(pContainer->m_uId == uId)
		return;

	CScriptDbContainer* pOtContainer = NULL;
	
	Lock(&m_lock);
	map<uint32, CScriptDbContainer*>::iterator iter = m_mapId2Container.find(uId);
	if(iter == m_mapId2Container.end())
	{
		printf("错误的线程id，请输入正确的值\n");
		Unlock(&m_lock);
		return;
	}

	pOtContainer = iter->second;
	
	if(pOtContainer->m_Type == eDebugerNone)
	{
		printf("切换的线程脚本调试器并没有运行\n");
		Unlock(&m_lock);
		return;
	}

	pContainer->m_Type = eDebugerSuspended;

	//把开始运行的db加入等待队列，然后把需要唤醒的db从等待队列里面删除
	Ast(!pContainer->m_pItContainer);
	pContainer->m_pItContainer = new list<CScriptDbContainer*>::iterator;
	m_lstWaitingContainer.push_back(pContainer);
	*pContainer->m_pItContainer = --m_lstWaitingContainer.end();

	m_lstWaitingContainer.erase(*pOtContainer->m_pItContainer);
	Ast(pOtContainer->m_pItContainer);
	delete pOtContainer->m_pItContainer;
	pOtContainer->m_pItContainer = NULL;

	pOtContainer->m_Type = eDebugerRunning;
	
	Unlock(&m_lock);

	//唤醒其他的db调试器，并且阻塞自己的
	PutSemaphore(&pOtContainer->m_smContainer);

	GetSemaphore(&pContainer->m_smContainer);
}

//---------------------------------------------------------------
//下面处理break

namespace sqr
{
	class CScriptBreakInfo
	{
	public:
		CScriptBreakInfo(const string& strBreak, const string& strSource, uint32 uLine);
		string  m_strBreak;
		string	m_strSource;
		uint32	m_uLine;

		static bool ParseBreak(const string& strBreak, string& strSource, uint32& uLine);
	};
}

CScriptBreakInfo::CScriptBreakInfo(const string& strBreak, const string& strSource, uint32 uLine)
: m_strBreak(strBreak)
, m_strSource(strSource)
, m_uLine(uLine)
{

}

bool CScriptBreakInfo::ParseBreak(const string& strBreak, string& strSource, uint32& uLine)
{
	string::size_type uPos = strBreak.find(':');
	if(uPos == string::npos)
		return false;

	strSource = strBreak.substr(0, uPos);
	string strLine = strBreak.substr(uPos + 1);

	uLine = (uint32)atoi(strLine.c_str());
	return true;
}

CScriptBreakInfo* CScriptDebugerMgr::GetBreak(const string& strBreak)
{
	Lock(&m_BreakLock);
	MapStr2Break_t::const_iterator iter = m_mapBreak.find(strBreak);
	if(iter == m_mapBreak.end())
	{
		Unlock(&m_BreakLock);
		return NULL;
	}
	Unlock(&m_BreakLock);
	return iter->second;
}

uint32 CScriptDebugerMgr::ms_uId = 0;

//输入调式的格式为文件名:行号 a.lua:1
void CScriptDebugerMgr::AddBreak(const char *szBreak)
{
	string strBreak(szBreak);
	string strSource;
	uint32 uLine;

	if(!CScriptBreakInfo::ParseBreak(strBreak, strSource, uLine))
	{
		printf("输入的break格式不正确，请输入 (文件名:行号) 的格式\n");
		return;
	}

	CScriptBreakInfo* pBreak = GetBreak(strBreak);
	if(pBreak)
		return;

	Lock(&m_BreakLock);
	
	m_bHasBreak = true;
	
	pBreak = new CScriptBreakInfo(strBreak, strSource, uLine);
	m_mapBreak.insert(make_pair(strBreak, pBreak));
	m_mapId2Break.insert(make_pair(ms_uId, pBreak));
	ms_uId++;
	Unlock(&m_BreakLock);

	//每增加一个break，我们就要写入文件，防止程序突然崩溃而没有保存断点信息
	SaveBreaks();
}

void CScriptDebugerMgr::DelBreak(uint32 uBreak)
{
	Lock(&m_BreakLock);

	MapId2Break_t::iterator iter = m_mapId2Break.find(uBreak);
	if(iter == m_mapId2Break.end())
	{
		Unlock(&m_BreakLock);
		printf("请输入正确的break编号\n");
		return;		
	}

	CScriptBreakInfo* pBreak = iter->second;
	m_mapId2Break.erase(uBreak);
	m_mapBreak.erase(pBreak->m_strSource);
	delete pBreak;
	
	if(m_mapId2Break.empty())
		m_bHasBreak = false;

	Unlock(&m_BreakLock);

	SaveBreaks();
}

void CScriptDebugerMgr::DelBreaks()
{
	Lock(&m_BreakLock);

	m_mapId2Break.clear();

	for(MapStr2Break_t::iterator iter = m_mapBreak.begin(); iter != m_mapBreak.end(); ++iter)
	{
		delete iter->second;
	}

	m_mapBreak.clear();

	m_bHasBreak = false;

	Unlock(&m_BreakLock);

	SaveBreaks();
}

void CScriptDebugerMgr::ListBreaks()
{
	Lock(&m_BreakLock);

	if(m_mapId2Break.empty())
	{
		Unlock(&m_BreakLock);
		return;	
	}

	printf("编号\t\t\t断点位置\n");
	MapId2Break_t::iterator iter = m_mapId2Break.begin();
	while(iter != m_mapId2Break.end())
	{
		printf("%d \t\t\t%s\n", iter->first, iter->second->m_strBreak.c_str());
		++iter;
	}

	Unlock(&m_BreakLock);
}

void CScriptDebugerMgr::CheckBreak(lua_State* pState, lua_Debug* pDebug)
{
	//如果一个break都没有，那么我们就不检查了
	if(!m_bHasBreak)
		return;

	if(pDebug->event != LUA_HOOKLINE)
		return;

	Lock(&m_BreakLock);
	if(m_mapBreak.empty())
	{
		Unlock(&m_BreakLock);
		return;
	}
	Unlock(&m_BreakLock);

	lua_getinfo(pState, "S", pDebug);

	const char* szSource = pDebug->source;
	if(*szSource != '@')
		return;

	lua_getinfo(pState, "l", pDebug);
	uint32 uLine = (uint32)pDebug->currentline;

	string strSource(szSource);

	//由于pdebug得到的source是完整的路径，我们只需要最后的文件名字就行了
	//把所有的"\\"替换成"/"，统一处理
	replace(strSource.begin(), strSource.end(), '\\', '/');
	string::size_type uPos = strSource.find_last_of('/');
	strSource = strSource.substr(uPos + 1);
	char buf[10];
	sprintf(buf, ":%u", uLine);
	strSource.append(buf);

	CScriptBreakInfo* pBreak = GetBreak(strSource);
	if(!pBreak)
		return;

	//找到了，就进入断点了哈
	CScriptDebuger* pDebuger=_S( pState ).GetData()->m_pDebuger;
	
	pDebuger->Debug(pState);
}

namespace sqr
{
	int SaveDebugBreaks()
	{
		CScriptDebugerMgr::Inst().SaveBreaks();
		return 0;
	}

	int LoadDebugBreaks()
	{
		wstring strFile;
		strFile.append(GetLogPath()).append(L"/ScriptBreak.log");

		CScriptDebugerMgr::Inst().LoadBreaks(strFile.c_str());
		return 0;
	}
}

void CScriptDebugerMgr::LoadBreaks(const wchar_t* szFileName)
{
	if(m_bDbFileLoad)
		return;

	wcsncpy(m_szBreakFile, szFileName, _countof(m_szBreakFile));
	m_szBreakFile[_countof(m_szBreakFile) - 1] = L'\0';

#ifdef _WIN32
	FILE* fp = _wfopen(m_szBreakFile, L"r");
#else	
	FILE* fp = fopen( utf16_to_utf8(m_szBreakFile).c_str(), "r" );
#endif

	if(!fp)
		return;

	while(true)
	{
		char buf[56];

		if (fgets(buf, sizeof(buf), fp) == NULL)
			break;

		for(int i = 0; i < sizeof(buf); i++)
		{
			if(buf[i] == '\n')
			{
				if(buf[i - 1] == '\r')
				{
					buf[i - 1] = '\0';
					break;
				}
				else
				{	
					buf[i] = '\0';
					break;
				}
			}
		}

		AddBreak(buf);		
	}

	fclose(fp);

	m_bDbFileLoad = true;
}

void CScriptDebugerMgr::SaveBreaks()
{
	if(!m_bDbFileLoad)
		return;

	if(m_mapBreak.empty())
		return;

#ifdef _WIN32
	FILE* fp = _wfopen(m_szBreakFile, L"w");
#else
	FILE* fp = fopen( utf16_to_utf8(m_szBreakFile).c_str(), "w" );
#endif

	MapStr2Break_t::iterator iter = m_mapBreak.begin();
	while(iter != m_mapBreak.end())
	{
		fprintf(fp, "%s\n", iter->first.c_str());
		++iter;
	}

	fflush(fp);
	fclose(fp);

	m_bDbFileLoad = false;
}
