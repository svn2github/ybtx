#include "stdafx.h"
#include "ScriptMgr.h"
#include "Cache.h"
#include "CheckAiDlg.h"
#include "SEPathMgr.h"
#include "SetCenterPosDlg.h"
#include "SetOffsetPositionDlg.h"
#include "EnumMgr.h"
#include "CodeCvs.h"
#include "ExpHelper.h"


CCache::CCache(void):m_bcNpc(BLOCK_SIZE),m_bcObj(BLOCK_SIZE),m_bcTrap(BLOCK_SIZE),m_bCheckAI(true), m_pFile(NULL)
{
	m_DynamicCenterPos.x = 0;
	m_DynamicCenterPos.y = 0;
	m_OffsetPos.x = 0;
	m_OffsetPos.y = 0;
}

CCache::~CCache(void)
{
	ClearAll();
}

void CCache::ClearAll()
{
	ClearNpc();
	ClearObj();
	ClearTrap();
	ClearAreaData();
	m_strDynamicFileName = "";
	m_DynamicCenterPos.x = 0;
	m_DynamicCenterPos.y = 0;
	g_pMapView->ClearSelectElement();
}

void CCache::ClearNpc()
{
	g_pMainDlg->OnClearPage(eTPT_NPC);
	g_pMainDlg->OnClearPage(eTPT_COLONY);
	NpcBC_Iter iter = m_bcNpc.begin();
	while (iter != m_bcNpc.end())
	{
		delete *iter;
		m_bcNpc.erase(iter);
		iter = m_bcNpc.begin();
	}
	m_bcNpc.clear();
	g_pMapView->SetUpdateMark(eUES_NPC);
	ClearPathKeyPos();

	m_mapNpcIndex.clear();
}

void CCache::ClearObj()
{
	g_pMainDlg->OnClearPage(eTPT_OBJ);
	ObjBC_Iter iter = m_bcObj.begin();
	while (iter != m_bcObj.end())
	{
		delete *iter;
		m_bcObj.erase(iter);
		iter = m_bcObj.begin();
	}
	m_bcObj.clear();
	g_pMapView->SetUpdateMark(eUES_OBJ);
	
	m_mapObjIndex.clear();
}

void CCache::ClearTrap()
{
	g_pMainDlg->OnClearPage(eTPT_TRAP);
	TrapBC_Iter iter = m_bcTrap.begin();
	while (iter != m_bcTrap.end())
	{
		delete *iter;
		m_bcTrap.erase(iter);
		iter = m_bcTrap.begin();
	}
	m_bcTrap.clear();
	g_pMapView->SetUpdateMark(eUES_TRAP);

	m_mapTrapIndex.clear();
}

void CCache::ClearAreaData()
{
	AreaData_Iter iter = m_mapAreaData.begin();
	while (iter != m_mapAreaData.end())
	{
		delete iter->second;
		++iter;
	}
	m_mapAreaData.clear();
}


void CCache::ClearPathKeyPos()
{
	PathKeyPosBC_Iter iter = m_bcPathKeyPos.begin();
	while (iter != m_bcPathKeyPos.end())
	{
		m_bcPathKeyPos.erase(iter);
		iter = m_bcPathKeyPos.begin();
	}
	m_bcPathKeyPos.clear();
}

PPlaceNpc CCache::Exp_AddPlaceNpc(int index, const char*	_name, 
								  float	_x, 
								  float _y
								  )
{
	_x += m_OffsetPos.x;
	_y += m_OffsetPos.y;
	PNPC pNPC = g_pOriginalData->GetNpc(_name);
	if (!g_pMainDlg->IsLeapCheck && pNPC == NULL && g_eModeType != eMT_PATH_EDIT)
	{
		CString s = _name;
		g_pMainDlg->MessageBox(s + " 不在Npc_Common表中,跳过加载.");
		return false;
	}
	if (g_eModeType == eMT_DYNAMIC || g_eModeType == eMT_PATH_EDIT)
	{
		_x += m_DynamicCenterPos.x;
		_y += m_DynamicCenterPos.y;
	}

	PlaceNpc newNpc;
	newNpc.name =	_name;
	newNpc.fPos.x = _x;
	newNpc.fPos.y = _y;
	if (g_eModeType != eMT_PATH_EDIT) 
	{
		newNpc.AIType = pNPC->_AI类型;
	}
	else
	{
		newNpc.AIType = "支持巡逻";
	}
	PPlaceNpc result = static_cast<PPlaceNpc>(AddNpc(&newNpc, _x,_y));
	if (result)
	{	
		if (index != -1)
		{
			result->index =index;
			assert( m_mapNpcIndex.insert(make_pair(index, result)).second);
		}

		if(g_pMapView->IsBlock((int)_x, (int) _y))
		{
			CString s;
			s.Format("%s [%0.2f,%0.2f] 在障碍里了!", _name, _x, _y);
			g_pMapView->MoveMapToCenter(_x, _y);
			g_pMainDlg->MessageBox(s);
		}
	}
	return static_cast<PPlaceNpc>(result);
}
bool CCache::Exp_AddPlaceObj(int index, const char* name, float x, float y, int _direction )
{
	x += m_OffsetPos.x;
	y += m_OffsetPos.y;
	if (!g_pMainDlg->IsLeapCheck && !g_pOriginalData->IsObjInCommonCfg(name))
	{
		CString s = name;
		g_pMainDlg->MessageBox(s + " 不在IntObj_Common表中,跳过加载.");
		return false;
	}
	if (g_eModeType == eMT_DYNAMIC)
	{
		x += m_DynamicCenterPos.x;
		y += m_DynamicCenterPos.y;
	}
	
	if (g_pOriginalData->IsBarrierObj(name) && g_eModeType == eMT_AREA_FB)
	{
		return AddBarrierObj(name, (int)x, (int)y);

	}
	else
	{
		PlaceObj Obj;
		Obj.name = name;
		Obj.direction = _direction;
		PPlaceObj result =  static_cast<PPlaceObj>(AddObj(&Obj, x, y));
		if (result)
		{
			if (index != -1)
			{
				result->index = index;
				assert(m_mapObjIndex.insert(make_pair(index, result)).second);
			}
			if(g_pMapView->IsBlock((int)x, (int) y))
			{
				CString s;
				s.Format("%s [%0.2f,%0.2f] 在障碍里了!", name, x, y);
				g_pMapView->MoveMapToCenter(x,y);
				g_pMainDlg->MessageBox(s);
			}
		}
		return result != NULL;
	}
}

bool CCache::Exp_AddPlaceTrap(int index, const char* name, float x, float y, int _direction)
{
	x += m_OffsetPos.x;
	y += m_OffsetPos.y;
	if (!g_pMainDlg->IsLeapCheck && !g_pOriginalData->IsTrapInCommonCfg(name))
	{
		CString s = name;
		g_pMainDlg->MessageBox(s + " 不在Trap_Common表中,跳过加载.");
		return false;
	}
	if (g_eModeType == eMT_DYNAMIC)
	{
		x += m_DynamicCenterPos.x;
		y += m_DynamicCenterPos.y;
	}
	PlaceTrap trap;
	trap.name = name;
	trap.direction = _direction;
	PPlaceTrap result = static_cast<PPlaceTrap>(AddTrap(&trap, x, y));
	if (result)
	{
		if (index != -1)
		{
			result->index = index;
			assert(m_mapTrapIndex.insert(make_pair(index, result)).second);
		}
		if(g_pMapView->IsBlock((int)x, (int) y))
		{
			CString s;
			s.Format("%s [%0.2f,%0.2f] 在障碍里了!", name, x, y);
			g_pMapView->MoveMapToCenter(x,y);
			g_pMainDlg->MessageBox(s);
		}
	}
	return result != NULL;
}

bool CCache::Exp_AddAreaLine(int x,int y1, int y2 ,const char* name)
{
	x +=  int(m_OffsetPos.x);
	y1 +=  int(m_OffsetPos.y);
	y2 +=  int(m_OffsetPos.y);
	m_bIsOldAreaFile = false;

	static const COLORREF DEFAULT_COLORS[] = {0XFF0000,0X00FF00,0X0000FF,0XFFFF00,0XFF00FF,0X00FFFF,
		0XC00000,0X00C000,0X0000C0,0XC0FF00,0XC000FF,0X00C0FF,
		0XC0C000,0XC000C0,0X00C0C0,0XFFC000,0XFF00C0,0X00FFC0,
		0X800000,0X008000,0X000080,0X80FF00,0X8000FF,0X0080FF,
		0X808000,0X800080,0X008080,0XFF8000,0XFF0080,0X00FF80,
		0X400000,0X004000,0X000040,0X40FF00,0X4000FF,0X0040FF,
		0X404000,0X400040,0X004040,0XFF4000,0XFF0040,0X00FF40};

	static const int COLORSCOUNT = sizeof(DEFAULT_COLORS)/sizeof(DEFAULT_COLORS[0]);
	int color;
	if(strcmp(name, "") == 0)
	{
		color = -1;
	}
	else if (!GetAreaData(name))
	{
		int areaCount = GetAreaCount();
		
		color = DEFAULT_COLORS[areaCount%COLORSCOUNT] + areaCount / COLORSCOUNT;
		g_pMainDlg->SetAreaColor(name, color);
	}
	else
	{
		color = GetAreaColor(name);
	}
	g_pMapView->DrawAreaLine(x, y1, y2, color);
	return true;
}

bool CCache::Exp_AddAreaMusic(const char* areaName, const char* musicName)
{
	Ast(GetAreaData(areaName));
	return SetAreaMusic(areaName, musicName);
}

bool CCache::Exp_SetAreaMaxPlayerNum(const char* areaName, int playNum)
{
	Ast(GetAreaData(areaName));
	SetAreaFbState(areaName, true);
	return SetAreaMaxPlayerNum(areaName, playNum);
}

bool CCache::Exp_SetAreaRatingPlayerNum(const char* areaName, int playNum)
{
	Ast(GetAreaData(areaName));
	return SetAreaRatingPlayerNum(areaName, playNum);
}

void CCache::Exp_AddDynamicFile(const char* name, const char* fileName)
{
	if(IsDynamicName(name))
	{
		cout << "!!!!!!!!!!!!!!!!!!!!!!! 同名  " << name << endl;
		return;
	}
	m_mapDynamicFile[name] = fileName;
}


CElement* CCache::AddNpc(PPlaceNpc pNpc, float x, float y)
{
	if(!g_pMainDlg->IsLeapCheck && (x < 0 || x > g_pMapView->GetMapGridWidth() || y < 0 || y > g_pMapView->GetMapGridHeight()))
	{
		if(g_eModeType == eMT_DYNAMIC)
		{
			g_pMainDlg->MessageBox("npc [" + pNpc->name + "] 超出地图范围, 跳过加载, 却确认中心坐标是不是设置合理 或者地图是否一致, 谨慎保存." );
		}
		else
		{
			g_pMainDlg->MessageBox("npc [" + pNpc->name + "] 超出地图范围, 跳过加载, 请确认地图是否一致,谨慎保存." );
		}
		return NULL;
	}
	PPlaceNpc pNewNpc = new PlaceNpc();
	pNewNpc->name =	pNpc->name;
	pNewNpc->fPos.x = x;
	pNewNpc->fPos.y = y;
	pNewNpc->moveType = pNpc->moveType;
	pNewNpc->moveRange = pNpc->moveRange;
	pNewNpc->direction = pNpc->direction;
	pNewNpc->minDelayTime = pNpc->minDelayTime;
	pNewNpc->maxDelayTime = pNpc->maxDelayTime;
	pNewNpc->speed = pNpc->speed;
	pNewNpc->idleType = pNpc->idleType;
	pNewNpc->AIType = pNpc->AIType;
	pNewNpc->colonyId = pNpc->colonyId;
	pNewNpc->colonyType = pNpc->colonyType;
	pNewNpc->type = pNpc->type;
	pNewNpc->isIgnoreClog = pNpc->isIgnoreClog;
	pNewNpc->Arrow.SetOwner(pNewNpc);
	if (m_bcNpc.insert(x,y,pNewNpc))
	{
		if(!g_pMainDlg->IsLeapCheck)
		{
			g_pMapView->SetUpdateMark(eUES_NPC);
			g_pMainDlg->OnAddElement(pNewNpc, eTPT_NPC);
			if (pNewNpc->colonyId != 0 && pNewNpc->colonyType == CEnumMgr::Inst().GetColonyTypeID("BOSS"))
			{
				g_pMainDlg->OnAddElement(pNewNpc, eTPT_COLONY);
			}
		}
		return static_cast<CElement*>(pNewNpc);
	}
	else
	{
		delete pNewNpc;
		return NULL;
	}
}

CElement* CCache::AddObj(PPlaceObj pObj, float x, float y)
{
	if(!g_pMainDlg->IsLeapCheck && (x < 0 || x > g_pMapView->GetMapGridWidth() || y < 0 || y > g_pMapView->GetMapGridHeight()))
	{
		if(g_eModeType == eMT_DYNAMIC)
		{
			g_pMainDlg->MessageBox("Obj [" + pObj->name + "] 超出地图范围, 跳过加载, 却确认中心坐标是不是设置合理 或者地图是否一致, 谨慎保存." );
		}
		else
		{
			g_pMainDlg->MessageBox("Obj [" + pObj->name + "] 超出地图范围, 跳过加载, 请确认地图是否一致,谨慎保存." );
		}
		return NULL;
	}
	PPlaceObj pNewObj = new PlaceObj();
	pNewObj->fPos.x = x;
	pNewObj->fPos.y = y;
	pNewObj->name = pObj->name;
	pNewObj->direction = pObj->direction;
	pNewObj->Arrow.SetOwner(pNewObj);
	if(m_bcObj.insert(x,y,pNewObj))
	{
		if(!g_pMainDlg->IsLeapCheck)
		{
			g_pMapView->SetUpdateMark(eUES_OBJ);
			g_pMainDlg->OnAddElement(pNewObj, eTPT_OBJ);
		}
		return static_cast<CElement*>(pNewObj);
	}
	else
	{
		delete pNewObj;
		return NULL;
	}
}

CElement* CCache::AddTrap(PPlaceTrap pTrap, float x, float y)
{
	if(!g_pMainDlg->IsLeapCheck && (x < 0 || x > g_pMapView->GetMapGridWidth() || y < 0 || y > g_pMapView->GetMapGridHeight()))
	{
		if(g_eModeType == eMT_DYNAMIC)
		{
			g_pMainDlg->MessageBox("Trap [" + pTrap->name + "] 超出地图范围, 跳过加载, 却确认中心坐标是不是设置合理 或者地图是否一致, 谨慎保存." );
		}
		else
		{
			g_pMainDlg->MessageBox("Trap [" + pTrap->name + "] 超出地图范围, 跳过加载, 请确认地图是否一致,谨慎保存." );
		}
		return NULL;
	}
	PPlaceTrap pNewTrap = new PlaceTrap;
	pNewTrap->fPos.x = x;
	pNewTrap->fPos.y = y;
	pNewTrap->name = pTrap->name;
	pNewTrap->direction = pTrap->direction;
	pNewTrap->Arrow.SetOwner(pNewTrap);
	if(m_bcTrap.insert(x,y,pNewTrap))
	{
		if(!g_pMainDlg->IsLeapCheck)
		{
			g_pMapView->SetUpdateMark(eUES_TRAP);
			g_pMainDlg->OnAddElement(pNewTrap, eTPT_TRAP);
		}
		return static_cast<CElement*>(pNewTrap);
	}
	else
	{
		delete pNewTrap;
		return NULL;
	}
}

bool CCache::AddPathKeyPos(PPathKeyPos pKeyPos)
{
	if(m_bcPathKeyPos.insert(pKeyPos->fPos.x,pKeyPos->fPos.y,pKeyPos))
	{
		return true;
	}
	return false;
}

bool CCache::DeleteNpc(PPlaceNpc pNpc)
{
	if (pNpc == NULL)
	{
		return false;
	}
	NpcBC_Iter iter = m_bcNpc.find( pNpc->fPos.x, pNpc->fPos.y);
	if (iter == m_bcNpc.end() || *iter != pNpc)
	{
		cout << "CCache::DeleteNpc  企图删除一个不再 m_bcNpc 管理中的NPC"<< endl;
		return false;
	}
	else
	{
		g_pMainDlg->OnDelElement(pNpc, eTPT_NPC);
		if (pNpc->colonyId != 0 && pNpc->colonyType == CEnumMgr::Inst().GetColonyTypeID("BOSS"))
		{
			g_pMainDlg->OnDelElement(pNpc, eTPT_COLONY);
		}
		if(pNpc->index != -1) //原始文件对象,删除索引
		{
			m_mapNpcIndex.erase(pNpc->index);
		}

		delete *iter;
		return m_bcNpc.erase(iter);
	}
}

bool CCache::DeleteObj(PPlaceObj pObj)
{
	ObjBC_Iter iter = m_bcObj.find( pObj->fPos.x, pObj->fPos.y);
	if (iter == m_bcObj.end() || *iter != pObj)
	{
		cout << "CCache::DeleteObj  企图删除一个不再 m_bcObj 管理中的Obj"<< endl;
		return false;
	}
	else
	{
		g_pMainDlg->OnDelElement(pObj, eTPT_OBJ);
		if(pObj->index != -1) //原始文件对象,删除索引
		{
			m_mapObjIndex.erase(pObj->index);
		}
		delete *iter;
		return m_bcObj.erase(iter);
	}
}

bool CCache::DeleteTrap(PPlaceTrap pTrap)
{
	TrapBC_Iter iter = m_bcTrap.find( pTrap->fPos.x, pTrap->fPos.y);
	if (iter == m_bcTrap.end() || *iter != pTrap)
	{
		cout << "CCache::DeleteTrap  企图删除一个不再 m_bcTrap 管理中的Trap"<< endl;
		return false;
	}
	else
	{
		g_pMainDlg->OnDelElement(pTrap, eTPT_TRAP);
		if(pTrap->index != -1) //原始文件对象,删除索引
		{
			m_mapTrapIndex.erase(pTrap->index);
		}
		delete *iter;
		return m_bcTrap.erase(iter);
	}
}

bool CCache::DeletePathKeyPos(PPathKeyPos pKeyPos)
{
	PathKeyPosBC_Iter iter = m_bcPathKeyPos.find(pKeyPos->fPos.x, pKeyPos->fPos.y);
	if (iter == m_bcPathKeyPos.end() || *iter != pKeyPos)
	{
		cout << "企图删除一个不在 m_bcPathKeyPos 管理中的PathKeyPos"<<endl;
		return false;
	}
	else
	{// 这里不要 delete PathKeyPos是在 Npc 内部创建的
		return m_bcPathKeyPos.erase(iter);
	}
}

bool CCache::LoadSceneData(const CString& filePath)
{
	CString path = filePath;
	::SetCurrentDirectory(CSEPathMgr::Inst().GetWorkingPath().GetBuffer());
	FILE* pFile  = fopen(path.GetBuffer(), "r");
	if(NULL==pFile) return false;
	fclose(pFile);
	
	ClearAll();
	CString str = "dofile \"" + path + "\"";
	str.Replace("\\","/");
	RunLuaFile(str.GetBuffer());
	return true;
}

bool CCache::LoadAreaData(const CString& filePath)
{
	CString path = filePath;
	::SetCurrentDirectory(CSEPathMgr::Inst().GetWorkingPath().GetBuffer());
	FILE* pFile  = fopen(path.GetBuffer(), "r");
	if(NULL==pFile) return false;
	fclose(pFile);

	g_pMainDlg->ClearArea();
	CString str = "dofile  \"" + path + "\"";
	str.Replace("\\","/");
	RunLuaFile(str.GetBuffer());
	if (m_bIsOldAreaFile)
	{
		g_pMapView->FillContour();
	}
	return true;
}

bool CCache::LoadDynamicData(const CString& name)
{
	ClearAll();
	if (IsDynamicName(name))
	{
		m_strDynamicFileName = name;
		CString fileName;
		GetDynamicFileName(name, fileName);
		g_pMainDlg->SetWindowText("动态文件编辑 -- " + m_strDynamicFileName);
		CString filePath = CSEPathMgr::Inst().GetDesignerPath() + "/scene/dynamic_file/" + fileName + ".lua";
		FILE* pFile  = fopen(filePath.GetBuffer(), "r");
		if(NULL==pFile) 
		{
			return true;
		}
		fclose(pFile);
		CString str = "dofile  \"" + filePath + "\"";
		RunLuaFile(str.GetBuffer());

		
		RunLuaFile("LoadDynamicData(\"" + name +"\")");
		return true;
	}
	else
	{
		m_strDynamicFileName =  "";
		g_pMainDlg->SetWindowText("动态文件编辑 -- " + m_strDynamicFileName);
		return false;
	}
}

int CCache::GetNpcFromRect(OUT vector<PPlaceNpc>& vec, const FRect& rect)
{
	COUT(GetNpcFromRect)
	return m_bcNpc.GetElement(vec, rect);
}

int CCache::GetObjFromRect(OUT vector<PPlaceObj>& vec, const FRect& rect)
{
	COUT(GetObjFromRect)
	return m_bcObj.GetElement(vec, rect);
}

int CCache::GetTrapFromRect(OUT vector<PPlaceTrap>& vec, const FRect& rect)
{
	COUT(GetTrapFromRect)
	return m_bcTrap.GetElement(vec, rect);
}

int CCache::GetPathKeyPosFromRect(OUT vector<PPathKeyPos>& vec, const FRect& rect)
{
	COUT(GetPathKeyPosFromRect)
	return m_bcPathKeyPos.GetElement(vec, rect);
}



bool CCache::SaveSceneFile(string savePath)
{
	string file = savePath;
	FILE* pFile  = fopen(file.c_str(), "wb");
	if (pFile == NULL)
	{
		return false;
	}
	size_t pos = savePath.rfind('\\');
	if (pos == -1)
	{
		pos = savePath.rfind('/');
	}
	string luaName = savePath.erase(0,pos+1);
	pos = luaName.rfind(".lua");
	string Name = luaName.erase(pos,pos+4);
	

	SaveNameField(pFile);
	char endString[CHAT_BUFFER_SIZE];

	sprintf(endString, "\n\ng_LifeOrigin:AddSinglePointBornTbl(\"scene/%s\", _npc)\n\n", Name.c_str());
	SaveNpc(pFile, endString);

	sprintf(endString, "\ng_IntObjServerMgr:AddIntObjTbl(\"scene/%s\", _obj)\n\n", Name.c_str());
	SaveObj(pFile, endString);

	sprintf(endString, "\ng_TrapServerMgr:AddTrapTbl(\"scene/%s\", _trap)\n\n", Name.c_str());
	SaveTrap(pFile, endString);
	fclose(pFile);
	return true;
}

bool CCache::SaveDynamicFile()
{
	if(m_strDynamicFileName == "")
	{
		g_pMainDlg->MessageBox("当前编辑对象为空.");
		return false;
	}
	::SetCurrentDirectory(CSEPathMgr::Inst().GetWorkingPath().GetBuffer());
	CString fileName;
	GetDynamicFileName(m_strDynamicFileName, fileName);
	if (fileName == "")
	{
		g_pMainDlg->MessageBox("当前编辑对象已在列表中删除.");
		return false;
	}
	CString dynamicFilePath = CSEPathMgr::Inst().GetDesignerPath() + "/scene/dynamic_file/" + fileName + ".lua";

	FILE* pFile  = fopen(dynamicFilePath.GetBuffer(), "wb");
	if (pFile == NULL)
	{
		return false;
	}
	
	CString listFilePath = CSEPathMgr::Inst().GetDesignerPath() + "/scene/dynamic_file/DynamicFileList.lua";
	FILE* pListFile  = fopen(listFilePath.GetBuffer(), "wb");
	if (pListFile == NULL)
	{
		fclose(pFile);
		return false;
	}
	SaveNameField(pFile);
	SaveCurDynamicFile(pFile, fileName);
	SaveDynamicFileList(pListFile);
	

	fclose(pFile);
	fclose(pListFile);
	ReLoadDynamicFile();
	return true;
}

void CCache::SaveDynamicFileList(FILE* pFile)
{
	CString strBegin = "tbl = {}";
	strcpy(m_szSave, strBegin.GetBuffer());
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	const char strIndexNode[] = "\ntbl[\"%s\"] = \"%s\"";
	CString fileName, name;
	map<CString, CString>::iterator iter = m_mapDynamicFile.begin();
	for (; iter != m_mapDynamicFile.end(); ++iter)
	{
		name = iter->first;
		fileName = iter->second;
		CString filePath = CSEPathMgr::Inst().GetDesignerPath() + "/scene/dynamic_file/" + fileName + ".lua";
		FILE* pFileTest  = fopen(filePath.GetBuffer(), "r");
		if(pFileTest) 
		{
			fclose(pFileTest);
			sprintf(m_szSave, strIndexNode, name, fileName);
			ConvertToUtf8(m_szSave);
			fwrite(m_szSave,strlen(m_szSave),1,pFile);
		}
	}
	CString strEnd = "\n\ng_DynamicCreateMgr:Init(tbl)";
	strcpy(m_szSave, strEnd.GetBuffer());
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	//LoadDynamicNames();
}

void CCache::SaveCurDynamicFile(FILE* pFile, const CString& fileName)
{
	CString strBegin = "tbl = {}";
	strcpy(m_szSave, strBegin.GetBuffer());
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	SaveNpc(pFile, "\ntbl.npc = _npc");
	SaveObj(pFile,"\ntbl.obj = _obj");
	SaveTrap(pFile,"\ntbl.trap = _trap");

	const char centerPos[] = "\ntbl.centerPos = { %0.2f, %0.2f}";
	sprintf(m_szSave, centerPos, m_DynamicCenterPos.x, m_DynamicCenterPos.y);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	CString strEnd = "\n\ng_DynamicCreateMgr:AddDynamicData(\"" + fileName +"\", tbl)";
	strcpy(m_szSave, strEnd.GetBuffer());
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
}

bool CCache::SaveDynamicPath()
{
	NpcBC_Iter npcIter = m_bcNpc.begin();
	for (; npcIter != m_bcNpc.end(); ++ npcIter)
	{
		PPlaceNpc pNpc = *npcIter;
		if ( (pNpc->moveType != 4 && pNpc->moveType != 5) && pNpc->pathList.size() == 0)
		{
			g_pMainDlg->MessageBox(pNpc->name + " 只有 1个巡逻点.(只有单线巡逻才能设置 1 个巡逻点) ");
			return false;
		}
	}


	::SetCurrentDirectory(CSEPathMgr::Inst().GetWorkingPath().GetBuffer());
	CString dynamicFilePath = CSEPathMgr::Inst().GetDesignerPath() + "/scene/DynamicPath.lua";

	m_pFile  = fopen(dynamicFilePath.GetBuffer(), "wb");
	if (m_pFile == NULL)
	{
		return false;
	}

	static const char strBegin[] = "local tbl = {\n";
	static const char strEnd [] = "}\n\n";
	static const char strFileEnd[] = "\n\ng_DynamicCreateMgr:InitPath(tbl)";


	
	static const char szNameField[] =
		"local nameField = {\n"
			"MoveType = 1,\n"
			"Speed = 2,\n"
			"MinDelayTime = 3,\n"
			"MaxDelayTime = 4,\n"
			"CenterPos = 5,\n"
			"Path = 6,\n"
		"}\n"
		;

	static const char szSetMt[] = 
		"for sn, st in pairs(tbl) do\n" 
			"for pn, p in pairs(st) do\n"
				"local mt = {__index = function(t,k)  return rawget(p, nameField[k]) end }\n"
				"setmetatable(p, mt )\n"
			"end\n"
		"end\n"
		;


	strcpy(m_szSave, strBegin);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, m_pFile);
	
	
	CString luaFun;
	luaFun.Format("SaveDynamicPath(\"%s\")", g_pMainDlg->GetCurSceneName().GetBuffer());

	RunLuaFile(luaFun.GetBuffer());

	strcpy(m_szSave, strEnd);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, m_pFile);
	
	strcpy(m_szSave, szNameField);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, m_pFile);

	strcpy(m_szSave, szSetMt);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, m_pFile);

	strcpy(m_szSave, strFileEnd);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, m_pFile);
	fclose(m_pFile);
	m_pFile = NULL;
	LoadDynamicPath(g_pMainDlg->GetCurSceneName());
	return true;
}

void CCache::Exp_SaveCurDynamicPath()
{
	//先保存当前编辑的场景 数据, 然后在从lua中保存没加载的 数据
	Exp_SaveSceneBegin(g_pMainDlg->GetCurSceneName().GetBuffer());

	NpcBC_Iter npcIter = m_bcNpc.begin();
	for (; npcIter != m_bcNpc.end(); ++ npcIter)
	{
		PPlaceNpc pNpc = *npcIter;
		Exp_SavePathBegin(pNpc->name.GetBuffer(), pNpc->moveType, pNpc->speed, pNpc->minDelayTime, pNpc->maxDelayTime, m_DynamicCenterPos.x, m_DynamicCenterPos.y);
		Exp_SavePathNode( pNpc->fPos.x, pNpc->fPos.y);
		PPathKeyPos pPos;
		CPlaceNpc::Path_Iter iter = pNpc->pathList.begin();
		for(; iter!= pNpc->pathList.end(); ++iter)
		{
			pPos = *iter;
			Exp_SavePathNode(pPos->fPos.x, pPos->fPos.y);
		}
		Exp_SavePathEnd();
	}
	Exp_SaveSceneEnd();
}

void CCache::Exp_SaveSceneBegin(const char* sceneName)
{
	static const char strSceneBegin[] = "[\"%s\"] = {";
	sprintf(m_szSave, strSceneBegin, sceneName, sceneName);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,m_pFile);
}

void CCache::Exp_SaveSceneEnd()
{
	static const char strSceneEnd[] = "},\n";
	strcpy(m_szSave, strSceneEnd);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,m_pFile);
}

void CCache::Exp_SavePathBegin(const char* pathName, int moveType, int speed, int minDelayTime, int maxDelayTime, float centerX, float centerY)
{
	static const char strPathBegin[] ="\n\t[\"%s\"] = {%d,%d,%d,%d,{%.2f, %.2f},{";
	sprintf(m_szSave, strPathBegin
		, pathName
		, moveType
		, speed
		, minDelayTime
		, maxDelayTime
		, centerX
		, centerY
		);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,m_pFile);
}

void CCache::Exp_SavePathNode(float x, float y)
{
	x -= m_DynamicCenterPos.x;
	y -= m_DynamicCenterPos.y;
	static const char strPathNode[] = "{%0.2f,%0.2f},";
	sprintf(m_szSave, strPathNode, x, y);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,m_pFile);
}

void CCache::Exp_SavePathEnd()
{
	static const char strPathEnd[] = "}},";
	strcpy(m_szSave, strPathEnd);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,m_pFile);
}


void CCache::Exp_SetDynamicCenterPos(float x, float y)
{
	m_DynamicCenterPos.x = x;
	m_DynamicCenterPos.y = y;
}

bool CCache::SaveAreaFile(string fileName)
{

	return true;
}

void CCache::SavePath(FILE* pFile, PPlaceNpc pNpc)
{
	CPlaceNpc::Path_Iter iter = pNpc->pathList.begin();
	if (iter == pNpc->pathList.end())
	{
		cout << " 存在空巡逻路径" << endl;
	}
	const char szPathNode[] = "{%0.2f,%0.2f},";
	strcpy(m_szSave, "{");
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,pFile);
	if (g_eModeType == eMT_DYNAMIC)
		sprintf(m_szSave,szPathNode, pNpc->fPos.x - m_DynamicCenterPos.x, pNpc->fPos.y - m_DynamicCenterPos.y);
	else
		sprintf(m_szSave,szPathNode, pNpc->fPos.x, pNpc->fPos.y);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	float x, y;
	PPathKeyPos pPos;
	for(; iter!= pNpc->pathList.end(); iter++)
	{
		pPos = *iter;
		x = pPos->fPos.x;
		y = pPos->fPos.y;
		if (g_eModeType == eMT_DYNAMIC)
		{
			x -= m_DynamicCenterPos.x;
			y -= m_DynamicCenterPos.y;
		}
		sprintf(m_szSave,szPathNode,x, y);
		ConvertToUtf8(m_szSave);
		fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	}
	strcpy(m_szSave, "},");
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,pFile);
}

void CCache::SaveNameField(FILE* pFile)
{
	const TCHAR szNameField[] =
		"local nameField = {\n"
			"Name = 1,\n"
			"GridX = 2,\n"
			"GridY = 3,\n"
			"Direction = 4,\n"
			"MoveType = 5,\n"
			"Speed = 6,\n"
			"MinDelayTime = 7,\n"
			"MaxDelayTime = 8,\n"
			"ColonyID = 9,\n"
			"ColonyType = 10,\n"
			"IdleType = 11,\n"
			"IsIgnoreClog = 12,\n"
			"Path = 13,\n"
		"}\n"
		;
	strcpy(m_szSave, szNameField);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
}

void CCache::ConvertToUtf8(TCHAR* m_szSave)
{
	//std::string tString = gbk_to_utf8(m_szSave);
	//int length = tString.length();
	//for(int i = 0; i < length; i++)
	//{
	//	m_szSave[i] = tString[i];
	//}
	//m_szSave[length] = '\0';
}

void CCache::SaveSingleNpc(FILE* pFile, PPlaceNpc pNpc)
{
	float x, y;
	x = pNpc->fPos.x;
	y = pNpc->fPos.y;
	if (g_eModeType == eMT_DYNAMIC)
	{
		x -= m_DynamicCenterPos.x;
		y -= m_DynamicCenterPos.y;
	}

	strcpy(m_szSave, "{");
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	sprintf(m_szSave,"\"%s\", ",pNpc->name.GetBuffer());
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	sprintf(m_szSave,"%.2f, ",x);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	sprintf(m_szSave,"%.2f, ",y);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	sprintf(m_szSave,"%d, ",pNpc->direction);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	sprintf(m_szSave,"%d, ", pNpc->moveType);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	if (pNpc->moveType == 0)
		sprintf(m_szSave,"nil, ");
	else
		sprintf(m_szSave,"%d, ", pNpc->speed);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	if (pNpc->moveType == 0)
		sprintf(m_szSave,"nil, ");
	else
		sprintf(m_szSave,"%d, ", pNpc->minDelayTime);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	if (pNpc->moveType == 0)
		sprintf(m_szSave,"nil, ");
	else
		sprintf(m_szSave,"%d, ", pNpc->maxDelayTime);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	sprintf(m_szSave,"%d, ", pNpc->colonyId);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	sprintf(m_szSave,"%d, ", pNpc->colonyType);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	if (pNpc->moveType != 0)
		sprintf(m_szSave,"nil, ");
	else
		sprintf(m_szSave,"%d, ", pNpc->idleType);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	sprintf(m_szSave,"%d, ", pNpc->isIgnoreClog);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	if (pNpc->moveType != 0 && pNpc->moveType != 3)
	{
		SavePath(pFile, pNpc);
	}

	strcpy(m_szSave, "},\n");
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
}

void CCache::SaveNpc(FILE* pFile, string endString)
{
	if (m_bcNpc.empty())
	{
		return;
	}

	const TCHAR szNpcBegin[] = "\nlocal _npc = {\n";
	strcpy(m_szSave, szNpcBegin);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	PPlaceNpc pNpc;
	int modifyCount = 0;
	for (NpcIndex_Iter iter = m_mapNpcIndex.begin(); iter != m_mapNpcIndex.end(); ++iter)
	{
		pNpc = iter->second;
		if (pNpc->pathList.size() == 0 && pNpc->moveType != 0 && pNpc->moveType != 3)//循环巡逻或往返巡逻不存在路径
		{
			pNpc->moveType = 0;//将移动类型改为固定
			++modifyCount;
		}
		SaveSingleNpc(pFile, pNpc);
	}
	
	
	for(NpcBC_Iter iter = m_bcNpc.begin(); iter!=m_bcNpc.end(); ++iter)
	{
		pNpc = *iter;
		if (pNpc->index != -1) //原始的对象 在上面先保存了, 这里不再保存
		{
			continue;
		}
		if (pNpc->pathList.size() == 0 && pNpc->moveType != 0 && pNpc->moveType != 3)//循环巡逻或往返巡逻不存在路径
		{
			pNpc->moveType = 0;//将移动类型改为固定
			++modifyCount;
		}
		SaveSingleNpc(pFile, pNpc);
	}
	strcpy(m_szSave, "}\n");
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	
	const TCHAR szSetMt[] = 
		"for i =1,#_npc do\n" 
			"local mt = {__index = function(t,k)  local j = i   return rawget(_npc[j], nameField[k]) end }\n"
			"setmetatable(_npc[i], mt )\n"
		"end\n";
	sprintf(m_szSave,szSetMt);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,pFile);
	
	strcpy(m_szSave, endString.c_str());
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);

	if(modifyCount > 0)
	{
		CString s;
		s.Format("存在 %d 处巡逻类型Npc无巡逻路径, 已被修改成 '固定' 类型", modifyCount);
		g_pMainDlg->MessageBox(s);
	}
}


void CCache::SaveObj(FILE* pFile,string endString)
{
	if(g_eModeType != eMT_AREA_FB && m_bcObj.empty())
	{
		return;
	}

	const TCHAR szObjBegin[] = "\nlocal _obj = {\n";
	const TCHAR szObjFixed[] =  "{\"%s\", %0.2f, %0.2f, %d},\n";
	strcpy(m_szSave, szObjBegin);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	float x, y;
	PPlaceObj pObj;
	for (ObjIndex_Iter iter = m_mapObjIndex.begin(); iter != m_mapObjIndex.end(); ++iter)
	{
		pObj = iter->second;
		x = pObj->fPos.x;
		y = pObj->fPos.y;
		if (g_eModeType == eMT_DYNAMIC)
		{
			x -= m_DynamicCenterPos.x;
			y -= m_DynamicCenterPos.y;
		}
		sprintf(m_szSave, szObjFixed, pObj->name.GetBuffer(), x, y,  pObj->direction);
		ConvertToUtf8(m_szSave);
		fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	}

	for(ObjBC_Iter iter = m_bcObj.begin(); iter != m_bcObj.end(); ++iter)
	{
		pObj = *iter;
		if (pObj->index != -1)//原始的对象 在上面先保存了, 这里不再保存
		{
			continue;
		}
		x = pObj->fPos.x;
		y = pObj->fPos.y;
		if (g_eModeType == eMT_DYNAMIC)
		{
			x -= m_DynamicCenterPos.x;
			y -= m_DynamicCenterPos.y;
		}
		sprintf(m_szSave, szObjFixed, pObj->name.GetBuffer(), x, y,  pObj->direction);
		ConvertToUtf8(m_szSave);
		fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	}
	if (g_eModeType == eMT_AREA_FB)
	{
		//CheckBarrierObj();
		CString barrierObjName = g_pMainDlg->GetBarrierObjName();
		int width = g_pMapView->GetMapGridWidth();
		int height = g_pMapView->GetMapGridHeight();
		int x, y;
		for (x = 0; x < width; x++)
		{
			for (y = 0; y < height; y++)
			{
				if (!g_pMapView->IsBarrierObj(x,y))
				{
					continue;
				}
				sprintf(m_szSave, szObjFixed, barrierObjName.GetBuffer(), x+0.5, y+0.5,  0);
				ConvertToUtf8(m_szSave);
				fwrite(m_szSave, strlen(m_szSave), 1, pFile);
			}
		}
	}
	strcpy(m_szSave, "}\n");
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	const TCHAR szSetMt[] = 
		"for i =1,#_obj do\n" 
		"local mt = {__index = function(t,k)  local j = i   return rawget(_obj[j], nameField[k]) end }\n"
		"setmetatable(_obj[i], mt )\n"
		"end\n";
	sprintf(m_szSave,szSetMt);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,pFile);

	strcpy(m_szSave, endString.c_str());
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
}

void CCache::SaveTrap(FILE* pFile,string endString)
{
	if (m_bcTrap.empty())
	{
		return;
	}
	const TCHAR szTrapBegin[] = "\nlocal _trap = {\n";
	const TCHAR szTrapFixed[] =  "{\"%s\", %0.2f, %0.2f, %d},\n";
	strcpy(m_szSave, szTrapBegin);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	float x, y;
	PPlaceTrap pTrap;
	for (TrapIndex_Iter iter = m_mapTrapIndex.begin(); iter != m_mapTrapIndex.end(); ++iter)
	{
		pTrap = iter->second;
		x = pTrap->fPos.x;
		y = pTrap->fPos.y;
		if (g_eModeType == eMT_DYNAMIC)
		{
			x -= m_DynamicCenterPos.x;
			y -= m_DynamicCenterPos.y;
		}
		sprintf(m_szSave, szTrapFixed, pTrap->name.GetBuffer(), x, y,  pTrap->direction);
		ConvertToUtf8(m_szSave);
		fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	}
	
	for(TrapBC_Iter iter = m_bcTrap.begin(); iter != m_bcTrap.end(); iter++)
	{
		pTrap = *iter;
		if (pTrap->index != -1)//原始的对象 在上面先保存了, 这里不再保存
		{
			continue;
		}
		x = pTrap->fPos.x;
		y = pTrap->fPos.y;
		if (g_eModeType == eMT_DYNAMIC)
		{
			x -= m_DynamicCenterPos.x;
			y -= m_DynamicCenterPos.y;
		}
		sprintf(m_szSave, szTrapFixed, pTrap->name.GetBuffer(), x, y,  pTrap->direction);
		ConvertToUtf8(m_szSave);
		fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	}

	strcpy(m_szSave, "}\n");
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
	const TCHAR szSetMt[] = 
		"for i =1,#_trap do\n" 
		"local mt = {__index = function(t,k)  local j = i   return rawget(_trap[j], nameField[k]) end }\n"
		"setmetatable(_trap[i], mt )\n"
		"end\n";
	sprintf(m_szSave,szSetMt);
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave,strlen(m_szSave),1,pFile);

	strcpy(m_szSave, endString.c_str());
	ConvertToUtf8(m_szSave);
	fwrite(m_szSave, strlen(m_szSave), 1, pFile);
}

void CCache::MoveNpc(PPlaceNpc pNpc, const FPos& fPos)
{
	if (!g_pMapView->CheckMoveNpc(pNpc, fPos))
	{
		return;
	}
	NpcBC_Iter iter = m_bcNpc.find(pNpc->fPos.x, pNpc->fPos.y);
	Ast(iter != m_bcNpc.end() || pNpc == *iter);
	if (m_bcNpc.Move(iter, fPos.x, fPos.y))
	{
		pNpc->SetPos(fPos);
	}
	else
	{
		cout << "Npc移动失败" << endl;
	}
}

void CCache::MoveObj(PPlaceObj pObj, const FPos& fPos)
{
	ObjBC_Iter iter = m_bcObj.find(pObj->fPos.x, pObj->fPos.y);
	Ast(iter != m_bcObj.end() || pObj == *iter);
	if (m_bcObj.Move(iter, fPos.x, fPos.y))
	{
		pObj->SetPos(fPos);
	}
	else
	{
		cout << "Obj移动失败" << endl;
	}
}

void CCache::MoveTrap(PPlaceTrap pTrap, const FPos& fPos)
{
	TrapBC_Iter iter = m_bcTrap.find(pTrap->fPos.x, pTrap->fPos.y);
	Ast(iter != m_bcTrap.end() || pTrap == *iter);
	if (m_bcTrap.Move(iter, fPos.x, fPos.y))
	{
		pTrap->SetPos(fPos);
	}
	else
	{
		cout << "Trap移动失败" << endl;
	}
}

void CCache::MovePathKeyPos(PPathKeyPos pKeyPos, const FPos& fPos)
{
	if(!g_pMapView->CheckMovePathKeyPos(pKeyPos, fPos))
	{
		return;
	}
	PathKeyPosBC_Iter iter = m_bcPathKeyPos.find(pKeyPos->fPos.x, pKeyPos->fPos.y);
	Ast(iter != m_bcPathKeyPos.end() || pKeyPos == *iter);
	if (m_bcPathKeyPos.Move(iter, fPos.x, fPos.y))
	{
		pKeyPos->fPos = fPos;
	}
	else
	{
		cout << "巡逻点移动失败" << endl;
	}
}

PPathKeyPos CCache::InsetPathKeyPos(CPathLine* pPathLine, const FPos& fPos)
{
	if (pPathLine == NULL || pPathLine->pBackKeyPos == NULL)
	{
		cout << "在巡逻线上添加巡逻点失败" << endl;
		return NULL;
	}
	return pPathLine->pBackKeyPos->pOwner->InsetPathKeyPos(pPathLine->pBackKeyPos,fPos);
}

PPlaceNpc CCache::FindNpc(const CString& name)
{
	NpcBC_Iter iter = m_bcNpc.begin();
	while (iter != m_bcNpc.end())
	{
		if ((*iter)->name == name)
		{
			return *iter;
		}
		iter++;
	}
	return NULL;
}

PPlaceObj CCache::FindObj(const CString& name)
{
	ObjBC_Iter iter = m_bcObj.begin();
	while (iter != m_bcObj.end())
	{
		if ((*iter)->name == name)
		{
			return *iter;
		}
		iter++;
	}
	return NULL;
}

PPlaceTrap CCache::FindTrap(const CString& name)
{
	TrapBC_Iter iter = m_bcTrap.begin();
	while (iter != m_bcTrap.end())
	{
		if ((*iter)->name == name)
		{
			return *iter;
		}
		iter++;
	}
	return NULL;
}

PPlaceNpc CCache::FindGroupBoss(int colonyId)
{
	NpcBC_Iter iter = m_bcNpc.begin();
	while (iter != m_bcNpc.end())
	{
		if ((*iter)->colonyId == colonyId && (*iter)->colonyType == CEnumMgr::Inst().GetColonyTypeID("BOSS"))
		{
			return *iter;
		}
		iter++;
	}
	return NULL;
}

bool CCache::IsColonyExist(int colonyId)
{
	NpcBC_Iter iter = m_bcNpc.begin();
	while (iter != m_bcNpc.end())
	{
		if ((*iter)->colonyId == colonyId)
		{
			return true;
		}
		iter++;
	}
	return false;
}

int CCache::GetNextColonyId()
{
	int colonyId = 1;
	do 
	{
		if (!IsColonyExist(colonyId))
		{
			return colonyId;
		}
		++colonyId;
	} while(true);
}

void CCache::GetColonyMember(OUT Element_Vec& vecElement,int iColonyId)
{
	if(iColonyId == 0)
	{
		return;
	}
	NpcBC_Iter iter = m_bcNpc.begin();
	while (iter != m_bcNpc.end())
	{
		if ( (*iter)->colonyId == iColonyId)
		{
			vecElement.push_back(static_cast<CElement*> (*iter));
		}
		++iter;
	}
}

bool CCache::AddBarrierObj(const CString& trapName, int gridX, int gridY)
{
	if(gridX < 0 || gridX > g_pMapView->GetMapGridWidth() || gridY < 0 || gridY > g_pMapView->GetMapGridHeight())
	{
		cout <<" 超出地图范围" << endl;
		return false;
	}
	//PPlaceObj pNewObj = new PlaceObj();
	//pNewObj->fPos.x = gridX + 0.5f; //放在格子中间
	//pNewObj->fPos.y = gridY + 0.5f;
	//pNewObj->name = trapName;
	// if( m_mapBarrierObj.insert(make_pair (make_pair (gridX,gridY),pNewObj)).second )
	// {
	//	 return true;
	// }
	// else
	// {
	//	 delete pNewObj;
	//	 return false;
	// }
	g_pMapView->AddBarrierObj(Pos(gridX, gridY));
	return true;
}
//
//bool CCache::DelBarrierObj(int gridX, int gridY)
//{
//	BarrierObjMap_Iter iter = m_mapBarrierObj.find(make_pair(gridX,gridY));
//	if (iter != m_mapBarrierObj.end())
//	{
//		delete iter->second;
//		m_mapBarrierObj.erase(iter);
//		return true;
//	}
//	return false;
//}
//
//bool CCache::IsHaveBarrierObj(int gridX, int gridY)
//{
//	return m_mapBarrierObj.find(make_pair(gridX,gridY)) !=  m_mapBarrierObj.end();
//}
//
//
//void CCache::CheckBarrierObj()
//{
//	map<pair<int,int>, CString> mapAddPos;//补充的OBJ  位置map
//
//	int x, y;
//	BarrierObjMap_Iter srcIter = m_mapBarrierObj.begin();
//	for (; srcIter != m_mapBarrierObj.end(); ++srcIter)
//	{
//		x = srcIter->first.first;
//		y = srcIter->first.second;
//		for (int i = 0; i < 8; i += 2)
//		{
//			if (IsHaveBarrierObj(x + OFFSET_8DIR[i][0],y + OFFSET_8DIR[i][1]))
//			{
//				int offset1 = i < 7 ? i + 1 : i - 7;
//				int offset2 = i > 1 ? i - 1 : i + 7;
//				int x1 = x + OFFSET_8DIR[offset1][0];
//				int y1 = y + OFFSET_8DIR[offset1][1];
//				int x2 = x + OFFSET_8DIR[offset2][0];
//				int y2 = y + OFFSET_8DIR[offset2][1];
//				if (!IsHaveBarrierObj(x1, y1)
//					&&!IsHaveBarrierObj(x2, y2)
//					&&!g_pMapView->IsBlock(x1, y1)
//					&&!g_pMapView->IsBlock(x2, y2)
//					&&mapAddPos.find(make_pair(x1,y1)) == mapAddPos.end()
//					&&mapAddPos.find(make_pair(x2,y2)) == mapAddPos.end())
//				{
//					mapAddPos.insert(make_pair(make_pair(x1,y1),srcIter->second->name));
//					cout << x << "  " << y << endl;
//					cout << x1 << "  " << y1 << endl;
//				}
//			}
//		}
//	}
//	
//	map<pair<int,int>, CString>::iterator addIter = mapAddPos.begin();
//	for(; addIter != mapAddPos.end(); ++addIter)
//	{
//		AddBarrierObj(addIter->second, addIter->first.first, addIter->first.second);
//	}
//}

bool CCache::IsDynamicName(const CString& name)
{
	return m_mapDynamicFile.find(name) != m_mapDynamicFile.end();
}

bool CCache::IsDynamicFileName(const CString& name)
{
	map<CString, CString>::iterator iter = m_mapDynamicFile.begin();
	for (; iter != m_mapDynamicFile.end(); ++iter)
	{
		if (iter->second == name)
		{
			return true;
		}
	}
	return false;
}

void CCache::GetDynamicFileName(const CString& name, OUT CString& fileName)
{
	map<CString, CString>::iterator iter = m_mapDynamicFile.find(name);
	if(iter != m_mapDynamicFile.end())
	{
		fileName = iter->second;
	}
}

bool CCache::GetDynamicName(const CString& fileName, OUT CString& name)
{
	map<CString, CString>::iterator iter = m_mapDynamicFile.begin();
	for (; iter != m_mapDynamicFile.end(); ++iter)
	{
		if (iter->second == fileName)
		{
			name = iter->first; 
			return true;
		}
	}
	return false;
}

void CCache::ReLoadDynamicFile()
{
	::SetCurrentDirectory(CSEPathMgr::Inst().GetWorkingPath().GetBuffer());
	CString fileName = CSEPathMgr::Inst().GetDesignerPath() + "/scene/dynamic_file/DynamicFileList.lua";
	FILE* pFile  = fopen(fileName.GetBuffer(), "r");
	if(NULL==pFile) 
	{
		return;
	}
	fclose(pFile);
	CString str = "dofile  \"" + fileName + "\"";

	RunLuaFile(str.GetBuffer());
}

void CCache::LoadDynamicNames()
{
	m_mapDynamicFile.clear();
	RunLuaFile("LoadDynamicNames()");
}

CString CCache::GetCurEditDynamicName()
{	
	return m_strDynamicFileName;
}

void CCache::SetCurEditDynamicName(const CString& name)
{
	m_strDynamicFileName = name;
	g_pMainDlg->SetWindowText("动态文件编辑 -- " + m_strDynamicFileName);
}

void CCache::RemoveDynamicName(const CString& name)
{
	map<CString, CString>::iterator iter = m_mapDynamicFile.find(name);
	if (iter != m_mapDynamicFile.end())
	{
		m_mapDynamicFile.erase(iter);
	}
}

void CCache::LoadDynamicPath(const CString& sceneName)
{
	::SetCurrentDirectory(CSEPathMgr::Inst().GetWorkingPath().GetBuffer());
	CString fileName = CSEPathMgr::Inst().GetDesignerPath() + "/scene/DynamicPath.lua";
	FILE* pFile  = fopen(fileName, "r");
	if(NULL != pFile) 
	{
		fclose(pFile);
		CString str = "dofile  \"" + fileName + "\"";
		RunLuaFile(str.GetBuffer());
	}

	ClearAll();
	m_mapDynamicFile.clear();
	CString s = "LoadDynamicPath(\"" + sceneName + "\")";
	RunLuaFile(s.GetBuffer());
}


bool CCache::SetCenterPos()
{
	CSetCenterPosDlg dlg;
	if ( IDOK == dlg.DoModal())
	{
		m_DynamicCenterPos = dlg.GetCenterPos();
		return true;
	}
	return false;
}

FPos CCache::GetCenterPos()
{
	return m_DynamicCenterPos;
}

void CCache::ComputeCenterPos()
{
	int count = m_bcNpc.size() + m_bcObj.size() + m_bcTrap.size() + m_bcPathKeyPos.size();
	if (count == 0)
	{
		m_DynamicCenterPos.x = 0;
		m_DynamicCenterPos.y = 0;
		return;
	}
	float minX = (float)g_pMapView->GetMapGridWidth(), minY = (float)g_pMapView->GetMapGridHeight();
	float maxX = 0, maxY = 0;


	for(NpcBC_Iter iter = m_bcNpc.begin(); iter!=m_bcNpc.end(); ++iter)
	{
		if ((*iter)->fPos.x < minX)
		{
			minX = (*iter)->fPos.x;
		}
		if ((*iter)->fPos.x > maxX)
		{
			maxX = (*iter)->fPos.x;
		}

		if ((*iter)->fPos.y < minY)
		{
			minY = (*iter)->fPos.y;
		}
		if ((*iter)->fPos.y > maxY)
		{
			maxY = (*iter)->fPos.y;
		}
	}
	for(ObjBC_Iter iter = m_bcObj.begin(); iter!=m_bcObj.end(); ++iter)
	{
		if ((*iter)->fPos.x < minX)
		{
			minX = (*iter)->fPos.x;
		}
		if ((*iter)->fPos.x > maxX)
		{
			maxX = (*iter)->fPos.x;
		}

		if ((*iter)->fPos.y < minY)
		{
			minY = (*iter)->fPos.y;
		}
		if ((*iter)->fPos.y > maxY)
		{
			maxY = (*iter)->fPos.y;
		}
	}
	for(TrapBC_Iter iter = m_bcTrap.begin(); iter!=m_bcTrap.end(); ++iter)
	{
		if ((*iter)->fPos.x < minX)
		{
			minX = (*iter)->fPos.x;
		}
		if ((*iter)->fPos.x > maxX)
		{
			maxX = (*iter)->fPos.x;
		}

		if ((*iter)->fPos.y < minY)
		{
			minY = (*iter)->fPos.y;
		}
		if ((*iter)->fPos.y > maxY)
		{
			maxY = (*iter)->fPos.y;
		}
	}
	for(PathKeyPosBC_Iter iter = m_bcPathKeyPos.begin(); iter!=m_bcPathKeyPos.end(); ++iter)
	{
		if ((*iter)->fPos.x < minX)
		{
			minX = (*iter)->fPos.x;
		}
		if ((*iter)->fPos.x > maxX)
		{
			maxX = (*iter)->fPos.x;
		}

		if ((*iter)->fPos.y < minY)
		{
			minY = (*iter)->fPos.y;
		}
		if ((*iter)->fPos.y > maxY)
		{
			maxY = (*iter)->fPos.y;
		}
	}
	m_DynamicCenterPos.x = (minX + maxX) / 2;
	m_DynamicCenterPos.y = (minY + maxY) / 2;
}

bool CCache::SetOffsetPos()
{
	CSetOffsetPositionDlg dlg(m_OffsetPos.x, m_OffsetPos.y);
	if ( IDOK == dlg.DoModal())
	{
		//FPos newOffsetPos;
		dlg.GetOffsetPos(m_OffsetPos);
		return true;
	}
	return false;
}

const FPos& CCache::GetOffsetPos()
{
	return m_OffsetPos;
}

void CCache::AddArea(const CString& strName)
{
	Ast(GetAreaData(strName) == NULL);
	m_mapAreaData[strName] = new AreaData(strName);
}

PAreaData CCache::GetAreaData(const CString& strName)
{
	AreaData_Iter iter = m_mapAreaData.find(strName);
	if (iter != m_mapAreaData.end())
	{
		return iter->second;
	}
	return NULL;
}

bool CCache::DelArea(const CString& strName)
{
	AreaData_Iter iter = m_mapAreaData.find(strName);
	if (iter != m_mapAreaData.end())
	{
		delete iter->second;
		m_mapAreaData.erase(iter);
		return true;
	}
	return false;
}

bool CCache::ModifyAreaName(const CString& strOldName, const CString& strNewName)
{
	Ast( GetAreaData(strNewName) == NULL);
	AreaData_Iter iter = m_mapAreaData.find(strOldName);
	if (iter != m_mapAreaData.end())
	{
		PAreaData pAreaData = iter->second;
		pAreaData->name = strNewName;
		m_mapAreaData.erase(iter);
		m_mapAreaData[strNewName] = pAreaData;
		return true;
	}
	return false;
}

bool CCache::SetAreaColor(const CString& strName, COLORREF color)
{
	PAreaData pAreaData = GetAreaData(strName);
	if (pAreaData)
	{
		pAreaData->color = color;
		return true;
	}
	return false;
}

bool CCache::SetAreaFbState(const CString& strName, bool bIsFb)
{
	PAreaData pAreaData = GetAreaData(strName);
	if (pAreaData)
	{
		pAreaData->isFb = bIsFb;
		return true;
	}
	return false;
}

bool CCache::SetAreaRatingPlayerNum(const CString& strName, uint32 nPalayerNum)
{
	PAreaData pAreaData = GetAreaData(strName);
	if (pAreaData)
	{
		pAreaData->ratingPlayerNum = nPalayerNum;
		return true;
	}
	return false;
}

bool CCache::SetAreaMaxPlayerNum(const CString& strName, uint32 nPalayerNum)
{
	PAreaData pAreaData = GetAreaData(strName);
	if (pAreaData)
	{
		pAreaData->maxPlayerNum = nPalayerNum;
		return true;
	}
	return false;
}

bool CCache::SetAreaMusic(const CString& strName, const CString& strMusic)
{
	PAreaData pAreaData = GetAreaData(strName);
	if (pAreaData)
	{
		pAreaData->music = strMusic;
		return true;
	}
	return false;
}

COLORREF CCache::GetAreaColor(const CString& strName)
{
	PAreaData pAreaData = GetAreaData(strName);
	if (pAreaData)
	{
		return pAreaData->color;
	}
	return -1;
}

uint32 CCache::GetAreaRatingPlayerNum(const CString& strName)
{
	return GetAreaData(strName)->ratingPlayerNum;
}

uint32 CCache::GetAreaMaxPlayerNum(const CString& strName)
{
	return GetAreaData(strName)->maxPlayerNum;
}

bool CCache::GetAreaFbState(const CString& strName)
{
	return GetAreaData(strName)->isFb;
}

CString CCache::GetAreaMusic(const CString& strName)
{
	return GetAreaData(strName)->music;
}

CString CCache::GetAreaName(COLORREF color)
{
	AreaData_Iter iter = m_mapAreaData.begin();
	while (iter != m_mapAreaData.end())
	{
		if (iter->second->color == color)
		{
			return iter->first;
		}
		++iter;
	}
	return ""; 
}

int CCache::GetAreaCount()
{
	return m_mapAreaData.size();
}

bool CCache::IsAreaColorUsed(COLORREF color)
{
	if (color == -1)
	{
		return false;
	}
	AreaData_Iter iter = m_mapAreaData.begin();
	while (iter != m_mapAreaData.end())
	{
		if (iter->second->color == color)
		{
			return true;
		}
		++iter;
	}
	return false;
}