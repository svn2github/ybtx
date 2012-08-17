#pragma once
#include "CEffectUnitHandler.h"
#include "FightDef.h"
#include "CTick.h"
#include "TFXAllocator.h"

namespace sqr
{
	class CRenderFont;
	class CRenderObject;
}
class CCharacterFollower;


class CFontSprite : public CEffectUnitHandler
{
public:
	static bool		ms_bShowHurt;
	CFontSprite(const string& strText, int32 nFontID, bool bBold, float fZoom);
	~CFontSprite();
	static void DisplayInfo(CRenderObject* pRenderObj,const TCHAR* szEffectFile,const TCHAR* szEffectName,const TCHAR* szFontName,const TCHAR* szDisplay, int32 nFontID, bool bBold, float fScale);
	bool RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS);
private:
	CRenderFont*	m_pFont;
	string			m_strText;
	int32			m_iSize;
	float			m_fZoom;
	bool			m_bFirstTime;
	float			m_height;
};

class CFightInfo;

class CDisplayHurt:public CTick , public CFXMallocObject
{
public:
	static void InitEffectFile();
	static void AddHurtInfo(uint32 uID, const char* szHurtInfo);
	CDisplayHurt(CCharacterFollower* pCharacter);
	void DisplayHurt(int iValue,EAgileType eAgileType,EHurtResult eHurtResult,bool bOverTime, bool bIsSkill, const CFPos& Pos);
	~CDisplayHurt();
	void OnTick();
private:	
	typedef deque<CFightInfo*,TFXAllocator<CFightInfo*> > VecFightInoData;
	typedef map<uint32, string,less<uint32>, TFXAllocator<pair<uint32,string> >  > MapHurtInfo;
	VecFightInoData m_vecFightInfoData;
	static MapHurtInfo ms_mapHurtInfo;
	CCharacterFollower* m_pCharacter;
	uint32 m_uCount;
};
