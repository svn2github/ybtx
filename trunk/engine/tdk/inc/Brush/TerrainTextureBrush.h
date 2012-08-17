#pragma once
#include "BaseBrush.h"

namespace sqr_tools
{
	class CTerrainTextureData;
	class CTerrainTextureBrush : public CBaseBrush
	{
	public:

		typedef vector<string> brushTexPool;

		CTerrainTextureBrush(void);
		~CTerrainTextureBrush(void);

		void InitTextureData();
		void SetBrushTextureNames(const brushTexPool& names);
		void SetHotLayer(const int nHotLayer)
		{
			m_nHotLayer = nHotLayer;
		}
		int  GetHotLayer() const
		{
			return m_nHotLayer;
		}
		void SetDirType(const int nType)
		{
			m_eDirType = (EDIR_TYPE)nType;
		}

		void SetWayType(const int nType)
		{
			m_eWayType = (EWAY_TYPE)nType;
		}

		void SetAlphaStrength(const float f)
		{
			m_fAlphaStrength = f;
		}
		float GetAlphaStrength() const
		{
			return m_fAlphaStrength;
		}

		virtual void MoveLButtonDownEvent(MSG_MOUSE_INF mouse);
		virtual bool KeyDownEvent(MSG_KEY_INF key);
		virtual void KeyUpEvent(MSG_KEY_INF key);
		virtual void CtrlKeyDownEvent(MSG_KEY_INF key);

		void	IncVertexAlphaB(uint32 uX, uint32 uY);
		void	DecVertexAlphaB(uint32 uX, uint32 uY);
		void	CutVertexAlpha(uint32 uX, uint32 uY);

	private:
		void	BrushLayerTwoTexture();
		void	ShowTexture();
	private:
		EWAY_TYPE				m_eWayType;
		EDIR_TYPE				m_eDirType;
		int						m_nHotLayer;
		float					m_fAlphaStrength;
		brushTexPool			m_vecBrushTextureNames;
		CTerrainTextureData*	m_pTerrainTextureData;
	};
}
