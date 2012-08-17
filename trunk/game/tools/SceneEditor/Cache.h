#pragma once

#include "PlaceElement.h"
#include "DataType.h"
#include "BlockContainer.h"
#include "MyDefine.h"
#include "../../engine/sdk/inc/StringHelper.h"

class CNewSceneEditorDlg;
class CMapView;
class CColonyEditDlg;

//用来保存和处理所有 用户编辑的数据 
class CCache: public virtual CDynamicObject
{
public:
	friend class CMapView;
	friend class CNewSceneEditorDlg;
	friend class CAreaEditDlg;
	friend class CColonyEditDlg;

	typedef	TBlockContainer<PPlaceNpc>		Npc_BC;
	typedef	TBlockContainer<PPlaceObj>		Obj_BC;
	typedef	TBlockContainer<PPlaceTrap>		Trap_BC;
	typedef	TBlockContainer<PPathKeyPos>	PathKeyPos_BC;
	typedef vector<CElement*>				 Element_Vec;

	typedef	Npc_BC::iterator		NpcBC_Iter;
	typedef	Obj_BC::iterator		ObjBC_Iter;
	typedef	Trap_BC::iterator		TrapBC_Iter;
	typedef	PathKeyPos_BC::iterator		PathKeyPosBC_Iter;

	typedef map<int, PPlaceNpc>			NpcIndex_Map;
	typedef map<int, PPlaceObj>			ObjIndex_Map;
	typedef map<int, PPlaceTrap>		TrapIndex_Map;

	typedef NpcIndex_Map::iterator	NpcIndex_Iter;
	typedef ObjIndex_Map::iterator	ObjIndex_Iter;
	typedef TrapIndex_Map::iterator	TrapIndex_Iter;
	
	typedef map<CString, PAreaData>		AreaData_Map;
	typedef AreaData_Map::iterator		AreaData_Iter;

	CCache(void);
	~CCache(void);
	void ClearAll();
	void ClearNpc();
	void ClearObj();
	void ClearTrap();
	void ClearPathKeyPos();

	void ClearAreaData();

	PPlaceNpc Exp_AddPlaceNpc(int index, const char*	_name, float	_x, float _y);
	bool Exp_AddPlaceObj(int index, const char* name, float x, float y,int _direction);
	bool Exp_AddPlaceTrap(int index, const char* name, float x, float y,int _direction);
	bool Exp_AddAreaLine(int x,int y1, int y2,const char* name);
	bool Exp_AddAreaMusic(const char* areaName, const char* musicName);
	bool Exp_SetAreaMaxPlayerNum(const char* areaName, int playNum);
	bool Exp_SetAreaRatingPlayerNum(const char* areaName, int playNum);
	void Exp_AddDynamicFile(const char* name, const char* fileName);

	
	PPlaceNpc FindNpc(const CString& name);
	PPlaceObj FindObj(const CString& name);
	PPlaceTrap FindTrap(const CString& name);
	PPlaceNpc FindGroupBoss(int colonyId);
	bool IsColonyExist(int colonyId);
	int	GetNextColonyId();
	void GetColonyMember(OUT Element_Vec& vecElement,int colonyId);
	
	bool DeleteNpc(PPlaceNpc pNpc);
	bool DeleteObj(PPlaceObj pObj);
	bool DeleteTrap(PPlaceTrap pTrap);
	bool AddPathKeyPos(PPathKeyPos pKeyPos);
	bool DeletePathKeyPos(PPathKeyPos pKeyPos);
	bool RemovePathKeyPos(PPathKeyPos pKeyPos);
	bool LoadSceneData(const CString& filePath);
	bool LoadAreaData(const CString& filePath);
	bool LoadDynamicData(const CString& name);
	int  GetNpcFromRect(OUT vector<PPlaceNpc>& vec, const FRect& rect);
	int  GetObjFromRect(OUT vector<PPlaceObj>& vec, const FRect& rect);
	int  GetTrapFromRect(OUT vector<PPlaceTrap>& vec, const FRect& rect);
	int  GetPathKeyPosFromRect(OUT vector<PPathKeyPos>& vec, const FRect& rect);
	CElement* AddNpc(PPlaceNpc pNpc,float x, float y);
	CElement* AddObj(PPlaceObj pObj, float x, float y);
	CElement* AddTrap(PPlaceTrap pTrap, float x, float y);


	//----------------编辑相关函数
	void MoveNpc(PPlaceNpc pNpc,const FPos& fPos);
	void MoveObj(PPlaceObj pObj, const FPos& fPos);
	void MoveTrap(PPlaceTrap pTrap, const FPos& fPos);
	void MovePathKeyPos(PPathKeyPos pKeyPos, const FPos& fPos);
	PPathKeyPos InsetPathKeyPos(CPathLine* pPathLine, const FPos& fPos);//插入一个巡逻点



	//-----------------保存相关函数-----------------------
	bool SaveSceneFile(string savePath);
	bool SaveAreaFile(string savePath);
	bool SaveDynamicFile();
	bool SaveDynamicPath();
	
	void SaveNameField(FILE* pFile);
	void SaveNpc(FILE* pFile, string endString);
	void SaveSingleNpc(FILE* pFile, PPlaceNpc pNpc);
	void SaveObj(FILE* pFile, string endString);
	void SaveTrap(FILE* pFile, string endString);
	void SavePath(FILE* pFile, PPlaceNpc pNpc);
	void SaveDynamicFileList(FILE* pFile);
	void SaveCurDynamicFile(FILE* pFile, const CString& fileName);
	
	void Exp_SaveCurDynamicPath();
	void Exp_SaveSceneBegin(const char* sceneName);
	void Exp_SaveSceneEnd();
	void Exp_SavePathBegin(const char* pathName, int moveType, int speed, int minDelayTime, int maxDelayTime, float centerX, float centerY);
	void Exp_SavePathNode(float x, float y);
	void Exp_SavePathEnd();
	void Exp_SetDynamicCenterPos(float x, float y);

	bool AddBarrierObj(const CString& trapName, int gridX, int gridY);

	void GetDynamicFileName(const CString& name, OUT CString& fileName);
	bool GetDynamicName(const CString& fileName, OUT CString& name);
	bool IsDynamicFileName(const CString& name);
	bool IsDynamicName(const CString& name);
	void ReLoadDynamicFile();
	void LoadDynamicNames();
	CString GetCurEditDynamicName();
	void SetCurEditDynamicName(const CString& name);
	void RemoveDynamicName(const CString& name);
	void LoadDynamicPath(const CString& sceneName);

	bool SetCenterPos();
	FPos GetCenterPos();
	void ComputeCenterPos();
	bool IsSetCenterPos();

	bool SetOffsetPos();
	const FPos& GetOffsetPos();

	void AddArea(const CString& strName);
	PAreaData GetAreaData(const CString& strName);
	bool DelArea(const CString& strName);
	bool ModifyAreaName(const CString& strOldName, const CString& strNewName);
	bool SetAreaColor(const CString& strName, COLORREF color);
	bool SetAreaFbState(const CString& strName, bool bIsFb);
	bool SetAreaRatingPlayerNum(const CString& strName, uint32 nPalayerNum);
	bool SetAreaMaxPlayerNum(const CString& strName, uint32 nPalayerNum);
	bool SetAreaMusic(const CString& strName, const CString& strMusic);

	COLORREF GetAreaColor(const CString& strName);
	uint32 GetAreaRatingPlayerNum(const CString& strName);
	uint32 GetAreaMaxPlayerNum(const CString& strName);
	bool   GetAreaFbState(const CString& strName);
	CString GetAreaMusic(const CString& strName);
	CString GetAreaName(COLORREF color);
	int	GetAreaCount();
	bool IsAreaColorUsed(COLORREF color); 

	map<CString, CString> m_mapDynamicFile;//动态摆怪文件
	void ConvertToUtf8(TCHAR* m_szSave);
private:
	Npc_BC			m_bcNpc;
	Obj_BC			m_bcObj;
	Trap_BC			m_bcTrap;
	PathKeyPos_BC	m_bcPathKeyPos;//与上面的3个容器不同,这里仅仅是记录路线的关键点, 而不是在该类创建的, 不在这里delete 

	NpcIndex_Map	m_mapNpcIndex;//存放原来文件npc索引, 保存时候先按原来顺序保存这里有的 再保存新加的
	ObjIndex_Map	m_mapObjIndex;
	TrapIndex_Map	m_mapTrapIndex;
	
	AreaData_Map	m_mapAreaData;

	CString			m_strDynamicFileName;
	
	bool m_bCheckAI;

	FPos m_DynamicCenterPos;
	FPos m_OffsetPos;


	bool m_bIsOldAreaFile;//为了兼容新老区域文件

	//对与动态摆怪文件,和动态巡逻文件 是将多个场景的 数据存在同一个文件中, 
	//在编辑时候C++只加载了一部分, 保存时候需要把lua部分的数据重新存回去,
	FILE*	m_pFile;
	char	m_szSave[CHAT_BUFFER_SIZE];
};
