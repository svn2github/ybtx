#pragma once
#include "TSingleton.h"
#include "CEventOwner.h"

namespace sqr
{
	struct SVegetationBrush
	{
		SVegetationBrush()
		{
			nDensityMin		= 1;
			nDensityMax		= 1;
			nBrightRatioMix = 0;
			nBrightRatioMax = 256;
			nXRotationMin	= 0;
			nXRotationMax	= 0;
			nYRotationMin	= 0;
			nYRotationMax	= 0;
			nZRotationMin	= 0;
			nZRotationMax	= 0;
			nBrushRadius	= 1;
			fXSizeMin		= 1.0f;
			fXSizeMax		= 1.0f;
			fYSizeMin		= 1.0f;
			fYSizeMax		= 1.0f;
			fZSizeMin		= 1.0f;
			fZSizeMax		= 1.0f;
		}

		int nDensityMin;
		int nDensityMax;
		int nBrightRatioMix;//-->CEditModelGroup::m_nAffectRatio
		int nBrightRatioMax;
		int nXRotationMin;
		int nXRotationMax;
		int nYRotationMin;
		int nYRotationMax;
		int nZRotationMin;
		int nZRotationMax;
		int nBrushRadius;
		float fXSizeMin;
		float fXSizeMax;
		float fYSizeMin;
		float fYSizeMax;
		float fZSizeMin;
		float fZSizeMax;
	};

	class CVegetationCtrlMgr: public Singleton<CVegetationCtrlMgr>
	{
	private:
		///刷草功能刷子对象
		SVegetationBrush	*m_pVegetationBrush;

	public:
		CVegetationCtrlMgr()
		{
			m_pVegetationBrush = NULL;
			m_pVegetationBrush = new SVegetationBrush;
		}

		~CVegetationCtrlMgr()
		{
			if( m_pVegetationBrush )
			{
				delete m_pVegetationBrush;
				m_pVegetationBrush = NULL;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//SVegetationBrush Operation
		// -----------------------------------------------------------------------------
		inline void SetVegetationBrushRadius( const int nBrushRadius )
		{
			m_pVegetationBrush->nBrushRadius = nBrushRadius;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelSizeXMin( const float fSizeXMin )
		{
			m_pVegetationBrush->fXSizeMin = fSizeXMin;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelSizeXMax( const float fSizeXMax )
		{
			m_pVegetationBrush->fXSizeMax = fSizeXMax;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelSizeYMin( const float fSizeYMin )
		{
			m_pVegetationBrush->fYSizeMin = fSizeYMin;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelSizeYMax( const float fSizeYMax )
		{
			m_pVegetationBrush->fYSizeMax = fSizeYMax;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelSizeZMin( const float fSizeZMin )
		{
			m_pVegetationBrush->fZSizeMin = fSizeZMin;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelSizeZMax( const float fSizeZMax )
		{
			m_pVegetationBrush->fZSizeMax = fSizeZMax;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelRotateXMin( const int nRotateXMin )
		{
			m_pVegetationBrush->nXRotationMin = nRotateXMin;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelRotateXMax( const int nRotateXMax )
		{
			m_pVegetationBrush->nXRotationMax = nRotateXMax;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelRotateYMin( const int nRotateYMin )
		{	
			m_pVegetationBrush->nYRotationMin = nRotateYMin;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelRotateYMax( const int nRotateYMax )
		{
			m_pVegetationBrush->nYRotationMax = nRotateYMax;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelRotateZMin( const int nRotateZMin )
		{
			m_pVegetationBrush->nZRotationMin = nRotateZMin;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelRotateZMax( const int nRotateZMax )
		{
			m_pVegetationBrush->nZRotationMax = nRotateZMax;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelBrightMin( const int nBrightMin )
		{
			m_pVegetationBrush->nBrightRatioMix = nBrightMin;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelBrightMax( const int nBrightMax )
		{
			m_pVegetationBrush->nBrightRatioMax = nBrightMax;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelDensityMin( const int nDensityMin )
		{
			m_pVegetationBrush->nDensityMin = nDensityMin;
		}

		// -----------------------------------------------------------------------------
		inline void SetModelDensityMax( const int nDensityMax )
		{
			m_pVegetationBrush->nDensityMax = nDensityMax;
		}

		// -----------------------------------------------------------------------------
		inline int GetVegetationBrushRadius() const
		{
			return m_pVegetationBrush->nBrushRadius;
		}

		// -----------------------------------------------------------------------------
		inline float GetModelSizeXMin() const
		{
			return m_pVegetationBrush->fXSizeMin;
		}

		// -----------------------------------------------------------------------------
		inline float GetModelSizeXMax() const
		{
			return m_pVegetationBrush->fXSizeMax;
		}

		// -----------------------------------------------------------------------------
		inline float GetModelSizeYMin() const
		{
			return m_pVegetationBrush->fYSizeMin;
		}

		// -----------------------------------------------------------------------------
		inline float GetModelSizeYMax() const
		{
			return m_pVegetationBrush->fYSizeMax;
		}

		// -----------------------------------------------------------------------------
		inline float GetModelSizeZMin() const
		{
			return m_pVegetationBrush->fZSizeMin;
		}

		// -----------------------------------------------------------------------------
		inline float GetModelSizeZMax() const
		{
			return m_pVegetationBrush->fZSizeMax;
		}

		// -----------------------------------------------------------------------------
		inline int GetModelRotateXMin() const
		{
			return m_pVegetationBrush->nXRotationMin;
		}

		// -----------------------------------------------------------------------------
		inline int GetModelRotateXMax() const
		{
			return m_pVegetationBrush->nXRotationMax;
		}
		// -----------------------------------------------------------------------------
		inline int GetModelRotateYMin() const
		{
			return m_pVegetationBrush->nYRotationMin;
		}

		// -----------------------------------------------------------------------------
		inline int GetModelRotateYMax() const
		{
			return m_pVegetationBrush->nYRotationMax;
		}
		// -----------------------------------------------------------------------------
		inline int GetModelRotateZMin() const
		{
			return m_pVegetationBrush->nZRotationMin;
		}

		// -----------------------------------------------------------------------------
		inline int GetModelRotateZMax() const
		{
			return m_pVegetationBrush->nZRotationMax;
		}

		// -----------------------------------------------------------------------------
		inline int GetModelBrightMin() const
		{
			return m_pVegetationBrush->nBrightRatioMix;
		}

		// -----------------------------------------------------------------------------
		inline int GetModelBrightMax() const
		{
			return m_pVegetationBrush->nBrightRatioMax;
		}

		// -----------------------------------------------------------------------------
		inline int GetModelDensityMin() const
		{
			return m_pVegetationBrush->nDensityMin;
		}

		// -----------------------------------------------------------------------------
		inline int GetModelDensityMax() const
		{
			return m_pVegetationBrush->nDensityMax;
		}

	};
}