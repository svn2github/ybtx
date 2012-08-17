#include "stdafx.h"
#include "CEditEffectProp.h"
#include "CEditUnitPropHelp.h"
#include "CEditEffectManager.h"
#include "StringHelper.h"

CEditEffectProp::CEditEffectProp( CBaseEffectGroup* pEffectGroup, IEffectManager* pEffectMgr, const EString& szName )
:CEffectProp(pEffectGroup,pEffectMgr,szName)
{
	InitTrackInfo();
}

//CEditEffectProp::CEditEffectProp( CBaseEffectGroup* pEffectGroup,CEditEffectProp* pProp)
//:CEffectProp(pEffectGroup,pProp->m_pEffectMgr,pProp->m_szFullEffectName)
//{
//	for(int i = 0;i<pProp->GetTracksNum();i++)
//	{
//		EMap< int, IEffectUnitProp* > tMapEffectUnitProp;
//		EMap<int,IEffectUnitProp*>::iterator iter = pProp->GetTrack(i).begin();
//		while (iter!=pProp->GetTrack(i).end())
//		{
//			tMapEffectUnitProp[iter->first] = iter->second;
//			tMapEffectUnitProp[iter->first]->AddRef();
//			iter++;
//		}
//		m_Tracks.push_back(tMapEffectUnitProp);
//	}
//	pProp->m_AttachKey = pProp->GetAttachKeys();
//
//	m_TimeRange		 = pProp->GetTimeRange();
//	for(int i = 0;i<pProp->GetAttachAniNum();i++)
//		m_AttachAniName.push_back(pProp->GetAttachAniName(i));
//	m_UserDesc		 = pProp->GetUserDesc();
//	m_bLoadRes		 = pProp->GetLoadRes();
//	m_bFrameSynch    = pProp->GetFrameSynch();       
//	m_bForbidScale	 = pProp->GetForbidScale();
//	InitTrackInfo();
//}

CEditEffectProp::~CEditEffectProp()
{
	int n = m_EditTracks.size();
	for( int i = 0; i < n; ++i )
		DeleteTrack( 0 );
}

int CEditEffectProp::GetTimeRange()	
{
	return m_TimeRange;
}

void	CEditEffectProp::SetTimeRange( int TimeRange )
{
	m_TimeRange = TimeRange;
}

void	CEditEffectProp::SetUserDesc( LPCTSTR str )
{
	m_UserDesc = str;
}

LPCTSTR	CEditEffectProp::GetUserDesc()
{
	return m_UserDesc.c_str();
}

void	CEditEffectProp::SetAttachAniName( LPCTSTR str )
{
	m_AttachAniName.clear();
	string Temp = str;
	TCHAR* pStr = (TCHAR*)Temp.c_str();
	TCHAR* pPre = pStr;
	while( true )
	{
		TCHAR c = *pStr;
		if( c == ',' || c == 0 )
		{
			*pStr = 0;
			m_AttachAniName.push_back( pPre );
			pPre = pStr + 1;
			if( !c )
				break;
		}
		pStr++;
	}
}

LPCTSTR	CEditEffectProp::GetAttachFullAniName()
{
	static EString str;
	str = "";
	for(EVector<EString>::iterator it = m_AttachAniName.begin(); it != m_AttachAniName.end(); it++ )
	{
		if( it != m_AttachAniName.begin() )
			str += ",";
		str += *it;
	}
	return str.c_str();
}

void CEditEffectProp::SetFrameSynch(bool bSyn)
{
	m_bFrameSynch = bSyn;
}


void CEditEffectProp::SetForbidScale(bool bForbid)
{
	m_bForbidScale = bForbid;
}

void CEditEffectProp::UpdateSelfProp(vector< map< int, EDITEFFECTKEY > >& vecRef)
{
	//ClearTrackInfo();
	//for (int i=0;i<(int)vecRef.size();++i)
	//{
	//	m_Tracks.push_back(EMap< int, IEffectUnitProp* >());
	//	for (CONST_KEY_MAP::iterator iter = vecRef[i].begin();iter!=vecRef[i].end();++iter)
	//	{
	//		IEffectUnitProp* pProp = iter->second.pEffectUnit ? iter->second.pEffectUnit->GetUnitProp() : NULL;
	//		m_Tracks[i][iter->first] = pProp;
	//		pProp->AddRef();
	//		m_AttachKey[pProp].nTrack = iter->second.nParentTrack;
	//		m_AttachKey[pProp].nKeyStartTime = iter->second.nParentStartTime;
	//	}
	//}
}

void CEditEffectProp::AddPropTrack()
{
	m_Tracks.push_back(EMap< int, IEffectUnitProp* >());
}

void CEditEffectProp::AddEffectTrack()
{
	CEditEffect* pEditEffect = NULL;
	BEGIN_FOREACH_EFFECT(pEditEffect);
	pEditEffect->m_Tracks.push_back(CEditEffect::sTrack());
	END_FOREACH_EFFECT();
}

void CEditEffectProp::SetPropKey(int i,int Pos,EDITEFFECTKEY* pEditEffectKey)
{
	IEffectUnitProp* pProp			= pEditEffectKey->GetEffectUnitProp()?pEditEffectKey->GetEffectUnitProp()->GetUnitProp():NULL;
	m_Tracks[i][Pos] = pProp;
	pProp->AddRef();
	int nTrack = pEditEffectKey->GetParentTrack();
	m_AttachKey[pProp].nTrack = nTrack;
	m_AttachKey[pProp].nKeyStartTime = pEditEffectKey->GetParentStartTime();
}

void CEditEffectProp::DelPropKey(int Ln,int Pos)
{
	EMap< int, IEffectUnitProp* >::iterator iter = m_Tracks[Ln].find(Pos);
	if(iter!=m_Tracks[Ln].end())
	{
		EMap< IEffectUnitProp*, AttachKey >::iterator it = m_AttachKey.find(iter->second);
		if(it!=m_AttachKey.end())
			m_AttachKey.erase(it);
		SafeRelease(iter->second);
		m_Tracks[Ln].erase(iter);
	}
}

void CEditEffectProp::DelAttachKey(IEffectUnitProp* pProp)
{
	EMap< IEffectUnitProp*, AttachKey >::iterator it = m_AttachKey.find(pProp);
	if(it!=m_AttachKey.end())
		m_AttachKey.erase(it);
}

void	CEditEffectProp::ClearTrackInfo()
{
	m_AttachKey.clear();
	for (int i=0;i<(int)m_Tracks.size();++i)
	{
		for(EMap< int, IEffectUnitProp* >::iterator iter=m_Tracks[i].begin();iter!=m_Tracks[i].end();++iter)
		{
			SafeRelease(iter->second);
		}
	}
	m_Tracks.clear();
}

void CEditEffectProp::ReplaceTwoTrack(int one,int another)
{
	EMap< int, IEffectUnitProp* >& oneRef = m_Tracks[one];
	EMap< int, IEffectUnitProp* >& anotherRef = m_Tracks[another];
	EMap< int, IEffectUnitProp* > temp = oneRef;
	oneRef = anotherRef;
	anotherRef = temp;
	int trackNum = m_Tracks.size();
	for (int i = 0;i<trackNum;++i)
	{
		for (EMap< int, IEffectUnitProp* >::iterator iter = m_Tracks[i].begin();iter != m_Tracks[i].end();++iter)
		{
			if(m_AttachKey[iter->second].nTrack == one)
			{
				m_AttachKey[iter->second].nTrack = another;
			}
			else if(m_AttachKey[iter->second].nTrack == another)
			{
				m_AttachKey[iter->second].nTrack = one;
			}
		}
	}
}

void CEditEffectProp::ClearEditEffects()
{
	EffectPool::iterator it,eit = m_EffectSet.end();
	for( it = m_EffectSet.begin() ;it != eit;++it )
		SafeRelease(*it);
	m_EffectSet.clear();
}

void CEditEffectProp::AddEditEffect(CEditEffect* pEditEffect)
{
	m_EffectSet.insert(pEditEffect);
}

void CEditEffectProp::DelEditEffect(CEditEffect* pEditEffect)
{
	m_EffectSet.erase(pEditEffect);
}

CEditEffectGroup*	CEditEffectProp::GetEffectGroup(void)
{
	return static_cast<CEditEffectGroup*>(m_pEffectGroup);
}

CEditEffect*		CEditEffectProp::CreateNewEffect(void)
{
	return new CEditEffect(CEditEffectManager::GetEditInst(), this);
}

int CEditEffectProp::SaveBin( CEditEffectWriteFile& File, const string& strEffectName )
{
	int TrackSize = m_EditTracks.size();
	File.write( (TCHAR*)&TrackSize, sizeof(int) );
	if( TrackSize == 0 )
	{
		string msg = "";
		format(msg, "特效包:%s 下的轨道为空，所以保存失败，请对其进行处理!", strEffectName.c_str());
		MessageBox( NULL, msg.c_str(), "错误提示", 0 );
		return GERROR;
	}

	for( int i = 0; i < TrackSize; ++i )
	{
		CEditTrack& CurTrack = GetEditTrack( i );
		int KeySize = CurTrack.GetKeySize();
		File.write( (TCHAR*)&KeySize, sizeof(int) );
		if( KeySize == 0 )
		{
			string msg = "";
			int    id = i + 1;
			format(msg, "特效包:%s 下的第%d个轨道的特效不属于任何现有特效类型，所以保存失败，请对其进行处理!", strEffectName.c_str(), id);
		}

		if(CurTrack.Save(File)==GERROR)
			return GERROR;
	}
	CEditEffectProp* pEditEProp = this;
	int timeRange = pEditEProp->GetTimeRange();
	File.write( (TCHAR*)&timeRange, sizeof(int) );

	int Num = pEditEProp->GetAttachAniNum();
	File.write( (TCHAR*)&Num, sizeof(int) );
	for (int i=0;i<Num;++i)
	{
		string strAniName = GetAttachAniName(i);
		int Len = strAniName.size();
		File.write( (TCHAR*)&Len, sizeof(int) );
		File.write( (TCHAR*)strAniName.c_str(), Len );
	}

	SaveStringT( File, GetUserDesc() );
	//保存特效帧同步信息
	bool bFrameSynch = GetFrameSynch();
	File.write((TCHAR*)&bFrameSynch,sizeof(bool));

	bool bForbidScale = GetForbidScale();
	File.write((TCHAR*)&bForbidScale,sizeof(bool));
	return 0;
}


void CEditEffectProp::AddTrack()						
{ 
	m_EditTracks.push_back( CEditTrack(this) ); 
	AddEffectTrack();
	AddPropTrack();
}

void CEditEffectProp::SetEffectTrack(int Ln,int Pos)
{
	CEditEffect* pEditEffect;
	BEGIN_FOREACH_EFFECT(pEditEffect);
	pEditEffect->GetEffectKey(Ln,Pos).pEffectUnit = ((CEffectManager*)m_pEffectMgr)->CreateEffectUnit( GetTrack(Ln)[Pos] );
	AttachKey Key = GetAttachKey( pEditEffect->GetEffectKey(Ln,Pos).pEffectUnit->GetProp() );
	pEditEffect->GetEffectKey(Ln,Pos).pParentEffectUnit = Key.nTrack < 0 ? NULL : pEditEffect->GetEffectKey(Key.nTrack,Key.nKeyStartTime).pEffectUnit;
	pEditEffect->OnEffectUintLinked(&(pEditEffect->GetEffectKey(Ln,Pos)));
	END_FOREACH_EFFECT();

	//EffectPool::iterator iter = m_EffectSet.begin();
	//while (iter!=m_EffectSet.end())
	//{
	//	CEditEffect* pEditEffect = *iter;
	//	pEditEffect->m_Tracks[Ln][Pos].pEffectUnit = ((CEffectManager*)m_pEffectMgr)->CreateEffectUnit( GetTrack(Ln)[Pos] );

	//	AttachKey Key = GetAttachKey( pEditEffect->m_Tracks[Ln][Pos].pEffectUnit->GetProp() );
	//	pEditEffect->m_Tracks[Ln][Pos].pParentEffectUnit = Key.nTrack < 0 ? NULL : pEditEffect->m_Tracks[Key.nTrack][Key.nKeyStartTime].pEffectUnit;

	//	iter++;
	//}
} 

void	CEditEffectProp::ReplaceTrackPos(int first,int second)
{
	CEditEffect* pEditEffect;
	BEGIN_FOREACH_EFFECT(pEditEffect);
	EMap< int, CEffect::EFFECTKEY >& firstTrackRef = pEditEffect->GetEffectKeys(first);
	EMap< int, CEffect::EFFECTKEY >& SecondTrackRef = pEditEffect->GetEffectKeys(second);
	EMap< int, CEffect::EFFECTKEY > temp = firstTrackRef;
	firstTrackRef = SecondTrackRef;
	SecondTrackRef = temp;
	END_FOREACH_EFFECT();

	ReplaceTwoTrack(first,second);
}

void CEditEffectProp::LinkParent(int Ln,int Pos)
{
	CEditEffect* pEditEffect;
	BEGIN_FOREACH_EFFECT(pEditEffect);
	if(pEditEffect->GetEffectKey(Ln,Pos).pEffectUnit)
	{
		IEffectUnit* pEffectUnit = pEditEffect->GetEffectKey(Ln,Pos).pEffectUnit;
		IEffectUnitProp* pProp = pEffectUnit->GetProp();
		AttachKey Key = GetAttachKey( pProp );
		pEditEffect->GetEffectKey(Ln,Pos).pParentEffectUnit = Key.nTrack < 0 ? NULL : pEditEffect->GetEffectKey(Key.nTrack,Key.nKeyStartTime).pEffectUnit;
	}
	END_FOREACH_EFFECT();
}

void CEditEffectProp::DelEffectTrack(int Ln,int Pos)
{
	CEditEffect* pEditEffect;
	BEGIN_FOREACH_EFFECT(pEditEffect);
	EMap<int,CEffect::EFFECTKEY>::iterator iter = pEditEffect->GetEffectKeys(Ln).find(Pos);
	if(iter!=pEditEffect->GetEffectKeys(Ln).end())
	{
		SafeRelease(iter->second.pEffectUnit);
		pEditEffect->GetEffectKeys(Ln).erase(iter);
	}
	END_FOREACH_EFFECT();
}

void CEditEffectProp::DeleteTrack( int Num )	
{ 
	int nTrackNum = GetTrackNum();

	for( int i = 0; i < nTrackNum; i++ )
	{
		CEditTrack& TestTrack = GetEditTrack( i );
		TestTrack.BreakLinkOfParentTrack(i,Num);
	}

	CEditTrack& tTrack = m_EditTracks[Num];
	tTrack.SubRefParentTrack(nTrackNum);

	if( Num < (int)m_EditTracks.size() )
	{
		m_EditTracks.erase( m_EditTracks.begin() + Num ); 

		CEditEffect* pEditEffect;
		BEGIN_FOREACH_EFFECT(pEditEffect);
		EMap< int, CEffect::EFFECTKEY >::iterator iter = pEditEffect->GetEffectKeys(Num).begin();
		while (iter!=pEditEffect->GetEffectKeys(Num).end())
		{
			SafeRelease(iter->second.pEffectUnit);
			iter++;
		}
		pEditEffect->DeleteEffectTrack(Num);
		END_FOREACH_EFFECT();

		EVector<EMap< int, IEffectUnitProp* >>::iterator pit = m_Tracks.begin() + Num;

		EMap< int, IEffectUnitProp* >::iterator ppit,ppeit = (*pit).end();
		for( ppit = (*pit).begin(); ppit!=ppeit; ++ppit )
		{
			DelAttachKey(ppit->second);
			SafeRelease(ppit->second);
		}
		m_Tracks.erase(pit);
	}
}

void CEditEffectProp::ShowTrack( int Num )	
{ 
	if(Num<(int)m_EditTracks.size())
	{
		CEditTrack& tTrack = m_EditTracks[Num];
		tTrack.Show();
		EffectShow(Num);
	}
}

void CEditEffectProp::EffectShow(int Num)
{
	CEditEffect* pEditEffect;
	BEGIN_FOREACH_EFFECT(pEditEffect);

	EMap< int, CEffect::EFFECTKEY >& tShowTrack = pEditEffect->GetEffectKeys(Num);
	CEditTrack& tTrack = m_EditTracks[Num];
	map<int,EDITEFFECTKEY>& tEditTrack = tTrack.GetEditKeys();
	map<int,EDITEFFECTKEY>::iterator it = tEditTrack.begin();
	while (it!=tEditTrack.end())
	{
		int pos = it->first;
		bool bVisible = it->second.GetEffectUnitProp()->GetUnitVisible();
		tShowTrack[pos].pEffectUnit->SetVisible(bVisible);
		it++;
	}
	
	END_FOREACH_EFFECT();
}

void CEditEffectProp::UnShowSelected()
{
	for(int i=0;i<(int)m_EditTracks.size();i++)
	{
		CEditTrack& tTrack = m_EditTracks[i];
		tTrack.UnShow();
	}
}

/*void	CEditEffectProp::ClearEffectTrack()
{
	for (int i=0;i<(int)m_Tracks.size();++i)
	{
		for(EMap< int, EFFECTKEY >::iterator iter = m_Tracks[i].begin();iter!=m_Tracks[i].end();++iter)
		{
			SafeRelease(iter->second.pEffectUnit);
		}
	}
	m_Tracks.clear();
}*/


void CEditEffectProp::InitTrackInfo()
{
	EVector< EMap< int, IEffectUnitProp* > >	& trackRef = m_Tracks;
	EMap< IEffectUnitProp*, AttachKey >	& attachRef = m_AttachKey;
	m_EditTracks.clear();
	for (int i=0;i<(int)trackRef.size();++i)
	{
		CEditTrack rTrack(this);
		map<int,EDITEFFECTKEY>& CurTrackKeys = rTrack.GetEditKeys();
		for (EMap< int, IEffectUnitProp* >::iterator iter = trackRef[i].begin();iter!=trackRef[i].end();++iter)
		{
			CEditUnitPropHelp* pPropHelp = CEditEffectManager::EffectUnitPropCast_ToPropHelp(iter->second);
			iter->second->AddRef();
			CurTrackKeys[iter->first].SetPos(iter->first);
			CurTrackKeys[iter->first].SetEffectUnitProp(pPropHelp,this);
			CurTrackKeys[iter->first].SetParentTrack(attachRef[iter->second].nTrack);
			CurTrackKeys[iter->first].SetParentStartTime(i,iter->first,attachRef[iter->second].nKeyStartTime);
		}
		m_EditTracks.push_back(rTrack);
	}

	for( int i = 0; i < (int)m_EditTracks.size(); i++ )
	{
		map< int, EDITEFFECTKEY >& CurTrack = GetEditTrack( i ).GetEditKeys();
		for( map< int, EDITEFFECTKEY >::iterator it = CurTrack.begin(); it != CurTrack.end(); it++ )
		{
			if ( it->second.GetParentTrack() >= 0 )
			{
				CEditTrack& rTrack = m_EditTracks[it->second.GetParentTrack()];
				map< int, EDITEFFECTKEY >& KeyEdits = rTrack.GetEditKeys();
				KeyEdits[it->second.GetParentStartTime()].AddRef();
			}
		}
	}
}

void CEditEffectProp::SetEffectUnitsProp(int TrackID,int Pos)
{
	CEditEffect* pEditEffect;
	BEGIN_FOREACH_EFFECT(pEditEffect);
	pEditEffect->GetEffectKey(TrackID,Pos).pEffectUnit->SetProp(m_EditTracks[TrackID].GetEditKey(Pos).GetEffectUnitProp()->GetUnitProp());
	pEditEffect->UpdateLink();
	END_FOREACH_EFFECT();
}

void	CEditEffectProp::CopyTrackBindInfo(int source,int dest)
{
	CEditEffect* pEditEffect;
	BEGIN_FOREACH_EFFECT(pEditEffect);
	pEditEffect->CopyTrackBindInfo(source,dest);
	END_FOREACH_EFFECT();
}
