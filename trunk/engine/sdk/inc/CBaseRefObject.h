#pragma once 
#include "CBaseObject.h"
#include "ModuleDefs.h"

namespace sqr 
{

	class MODULE_API CBaseRefObject : public CBaseObject
	{
	public:
		CBaseRefObject();
		~CBaseRefObject();

		virtual EBaseObjectType GetBaseObjectType() const = 0;

		void SetObjectFlag(uint8 uFlagBit);

		bool VisibleToScript()const;
		bool CreatedByScript()const;

		void SetScriptRefNum(int32 num);
		
		bool RefByScript() const;

		int32 GetScriptRefNum() const;

	private:
		int32	m_nScriptRefNum;
		uint8	m_uFlag;
	};

}
