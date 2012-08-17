#pragma once
#include "stdafx.h"
#include "CDynamicObject.h"

class CCharacter;
class CModelPiece;
class CPlayerModelPart;
using namespace sqr;
namespace sqr
{
	class CRenderObject;
}
enum EModelType
{
	eMT_Character,
	eMT_IntObj
};


struct NPCData
{
	NPCData(const string &name, const string &AItype):sName(name),sAItype(AItype){};
	string sName;
	string sAItype;
};

typedef std::map<string,CCharacter*>	CHARACTER_MAP;
typedef	std::vector<string>				CHARACTERNAME_VEC;
typedef std::map<string,NPCData>		NPCDATA_MAP;
typedef std::vector<CPlayerModelPart*>	ModelPART_VEC;
typedef std::map<string,CPlayerModelPart*> ModelPART_MAP;
class CDataCache
	:public virtual CDynamicObject
{ 
public:
	CDataCache();
	void AddNPC(const TCHAR* name, const TCHAR* AItype);
	string getNpcAi(const string& name);
	void AddCharacter(CCharacter* pCharacter, int type);
	void ShowCharacterName(CTreeCtrl&	treeCtr);
	void ShowIntObjName(CTreeCtrl& treeCtr);
	void DisPlayCharacter(string name, CRenderObject* renderObj,EModelType type);
	void AddAnimateGroupName (CRenderObject* randerObj,CTreeCtrl& treeCtr);
	string GetModelFileName() {return m_curModelFileName;}
	void AddPlayerModelPart(CPlayerModelPart* pModelPart);

	const ModelPART_VEC&	GetPlayerModelPart(void);

	static string GetNpc_ObjRealName(string& strName);
	string GetNpc_ObjNameWithMod(string& strName,EModelType type);
	CPlayerModelPart* GetPlayerModePart(const string& str);
	
private:	
	CHARACTER_MAP			m_mapCharacter;
	CHARACTERNAME_VEC		m_vecCharacterName;
	
	CHARACTER_MAP			m_mapIntObj;
	CHARACTERNAME_VEC		m_vecIntObjName;
	string					m_curModelFileName;
	NPCDATA_MAP				m_mapNpcCommon;

	ModelPART_VEC			m_vecPlayerModelPart;
	ModelPART_MAP			m_mapPlayerModelPart;
};

inline const ModelPART_VEC& CDataCache::GetPlayerModelPart(void)
{
	return m_vecPlayerModelPart;
}