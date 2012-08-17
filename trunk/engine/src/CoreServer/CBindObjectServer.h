#pragma once
#include "TBindObject.h"
#include "CCoreObjectDictator.h"

#ifdef GetObject
#undef GetObject
#endif

namespace sqr
{
	class CBindObjectServer
		:public TBindObject<CCoreObjectDictator>
	{
	public:
		CBindObjectServer(CCoreObjectDictator* pCoreObj);
		virtual ~CBindObjectServer();

		void SetSize(float fSize, uint32 uDimension = 0);
		void SetStealth(float fStealth, uint32 uDimension = 0);
		void SetViewGroup(uint32 uViewGroup, uint32 uDimension = 0);
		//float GetSize(uint32 uDimension = 0)const;
		//float GetStealth(uint32 uDimension = 0)const;
		//uint32 GetViewGroup(uint32 uDimension = 0)const;

		float GetVirtualSize();
		float GetVirtualStealth();
		uint32 GetVirtualViewGroup();


		void OnDisbindParentSingle();
		void OnDisbindChildSingle();
		void OnBeforeBindTo(uint32 uParentID, bool bRet);

	private:
		float GetRealSize();
		float GetRealStealth();
		uint32 GetRealViewGroup();
		void SetRealSize(float fSize);
		void SetRealStealth(float fSize);
		void SetRealViewGroup(uint32 uViewGroup);

		CBindObjectServer* GetBindObject(uint32 uObjectID);
		CBindObjectServer* GetRootBindObject();
		//uint32 GetGlobalID();

		typedef bool (*ENUM_OP_FUNC)(CBindObjectServer* pThis, void* pValue);
		void EnumAllChildObj(CBindObjectServer* pRoot, ENUM_OP_FUNC pFunc, void* pValue);
		void EnumParentObj(CBindObjectServer* pRoot, ENUM_OP_FUNC pFunc, void* pValue);
		void EnumParentResumeSize(CBindObjectServer* pCur);
		void EnumParentResumeStealth(CBindObjectServer* pCur);
		static bool SetBindingSize(CBindObjectServer* pThis, void* pValue);
		static bool SetBindingStealth(CBindObjectServer* pThis, void* pValue);
		static bool SetBindingViewGroup(CBindObjectServer* pThis, void* pValue);
		static bool LargenSetBindingSize(CBindObjectServer* pThis, void* pValue);
		static bool ShortenSetBindingStealth(CBindObjectServer* pThis, void* pValue);
		static bool GetMaxVirtualSize(CBindObjectServer* pThis, void* pValue);
		static bool GetMinVirtualStealth(CBindObjectServer* pThis, void* pValue);
		void TestEnumSetAllChildSize();

	protected:

	private:
		float	m_fVirtualSize;
		float	m_fVirtualStealth;
		uint32	m_uVirtualViewGroup;

	};
};

