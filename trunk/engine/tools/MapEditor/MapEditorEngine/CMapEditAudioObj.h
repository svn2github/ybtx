#pragma once
#include "scene.h"
#include "CMapEditObject.h"

namespace sqr
{
	class CMapEditAudioObj : public CMapEditObject
	{
		DWORD	m_dwPlayTimeLenMin;		///播放时间长度最小值
		DWORD	m_dwPlayTimeLenMax;		///播放时间长度最大值
		DWORD	m_dwPlayIntervalMin;	///2段播放时间间隔长度最小值
		DWORD	m_dwPlayIntervalMax;	///2段播放时间间隔长度最大值
		DWORD	m_dwCurRandomPlayTime;	///当前随机播放时间长度
		DWORD	m_dwCurRandomInterval;	///当前随机2段播放时间间隔长度
		DWORD	m_dwPlayTime;			///当前已经播放多久时间
		DWORD	m_dwIntervalTime;		///当前间隔多久时间
		bool	m_bNeedPlayBegin;
		bool	m_bNeedIntervalBegin;
		
	public:

		CMapEditAudioObj();

		virtual ~CMapEditAudioObj();
		virtual void Render();

		virtual void SetSceneCueName(const string& name);	

		virtual string GetSceneCueName() const
		{
			return m_strSceneCueName;
		}

		virtual void StopSceneCue( const bool bStop, const bool bAuto = true );
		virtual bool IsNeedPlay();
		virtual bool PlaySceneCue(DWORD dwTime);
		virtual bool GetIsStopSound();

		virtual void	SetPlayTimeLenMin( const DWORD dwTimeMin );
		virtual void	SetPlayTimeLenMax( const DWORD dwTimeMax );
		virtual void	SetPlayIntervalMin( const DWORD dwTimeMin );
		virtual void	SetPlayIntervalMax( const DWORD dwTimeMax );
		virtual DWORD	GetPlayTimeLenMin();
		virtual DWORD	GetPlayTimeLenMax();
		virtual DWORD	GetPlayIntervalMin();
		virtual DWORD	GetPlayIntervalMax();
		virtual DWORD	GetPlayTimeLen()
		{
			return m_dwCurRandomPlayTime;
		}
		virtual DWORD	GetCurPlayIntervalTime()
		{
			return m_dwCurRandomInterval;
		}

		virtual void	SetNeedPlayBeginMark();
		
		virtual void	AddObjectRenderDialog(CTObject *pObject);
		virtual void	SetVisible(bool b);
		virtual void	HideSelf();
		virtual void	ShowSelf();
		virtual void    SetLocked(bool b);
		
	};
}
