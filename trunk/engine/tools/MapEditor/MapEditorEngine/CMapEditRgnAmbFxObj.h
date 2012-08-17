#pragma once
#include "scene.h"
#include "CMapEditObject.h"

namespace sqr_tools
{
	class CEditModel;
}

namespace sqr
{
	class CPieceGroup;
	class IEffect;
	class IEffectGroup;
	class CWireBox;

	class CMapEditRgnAmbFxObj : public CMapEditObject
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

		///返回值表示该音效是否为间隔暂停状态
		bool		 IsNeedRender(DWORD dwTime);

	public:

		CMapEditRgnAmbFxObj();

		virtual ~CMapEditRgnAmbFxObj();
		virtual void  Render();
		
		virtual void  SetPlayTimeLenMin( const DWORD dwTimeMin );
		virtual void  SetPlayTimeLenMax( const DWORD dwTimeMax );
		virtual void  SetPlayIntervalMin( const DWORD dwTimeMin );
		virtual void  SetPlayIntervalMax( const DWORD dwTimeMax );
		virtual DWORD GetPlayTimeLenMin();
		virtual DWORD GetPlayTimeLenMax();
		virtual DWORD GetPlayIntervalMin();
		virtual DWORD GetPlayIntervalMax();
		virtual void  SetNeedPlayBeginMark();
	};
}
