#pragma once
#include "TAppConfig.h"
#include "CTraitsStress.h"

namespace sqr
{

	class CAppConfigStress
		:public TAppConfig<CTraitsStress>
	{
	private:
		typedef TAppConfig<CTraitsStress> ParentConfig_t;
	public:
		CAppConfigStress(istream& strmConfig);
		~CAppConfigStress(void);

	private:
		int32 m_LangID;
	};

}

