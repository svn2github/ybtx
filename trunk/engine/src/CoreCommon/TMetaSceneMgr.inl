#pragma once
#include "TMetaSceneMgr.h"

template<typename Traits>
TMetaSceneMgr<Traits>::TMetaSceneMgr(void)
{
	Ast( !IntInst() );
	IntInst() = static_cast<ImpMetaSceneMgr_t*>(this);
}

template<typename Traits>
TMetaSceneMgr<Traits>::~TMetaSceneMgr(void)
{
	IntInst() = NULL;
}

template<typename Traits>
typename TMetaSceneMgr<Traits>::ImpMetaScene_t*
TMetaSceneMgr<Traits>::CreateMetaScene(const char* szSceneName, uint32 SceneVer)
{
	return new ImpMetaScene_t(szSceneName,SceneVer,static_cast<ImpMetaSceneMgr_t*>(this));
}

template<typename Traits>
void TMetaSceneMgr<Traits>::DestroyMetaScene(ImpMetaScene_t* pScene)
{
	if(!pScene)
		return;
	if(pScene->GetMetaSceneMgr() != static_cast<ImpMetaSceneMgr_t*>(this))
		GenErr("MetaScene isn't created by this MetaSceneMgr");
	delete pScene;
}

template<typename Traits>
typename Traits::MetaSceneMgr_t*& TMetaSceneMgr<Traits>::IntInst()
{
	static ImpMetaSceneMgr_t* ls_Inst=NULL;
	return ls_Inst;
}

template<typename Traits>
typename Traits::MetaSceneMgr_t* TMetaSceneMgr<Traits>::Inst()
{
	return IntInst();
}
