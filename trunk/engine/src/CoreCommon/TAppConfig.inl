#pragma once
#include "TAppConfig.h"
#include "TAppConfig_inl.inl"
#include "ExpHelper.h"
#include <stdarg.h>
#include <fstream>
#include "CodeCvs.h"
#include "CXmlConfig.inl"

template<typename Traits>
typename TAppConfig<Traits>::ImpAppConfig_t*& TAppConfig<Traits>::IntInst()
{
	static ImpAppConfig_t*	ls_pInst=NULL;
	return ls_pInst;
}

template<typename Traits>
typename TAppConfig<Traits>::ImpAppConfig_t* TAppConfig<Traits>::Inst()
{
	ImpAppConfig_t* pInst = IntInst();
	Ast( pInst );

	return pInst;
}

template<typename Traits>
TAppConfig<Traits>::TAppConfig(istream& strmConfig)
{
	if( IntInst() )
		GenErr("AppConfig单件不能重复创建");
	
	IntInst() = static_cast<ImpAppConfig_t*>(this);

	m_pConfig = NULL;
	m_pConfig = new CXmlConfig( "main_config" , strmConfig );
	
	m_uBaseTickCyc	=	DftGet<uint32>( 33,"base_tick_cyc" );
	m_uConnMaxExp	=	DftGet<uint32>( 20,"conn_max_exp" );
	m_bPkgFileUsed =	DftGet<bool>( true, "pkg_file_used");
}

template<typename Traits>
TAppConfig<Traits>::~TAppConfig(void)
{
	delete m_pConfig;

	Ast( IntInst() );
	IntInst() = NULL;
}


template<typename Traits>
uint32 TAppConfig<Traits>::GetGCStepInterval()const
{
	return Get<uint32>( "collectgarbage" , "interval" );	
}


template<typename Traits>
uint32 TAppConfig<Traits>::GetGCStep()const
{
	return Get<uint32>( "collectgarbage" , "step" );	
}


template<typename Traits>
float TAppConfig<Traits>::GetGCCallbackRatio()const
{
	return DftGet<float>( 1, "collectgarbage" , "callback_ratio" );	
}

template<typename Traits>
float TAppConfig<Traits>::GetGCNormalUnpackRatio()const
{
	return DftGet<float>( 5, "collectgarbage" , "normal_unpack_ratio" );	
}

template<typename Traits>
float TAppConfig<Traits>::GetGCSerialUnpackRatio()const
{
	return DftGet<float>( 30, "collectgarbage" , "serial_unpack_ratio" );	
}


template<typename Traits>
CXmlConfig* TAppConfig<Traits>::XmlConfig()const
{
	return m_pConfig;
}


template<typename Traits>
uint32 TAppConfig<Traits>::GetBaseTickCyc()const
{
	return m_uBaseTickCyc;
}

template<typename Traits>
bool TAppConfig<Traits>::AutoMinimizeConsole()const
{
	return DftGet<bool>( true , "console_config" , "auto_minimize" );
}

template<typename Traits>
bool TAppConfig<Traits>::ShowConsole()const
{
	return DftGet<bool>( true , "console_config" , "show" );
}

template<typename Traits>
string TAppConfig<Traits>::GetConsoleTitle()const
{
	return DftGet<string>( "sqr engine", "console_config" , "title" );
}


template<typename Traits>
uint32 TAppConfig<Traits>::GetConnExpBearNum()const
{
	return m_uConnMaxExp;
}

template<typename Traits>
uint32 TAppConfig<Traits>::GetNetThreadNum()const
{
	return DftGet<uint32>( 0,"net_thread_num" );
}

template<typename Traits>
bool TAppConfig<Traits>::PkgFileUsed()const
{
	return m_bPkgFileUsed;
}


