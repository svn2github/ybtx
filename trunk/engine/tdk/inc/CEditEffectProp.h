#pragma once
#include "CEditEffectGroup.h"
#include "CEditEffect.h"
#include "CEditEffectTrack.h"
#include <vector>
using namespace std;

typedef void(* ESFN_PROC)(CEditEffect*);

#define BEGIN_FOREACH_EFFECT( pVar ) \
	EffectPool::iterator iter = m_EffectSet.begin();\
	while (iter!=m_EffectSet.end())\
	{\
		pVar = *iter++;\

#define END_FOREACH_EFFECT()	}

class CEditEffectProp : public CEffectProp
{
	friend class CEditEffectManager;
	friend class CEditEffect;

protected:
	vector< CEditTrack >	m_EditTracks;

public:
	virtual void					AddTrack()						;
	virtual int						GetTrackNum()					{ return m_EditTracks.size(); }
	virtual CEditTrack&				GetEditTrack( int Num )				
	{
		assert(!m_EditTracks.empty());
		return m_EditTracks[Num]; 
	}

	virtual CEditTrack*				GetEditTrackPtr( int Num )				
	{
		assert(!m_EditTracks.empty());
		return &(m_EditTracks[Num]); 
	}

	virtual void					DeleteTrack( int Num )			;
	virtual void					ShowTrack( int Num )			;
	void							InitTrackInfo();
	virtual int						SaveBin( CEditEffectWriteFile& File, const string& strEffectName );
	virtual void					UnShowSelected()				;
	//void							ClearEffectTrack();
	void							SetEffectTrack(int Ln,int Pos);
	void							DelEffectTrack(int Ln,int Pos);
	void							LinkParent(int Ln,int Pos);
	void							ShowSelected(int Num,int sel)	;	
	void							ReplaceTrackPos(int first,int second);
	void							CopyTrackBindInfo(int source,int dest);

public:
	CEditEffectProp( CBaseEffectGroup* pEffectGroup, IEffectManager* pEffectMgr, const EString& szName );
	~CEditEffectProp();
	virtual int		GetTimeRange()	;
	virtual void	SetTimeRange( int TimeRange );

	virtual void	SetUserDesc( LPCTSTR str );
	virtual LPCTSTR	GetUserDesc();
	virtual void	SetAttachAniName( LPCTSTR str ) ;
	virtual LPCTSTR	GetAttachFullAniName();

	void SetFrameSynch(bool bSyn);
	void SetForbidScale(bool bForbid);
	void UpdateSelfProp(vector< map< int, EDITEFFECTKEY > >& vecRef);

	void ClearTrackInfo();
	void AddPropTrack();
	void SetPropKey(int i,int Pos,EDITEFFECTKEY* pEditEffectKey);
	void DelPropKey(int Ln,int Pos);
	void ReplaceTwoTrack(int one,int another);
	void DelAttachKey(IEffectUnitProp* pProp);

	void AddEditEffect(CEditEffect* pEditEffect);
	void DelEditEffect(CEditEffect* pEditEffect);
	void ClearEditEffects();

	CEditEffectGroup*	GetEffectGroup(void);
	CEditEffect*		CreateNewEffect(void);
	void				AddEffectTrack(void);
	void				SetEffectUnitsProp(int TrackID,int Pos);
	void				EffectShow(int Num);

	float				GetCurFrame(){return (*m_EffectSet.begin())->GetCurFrame();}
	IEffectUnit*		GetEffectUnit(int ln,int pos){return (*m_EffectSet.begin())->GetEffectKey(ln,pos).pEffectUnit;}
protected:
	typedef set<CEditEffect*> EffectPool;
	EffectPool	m_EffectSet;
};

