#include "stdafx.h"
#include "CBindObjectServer.h"
#include "ExpHelper.h"
#include "TBindObject.inl"



//#define FORCE_BINDOBJ_AOI_ATTR_EQUAL

//第0层AOI读写的容器适配接口，这部分可以考虑怎么做得更灵活
float CBindObjectServer::GetRealSize()
{
	return this->GetObject()->GetRealSyncSize();
}

float CBindObjectServer::GetRealStealth()
{
	return this->GetObject()->GetRealSyncStealth();
}

uint32 CBindObjectServer::GetRealViewGroup()
{
	return this->GetObject()->GetRealSyncGroup();
}

void CBindObjectServer::SetRealSize(float fSize)
{
	this->GetObject()->SetRealSyncSize(fSize);
}

void CBindObjectServer::SetRealStealth(float fStealth)
{
	this->GetObject()->SetRealSyncStealth(fStealth);
}

void CBindObjectServer::SetRealViewGroup(uint32 uGroupID)
{
	this->GetObject()->SetRealSyncGroup(uGroupID);
}

float CBindObjectServer::GetVirtualSize()
{
	return m_fVirtualSize;
}

float CBindObjectServer::GetVirtualStealth()
{
	return m_fVirtualStealth;
}

uint32 CBindObjectServer::GetVirtualViewGroup()
{
	return m_uVirtualViewGroup;
}

CBindObjectServer* CBindObjectServer::GetBindObject(uint32 uObjectID)
{
	CCoreObjectServer* pObject = CCoreObjectServer::GetObjectServer(uObjectID);
	if(!pObject)
		return NULL;
	CCoreObjectDictator* pObjectDictator = class_cast<CCoreObjectDictator*>(pObject);
	return pObjectDictator ? pObjectDictator->m_pBindObj : NULL;
}

CBindObjectServer* CBindObjectServer::GetRootBindObject()
{
	return this->GetRootObject()->m_pBindObj;
}

//uint32 CBindObjectServer::GetGlobalID()
//{
//	return this->GetObject()->GetGlobalID();
//}








CBindObjectServer::CBindObjectServer(CCoreObjectDictator* pCoreObj)
:TBindObject<CCoreObjectDictator>(pCoreObj)
,m_fVirtualSize(0.0f)
,m_fVirtualStealth(0.0f)
,m_uVirtualViewGroup(0)
{

}

CBindObjectServer::~CBindObjectServer()
{

}

void CBindObjectServer::SetSize(float fSize, uint32 uDimension)
{
	if(uDimension == 0)
	{
		m_fVirtualSize = fSize;
		if(IsBindingOrBoundObj())
		{
			float fCurSize = GetRealSize();
#ifdef FORCE_BINDOBJ_AOI_ATTR_EQUAL
			CBindObjectServer* pRoot = this->GetRootBindObject();
			if(fSize < fCurSize)
			{
				EnumAllChildObj(pRoot, CBindObjectServer::GetMaxVirtualSize, &fSize);
			}
			EnumAllChildObj(pRoot, CBindObjectServer::SetBindingSize, &fSize);
#else
			if(fSize < fCurSize)
			{
				EnumAllChildObj(this, CBindObjectServer::GetMaxVirtualSize, &fSize);
				//cout << "绑定态设置[" << this->GetSelfID() << "]0层Size = " << fSize << endl;
				SetRealSize(fSize);

				CBindObjectServer* pParent = GetBindObject(m_uBindParentID);
				if(pParent && fCurSize >= pParent->GetRealSize())
					EnumParentResumeSize(pParent);
			}
			else if(fSize > fCurSize)
			{
				//cout << "绑定态设置[" << this->GetSelfID() << "]0层Size = " << fSize << endl;
				SetRealSize(fSize);
				EnumParentObj(this, CBindObjectServer::LargenSetBindingSize, &fSize);
			}
#endif
			return;
		}
	}
	//因为能设置0层Size的CCoreObjectDictator函数跟能设置0层以外Size的CCoreObjServer函数正好分离，所以以下不需调用，否则需要考虑写适配
	//m_pAoiObj->SetSize(fSize, uDimension);
}

//float CBindObjectServer::GetSize(uint32 uDimension)const
//{
//	if(uDimension == 0 && IsBindingOrBoundObj())
//		return m_fVirtualSize;
//	return m_pAoiObj->GetSize(uDimension) / eGridSpan;
//}


void CBindObjectServer::SetStealth(float fStealth, uint32 uDimension)
{
	//cout<<"SetStealth "<<fStealth<<endl;
	if(uDimension == 0)
	{
		m_fVirtualStealth = fStealth;
		if(IsBindingOrBoundObj())
		{
			float fCurStealth = GetRealStealth();
#ifdef FORCE_BINDOBJ_AOI_ATTR_EQUAL
			CBindObjectServer* pRoot = this->GetRootBindObject();
			if(fStealth > fCurStealth)
			{
				EnumAllChildObj(pRoot, CBindObjectServer::GetMinVirtualStealth, &fStealth);
			}
			EnumAllChildObj(pRoot, CBindObjectServer::SetBindingStealth, &fStealth);
#else
			if(fStealth > fCurStealth)
			{
				EnumAllChildObj(this, CBindObjectServer::GetMinVirtualStealth, &fStealth);
				//cout << "绑定态设置[" << this->GetSelfID() << "]0层Stealth = " << fStealth << endl;
				SetRealStealth(fStealth);

				CBindObjectServer* pParent = GetBindObject(m_uBindParentID);
				if(pParent && fCurStealth <= pParent->GetRealStealth())
					EnumParentResumeStealth(pParent);
			}
			else if(fStealth < fCurStealth)
			{
				//cout << "绑定态设置[" << this->GetSelfID() << "]0层Stealth = " << fStealth << endl;
				SetRealStealth(fStealth);
				EnumParentObj(this, CBindObjectServer::ShortenSetBindingStealth, &fStealth);
			}
#endif
			return;
		}
	}
	//因为能设置0层Stealth的CCoreObjectDictator函数跟能设置0层以外Stealth的CCoreObjServer函数正好分离，所以以下不需调用，否则需要考虑写适配
	//m_pAoiObj->SetStealth(fStealth, uDimension);
}

//float CBindObjectServer::GetStealth(uint32 uDimension)const
//{
//	if(uDimension == 0 && IsBindingOrBoundObj())
//		return m_fVirtualStealth;
//	return m_pAoiObj->GetStealth(uDimension) / eGridSpan;
//}

//uint32 CBindObjectServer::GetViewGroup(uint32 uDimension)const
//{
//	if(uDimension == 0 && IsBindingOrBoundObj())
//		return m_uVirtualViewGroup;
//	return m_pAoiObj->GetViewGroup(uDimension);
//}

void CBindObjectServer::SetViewGroup(uint32 uViewGroup, uint32 uDimension)
{
	if(uDimension == 0)
	{
		m_uVirtualViewGroup = uViewGroup;
		if(IsBindingOrBoundObj())		//这里改成IsBoundObj()能稍微减少判断的次数，但容易造成相关逻辑的混乱
		{
			CBindObjectServer* pRoot = this->GetRootBindObject();
			if(pRoot == this)
			{
				if(uViewGroup != GetRealViewGroup())
				{
					EnumAllChildObj(pRoot, CBindObjectServer::SetBindingViewGroup, &uViewGroup);
				}
			}
			return;
		}
		//else设置无效
	}
	//因为能设置0层ViewGroup的CCoreObjectDictator函数跟能设置0层以外ViewGroup的CCoreObjServer函数正好分离，所以以下不需调用，否则需要考虑写适配
	//m_pAoiObj->SetViewGroup(uViewGroup, uDimension);
}

bool CBindObjectServer::SetBindingSize(CBindObjectServer* pThis, void* pValue)
{
	//cout << "绑定态设置[" << pThis->GetSelfID() << "]0层Size = " << (*(float*)pValue) << endl;
	pThis->SetRealSize(*(float*)pValue);
	return true;
}

bool CBindObjectServer::SetBindingStealth(CBindObjectServer* pThis, void* pValue)
{
	//cout << "绑定态设置[" << pThis->GetSelfID() << "]0层Stealth = " << (*(float*)pValue) << endl;
	pThis->SetRealStealth(*(float*)pValue);
	return true;
}

bool CBindObjectServer::SetBindingViewGroup(CBindObjectServer* pThis, void* pValue)
{
	//cout << "绑定态设置[" << pThis->GetSelfID() << "]0层ViewGroup = " << (*(float*)pValue) << endl;
	pThis->SetRealViewGroup(*(uint32*)pValue);
	return true;
}

bool CBindObjectServer::LargenSetBindingSize(CBindObjectServer* pThis, void* pValue)
{
	if(*(float*)pValue > pThis->GetRealSize())
	{
		//cout << "绑定态设置[" << pThis->GetSelfID() << "]0层Size = " << (*(float*)pValue) << endl;
		pThis->SetRealSize(*(float*)pValue);
		return true;
	}
	return false;
}

bool CBindObjectServer::ShortenSetBindingStealth(CBindObjectServer* pThis, void* pValue)
{
	if(*(float*)pValue < pThis->GetRealStealth())
	{
		//cout << "绑定态设置[" << pThis->GetSelfID() << "]0层Stealth = " << (*(float*)pValue) << endl;
		pThis->SetRealStealth(*(float*)pValue);
		return true;
	}
	return false;
}

bool CBindObjectServer::GetMaxVirtualSize(CBindObjectServer* pThis, void* pValue)
{
	float fCurSize = pThis->m_fVirtualSize;
	float fLastSize = *(float*)pValue;
	if(fCurSize > fLastSize)
	{
		*(float*)pValue = fCurSize;
	}
	return true;
}

bool CBindObjectServer::GetMinVirtualStealth(CBindObjectServer* pThis, void* pValue)
{
	float fCurStealth = pThis->m_fVirtualStealth;
	float fLastStealth = *(float*)pValue;
	if(fCurStealth < fLastStealth)
	{
		*(float*)pValue = fCurStealth;
	}
	return true;
}




void CBindObjectServer::TestEnumSetAllChildSize()
{
	CBindObjectServer* pRoot = this;
	uint32 uViewGroup = 0;
	EnumAllChildObj(pRoot, CBindObjectServer::GetMaxVirtualSize, &uViewGroup);
}







void CBindObjectServer::OnBeforeBindTo(uint32 uParentID, bool bRet)
{
	if(bRet)
	{
		CBindObjectServer* pParent = GetBindObject(uParentID);
		Ast(pParent);		//OnBeforeBindTo的bRet = true时，pParent一定存在，否则就是异常
#ifdef FORCE_BINDOBJ_AOI_ATTR_EQUAL
		CBindObjectServer* pParentRoot = pParent->GetRootBindObject();
		CBindObjectServer* pCurRoot = this;	//OnBeforeBindTo的bRet = true时，pCurRoot一定是this，否则就是异常
		Ast(this == this->GetRootBindObject());

		float fParentTreeMaxSize = pParent->GetRealSize();
		float fCurTreeMaxSize = GetRealSize();
		if(fCurTreeMaxSize < fParentTreeMaxSize)
		{
			EnumAllChildObj(pCurRoot, CBindObjectServer::SetBindingSize, &fParentTreeMaxSize);
		}
		else if(fCurTreeMaxSize > fParentTreeMaxSize)
		{
			EnumAllChildObj(pParentRoot, CBindObjectServer::SetBindingSize, &fCurTreeMaxSize);
		}

		float fParentTreeMinStealth = pParent->GetRealStealth();
		float fCurTreeMinStealth = GetRealStealth();
		if(fCurTreeMinStealth > fParentTreeMinStealth)
		{
			EnumAllChildObj(pCurRoot, CBindObjectServer::SetBindingStealth, &fParentTreeMinStealth);
		}
		else if(fCurTreeMinStealth > fParentTreeMinStealth)
		{
			EnumAllChildObj(pParentRoot, CBindObjectServer::SetBindingStealth, &fCurTreeMinStealth);
		}

		uint32 uParentViewGroup = pParentRoot->GetRealViewGroup();
		EnumAllChildObj(pCurRoot, CBindObjectServer::SetBindingViewGroup, &uParentViewGroup);
#else
		float fParentSize = pParent->GetRealSize();
		float fCurSize = GetRealSize();
		if(fParentSize < fCurSize)
		{
			EnumParentObj(pParent, CBindObjectServer::LargenSetBindingSize, &fCurSize);
		}

		//float fParentStealth = pParent->GetRealStealth();
		float fCurStealth = GetRealStealth();
		if(fCurStealth > fCurStealth)
		{
			EnumParentObj(pParent, CBindObjectServer::ShortenSetBindingStealth, &fCurStealth);
		}

		uint32 fParentViewGroup = pParent->GetRealViewGroup();
		uint32 fCurViewGroup = GetRealViewGroup();
		if(fCurViewGroup != fParentViewGroup)
		{
			EnumAllChildObj(this, CBindObjectServer::SetBindingViewGroup, &fParentViewGroup);
		}
#endif
	}
}

void CBindObjectServer::OnDisbindChildSingle()
{
	Ast(this == this->GetRootBindObject());

#ifdef FORCE_BINDOBJ_AOI_ATTR_EQUAL
	float fCurTreeMaxSize = 0.0f;
	EnumAllChildObj(this, CBindObjectServer::GetMaxVirtualSize, &fCurTreeMaxSize);
	if(fCurTreeMaxSize != GetRealSize())
	{
		EnumAllChildObj(this, CBindObjectServer::SetBindingSize, &fCurTreeMaxSize);
	}

	float fCurTreeMinStealth = m_fVirtualStealth;
	EnumAllChildObj(this, CBindObjectServer::GetMinVirtualStealth, &fCurTreeMinStealth);
	if(fCurTreeMinStealth != GetRealStealth())
	{
		EnumAllChildObj(this, CBindObjectServer::SetBindingStealth, &fCurTreeMinStealth);
	}
#endif

	if(m_uVirtualViewGroup != GetRealViewGroup())
	{
		EnumAllChildObj(this, CBindObjectServer::SetBindingViewGroup, &m_uVirtualViewGroup);
	}
}

void CBindObjectServer::OnDisbindParentSingle()
{
#ifdef FORCE_BINDOBJ_AOI_ATTR_EQUAL
	CBindObjectServer* pCurRoot = this->GetRootBindObject();

	float fCurTreeMaxSize = 0.0f;
	EnumAllChildObj(pCurRoot, CBindObjectServer::GetMaxVirtualSize, &fCurTreeMaxSize);
	if(fCurTreeMaxSize != GetRealSize())
	{
		EnumAllChildObj(pCurRoot, CBindObjectServer::SetBindingSize, &fCurTreeMaxSize);
	}

	float fCurTreeMinStealth = m_fVirtualStealth;
	EnumAllChildObj(pCurRoot, CBindObjectServer::GetMinVirtualStealth, &fCurTreeMinStealth);
	if(fCurTreeMinStealth != GetRealStealth())
	{
		EnumAllChildObj(pCurRoot, CBindObjectServer::SetBindingStealth, &fCurTreeMinStealth);
	}
#else
	EnumParentResumeSize(this);
	EnumParentResumeStealth(this);
#endif
}

void CBindObjectServer::EnumAllChildObj(CBindObjectServer* pRoot, ENUM_OP_FUNC pFunc, void* pValue)
{
	(*pFunc)(pRoot, pValue);
	SetUInt32& setBindChildID = pRoot->m_setBindChildID;
	for(SetUInt32::iterator itr = setBindChildID.begin(); itr != setBindChildID.end(); ++itr)
	{
		CBindObjectServer* pChild = GetBindObject(*itr);
		if(pChild)
			EnumAllChildObj(pChild, pFunc, pValue);
	}
}

void CBindObjectServer::EnumParentObj(CBindObjectServer* pRoot, ENUM_OP_FUNC pFunc, void* pValue)
{
	if((*pFunc)(pRoot, pValue) && pRoot->m_uBindParentID)
	{
		CBindObjectServer* pParent = GetBindObject(pRoot->m_uBindParentID);
		if(pParent)
			EnumParentObj(pParent, pFunc, pValue);
	}
}

void CBindObjectServer::EnumParentResumeSize(CBindObjectServer* pCur)
{
	for(;;)
	{
		float fCurTreeMaxSize = 0.0f;
		EnumAllChildObj(pCur, CBindObjectServer::GetMaxVirtualSize, &fCurTreeMaxSize);
		float pCurSize = pCur->GetRealSize();
		if(fCurTreeMaxSize < pCurSize)
		{
			//cout << "绑定态设置[" << pCur->GetSelfID() << "]0层Size = " << fCurTreeMaxSize << endl;
			pCur->SetRealSize(fCurTreeMaxSize);
			CBindObjectServer* pParent = GetBindObject(m_uBindParentID);
			if(!pParent || pCurSize < pParent->GetRealSize())
				break;
			pCur = pParent;
		}
		else
			break;
	}
}

void CBindObjectServer::EnumParentResumeStealth(CBindObjectServer* pCur)
{
	for(;;)
	{
		float fCurTreeMinStealth = pCur->m_fVirtualStealth;
		EnumAllChildObj(pCur, CBindObjectServer::GetMinVirtualStealth, &fCurTreeMinStealth);
		float pCurStealth = pCur->GetRealStealth();
		if(fCurTreeMinStealth > pCurStealth)
		{
			//cout << "绑定态设置[" << pCur->GetSelfID() << "]0层Stealth = " << fCurTreeMinStealth << endl;
			pCur->SetRealStealth(fCurTreeMinStealth);
			CBindObjectServer* pParent = GetBindObject(m_uBindParentID);
			if(!pParent || pCurStealth > pParent->GetRealStealth())
				break;
			pCur = pParent;
		}
		else
			break;
	}
}
