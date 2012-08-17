#pragma once
#include "CEditEfffectClassDefine.h"
#include "IEffectUnitProp.h"
#include "PkgStream.h"

class CEditEffectReadFile : public IEffectRead
{
	ipkgstream m_File;
	int nPos;

public:
	CEditEffectReadFile( const wchar_t* szAlize, const TCHAR* szFileName, int32 nFlag )
		: m_File( szAlize, szFileName, nFlag!=0 ), nPos(0){}

	bool fail(){ return m_File.fail(); }

	void close(){ return m_File.close(); }

	void read( void* pBuf, size_t uSize )
	{
		nPos += uSize;
		m_File.read( (TCHAR*)pBuf, uSize );
	}
};

class CEditEffectWriteFile : public IEffectWrite
{
	opkgstream m_File;
	int nPos;

public:
	CEditEffectWriteFile( const TCHAR* szFileName, int32 nFlag )
		: m_File( szFileName, nFlag ), nPos(0){}

	bool fail(){ return m_File.fail(); }

	void close(){ return m_File.close(); }

	void write( void* pBuf, size_t uSize )
	{
		nPos += uSize;
		m_File.write( (TCHAR*)pBuf, uSize );
	}
};

class CEditTrack;
class EDITEFFECTKEY
{
public:
	EDITEFFECTKEY() : nRef(0)
		, nParentTrack(-1)
		, nParentStartTime(-1)
		, bSelected(FALSE)
		, nPos(-1)
		, bOffsetOnly(FALSE)
		, m_pEffectUnitProp(NULL)
	{
	}

	~EDITEFFECTKEY()
	{
	}

	CEditUnitPropHelp*	GetEffectUnitProp(){return m_pEffectUnitProp;}
	CEditEffectProp*	GetEditEffectProp();
	int					GetParentTrack(){return nParentTrack;}
	int					GetParentStartTime(){return nParentStartTime;}
	int					GetPos(){return nPos;}
	UINT				GetRef(){return nRef;}
	BOOL				IsSelected(){return bSelected;}
	BOOL				IsNullEffectUnitProp(){return m_pEffectUnitProp?FALSE:TRUE;}
	
	void				AddRef(){nRef++;}
	void				Release(){nRef--;}

	void				SetParentTrack(int ParentTrack);

	void				SetParentStartTime(int nTrack,int Pos,int ParentStartTime);

	void				SetPos(int Pos)
	{
		nPos = Pos;
	}

	void				SetSelected(BOOL b){bSelected = b;}
	void				SetEffectUnitProp(CEditUnitPropHelp* pEffectUnitProp,CEditEffectProp* pEditEffectProp)
	{
		m_pEffectUnitProp = pEffectUnitProp;
		m_pEditEffectProp = pEditEffectProp;
	}
	
protected:

	UINT				nRef;
	int					nParentTrack;
	int					nParentStartTime;
	int					nPos;

	BOOL				bSelected;
	BOOL				bOffsetOnly;

	CEditUnitPropHelp*	m_pEffectUnitProp;
	CEditEffectProp*	m_pEditEffectProp;
	CEditTrack*			m_pEditTrack;
};

class CEditTrack
{
public:
	CEditTrack(){}
	CEditTrack(CEditEffectProp* pEditProp);
	~CEditTrack();
	void					BreakLinkOfParentTrack(int nTrack,int Num);
	void					SubRefParentTrack(int nTrackNum);
	void					Show();
	void					UnShow();
	int						GetKeySize();
	int						Save(CEditEffectWriteFile& File);
	void					SubRefPos(int Pos);
	map<int,EDITEFFECTKEY>& GetEditKeys();
	EDITEFFECTKEY&			GetEditKey(int Pos);
	EDITEFFECTKEY*			GetEditKeyPtr(int Pos);
	void					SetPos();
	void					TrackUp(int& m_CurSelTrack);
	void					TrackDown(int& m_CurSelTrack);
	void					BreakLink(int Ln,int& m_CurSelTime);
	void					DeleteKeyLink(int& m_CurSelTrack,int& m_CurSelTime);
	void					ExpandKey(int nTrack,DWORD Flag,int& m_CurSelTime);
	int						GetTimeRang(int ParentStartTime);
	void					SetTimeRang(int& TimeRang);
	void					ClearKeys();
	void					InsertKey(int Ln,int Pos,EDITEFFECTKEY key);
	void					DeleteKey(int Ln,int Pos);
	void					SynSubTrack(int Ln,int Pos);
	void					SetLPEditEffect(CEditEffect* pEditEffect);
	CEditEffectProp*		GetEditEffectProp();
protected:
	CEditEffectProp*			m_pEditProp;
	map<int ,EDITEFFECTKEY>		m_EditKeys; 
};