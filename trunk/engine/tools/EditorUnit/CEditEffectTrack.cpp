#include "stdafx.h"
#include "CEditEffectTrack.h"
#include "CEditEffectProp.h"
#include "CEditUnitPropHelp.h"


CEditEffectProp*	EDITEFFECTKEY::GetEditEffectProp(void)
{
	//return m_pEditTrack->GetEditEffectProp();
	return m_pEditEffectProp;
}

void CEditTrack::BreakLinkOfParentTrack(int nTrack,int Num)
{
	for( map< int, EDITEFFECTKEY >::iterator it = m_EditKeys.begin(); it != m_EditKeys.end(); it++ )
	{
		if( it->second.GetParentStartTime() >= 0 )
		{
			if( it->second.GetParentTrack() == Num )
			{
				it->second.SetParentTrack(-1);
				it->second.SetParentStartTime(nTrack,it->first,-1);
			}
			else if( it->second.GetParentTrack() > Num )
				it->second.SetParentTrack(it->second.GetParentTrack() - 1);
		}
	}
}

void CEditTrack::SubRefParentTrack(int nTrackNum)
{
	for( map<int ,EDITEFFECTKEY>::iterator it = m_EditKeys.begin(); it != m_EditKeys.end(); it++ )
	{
		if( it->second.GetParentStartTime() >= 0 && it->second.GetParentTrack() < nTrackNum )
		{
			CEditTrack& tParentTrack = m_pEditProp->GetEditTrack(it->second.GetParentTrack());
			tParentTrack.SubRefPos(it->second.GetParentStartTime());

			//GetTrack(it->second.GetParentTrack())[it->second.GetParentStartTime()].Release();
		}
	}
}

void CEditTrack::SubRefPos(int Pos)
{
	m_EditKeys[Pos].Release();
}

void CEditTrack::Show()
{
	for( map< int, EDITEFFECTKEY >::iterator it = m_EditKeys.begin(); it != m_EditKeys.end(); it++ )
	{
		EDITEFFECTKEY &CurrEffectKey	= it->second;
		bool	  bVisible			= CurrEffectKey.GetEffectUnitProp()->GetUnitVisible();
		CurrEffectKey.GetEffectUnitProp()->SetUnitVisible( !bVisible );
	}
}

void CEditTrack::UnShow()
{
	for( map< int, EDITEFFECTKEY >::iterator it = m_EditKeys.begin(); it != m_EditKeys.end(); it++ )
		it->second.SetSelected(FALSE);
}

int  CEditTrack::GetKeySize()
{
	return m_EditKeys.size();
}

int CEditTrack::Save(CEditEffectWriteFile& File)
{
	for( map< int, EDITEFFECTKEY >::iterator it = m_EditKeys.begin(); it != m_EditKeys.end(); it++ )
	{
		CEditUnitPropHelp* pEffectUnit = it->second.GetEffectUnitProp();
		string Desc = pEffectUnit->GetUnitProp()->GetClassDescription();
		int Len = Desc.size();
		const GUID ClassID = pEffectUnit->GetUnitProp()->GetGUID();
		UINT ID = pEffectUnit->GetUnitProp()->GetID();
		int Pos = max(it->first,0);
		int nParentTrack = it->second.GetParentTrack();
		int nParentStartTime = it->second.GetParentStartTime();
		File.write( (TCHAR*)&ClassID, sizeof(GUID) );
		File.write( (TCHAR*)&Len, sizeof(int) );
		File.write( (TCHAR*)Desc.c_str(), Len );
		File.write( (TCHAR*)&ID, sizeof(UINT) );
		File.write( (TCHAR*)&Pos, sizeof(int) );
		File.write( (TCHAR*)&nParentTrack, sizeof(int) );
		File.write( (TCHAR*)&nParentStartTime, sizeof(int) );

		if( FAILED( ( pEffectUnit->GetUnitProp() )->SaveBin( File ) ) )
			return GERROR;
	}
	return 0;
}

map<int,EDITEFFECTKEY>& CEditTrack::GetEditKeys()
{
	return m_EditKeys;
}

EDITEFFECTKEY& CEditTrack::GetEditKey(int Pos)
{
	return m_EditKeys[Pos];
}

EDITEFFECTKEY* CEditTrack::GetEditKeyPtr(int Pos)
{
	return &(m_EditKeys[Pos]);
}

void CEditTrack::SetPos()
{
	for( map< int, EDITEFFECTKEY >::iterator it = m_EditKeys.begin(); it != m_EditKeys.end(); )
	{
		it->second.SetPos(it->first);
		it++;
	}
}

void CEditTrack::TrackUp(int& m_CurSelTrack)
{
	map< int, EDITEFFECTKEY >& Replace = m_pEditProp->GetEditTrack( m_CurSelTrack - 1 ).GetEditKeys();
	map< int, EDITEFFECTKEY > Temp = Replace;
	Replace = m_EditKeys;
	m_EditKeys = Temp;
	for( int i = 0; i < m_pEditProp->GetTrackNum(); i++ )
	{
		map< int, EDITEFFECTKEY >& TestTrack = m_pEditProp->GetEditTrack( i ).GetEditKeys();
		for( map< int, EDITEFFECTKEY >::iterator it = TestTrack.begin(); it != TestTrack.end(); it++ )
		{
			if( it->second.GetParentStartTime() >= 0 )
			{
				if( it->second.GetParentTrack() == m_CurSelTrack - 1 )
					it->second.SetParentTrack(m_CurSelTrack);
				//it->second.nParentTrack = m_CurSelTrack;
				else if( it->second.GetParentTrack() == m_CurSelTrack )
					it->second.SetParentTrack(m_CurSelTrack - 1);
				//it->second.nParentTrack = m_CurSelTrack - 1;
			}
		}
	}
	m_pEditProp->ReplaceTrackPos(m_CurSelTrack - 1,m_CurSelTrack);
}

void CEditTrack::TrackDown(int& m_CurSelTrack)
{
	map< int, EDITEFFECTKEY >& Replace = m_pEditProp->GetEditTrack( m_CurSelTrack + 1 ).GetEditKeys();
	map< int, EDITEFFECTKEY > Temp = Replace;
	Replace = m_EditKeys;
	m_EditKeys = Temp;
	for( int i = 0; i < m_pEditProp->GetTrackNum(); i++ )
	{
		map< int, EDITEFFECTKEY >& TestTrack = m_pEditProp->GetEditTrack( i ).GetEditKeys();
		for( map< int, EDITEFFECTKEY >::iterator it = TestTrack.begin(); it != TestTrack.end(); it++ )
		{
			if( it->second.GetParentStartTime() >= 0 )
			{
				if( it->second.GetParentTrack() == m_CurSelTrack + 1 )
					it->second.SetParentTrack(m_CurSelTrack);
				//it->second.nParentTrack = m_CurSelTrack;
				else if( it->second.GetParentTrack() == m_CurSelTrack )
					it->second.SetParentTrack(m_CurSelTrack + 1);
				//it->second.nParentTrack = m_CurSelTrack + 1;
			}
		}
	}
	m_pEditProp->ReplaceTrackPos(m_CurSelTrack,m_CurSelTrack+1);
}
void CEditTrack::BreakLink(int Ln,int& m_CurSelTime)
{
	if(m_EditKeys.empty())
		return;

	map< int, EDITEFFECTKEY >::iterator it = m_EditKeys.upper_bound( m_CurSelTime );
	if(it!=m_EditKeys.begin())
		it--;

	if( it->first < m_CurSelTime && 
		it->first + (int)it->second.GetEffectUnitProp()->GetUnitProp()->GetTimeRang() > m_CurSelTime )
	{
		if( it->second.GetRef() )
		{
			for( int i = 0; i < m_pEditProp->GetTrackNum(); i++ )
			{
				map< int, EDITEFFECTKEY >& TestTrack = m_pEditProp->GetEditTrack( i ).GetEditKeys();
				for( map< int, EDITEFFECTKEY >::iterator itChild = TestTrack.begin(); itChild != TestTrack.end(); itChild++ )
				{
					if( itChild->second.GetParentStartTime() == it->first )
					{
						itChild->second.SetParentTrack(-1);
						itChild->second.SetParentStartTime(i,itChild->first,-1);
					}
				}
			}
		}
		if( it->second.GetParentStartTime() >= 0 )
			m_pEditProp->GetEditTrack( it->second.GetParentTrack() ).SubRefPos(it->second.GetParentStartTime())/*[it->second.GetParentStartTime()].Release()*/;
		DeleteKey(Ln,it->first);
		//m_EditKeys.erase( it );
	}
}

void CEditTrack::DeleteKeyLink(int& m_CurSelTrack,int& m_CurSelTime)
{
	map< int, EDITEFFECTKEY >::iterator it = m_EditKeys.upper_bound( m_CurSelTime );
	it--;

	if( it->first < m_CurSelTime && 
		it->first + (int)it->second.GetEffectUnitProp()->GetUnitProp()->GetTimeRang() > m_CurSelTime && 
		it->second.GetParentStartTime() >= 0 )
	{
		m_pEditProp->GetEditTrack( it->second.GetParentTrack() ).SubRefPos(it->second.GetParentStartTime());/*[it->second.GetParentStartTime()].Release();*/
		it->second.SetParentTrack(-1);
		it->second.SetParentStartTime(m_CurSelTrack,it->first,-1);
	}
}

void CEditTrack::ExpandKey(int nTrack,DWORD Flag,int& m_CurSelTime)
{
	map< int, EDITEFFECTKEY >::iterator it = m_EditKeys.upper_bound( m_CurSelTime );
	it--;

	if( it->first >= m_CurSelTime || 
		it->first + (int)it->second.GetEffectUnitProp()->GetUnitProp()->GetTimeRang() <= m_CurSelTime ||
		it->second.GetRef() )
		return;
	if( Flag & 0x01 )
	{
		int Start = 0;
		if( it != m_EditKeys.begin() )
		{
			map< int, EDITEFFECTKEY >::iterator pre = it;
			pre--;
			Start = pre->first + pre->second.GetEffectUnitProp()->GetUnitProp()->GetTimeRang();
		}
		if( it->second.GetParentStartTime() >= 0 )
			Start = max( Start, it->second.GetParentStartTime() );

		EDITEFFECTKEY Key = it->second;
		Key.GetEffectUnitProp()->GetUnitProp()->SetTimeRang( it->first + it->second.GetEffectUnitProp()->GetUnitProp()->GetTimeRang() - Start );
		DeleteKey(nTrack,it->first);
		//m_EditKeys.erase( it );
		//m_EditKeys[Start] = Key;
		InsertKey(nTrack,Start,Key);
		it = m_EditKeys.find( Start );
	}
	if(  Flag & 0x02  )
	{
		map< int, EDITEFFECTKEY >::iterator next = it;
		next++;
		int End = m_pEditProp->GetTimeRange();
		if( next != m_EditKeys.end() )
			End = next->first;
		if( it->second.GetParentStartTime() >= 0 )
			End = min( End, it->second.GetParentStartTime() + (int)m_pEditProp->GetEditTrack( it->second.GetParentTrack()).GetTimeRang(it->second.GetParentStartTime()));
		it->second.GetEffectUnitProp()->GetUnitProp()->SetTimeRang( End - it->first );
	}
}

int CEditTrack::GetTimeRang(int ParentStartTime)
{
	return m_EditKeys[ParentStartTime].GetEffectUnitProp()->GetUnitProp()->GetTimeRang();
}

void CEditTrack::SetTimeRang(int& TimeRang)
{
	for( map< int, EDITEFFECTKEY >::iterator it = m_EditKeys.begin(); it != m_EditKeys.end(); it++ )
	{
		if( it->first >= TimeRang )
		{
			if( it == m_EditKeys.begin() )
			{
				m_EditKeys.erase( it );
				break;
			}
			else
			{
				m_EditKeys.erase( it );
				it--;
			}
		}
		else if( it->first + (int)it->second.GetEffectUnitProp()->GetUnitProp()->GetTimeRang() >= TimeRang )
		{
			it->second.GetEffectUnitProp()->GetUnitProp()->SetTimeRang( TimeRang - it->first );
		}
	}
}

void CEditTrack::InsertKey(int Ln,int Pos,EDITEFFECTKEY key)
{
	key.SetPos(Pos);
	m_EditKeys[Pos] = key;
	// 同步属性,同步m_Track
	m_pEditProp->SetPropKey(Ln,Pos,&m_EditKeys[Pos]);
	m_pEditProp->SetEffectTrack(Ln,Pos);
}	

void CEditTrack::DeleteKey(int Ln,int Pos)
{
	SynSubTrack(Ln,Pos);
	map<int,EDITEFFECTKEY>::iterator iter = m_EditKeys.find(Pos);
	if(iter!=m_EditKeys.end())
		m_EditKeys.erase(iter);
}

void CEditTrack::SynSubTrack(int Ln,int Pos)
{
	m_pEditProp->DelEffectTrack(Ln,Pos);
	m_pEditProp->DelPropKey(Ln,Pos);
}

void CEditTrack::ClearKeys()
{
	m_EditKeys.clear();
}

CEditTrack::CEditTrack(CEditEffectProp* pEditProp)
{
	m_pEditProp = pEditProp;
	//m_pEditProp->m_Tracks.push_back(CEditEffect::sTrack());
	//m_pEditProp->AddPropTrack();
}

CEditEffectProp* CEditTrack::GetEditEffectProp()
{
	return m_pEditProp;
}

CEditTrack::~CEditTrack()
{
	m_pEditProp = NULL;
}

// void CEditTrack::SetLPEditEffect(CEditEffect* pEditEffect)
// {
// 	m_pEditProp = pEditEffect;
// }

void EDITEFFECTKEY::SetParentStartTime(int nTrack,int Pos,int ParentStartTime)
{
	nParentStartTime = ParentStartTime;
	m_pEditEffectProp->SetPropKey(nTrack,Pos,this);
	m_pEditEffectProp->LinkParent(nTrack,Pos);

	//m_pEditTrack->GetEditEffectProp()->SetPropKey(nTrack,Pos,this);
	//m_pEffectUnitProp->GetEffectProp()->LinkParent(nTrack,Pos);
}

void EDITEFFECTKEY::SetParentTrack(int ParentTrack)
{
	nParentTrack = ParentTrack;
}