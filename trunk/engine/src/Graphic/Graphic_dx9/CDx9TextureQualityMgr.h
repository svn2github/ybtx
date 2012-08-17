#pragma once
#include "CDx9Texture.h"
#include "TSingleton.h"
#include "SyncSystem.h"

namespace sqr
{
	class CDx9TextureQualityMgr 
		: public Singleton<CDx9TextureQualityMgr>
		, public CGraphicMallocObject
	{
		friend class DX9Graphic;
		friend class CDx9Texture;
	protected:
		CDx9TextureQualityMgr();
		~CDx9TextureQualityMgr();

	public:
		UINT	GetQualityLevel(void);
		void	SetQualityLevel( UINT QualityLevel );
		UINT	GetInCtrlSize();
		UINT	GetOutCtrlSize();
		void	Update(CSyncProcess* pProcess = NULL);
	private:
		typedef	GSet<CDx9Texture*>	QualityTextureSet;
		bool	_AddTexture( CDx9Texture* pText );
		bool	_DelTexture( CDx9Texture* pText );
		void	_ChangeTextureSize(int32 Size ,CDx9Texture* pText = NULL );
		QualityTextureSet	m_setTextures;
		UINT				m_uQualityLevel;
		UINT				m_sInCtrlTexSize;
		UINT				m_sOutCtrlTexSize;
	};

	inline UINT	CDx9TextureQualityMgr::GetQualityLevel(void)
	{
		return m_uQualityLevel;
	}

	inline void CDx9TextureQualityMgr::SetQualityLevel( UINT QualityLevel )
	{
		m_uQualityLevel = QualityLevel;
	}

	inline UINT	CDx9TextureQualityMgr::GetInCtrlSize()
	{
		return m_sInCtrlTexSize;
	}

	inline UINT	CDx9TextureQualityMgr::GetOutCtrlSize()
	{
		return m_sOutCtrlTexSize;
	}
}