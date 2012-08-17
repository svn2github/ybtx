#pragma once

#include "CCtrlBase.h"

namespace sqr
{
	class IEffectGroup;
}

namespace sqr_tools
{
	class CMapEffectCtrlImp :public CCtrlBase
	{
		REG_UI_IMP(CMapEffectCtrlImp);

	public:

		typedef map< string, vector< string > > EffectNameList;
		typedef map< string, string >			EffectFileList;
	
		struct stAmbientEffectInfo
		{
			string strEffect;
			int    nProbability;

			stAmbientEffectInfo()
			{
				strEffect = "";
				nProbability = 0;
			}
		};

	public:
		CMapEffectCtrlImp(void* param);
		~CMapEffectCtrlImp(void);

	private:
		EffectNameList		m_mapLocalEffectNameList;
		EffectNameList		m_mapAmbientEffectNameList;
		EffectFileList		m_mapLocalEffectFileList;
		EffectFileList		m_mapAmbientEffectFileList;
		IEffectGroup*		m_IEffectGroupForSceneLight;
		map<string, void *>	m_LocalEffectGroupList;
		string				m_strActiveEffectName;

		virtual void GetEffectFiles(const string& effectDir, EffectNameList& mapEffectGroupList);

	public:

		virtual void Update();
		virtual bool _ProcMsg(const CMsg& msg );

		virtual void LoadMapLocalEffect();
		virtual void LoadMapAmbientEffect();
		virtual bool CreateEffectGroup( const string& strFXFileName, const string& effectname, void ** p );

		virtual int	 GetLocalEffectGroupCount();
		virtual void GetLocalEffectGroupName(const int index, string& groupname);
		virtual int  GetLocalEffectUnitCount(const string& groupname);
		virtual void GetLocalEffectUnitName(const int index, const string& groupname, string& effectname);

		virtual int	 GetAmbientEffectGroupCount();
		virtual void GetAmbientEffectGroupName(const int index, string& groupname);
		virtual int  GetAmbientEffectUnitCount(const string& groupname);
		virtual void GetAmbientEffectUnitName(const int index, const string& groupname, string& effectname);

		virtual void SetActiveEffectName(const string& name);
		virtual string GetActiveEffectName(const string& name);

		virtual void GetEffectFileName(const string& effectname, string& effectfilename);

		virtual void SetContext(CEditContext* pContext);
	};

}