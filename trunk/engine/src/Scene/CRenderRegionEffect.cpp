#include "stdafx.h"
#include "CRenderRegionEffect.h"
#include "TSqrAllocator.inl"
#include "IEffectGroup.h"
#include "IEffect.h"
#include "IEffectManager.h"
#include "CDataScene.h"

const DWORD Minute  = 60000;// 60*1000

sqr::CRegionAmbientEffec::CRegionAmbientEffec()
{
	m_pEffectRenderObj = NULL;
}

sqr::CRegionAmbientEffec::~CRegionAmbientEffec()
{
	//if( m_pEffectRenderObj )
	//	m_pEffectRenderObj->ClearLink();

	SafeRelease(m_pEffectRenderObj);
}

bool sqr::CRegionAmbientEffec::CreateAmbientEffect( const CRgnAmbientFxInfo* pAmbFxInfo )
{
	SQR_TRY
	{
		m_uPlayTimeLenMin		= pAmbFxInfo->nPlayMin;
		m_uPlayTimeLenMax		= pAmbFxInfo->nPlayMax;
		m_uPlayIntervalMin		= pAmbFxInfo->nIntervalMin;
		m_uPlayIntervalMax		= pAmbFxInfo->nIntervalMax;
		
		SString effectname	    = pAmbFxInfo->szFxName;
		SString effectfilename  = "fx\\setting\\mapfx_ambient_yb\\" + effectname + ".efx";

		IEffectGroup* pGroup = NULL;
		if (FAILED(IEffectManager::GetInst()->CreateEffectGroup( effectfilename.c_str(), &pGroup )))
		{
			ostringstream strm;
			strm<<"CreateEffectGroup failed with file \""<<effectfilename<<"\"!";
			ArtErr(strm.str());
		}

		IEffect* pEffect			= NULL;
		if( FAILED( IEffectManager::GetInst()->CreateEffect( pGroup, effectname.c_str(), &pEffect ) ) )
		{
			ostringstream strm;
			strm<<"CreateEffect failed with file \""<<effectname<<"\"!";
			ArtErr(strm.str());
			SafeRelease( pGroup );

			return false;
		}

		m_pEffectRenderObj = pEffect;
		SafeRelease( pGroup );
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CreateAmbFx");
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;


	return true;
}

void sqr::CRegionAmbientEffec::Render( const uint32 uCurTime, const CVector3f& efxpos )
{
	SQR_TRY
	{
		if( m_pEffectRenderObj && IsNeedPlay(uCurTime) )
		{
			CMatrix matWorld;
			CVector3f pos = efxpos;
			pos.y = 0.0f;
			matWorld.SetTranslate(pos);

			CColor matColor = CColor( 0xff, 0x80, 0x80, 0x80 );

			sMaterial Mat = 
			{
				CColor4((uint32)matColor ), 
				CColor4((uint32)matColor ), 
				CColor4((uint32)0),
				CColor4((uint32)0xffffffff),
				0
			};

			RenderObjStyle objStyle = 
			{
				ROS_LIGHTMAT1,
				true,
				true,
				DELTARS_DIFFUSE|DELTARS_AMBIENT,
				(uint32)&Mat
			};
			m_pEffectRenderObj->UpdateTime(uCurTime);
			m_pEffectRenderObj->Render( matWorld, &objStyle );
		}
	}
	SQR_CATCH(exp) 
	{
		exp.AppendType("AmbFxRender");
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;
}

bool sqr::CRegionAmbientEffec::IsNeedPlay( const uint32 dwTime )
{
	if( m_uPlayTimeLenMin == 0 || m_uPlayIntervalMin == 0 )
		return true;

	if( m_uPlayTimeLenMin < m_uPlayTimeLenMax && m_uPlayIntervalMin < m_uPlayIntervalMax )
	{
		if( m_bNeedPlayBegin )
		{
			uint32 nRandom = this->GetRandomValue(m_uPlayTimeLenMin,  m_uPlayTimeLenMax);
			m_uCurRandomPlayTime = nRandom * Minute;
			m_uPlayTime = dwTime;
			m_bNeedPlayBegin = false;
		}

		uint32 diff = dwTime - m_uPlayTime;
		if( diff < m_uCurRandomPlayTime )
		{
			return true;
		}
		else
		{
			if( m_bNeedIntervalBegin )
			{
				DWORD nRandom = this->GetRandomValue(m_uPlayIntervalMin, m_uPlayIntervalMax);
				m_uCurRandomInterval = nRandom * Minute;
				m_uIntervalTime = dwTime;
				m_bNeedIntervalBegin = false;
			}

			diff = dwTime - m_uIntervalTime;
			if( diff > m_uCurRandomInterval )
			{
				m_bNeedPlayBegin	  = m_bNeedIntervalBegin = true;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
		return true;

	return false;
}

uint32 sqr::CRegionAmbientEffec::GetRandomValue( uint32 nLowBound, uint32 nHighBound )
{
	if ( nLowBound >= nHighBound )
		return nLowBound;

	uint32 n = nHighBound - nLowBound + 1;

	return nLowBound + rand() % n;
}

sqr::CRegionAmbientEffectMgr::CRegionAmbientEffectMgr()
{

}

sqr::CRegionAmbientEffectMgr::~CRegionAmbientEffectMgr()
{
	this->DestroyRegionAmbientEffect();
}

void sqr::CRegionAmbientEffectMgr::CreateRegionAmbientEffect( const SVector<CRgnAmbientFxInfo*>& rgnAmbFxInfoList )
{
	this->DestroyRegionAmbientEffect();

	CPkgFile::EnableAsyncRead( false );

	size_t size = rgnAmbFxInfoList.size();
	for( size_t i = 0; i < size; ++i )
	{
		CRegionAmbientEffec* pTemp = new CRegionAmbientEffec;
		pTemp->CreateAmbientEffect(rgnAmbFxInfoList[i]);
	
		m_pRegAmbientEffectList.push_back(pTemp);
	}

	CPkgFile::EnableAsyncRead( true );
}

void sqr::CRegionAmbientEffectMgr::DestroyRegionAmbientEffect()
{
	SQR_TRY
	{
		size_t size = m_pRegAmbientEffectList.size();
		for( size_t i = 0; i < size; ++i )
		{
			CRegionAmbientEffec *pTemp = m_pRegAmbientEffectList[i];
			GfkSafeDelete(pTemp);
			pTemp = NULL;
		}
	}
	SQR_CATCH(exp) 
	{
		exp.AppendType("DestroyAmbFx");
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;
	m_pRegAmbientEffectList.clear();
}

void sqr::CRegionAmbientEffectMgr::Render(const uint32 uCurTime, const CVector3f& efxpos)
{
	size_t size = m_pRegAmbientEffectList.size();
	for( size_t i = 0; i < size; ++i )
	{
		m_pRegAmbientEffectList[i]->Render(uCurTime, efxpos);
	}
}
