#include "stdafx.h"
#include "CAppConfigStress.h"
#include "TAppConfig.inl"

template class TAppConfig<CTraitsStress>;


CAppConfigStress::CAppConfigStress(istream& strmConfig)
:ParentConfig_t(strmConfig)
{
	m_LangID = Get<int32>( "language_id" );
}

CAppConfigStress::~CAppConfigStress(void)
{
}

