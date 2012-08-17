#pragma once
#include "CTexture.h"
#include "TSingleton.h"
#include "SyncSystem.h"

namespace sqr
{
	class CTextureQualityMgr 
		: public Singleton<CTextureQualityMgr>
		, public CGraphicMallocObject
	{
	public:
		CTextureQualityMgr();
		~CTextureQualityMgr();

		UINT	GetQualityLevel(void);
		void	SetQualityLevel( UINT QualityLevel );
		UINT	GetInCtrlSize();
		UINT	GetOutCtrlSize();
		void	Update(IProcessCallBack* pProcess = NULL);

		bool	_AddTexture( IResTexture* pText );
		bool	_DelTexture( IResTexture* pText );
		void	_ChangeTextureSize(int32 Size ,IResTexture* pText = NULL );
	private:
		typedef		GSet<IResTexture*>	QualityTextureSet;
		QualityTextureSet	m_setTextures;
		UINT				m_uQualityLevel;
		UINT				m_sInCtrlTexSize;
		UINT				m_sOutCtrlTexSize;
	};

	inline UINT	CTextureQualityMgr::GetQualityLevel(void)
	{
		return m_uQualityLevel;
	}

	inline UINT	CTextureQualityMgr::GetInCtrlSize()
	{
		return m_sInCtrlTexSize;
	}

	inline UINT	CTextureQualityMgr::GetOutCtrlSize()
	{
		return m_sOutCtrlTexSize;
	}
}