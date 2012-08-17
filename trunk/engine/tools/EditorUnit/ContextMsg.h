#pragma once
#include "CMsg.h"
#include "CRenderNode.h"

//定义Context产生的系统消息
namespace sqr_tools
{
	class MSG_SYS : public CMsg{
	protected: MSG_SYS(const MSG_TYPE& fType,const MSG_INFO& fInfo):CMsg(MGS_SYSMSG,fType,fInfo){}
	};

	class MSG_SYS_INIT : public MSG_SYS{
	public:	MSG_SYS_INIT():MSG_SYS(MGT_SYS_INIT,0){}
	};

	class MSG_SYS_RESIZE : public MSG_SYS{
	public:	MSG_SYS_RESIZE():MSG_SYS(MGT_SYS_RESIZE,0){}
	};

	class MSG_SYS_SELCHANGE : public MSG_SYS{
	public:	MSG_SYS_SELCHANGE(CRenderNode* pSelObj):MSG_SYS(MGT_SYS_SELCHANGE,(void*)pSelObj){}
	};

	class MSG_SYS_RENDER : public MSG_SYS{
	public:	MSG_SYS_RENDER():MSG_SYS(MGT_SYS_RENDER,0){}
	};

	class MSG_SYS_ADDBONE : public MSG_SYS{
	public: MSG_SYS_ADDBONE(const MSG_INFO& fInfo):MSG_SYS(MGT_SYS_ADDBONE,fInfo){}
	};

	class MSG_SYS_SELBONECHANGE : public MSG_SYS{
	public:	MSG_SYS_SELBONECHANGE(const MSG_INFO& fInfo):MSG_SYS(MGT_SYS_SELBONE,fInfo){}
	};
}
