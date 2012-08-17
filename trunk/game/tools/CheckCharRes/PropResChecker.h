#pragma once
#include <set>
#include "BaseTypes.h"
#include "ieffecthandler.h"

using namespace std;
using namespace sqr;

enum ERes_Type
{
	eRT_Model,
	eRT_Anim,
	eRT_Eff,
	eRT_End
};

enum RES_TYPE
{
	RFT_ARP = MAKEFOURCC( 'A', 'R', 'P', 0 ),		//模型配置文件	
	RFT_ARA = MAKEFOURCC( 'A', 'R', 'A', 0 ),		//动画配置文件	
	RFT_ARE = MAKEFOURCC( 'A', 'R', 'E', 0 ),		//特效文件
};

class PropResChecker;
namespace sqr
{
	class CGraphic;
	class IEffectManager;
}

struct ResCreateInfo
{
	ResCreateInfo()
	{
		pRes = NULL;
		eResType = 0;
	}
	GraphicRes*		pRes;
	uint32					eResType;		//资源类型:ARA,ARP,ARS,ARE,ARAS		(必填)
	string					szClassName;	//资源类名							(必填)
	string					szParam;		//类型参数
};

typedef map<string, bool>			MapChkPart;
typedef map<string, MapChkPart*>	MapChkFile;
typedef map<string, uint32>			MapWeaponAttackCount;
typedef vector<string>				VecWAEff;
typedef map<string, PropResChecker*>	MapPropRes;
typedef set<string>					SetAnimPrefix;

#define WA_EFF_UBOUND	10

class CheckEffectHandler : public IEffectHandler
{
public:
	// 用此函数，可使特效理解地表
	virtual float GetTerrainHeight( float xGround, float yGround ){return 0.0f;};
	/// 格三角形排列是反的还是正的
	virtual bool IsBackGrid(const CVector3f& pos) const {return true;};
	static CheckEffectHandler StaticHandle;
};

class PropResChecker //public IEffectHandler
{
public:
	static void CheckPropResLine(uint32 iLine, const TCHAR* sName, const TCHAR* sModelFile,
		const TCHAR* sAniFile, const TCHAR* EffectFile, const TCHAR* sPieceName,
		const TCHAR* sRenderStyle, const TCHAR* sHidePiece, bool bDynamic);
	static void CheckWeaponLine(uint32 iLine, const TCHAR* sName, const TCHAR* sResID,
		const TCHAR* sAttackEffect, const TCHAR* sAniFile);
	static void CheckSkillAllAnim(const string& sName, const string& sPieceName,
		const string& sTitle);
	static void CheckSkillAllEff(const string& sName, const string& sEff, bool bDynamic,
		const string& sTitle);
	static void CheckSkillAllModel(const string& sName, const string& sModel,
		const string& sTitle);
	static void BeginCheck();
	static void EndCheck();
	static MapPropRes&	GetPropResMap() {return m_mapPropRes;}
	static SetAnimPrefix&		GetAnimPrefixSet() {return m_setAnimPrefix;}

private:
	static void CheckExistModelFiles(const string& sName, const string& sModelFile, bool bDynamic,
		string& sModelFilename, const TCHAR* sPieceName);
	static void CheckExistAnimFiles(const string& sName, const string& sAnimFile, bool bDynamic,
		bool bIsWeapon, const TCHAR* sPieceName = "");
	static void CheckWAExistEffFiles(const string& sName, const string& sEffFile);
	static void CheckSkillExistEffFiles(const string& sName, const string& sEff,
		bool bDynamic, const TCHAR* sPieceName);
	static void CheckExistLuminiEffectFiles(const string& sName, const string& sModelFilename,
		const string& sWeaponEffect, const string& RenderStyle, const TCHAR* sPieceName);
	//static void CheckExistFile(const TCHAR* szFileName, RES_TYPE eResType, const TCHAR* sPieceName);
	static void UniqueCheckExist(const string& sName, const string& sWholePath,
		const TCHAR* sPieceName, ERes_Type eResType);

	static bool CheckEmpty(const string& sItem, const string& sName);
	static void CheckInPieceMap(const string& sPieceName);
	static void SetLineNo(uint32 iLine) {m_iLine = iLine;}
	static bool Init();
	static uint32 GetWAAnimCount(const string& sName);
	static uint32 GetWAEffCount(const string& sName);
	static bool CreateWAEffVector(TCHAR* sWAEff[], const size_t iLength);
	static void SplitString(const string& sContent, TCHAR cSplitter, string& sPart1, string& sPart2);

	static pair<bool, bool> CheckExistPiece(const TCHAR* szFileName, RES_TYPE eResType, const TCHAR* sPieceName);

	static uint32		m_iLine;
	static set<string>	m_setPiece;
	static bool			__init;
	static RES_TYPE		m_arrResType[eRT_End];
	static map<RES_TYPE, ERes_Type>	m_mapResIndex;
	static string		m_sResTypeName[eRT_End];
	static CGraphic*			m_pGraphic;
	static IEffectManager*			m_pEffectMgr;

	static MapChkFile	m_mapCheckedFile[eRT_End];
	static MapChkPart	m_mapCheckedPart[eRT_End];
	static MapWeaponAttackCount		m_mapWAAnimCount;
	static MapWeaponAttackCount		m_mapWAEffCount;
	static VecWAEff		m_arrWAEff[WA_EFF_UBOUND];
	static MapPropRes	m_mapPropRes;
	static SetAnimPrefix			m_setAnimPrefix;
	static std::ofstream*		m_pLogFile;

	string				m_sName;
	string				m_sModelFile;
	string				m_sAniFile;
	string				m_sEffFile;
	string				m_sPieceName;
	string				m_sRenderStyle;
	string				m_sHidePiece;
	bool				m_bDynamic;
};