#include "StdAfx.h"
#include "CSkeletonPropCom.h"
#include "SkeletonEditCtrl/CSkeletonEditImp.h"
#include "CtrlImpHelper.h"
#include "CUIBase.h"
#include "dthelp.h"

#define SkeletonImp ((CSkeletonEditImp*)GetCoreCtrl())

sqr_tools::CSkeletonPropCom::CSkeletonPropCom()
{
}

sqr_tools::CSkeletonPropCom::~CSkeletonPropCom()
{

}

char* CSkeletonPropCom::GetImpTypeName(void)
{
	return GET_IMP_NAME(CSkeletonEditImp);
}

bool CSkeletonPropCom::显示骨骼::get()
{
	return m_bShowSkeleton;
}

void CSkeletonPropCom::显示骨骼::set(bool value)
{
	m_bShowSkeleton = value;
	if (SkeletonImp)
		SkeletonImp->SetShowSkeleton(value);
}

String^ CSkeletonPropCom::当前骨骼文件::get()
{
	return m_strSkeletonName;
}

void CSkeletonPropCom::当前骨骼文件::set(String^  value)
{
	if (!value)
		return;

	m_strSkeletonName = value;
}

String^ CSkeletonPropCom::当前选中骨骼::get()
{
	return m_strSeletedSkeletonName;
}

void CSkeletonPropCom::当前选中骨骼::set(String^  value)
{
	if (!value)
		return;

	m_strSeletedSkeletonName = value;
	std::string strCLR = MagStringToUnmagString(value);
	if (SkeletonImp)
		SkeletonImp->ChangeCurSelSkeBoneName(strCLR);
}

String^ CSkeletonPropCom::保存骨骼文件::get()
{
	return m_strSkeletonFileInfo;
}

void CSkeletonPropCom::保存骨骼文件::set(String^  value)
{
	if (!value)
		return;

	m_strSkeletonFileInfo = "请保存";
	if (SkeletonImp)
	{
		String^ msg = gcnew String(SkeletonImp->SaveSke().c_str());
		MessageBox::Show(msg, "提示", MessageBoxButtons::OK);
	}
}

int CSkeletonPropCom::调整骨骼大小::get()
{
	return m_iSkeletonSize;
}

void CSkeletonPropCom::调整骨骼大小::set(int  value)
{
	if (!value)
		return;
	
	m_iSkeletonSize = value;
	int delta = m_iSkeletonSize - m_iLastSkeletonSize;
	if (SkeletonImp)
		SkeletonImp->SetSkeBoneSize(delta>0?1:-1);
	m_iLastSkeletonSize = value;
}

System::Windows::Forms::ContainerControl^ CSkeletonPropCom::ControlParent::get()
{
	return m_pUserCtrl;
}

void CSkeletonPropCom::ControlParent::set(System::Windows::Forms::ContainerControl^ value)
{
	m_pUserCtrl = value;
}

System::Void CSkeletonPropCom::_InitCtrl( void )
{

}

void sqr_tools::CSkeletonPropCom::Update( void )
{
	if (!SkeletonImp || !m_pUserCtrl)
		return;

	std::string skeName = SkeletonImp->GetCurSkeName();
	std::string curSelectedSkeName = SkeletonImp->GetCurSelSkeBoneName();
	当前骨骼文件 = gcnew String(skeName.c_str());
	当前选中骨骼 = gcnew String(curSelectedSkeName.c_str());
	if (skeName.empty())
		m_iSkeletonSize = m_iLastSkeletonSize = 0;
	m_pUserCtrl->Refresh();
}

