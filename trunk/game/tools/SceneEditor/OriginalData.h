#pragma once
#include "DataType.h"
#include "NpcPage.h"
#include "ObjPage.h"
#include "TrapPage.h"

class CNewSceneEditorDlg;
class CChoiceMap;

//此类管理些原始数据,(不会改变的配置表数据, 如 npc_common, trap_common,IntObj_Common 等)
class COriginalData: public virtual CDynamicObject
{
public:
	COriginalData(void);
	~COriginalData(void);
	
	friend CNpcPage;
	friend CObjPage;
	friend CTrapPage;
	friend CNewSceneEditorDlg;
	friend CChoiceMap;
public:
	PNPC  Exp_AddNpc(const char* _名字,
					   const char*		_类型,
					   const char*  _AI类型,
					   uint8		_Npc是否唯一,
					   uint8		_NPC阵营,
					   uint8		_等级下限,
					   uint8		_等级上限,
					   uint32		_刷新时间,
					   uint8		_职业
					   );
	void Exp_AddIntObj(const char *name, uint8 isSingle, uint8 isBarrier);
	void Exp_AddTrap(const char *name, uint8 isSingle);
	bool Exp_AddSceneCfg(const char* sceneName, const char* mapFile, const char* sceneFile, const char* areaFile);

	void DelAll();
	bool IsSingleNpc(const CString& name);
	bool IsSingleObj(const CString& name);
	bool IsSingleTrap(const CString& name);
	bool IsBarrierObj(const CString& name);
	bool IsNpcInCommonCfg(const CString& name);
	bool IsObjInCommonCfg(const CString& name);
	bool IsTrapInCommonCfg(const CString& name);
	PNPC GetNpc(const CString& name);

	CString GetMapFile(const CString& sceneName);
	CString GetSceneFile(const CString& sceneName);
	CString GetAreaFile(const CString& sceneName);
private:
	typedef std::map<CString, PNPC> NpcData_Map;
	typedef NpcData_Map::iterator  NpcData_Iter;
	typedef std::map<CString, POBJ> ObjData_Map;
	typedef ObjData_Map::iterator  ObjData_Iter;
	typedef std::map<CString, PTRAP> TrapData_Map;
	typedef TrapData_Map::iterator  TrapData_Iter;
	typedef std::vector<CString> BarrierObj_Vec;
	typedef BarrierObj_Vec::iterator	BarrierObj_Iter;
	typedef map<CString, PSceneCfg>		SceneCfg_Map;
	typedef SceneCfg_Map::iterator	SceneCfg_Iter;

	SceneCfg_Map				m_mapSceneCfg;
	NpcData_Map					m_mapNpcData;
	ObjData_Map					m_mapObjData;
	TrapData_Map				m_mapTrapData;
	BarrierObj_Vec				m_vecBarrierObj;
};
