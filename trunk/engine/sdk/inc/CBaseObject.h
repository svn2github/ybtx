#pragma once
#include "ModuleDefs.h"
#include <typeinfo>
#include <string>


namespace sqr
{
	enum EBaseObjectScriptFlag
	{
		eBOSF_VisableToScript = 0X01,
		eBOSF_CreatedByScript = 0X02
	};

	enum EBaseObjectType 
	{
		eBOT_Static = 0,
		eBOT_Share,
		eBOT_Dynamic,
		eBOT_None,
	};

	class MODULE_API CBaseObject
	{
	public:
		CBaseObject(void);
		virtual ~CBaseObject(void);

		virtual EBaseObjectType GetBaseObjectType() const = 0;

		virtual void SetObjectFlag(uint8 uFlagBit);

		virtual bool VisibleToScript()const;
		virtual bool CreatedByScript()const;

		virtual void SetScriptRefNum(int32 num);

		virtual bool RefByScript() const;

		virtual int32 GetScriptRefNum() const;

		virtual uint32 GetBaseObjectID() const;
	};

}
