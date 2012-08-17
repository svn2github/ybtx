#pragma once
#include "CShareObject.h"

namespace sqr
{
	class CXmlConfig;

	template<typename Traits>
	class TAppConfig
		:public virtual CShareObject
	{
		typedef typename Traits::AppConfig_t		ImpAppConfig_t;
	public:
		static ImpAppConfig_t* Inst();

		uint32 GetBaseTickCyc()const;
		bool AutoMinimizeConsole()const;
		bool ShowConsole()const;
		string GetConsoleTitle()const;
		uint32 GetConnExpBearNum()const;
		uint32 GetNetThreadNum()const;
		bool PkgFileUsed() const;

		uint32 GetGCStepInterval() const;
		uint32 GetGCStep() const;
		float GetGCCallbackRatio() const;
		float GetGCNormalUnpackRatio() const;
		float GetGCSerialUnpackRatio() const;

		CXmlConfig* XmlConfig()const;

		template<typename ValueType>
		ValueType DftGet(ValueType DefaultValue,const char* szElemName)const;
		template<typename ValueType>
		ValueType DftGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2)const;
		template<typename ValueType>
		ValueType DftGet(ValueType DefaultValue,const char* szElemName1,const char* szElemName2,const char* szElemName3)const;

		template<typename ValueType>
		ValueType Get(const char* szElemName)const;
		template<typename ValueType>
		ValueType Get(const char* szElemName1,const char* szElemName2)const;
		template<typename ValueType>
		ValueType Get(const char* szElemName1,const char* szElemName2,const char* szElemName3)const;
	protected:
		TAppConfig(istream& strmConfig);
		~TAppConfig(void);
	private:

		CXmlConfig*	m_pConfig;

		static ImpAppConfig_t*& IntInst();

		uint32	m_uBaseTickCyc;
		uint32	m_uConnMaxExp;
		bool	m_bPkgFileUsed;
	};
}
