#include "stdafx.h"
#include "CDisplayHurt.h"
#include "Random.h"
#include "Algorithm.h"
#include "CRenderObject.h"
#include "CGraphic.h"
#include "CRenderGroup.h"
#include "CRenderFont.h"
#include "CCharacterFollower.h"
#include "CAppClient.h"
#include "CFontMgr.h"
#include "CEntityClientManager.h"
#include "ICoreObjectPerformerHandler.h"
#include "CCoreObjectPerformer.h"
#include "CCoreSceneClient.h"
#include "CSceneCommon.h"
#include "CCoreObjectFollower.h"
#include "CoreCommon.h"

bool CFontSprite::ms_bShowHurt = true;

CFontSprite::CFontSprite(const string& strText, int32 nFontID, bool bBold, float fZoom)
:m_strText(strText)
,m_iSize(0)
,m_fZoom(fZoom)
,m_bFirstTime(true)
,m_height(0.0f)
{
	m_pFont = NULL;
	CGraphic::GetInst()->CreateFont(nFontID, bBold, 0.0f, &m_pFont);
}

CFontSprite::~CFontSprite()
{
	SafeRelease( m_pFont );
};

void CFontSprite::DisplayInfo(CRenderObject* pRenderObj,const TCHAR* szEffectFile,const TCHAR* szEffectName,const TCHAR* szFontName,const TCHAR* szDisplay, int32 nFontID, bool bBold, float fScale)
{
	if (NULL==pRenderObj||!ms_bShowHurt)
		return;
	int32 nIndex = pRenderObj->AddEffect( szEffectFile,szEffectName, 0, NULL);

	IEffectUnitHandler* FxHandler = new CFontSprite( szDisplay, nFontID, bBold, fScale);
	pRenderObj->SetEffectUnitHandler( nIndex, FxHandler, szFontName );
	SafeRelease(FxHandler);
}

bool CFontSprite::RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS )
{
	IEffectUnitProp* pProp = pUnit->GetProp();
	if( !m_iSize )
	{
		/*EFFECT_PROP Prop[256];
		pProp->GetProp( Prop, 256 );
		for( int i = 0; i < 256; i++ )
		{
			if( Prop[i].Name() != "最大尺寸" )
				continue;
			m_iSize = limit3( (int32)Prop[i].Data().Float(), 1 );
			break;
		}*/
		float fMaxSize;
		if(pProp->GetMaxSize(fMaxSize))
			m_iSize = limit3( (int32)fMaxSize, 1 );
	}

	if( !m_pFont /*|| bAlphaBlend */|| !pUnit->IsInRenderTime() )//uCurTime - pUnit->GetStartTime() > (uint32)pProp->GetTimeRang() )
		return false;

	const EMap<float,CVector3f>&	mapOffSet = pProp->GetOffset();
	const EMap<float,CColor>&	mapColor  = pProp->GetColor();
	const EMap<float,BYTE>&		mapScale  = pProp->GetScale();
	float Percent = pUnit->GetUnitPercent();// (float)uCurTime - (float)pUnit->GetStartTime() )/pProp->GetTimeRang();
	CVector3f Offset = CVector3f( 0, 0, 0 );
	CColor Color = 0;
	float fScale = 1.0f;
	if( mapOffSet.size() )
		Offset = GetMapValue( mapOffSet, Percent );
	if( mapColor.size() )
		Color = GetMapValue( mapColor, Percent );
	if( mapScale.size() )
		fScale = GetMapValue( mapScale, Percent )/255.0f;
	if( !pUnit->GetParent() )
	{
		return false;
	}

	CCamera* pCamera = CGraphic::GetInst()->GetActiveRenderTarget()->GetCamera();
	CMatrix matViewAndProject, matView, matBillBoard;
	pCamera->GetBillboardNoneX(matBillBoard);

	Offset.Rotate(matBillBoard);
	if (m_bFirstTime)
	{
		m_height = pUnit->GetParent()->GetAABB().getSize().y;
		m_bFirstTime = false;
	}
	Offset.y += m_height;
	CIRect rt;
	
	//pGraphic->GetCamera( &pCamera );
	CMainWindowTarget::GetInst()->GetRect( rt );
	matViewAndProject = pCamera->getViewProjectMatrix();//GetViewAndProject( matViewAndProject );
	matView = pCamera->getViewMatrix();//GetCamera(  );

	CVector3f xCamera = CVector3f( matView._11, matView._21, matView._31 );
	CVector3f vScreen = SceneToScreen( rt, matWorld.GetAxis(3) + Offset, matViewAndProject );

	CVector3f vSize   = SceneToScreen( rt, matWorld.GetAxis(3) + Offset + xCamera, matViewAndProject );
	float fSize		  = fabs( vSize.x - vScreen.x )*m_iSize*fScale*m_fZoom;
	CVector2f vec2f = m_pFont->CalculateSize((utf8_to_utf16(m_strText)).c_str());
	float fontscale = m_pFont->GetScale(fSize);

	vScreen.x -= vec2f.x/2 * fontscale;
	vScreen.y -= vec2f.y/2 * fontscale;

	bool bLastFog = CMainWindowTarget::GetInst()->GetGroup()->IsFogEnabled();
	CMainWindowTarget::GetInst()->GetGroup()->EnableFog(false);
	RenderTextParam param(fSize);
	param.SetPosition(vScreen.x, vScreen.y, 0.0f);// 要求不被模型挡住, 就画在最前面了(z=0)
	param.SetColor(Color);
	param.SetRect((RECT)rt);
	param.SetText(m_strText);
	m_pFont->DrawText(param,true);

	if( bLastFog )
		CMainWindowTarget::GetInst()->GetGroup()->EnableFog(bLastFog);

	return false;
}

CDisplayHurt::MapHurtInfo CDisplayHurt::ms_mapHurtInfo;

class CDisplayHandler : public ICoreObjectPerformerHandler , public CTick ,public CFXMallocObject
{
public:
	CDisplayHandler(CFPos DesPos,CFPos CurPos,const string& strDisplay,const string& strEffectFile,const string& strEffectName,const string& strFontName,bool bIsSkill,uint32& uAttackIndex,uint32& uSkillIndex)
	{
		CFPos Pos;
		float fSpeed, fPixelDistance;
		m_pObj = CCoreSceneClient::Inst()->CreateObjectPerformer(DesPos,this);
		m_pObj->SetHeight(50);
		CVector2f vec2f(DesPos.x-CurPos.x,DesPos.y-CurPos.y);
		vec2f.Normalize();
		if (bIsSkill)
		{
			uint32 uCount = (uSkillIndex++)%2+1;
			fSpeed = 12+float(uCount)*6;
			fPixelDistance = (1.0f+float(uCount)*1.5f)*eGridSpan;
		}
		else
		{
			uint32 uCount = (uAttackIndex++)%2+1;
			fSpeed = 15+float(uCount)*6;
			fPixelDistance =(2.0f+float(uCount)*1.5f)*eGridSpan;
		}
		Pos.x = vec2f.x * fPixelDistance+ DesPos.x;
		Pos.y = vec2f.y * fPixelDistance+ DesPos.y;

		EMoveToResult eResult = 	m_pObj->MoveTo(Pos,fSpeed,eFPT_Line,eBT_HighBarrier,0);
		if (eResult!=eMTR_Success)
		{
			CAppClient::Inst()->RegisterTick(this, 1000);
		}
		CFontSprite::DisplayInfo(m_pObj->GetRenderObject(),strEffectFile.c_str(),strEffectName.c_str(), strFontName.c_str(),strDisplay.c_str(),CFontMgr::TEXTURE_FONT_ID, false, 2);
	}
private:
	~CDisplayHandler()
	{
		CAppClient::Inst()->UnRegisterTick(this);
	}
	void OnTick()
	{
		m_pObj->SetHandler(NULL);
		CCoreSceneClient::Inst()->DestroyObjectPerformer(m_pObj);
		delete this;
	}
	void OnMoveEnded(EMoveEndedType eMEType) 
	{
		CAppClient::Inst()->RegisterTick(this, 1000);

	}
	void OnDestroy() 
	{
		m_pObj->SetHandler(NULL);
		delete this;
	}
	CCoreObjectPerformer* m_pObj; 
};

class CFightInfo :public CFXMallocObject
{
public:
	CFightInfo(const string& sDisplay,EHurtResult eHurtResult,bool bSkill,const CFPos& Pos)
		:m_strDisplay(sDisplay)
		,m_strFontName("num")
		,m_bSkill(bSkill),
		m_Pos(Pos)
	{
		if (eHR_Strike==eHurtResult)
		{
			if (bSkill)
			{
				m_strEffectName = "crit_injurynum";
			}
			else
			{
				m_strEffectName = 	"crit_normal_injurynum";
			}
		}
		else
		{
			if (bSkill)
			{
				m_strEffectName = "injurynum";
			}
			else
			{
				m_strEffectName = 	"normal_injurynum";
			}
		}
	}

	CFightInfo(const string& sDisplay,bool bSkill, const CFPos& Pos)
		:m_strDisplay(sDisplay)
		,m_strFontName("num")
		,m_bSkill(bSkill)
		,m_Pos(Pos)
	{
		m_strEffectName = "mixstate";
	}

	CFightInfo(const string& sDisplay,EHurtResult eHurtResult)
		:m_strDisplay(sDisplay)
		,m_strFontName("num")
		,m_bSkill(true),
		m_Pos(0,0)
	{
		if (eHR_Strike==eHurtResult)
		{
			m_strEffectName = "crit_reHPnum";
		}
		else
		{
			m_strEffectName = "reHPnum";
		}
	}

	CFightInfo(const string& sDisplay)
		:m_strDisplay(sDisplay)
		,m_strFontName("num")
		,m_bSkill(true),
		m_Pos(0,0)
	{
		m_strEffectName = "reMPnum";
	}

	CFightInfo(CCharacterFollower* pCharacter,const string& sDisplay,uint32& uCount)
		:m_strDisplay(sDisplay)
		,m_strFontName("num")
		,m_bSkill(true),
		m_Pos(0,0)
	{
		if (pCharacter->CastToCharacterDirector())
		{
			m_strEffectName = "dotnum";
		}
		else
		{
			TCHAR cIndex[MAX_PATH];
			_itoa(uCount++%2+1,cIndex,10);
			m_strEffectName = "dotnum"+string(cIndex);
		}
	}

	void Display(CCharacterFollower* pCharacter,uint32& uSkillCount,uint32& uAttakCount)
	 {
		static CFPos Pos(0,0);
		CFPos DesFPos = pCharacter->GetPixelPos();
		CPos DesPos;
		pCharacter->GetGridPos(DesPos);
		CPos CurPos;
		PixelPos2GridPos(CurPos,m_Pos);
		if (pCharacter->CastToCharacterDirector())
		{
			m_strEffectName="playernum/"+m_strEffectName;
		}
		else
		{
			m_strEffectName="aimnum/"+m_strEffectName;
		}
		if (DesPos == CurPos||Pos == m_Pos)
		{
			string strEffectFile="fx/setting/number1.efx";
			CFontSprite::DisplayInfo(pCharacter->GetRenderObject(),strEffectFile.c_str(),m_strEffectName.c_str(), m_strFontName.c_str(),m_strDisplay.c_str(),CFontMgr::TEXTURE_FONT_ID, false, 2);
		}
		else
		{
			string strEffectFile="fx/setting/number2.efx";
			new CDisplayHandler(DesFPos,m_Pos,m_strDisplay,strEffectFile,m_strEffectName,m_strFontName,m_bSkill,uSkillCount,uAttakCount);
		}
	 }
private:
	string m_strDisplay;
	string m_strEffectName;
	string m_strFontName;
	bool m_bSkill;
	CFPos m_Pos;
};

CDisplayHurt::CDisplayHurt(CCharacterFollower* pCharacter)
:m_pCharacter(pCharacter)
,m_uCount(0)
{	
	CAppClient::Inst()->RegisterTick(this,100);
}

CDisplayHurt::~CDisplayHurt()
{
	CAppClient::Inst()->UnRegisterTick(this);
	while (!m_vecFightInfoData.empty())
	{
		CFightInfo* pInfo = *m_vecFightInfoData.begin();
		delete pInfo;
		m_vecFightInfoData.pop_front();
	}
	m_pCharacter->ClearDisplayHurt();
}

void CDisplayHurt::AddHurtInfo(uint32 uID, const char* szHurtInfo)
{
	ms_mapHurtInfo.insert(make_pair(uID, string(szHurtInfo)));
}

void CDisplayHurt::OnTick()
{
	CRenderObject* pRenderObj = m_pCharacter->GetRenderObject();
	if (!pRenderObj||m_vecFightInfoData.empty())
	{
		return;
	}
	uint32 uSkillCount=0; 
	uint32 uAttakCount=0; 
	uint32 uOverTimeCount=0;
	while (!m_vecFightInfoData.empty())
	{
		CFightInfo* pInfo = *m_vecFightInfoData.begin();
		pInfo->Display(m_pCharacter,uSkillCount,uAttakCount);
		delete pInfo;
		m_vecFightInfoData.pop_front();
	}
}

void CDisplayHurt::DisplayHurt(int iValue,EAgileType eAgileType,EHurtResult eHurtResult,bool bOverTime, bool bIsSkill, const CFPos& Pos)
{
	TCHAR cDisplay[MAX_PATH];
	CFightInfo* pInfo = NULL;
	switch (eAgileType)
	{
		case eAT_HP:
		{
			switch (eHurtResult)
			{
			case eHR_Block:
				if (iValue==0)
				{
					pInfo = new CFightInfo(ms_mapHurtInfo[200007],bIsSkill,Pos);
					break;
				}
			case eHR_Hit:
			case eHR_Resist:
				{
					if (iValue==0)
					{
						return;
					}

					if (iValue>0)
					{
						_itoa(iValue,cDisplay,10);
						pInfo = new CFightInfo(cDisplay,eHurtResult);
					}
					else
					{
						_itoa(-iValue,cDisplay,10);
						if (bOverTime)
						{
							pInfo = new CFightInfo(m_pCharacter,cDisplay,m_uCount);
						}
						else
						{
							pInfo = new CFightInfo(cDisplay,eHurtResult,bIsSkill,Pos);
						}		
					}
				}
				break;
			case eHR_Strike:
				{
					if (iValue==0)
					{
						return;
					}
					if (iValue>0)
					{
						_itoa(iValue,cDisplay,10);	
						pInfo = new CFightInfo(cDisplay,eHurtResult);
					}
					else
					{
						_itoa(-iValue,cDisplay,10);	
						if (bOverTime)
						{
							pInfo = new CFightInfo(m_pCharacter,cDisplay,m_uCount);
						}
						else
						{
							pInfo = new CFightInfo(cDisplay,eHurtResult,bIsSkill,Pos);
						}
					}		 
				}
				break;
			case eHR_Miss:
				{
					pInfo = new CFightInfo(ms_mapHurtInfo[200001],bIsSkill,Pos);
				}
				break;
			case eHR_PhyDodge:
			case eHR_MagDodge:
				{
					pInfo = new CFightInfo(ms_mapHurtInfo[200002],bIsSkill,Pos);
				}
				break;
			case eHR_Immune:
				{
					pInfo = new CFightInfo(ms_mapHurtInfo[200003],bIsSkill,Pos);
					
				}
				break;
			case eHR_Parry:
				{
					pInfo = new CFightInfo(ms_mapHurtInfo[200004],bIsSkill,Pos);
				}
				break;
			case eHR_ComResist:
				{
					pInfo = new CFightInfo(ms_mapHurtInfo[200005],bIsSkill,Pos);
				}
				break;
			case eHR_EmptyAni:
				{
					pInfo = new CFightInfo(ms_mapHurtInfo[200006],bIsSkill,Pos);
				}
				break;
			}
		}
		break;
	case eAT_MP:
	case eAT_RP:
		if (iValue!=0)
		{
			_itoa(iValue,cDisplay,10);
			pInfo = new CFightInfo(cDisplay);
		}
		break;
	}
	if (pInfo)
	{
		m_vecFightInfoData.push_back(pInfo);
	}
}
