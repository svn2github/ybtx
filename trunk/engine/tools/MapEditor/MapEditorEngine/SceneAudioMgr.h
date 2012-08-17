#pragma once
#include "TSingleton.h"

namespace sqr
{
	class CTObject;
	class CSceneAudioMgr: public Singleton<CSceneAudioMgr>
	{
	private:
		///音效
		int						m_nCueRange;
		map<string,int8>		m_mapTerMaterialsIndexs;   //first: texname name second: material index
		map<string,int8>		m_mapMaterialIndexs;	   //first glass, sold, sand... second: material index
		map<int8,string>		m_mapIndexMaterials;	   //second glass, sold, sand... first: material index
		string					m_strPhyleName;			   //种族名字
		int8					m_nCurSeleteMaterialIndex; ///当前在图素包中选中的材质索引

		vector< CTObject *>		m_vecIndividualSoundObjects;
	public:
		CSceneAudioMgr();
		~CSceneAudioMgr();

		void	createAudioSystem();
		void	UpdateAudio();

		void	SetPhyleName( const string& strPhyleName, const string& strDefaultMatName );
		string  GetPhyleMaterialName(int8 index);	

		void	SetMaterialIndex( string& name, int8 index );
		int8	GetMaterialIndex( string& name );
		string	GetMaterialName(const int8 index) const;

		void	SetTerMaterialIndex( vector<string>& names, int8 index );
		int8	GetTerMaterialIndex( string& name );

		void	CalTerrainMaterialIndex();
		
		///场景音效模型show/hide
		void SwitchSoundObjectsShow(bool bRenderSmallMap = false);
		///场景音效play/stop
		void SwitchSoundPlay();
		///修改音效范围
		void ChangeSoundRange();

		void SetSpeedOfSoundVariable(float fValue);
		void SetAmbientVolume(const float fValue);

		inline void SetSceneCueBoxRange(const int nRange)
		{
			m_nCueRange = nRange;
		}

		inline int GetSceneCueBoxRange() const
		{
			return m_nCueRange;
		}

		// -----------------------------------------------------------------------------
		inline void SetCurSeleteMaterialIndex( const int8 nMatIndex )
		{
			m_nCurSeleteMaterialIndex = nMatIndex;
		}

		// -----------------------------------------------------------------------------
		inline int8 GetCurSeleteMaterialIndex( )
		{
			return m_nCurSeleteMaterialIndex;
		}

		inline void AddIndividualSoundObject( CTObject * p )
		{
			m_vecIndividualSoundObjects.push_back(p);
		}

		inline void RemoveIndividualSoundObject(CTObject * p)
		{
			m_vecIndividualSoundObjects.erase(
				remove(m_vecIndividualSoundObjects.begin(), m_vecIndividualSoundObjects.end(), p), m_vecIndividualSoundObjects.end() );
		}
	};
}