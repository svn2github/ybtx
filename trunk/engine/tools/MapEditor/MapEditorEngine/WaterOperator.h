#pragma once
#include "TSingleton.h"

namespace sqr
{
	class CRiver;

	class CWaterOperator : public Singleton<CWaterOperator>
	{
	private:
		// wave
		int						m_nWavePerGrid;
		int						m_nWaveSize;
		int						m_nWaveOff;
		float					m_fWaveSpeed;
		int						m_nWaveLife;
		string					m_strWaveTextureName;
		bool					m_bAddWave;
		bool					m_bVisualizeWaterBase;
		bool					m_bChangeWaterVetexAlpha;

		map<string, CRiver*>	m_mapRiverSpecies;
		HWND					m_hTextureView;
	public:

		CWaterOperator();
		~CWaterOperator();

		void	SetWaveDensity(int n);

		void	SetWaveTexture(string strTextureFileName);
		string	GetWaveTextureName() const;

		void	SetWaveSize(int n);
		void	SetWaveOff(int n);
		void	SetWaveSpeed(float f);
		void	SetWaveLife(int n);

		int		GetWaveDensity() const;
		int		GetWaveSize() const;
		int		GetWaveOff() const;
		float	GetWaveSpeed() const;
		int		GetWaveLife() const;
		float	GetWaveSpeedForClient() const;
		int		GetWaveLifeForClient() const;

		bool	IsAddWave() const;
		void	SetAddWave( bool b );

		bool	IsWaterHigTerrain() const;
		void	SetWaterHigTerrain( bool b );

		inline void SetDoesVisualizeWaterBase(bool b)
		{
			m_bVisualizeWaterBase = b;
		}

		inline bool GetDoesVisualizeWaterBase()
		{
			return m_bVisualizeWaterBase;
		}

		inline void	SetIsChangeWaterVetexAlpha(const bool b)
		{
			m_bChangeWaterVetexAlpha = b;
		}

		inline bool	GetIsChangeWaterVetexAlpha() const 
		{
			return m_bChangeWaterVetexAlpha;
		}

		void	NewRiver(string const & strItemName);
		void	RebirthRiver(string & strItemName);
		CRiver* GetRiver();

		void	SetTextureHwnd(HWND hTextureView)
		{
			m_hTextureView = hTextureView;
		}
	};
}
