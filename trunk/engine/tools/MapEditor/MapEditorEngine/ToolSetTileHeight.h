#pragma once
#include "CEventOwner.h"
#include "ExsertStructDB.h"
#include "TSingleton.h"

namespace sqr
{
	class ITexture;
	struct RenderState;
	class CSquare;

	class CToolSetTileHeight: public Singleton<CToolSetTileHeight>
	{
	public:
		struct SProcessParams
		{
			SProcessParams()
			{
				dwBrushSizeInX = 1;
				dwBrushSizeInZ = 1;
				eWayType = sqr::EWT_INC;
				eHeightType = sqr::EHT_GAUSS;
				eDirType = sqr::EDT_BOTH;
				eTemplateType = sqr::EHTT_GAUSSCUT;
			}

			DWORD dwBrushSizeInX;
			DWORD dwBrushSizeInZ;
			sqr::EWAY_TYPE eWayType;
			sqr::EHEIGHT_TYPE eHeightType;
			sqr::EDIR_TYPE eDirType;
			sqr::EHEIGHT_TEMPLATE_TYPE eTemplateType;
		};

	private:

		HWND			m_hGroundView;

		SProcessParams	m_processParams;
		SProcessParams	m_processParamsLock;

		float			m_fReferenceHeight;

		vector<int>		m_vecGridIndicesLock;
		int				m_nGridIndexLockCenter;
	
		CSquare*		m_pLockArea;

		//画刷纹理对象
		ITexture *		m_pBrushTexture;
		RenderState *	m_pRS;

		float			m_fHeightStrength;	// 画刷强度
		float			m_fCutRadio;
		int				m_nLiftPower;

		void			ShowAreLockEvent();
		void			OperaterTileHeight();
	public:

		CToolSetTileHeight();
		~CToolSetTileHeight();
		void			OnEvent(const CMsg& msg);
		void			Render();
		vector<int>&	GetBrushGridIndices();

		DWORD			GetBrushSizeInX();
		void			SetBrushSizeInX(DWORD dw);
		void			IncBrushSizeInX();
		void			DecBrushSizeInX();

		DWORD			GetBrushSizeInZ();
		void			SetBrushSizeInZ(DWORD dw);
		void			IncBrushSizeInZ();
		void			DecBrushSizeInZ();

		void			SetHeightType(const int nType);
		sqr::EHEIGHT_TYPE GetHeightType();

		void			SetDirType(const int n);
		void			SetWayType(const int n);
		void			SetTemplateType(const int nType);

		void			InitProcessParams(HWND hGroundView);

		inline void		SetHeightStrength(const float f)
		{
			m_fHeightStrength = f;
		}

		inline float	GetHeightStrength() const
		{
			return m_fHeightStrength;
		}

		inline void		SetCutRadio(const  float f)
		{
			m_fCutRadio = f;
		}

		inline float	GetCutRadio() const
		{
			return m_fCutRadio;
		}

		inline void		SetLiftPower(const int n)
		{
			m_nLiftPower = n;
		}

		inline int		GetLiftPower() const
		{
			return m_nLiftPower;
		}
		
	};
}