#pragma once
#include "Scene.h"
#include "CRefObject.h"
#include "CVector3.h"
#include "CVector2.h"

namespace sqr
{
	class ITexture;
	class CWaterMgr;
	struct RenderState;
	class CWaterSurface;

	#define MAX_WAVENUM_PER_GRID  4
	#define WAVE_RANDOM_ROTATION 0.3f

	SmartPointer(CRiverWave);
	/// ÀË»¨
	class CRiverWave : public CRefObject
	{
	public:

		struct SWavePiece
		{
			bool		bInUse;
			int			nLife;
			int			nLifeElapsed;
			float		fSpeed;
			CVector3f vDir;
			DWORD dwClr[4];
			CVector2f vUV[4];
			CVector3f vCornersMoving[4];
		};

	private:

		float m_fBase;
		int m_nBase;
		CVector3f m_vCenter;
		CVector3f m_vCorners[4];
		SWavePiece	m_WavePiecesReserved[MAX_WAVENUM_PER_GRID];
		DWORD m_dwLifeIdleCycle;
		int m_nWaveCycle;

	public:

		CRiverWave::CRiverWave();
		void		Create(int n, CVector3f vCenter);
		void		Update();
		void		Render();
		SWavePiece& GetWavePiece(int i);
		int			GetBase() const;
		CVector3f   GetCenter() const ;
	};

	/// Ë®Ãæ.
	class CRiver : public ISceneNode
	{
		map<index_t, set<CRiverWavePtr> > m_mapRegionWaves;
		vector<string> m_vecTextureNames;
		string m_strCurrentTextureHandle;
		string m_strRiverWaveTextureName;
		string m_strItemName;
		int m_nCnt;
		DWORD m_dwTextureBaseIndex;

		ITexture *m_pRiverWaveTexture;
		RenderState *m_pRS;
		bool m_bEnableFog;

		CWaterSurface* m_pWaterSurface;
		set<index_t> m_setDirtyRegion;
	private:
		static CWaterMgr*	   s_pWaterMgr;
	public:
		CRiver(ISceneNode* pParent, bool bIsVisible);
		~CRiver();
		void Create(vector<string> vecTextureNames);

		void Render();
		void UpdateFPSLimited();
		CRiverWave* AddWave(index_t regionIndex);
		void RemoveWave(index_t regionIndex, const CRiverWavePtr& pWave);
		void SetItemName(string const& strItemName);
		string& GetItemName();
		DWORD SaveTexture(FILE* fp, DWORD dwTextureBaseIndex);
		DWORD GetTextureBaseIndex();
		DWORD GetTextureCount();
		vector<string> GetTextureNams(DWORD dwTextureBaseIndex);
		void UpdateWaveTexture();
		void AddDirtyRegion(index_t regionIndex);
	private:
		void UpdateRegion(index_t regionIndex);
	};

	//------------------------------------------------------------------------------
	inline DWORD CRiver::GetTextureBaseIndex()
	{
		return m_dwTextureBaseIndex;
	}

	//------------------------------------------------------------------------------
	inline DWORD CRiver::GetTextureCount()
	{
		return m_vecTextureNames.size();
	}

	//------------------------------------------------------------------------------
	inline void CRiver::SetItemName(string const & strItemName)
	{
		m_strItemName = strItemName;
	}

	//------------------------------------------------------------------------------
	inline string & CRiver::GetItemName()
	{
		return m_strItemName;
	}

	//------------------------------------------------------------------------------
	inline CRiverWave::SWavePiece & CRiverWave::GetWavePiece(int i)
	{
		return m_WavePiecesReserved[i];
	}

	//------------------------------------------------------------------------------
	inline int CRiverWave::GetBase() const
	{
		return m_nBase;
	}

	//------------------------------------------------------------------------------
	inline CVector3f CRiverWave:: GetCenter() const
	{
		return m_vCenter;
	}

	//------------------------------------------------------------------------------
	inline void
	CRiver::AddDirtyRegion( index_t regionIndex )
	{
		m_setDirtyRegion.insert(regionIndex);
	}
}