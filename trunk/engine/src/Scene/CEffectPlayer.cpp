#include "stdafx.h"
#include "CEffectPlayer.h"
#include "TIDPtrMapIter.h"
#include "TIDPtrMap.inl"
#include "TIDPtrMapIter.inl"
#include "IEffectManager.h"
#include "CEffectAsynFun.h"
#include "TSqrAllocator.inl"

const uint32 CEffectPlayer::PLAY_LOOP	= uint32(-1);
const uint32 CEffectPlayer::PLAY_ONCE	= uint32(0);
const uint32 MAX_EFFECT	= 32;



EffectNode::EffectNode()
{
	pEffect = NULL;
	iTime	= 0;
	iIndex	= 0xffffffff;
	_Uint32	= 0;
}

EffectNode::~EffectNode()
{
	if(pEffect)
	{
		pEffect->OnUnlinked();
		pEffect->Release();
	}
}

FxCreateInfo::FxCreateInfo(IEffectGroup* pGrp, const SString& str)
: pEffectGroup(pGrp),szFxName(str)
{
}

CEffectPlayer::CEffectPlayer(void)
{
}

CEffectPlayer::~CEffectPlayer(void)
{
	ClearAllEfx();
	m_AsynParams.Clear();
}

void CEffectPlayer::AyncFun(void)
{
	SQR_TRY
	{
		FXDelPool::iterator it,eit = m_fxDelPool.end();
		for(it = m_fxDelPool.begin(); it!=eit; ++it)
			delete *it;
	}
	SQR_CATCH(exp)
	{	
		GfkLogExp(exp);
	}
	SQR_TRY_END;

	m_fxDelPool.clear();
	FXAyncPool::iterator it,eit = m_fxPool.end();
	IEffectGroup* pEffectGrp = NULL; 
	IEffectManager* pEffectMgr = IEffectManager::GetInst();
	for( it = m_fxPool.begin();it!=eit; )
	{
		pEffectGrp = it->second.pEffectGroup;
		if( pEffectGrp->IsValid() )
		{
			EffectNode* pEfxNode = NULL;
			if( m_pEfxMap.Get(pEfxNode,it->first) )
			{
				SafeRelease(pEfxNode->pEffect);
				if( FAILED(pEffectMgr->CreateEffect( pEffectGrp, it->second.szFxName.c_str(), &pEfxNode->pEffect ) )|| !_LoadedEffect(pEfxNode) )
				{
					m_pEfxMap.Del(--pEfxNode->iIndex);
					delete pEfxNode;	
				}
			}
			SafeRelease(it->second.pEffectGroup);
			it = m_fxPool.erase(it);
		}
		else
			++it;
	}
	m_AsynParams.Do();
}

uint32 CEffectPlayer::AddEffectPtr( IEffect* pEfx , uint32 nPlayTime )
{
	if( IsFull() )
		return 0;

	EffectNode* pEfxNode = new EffectNode;
	pEfxNode->iTime  = nPlayTime;
	pEfxNode->iIndex = m_pEfxMap.Add(pEfxNode);
	pEfxNode->pEffect = pEfx; 
	if( !_RegisterEffect(pEfxNode) || !_LoadedEffect(pEfxNode) )
	{
		m_pEfxMap.Del(pEfxNode->iIndex);
		delete pEfxNode;
		return 0;
	}
	pEfx->AddRef();
	return ++pEfxNode->iIndex;
}

bool CEffectPlayer::IsFull(void)
{
	if(m_pEfxMap.Size() < MAX_EFFECT)
		return false;

	if(m_fxPool.size() == 0)
		return true;

	int Flag = 0;
	SQR_TRY
	{	
		AyncFun();

		if(m_pEfxMap.Size() < MAX_EFFECT)
			return false;

		if(m_fxPool.size() == 0)
			return true;

		Flag = 1;
		FXAyncPool::iterator it =  m_fxPool.begin();
		EffectNode* pEfxNode = NULL;
		if( m_pEfxMap.Get(pEfxNode,it->first) )
		{
			
			SafeRelease(pEfxNode->pEffect);
			Flag = 2;
			SafeRelease( it->second.pEffectGroup );
			Flag = 3;
			m_pEfxMap.Del(--pEfxNode->iIndex);
			Flag = 4;
			delete pEfxNode;	
			Flag = 5;
			it = m_fxPool.erase(it);
			return false;
		}
	}
	SQR_CATCH(exp)
	{
		switch(Flag)
		{
		case 0:
			exp.AppendMsg("AyncFun");
			break;
		case 1:
			exp.AppendMsg("ReleaseEffect");
			break;
		case 2:
			exp.AppendMsg("ReleaseGroup");
			break;
		case 3:
			exp.AppendMsg("DeleteIdx");
			break;
		case 4:
			exp.AppendMsg("DeleteNode");
			break;
		case 5:
			exp.AppendMsg("EraseIt");
			break;
		default:
			break;
		}
		GfkLogExp(exp);
	}
	SQR_TRY_END;
	
	return true;
}

//增加当前被动作同步的渲染特效
// nPlayerTime
// -1： 一直循环直到外部中止
// 0： 播放一次
// other： 播放时间
uint32	CEffectPlayer::AddEffect( const char* szFxFile, const char* szFx, uint32 nPlayTime, IModelHandler * pHandler)
{
	//SString temp = szFxFile;
	if( szFxFile == NULL || szFx == NULL || szFxFile[0] == 0 || szFx[0] == 0 || IsFull() )// || -1==temp.find("ren") ) // || nPlayTime!=0 )
		return 0;

	IEffectManager* pEffectMgr = IEffectManager::GetInst();
	IEffectGroup* pEffectGrp;

	uint8 i = 1;
	SQR_TRY	
	{
		if( FAILED( pEffectMgr->CreateEffectGroup( szFxFile, &pEffectGrp ) ) )
		{
			return 0;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("AddEffect异常");
		GfkLogExp(exp);
	}
	SQR_TRY_END;

	EffectNode* pEfxNode = new EffectNode;
	pEfxNode->iTime  = nPlayTime;
	pEfxNode->iIndex = m_pEfxMap.Add(pEfxNode);
	if( !_RegisterEffect(pEfxNode) )
	{
		m_pEfxMap.Del(pEfxNode->iIndex);
		delete pEfxNode;
		SafeRelease(pEffectGrp);
		return 0;
	}

	if( pEffectGrp->IsValid() )
	{
		if( FAILED( pEffectMgr->CreateEffect( pEffectGrp, szFx, &pEfxNode->pEffect ) || !_LoadedEffect(pEfxNode) ) )
		{
			m_pEfxMap.Del(pEfxNode->iIndex);
			delete pEfxNode;
			SafeRelease(pEffectGrp);
			return 0;
		}
		SafeRelease(pEffectGrp);
	}
	else
	{
		m_fxPool.insert(FXAyncPool::value_type(pEfxNode->iIndex,FxCreateInfo(pEffectGrp,szFx)));
	}		
	return ++pEfxNode->iIndex;
}

void 	CEffectPlayer::SetEffectUnitHandler( uint32 nIndex, IEffectUnitHandler* pUnitHandler, const char* szUnitName )
{
	EffectNode* pEfxNode = NULL;
	if( m_pEfxMap.Get(pEfxNode,nIndex-1) )
	{
		if(pEfxNode->pEffect)
			pEfxNode->pEffect->SetRenderCallback( pUnitHandler, szUnitName );
		else	
		{
			m_AsynParams.Insert(new SetEffectHandlerName(this,nIndex,pUnitHandler,szUnitName));
		}
	}
}

void	CEffectPlayer::SetEffectUnitHandler( uint32 nIndex, IEffectUnitHandler* pUnitHandler, const char* szUnitName,uint32 eEffectUnitType)
{
	EffectNode* pEfxNode = NULL;
	if( m_pEfxMap.Get(pEfxNode,nIndex-1) )
	{
		if(pEfxNode->pEffect)
			pEfxNode->pEffect->SetRenderCallback( pUnitHandler, (EBaseEffectUnit)eEffectUnitType);
		else	
		{
			m_AsynParams.Insert(new SetEffectHandlerType(this,nIndex,pUnitHandler,szUnitName,eEffectUnitType));
		}
	}
}


bool	CEffectPlayer::DelEffect(uint32 uID)
{
	--uID;
	EffectNode* pEfxNode = NULL;
	FXAyncPool::iterator it = m_fxPool.find(uID);
	if(it!=m_fxPool.end())
	{
		SafeRelease(it->second.pEffectGroup);//->Release();
		m_fxPool.erase(it);
	}

	if( m_pEfxMap.Get(pEfxNode,uID) )
	{
		m_fxDelPool.insert(pEfxNode);
		m_pEfxMap.Del(uID);
		return true;
	}
	return false;
}

void	CEffectPlayer::ClearAllEfx(void)
{
	SQR_TRY
	{
		FXDelPool::iterator it,eit = m_fxDelPool.end();
		for(it = m_fxDelPool.begin(); it!=eit; ++it)
			delete *it;
	}
	SQR_CATCH(exp)
	{	
		GfkLogExp(exp);
	}
	SQR_TRY_END;

	m_fxDelPool.clear();

	TIDPtrMap< EffectNode >::Iterator it,eit = m_pEfxMap.End();
	for( it = m_pEfxMap.Begin(); it!=eit; ++it )
	{
		delete &(*it);
	}

	FXAyncPool::iterator fit,feit = m_fxPool.end();
	for( fit = m_fxPool.begin();fit!=feit; ++fit )
	{
		SafeRelease(fit->second.pEffectGroup);//fit->second.pEffectGroup->Release();
	}

	m_fxPool.clear();
	m_pEfxMap.Clear();
}

void	CEffectPlayer::ClearSyncEfx(void)
{
	TIDPtrMap< EffectNode >::Iterator it,eit = m_pEfxMap.End();
	for( it = m_pEfxMap.Begin(); it!=eit; ++it )
	{
		if( it->pEffect && it->pEffect->IsFrameSynch() )
			m_pEfxMap.Del( --it->iIndex );
	}
}

IEffect* CEffectPlayer::GetEffect(uint32 uID)
{
	EffectNode* pEfxNode = NULL;
	if( m_pEfxMap.Get(pEfxNode,uID-1) )
	{
		return pEfxNode->pEffect;
	}
	return NULL;
}

bool CEffectPlayer::_RegisterEffect( EffectNode* pEfxNode )
{
	return true;
}

bool CEffectPlayer::_LoadedEffect( EffectNode* pEfxNode )
{
	return true;
}
