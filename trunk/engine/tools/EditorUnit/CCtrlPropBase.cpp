#include "StdAfx.h"
#include "CCtrlPropBase.h"

namespace sqr_tools
{
	CCtrlPropBase::CCtrlPropBase()
		: m_pCtrlData(NULL)
	{

	}

	CCtrlPropBase::~CCtrlPropBase()
	{

	}

	void CCtrlPropBase::OnDataReFresh()
	{
		CCtrlBase::Update();
	}

	void CCtrlPropBase::SetEditCtrlData(void* ctrldata)
	{
		if (ctrldata)
			m_pCtrlData = static_cast<ECtrlData*>(ctrldata);
		else
			m_pCtrlData = NULL;
		CCtrlBase::Update();
	}

	void CCtrlPropBase::RegCtrlPropData(ECtrlDataType tp, void* propdata)
	{
		m_pCtrlData = ECtrlData::CreateCtrlData(tp, NULL, NULL,  NULL, propdata);
		CCtrlBase::Update();
	}

	void* CCtrlPropBase::GetEditData()
	{
		if (m_pCtrlData)
			return m_pCtrlData->m_pData;
		return NULL;
	}

	ECtrlDataType	CCtrlPropBase::GetDataType()
	{
		if (m_pCtrlData)
			return m_pCtrlData->m_eDataType;
		return E_TYPECOUNT;
	}

	const char*	CCtrlPropBase::GetDataHelpInfo()
	{
		if (m_pCtrlData)
		{
			if (m_pCtrlData->m_pHelpInfo)
				return (*static_cast<string*>(m_pCtrlData->m_pHelpInfo)).c_str();
		}
		return NULL;
	}

	void CCtrlPropBase::OnDataChanged()
	{
		if (m_pCtrlData)
		{
			if (m_pCtrlData->m_pFunc)
				((PFN_UPDATECALLBACK)(m_pCtrlData->m_pFunc))(m_pCtrlData->m_pObj);
			Update();
		}
	}

}



