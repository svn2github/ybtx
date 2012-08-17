
#ifndef _CASYNCBTDOWNLOADER_H_
#define _CASYNCBTDOWNLOADER_H_

#include "BaseTypes.h"


namespace sqr
{
class CBTUpdater_Imp;
class IAsyncUpdateHandler;

class CAsyncUpdater
{
public:
	CAsyncUpdater();
	~CAsyncUpdater();

	int DownloadPatch(IAsyncUpdateHandler* updater, const char* Host, const char* File, const char* szCurPath);

private:
	CBTUpdater_Imp*	m_pImp;
};
}

#endif
