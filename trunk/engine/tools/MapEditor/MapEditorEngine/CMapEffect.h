#pragma once
#include "TSingleton.h"


namespace sqr
{
	class IEffectGroup;
	class CTObject;

	class CMapEffect : public Singleton<CMapEffect>
	{
	private:
		HWND							   m_hFXSetView;

	public:
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

		IEffectGroup*						m_IEffectGroupForSceneLight;
		map<string, void *>					EffectGroupList;
		map<string, void *>					RgnAmbientFxGroupList;
		map<string, stAmbientEffectInfo *>  mapAmientUseEffect;
		map< string, vector< string > >		mapLocalEffectNameList;

		CMapEffect();
		virtual ~CMapEffect();


	public:
		void  CreateEffectGroup(const string& strFXFileName, const string& effectname, void ** p);
		void  GetFXNames(string & strFXFileName, map< string, vector< string > > & mapFX, void ** p);
		void* GetLocalFXGroup( const string& effectname );
		void  ToolSetModel_LinkEffect(const string& effectname, CTObject * pModel);
		void  ToolSetModel_LinkEffect(const string& effectname, void * pAmbient = NULL);
		void  ToolSetModel_LinkEffect(string str, void * p, void * pAmbient);

		void  AddUseAmbientEffect(const string& effectname, const int nProbability);
		void  DeleteUseAmbientEffect(const string& effectname);

		///region ambient effect
		void  CreateRgnAmbientFxGroup(const string& strFXFileName, const string& effectname, void ** p);
		void* GetRgnAmbientFXGroup( const string& effectname );
		bool  ToolSetModel_LinkRgnFxLink( const string& effectname, void * pAmbient );

		void  SetLocalEffectGroupNames(const map< string, vector< string > >& list);

		void  GetLocalEffectGroupNames(map< string, vector< string > >& list)
		{
			list = this->mapLocalEffectNameList;
		}

		void SetEffectHWND( HWND hFXSetView)
		{
			m_hFXSetView = hFXSetView;
		}

		void SaveAmbientFXInfo(FILE * fp);
		void SaveAmbientFXInfoAsClient(FILE * fp);

		void ReadAmbientFXInfo(FILE * fp);
		void LoadAmbientFXModel();
	};
}
