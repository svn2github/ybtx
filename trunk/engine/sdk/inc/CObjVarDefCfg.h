#pragma once


namespace sqr
{
	class CObjVarDef;
	class CSyncCoreObjectDictator;
	class CCoreObjectDictator;
	class CCoreObjectFollower;

	class CObjVarDefCfg
	{
	public:
		CObjVarDefCfg(void);
		~CObjVarDefCfg(void);

		uint32 Get(const char* szFileName);
	private:
		friend class CSyncCoreObjectDictator;
		friend class CCoreObjectDictator;
		friend class CCoreObjectFollower;

		CObjVarDef* Get(uint32 uDefId)const;

		map<uint32,CObjVarDef*>	m_mapId2Def;
		map<string,uint32>		m_mapName2Id;

		static uint32& GetCurDefID();
	};
}

