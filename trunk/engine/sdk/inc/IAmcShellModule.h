// ¶¯Ì¬Ä£ÐÍ


#ifndef __IAMCSHELLMODULE__
#define __IAMCSHELLMODULE__


// Include files
#include "Module.h"

class CBaseAmc;

class IAmcShellModule
			:public IBase
{

public:
	interface_version(IAmcShellModule,0);
	virtual CBaseAmc* GetShell(void* pCoreHandle)=0;
	virtual void ReleaseShell(CBaseAmc* pBaseAmc)=0;
};// END CLASS DEFINITION IAmcShellModule

#endif // __IAMCSHELLMODULE__
