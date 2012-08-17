#include "StdAfx.h"
#include "CBoundingBoxPropCom.h"
#include "BoundingBoxEditCtrl/CBoundingBoxEditImp.h"
#include "CtrlImpHelper.h"
#include "CUIBase.h"

#define BoundingBoxImp ((CBoundingBoxEditImp*)GetCoreCtrl())

sqr_tools::CBoundingBoxPropCom::CBoundingBoxPropCom()
{
}

sqr_tools::CBoundingBoxPropCom::~CBoundingBoxPropCom()
{
}

char* CBoundingBoxPropCom::GetImpTypeName(void)
{
	return GET_IMP_NAME(CBoundingBoxEditImp);
}

float CBoundingBoxPropCom::xDelta::get()
{
	return m_fXDelta;
}

void CBoundingBoxPropCom::xDelta::set(float value)
{
	m_fXDelta = value;
	if (BoundingBoxImp)
		BoundingBoxImp->SetPieceBoxInfo(m_fXDelta, m_fYDelta, m_fZDelta, m_fXSize, m_fYSize, m_fZSize);
}

float CBoundingBoxPropCom::yDelta::get()
{
	return m_fYDelta;
}

void CBoundingBoxPropCom::yDelta::set(float value)
{
	m_fYDelta = value;
	if (BoundingBoxImp)
		BoundingBoxImp->SetPieceBoxInfo(m_fXDelta, m_fYDelta, m_fZDelta, m_fXSize, m_fYSize, m_fZSize);
}

float CBoundingBoxPropCom::zDelta::get()
{
	return m_fZDelta;
}

void CBoundingBoxPropCom::zDelta::set(float value)
{
	m_fZDelta = value;
	if (BoundingBoxImp)
		BoundingBoxImp->SetPieceBoxInfo(m_fXDelta, m_fYDelta, m_fZDelta, m_fXSize, m_fYSize, m_fZSize);
}

float CBoundingBoxPropCom::xSize::get()
{
	return m_fXSize;
}

void CBoundingBoxPropCom::xSize::set(float value)
{
	m_fXSize = value;
	if (BoundingBoxImp)
		BoundingBoxImp->SetPieceBoxInfo(m_fXDelta, m_fYDelta, m_fZDelta, m_fXSize, m_fYSize, m_fZSize);
}

float CBoundingBoxPropCom::ySize::get()
{
	return m_fYSize;
}

void CBoundingBoxPropCom::ySize::set(float value)
{
	m_fYSize = value;
	if (BoundingBoxImp)
		BoundingBoxImp->SetPieceBoxInfo(m_fXDelta, m_fYDelta, m_fZDelta, m_fXSize, m_fYSize, m_fZSize);
}

float CBoundingBoxPropCom::zSize::get()
{
	return m_fZSize;
}

void CBoundingBoxPropCom::zSize::set(float value)
{
	m_fZSize = value;
	if (BoundingBoxImp)
		BoundingBoxImp->SetPieceBoxInfo(m_fXDelta, m_fYDelta, m_fZDelta, m_fXSize, m_fYSize, m_fZSize);
}

bool CBoundingBoxPropCom::showBigBox::get()
{
	return m_bShowBigBox;
}

void CBoundingBoxPropCom::showBigBox::set(bool value)
{
	m_bShowBigBox = value;
	if (BoundingBoxImp)
		BoundingBoxImp->SetShowBigBox(value);
}

bool CBoundingBoxPropCom::showSmaBox::get()
{
	return m_bShowSmaBox;
}

void CBoundingBoxPropCom::showSmaBox::set(bool value)
{
	m_bShowSmaBox = value;
	if (BoundingBoxImp)
		BoundingBoxImp->SetShowSmallBox(value);
}

System::Windows::Forms::ContainerControl^ CBoundingBoxPropCom::ControlParent::get()
{
	return m_pUserCtrl;
}

void CBoundingBoxPropCom::ControlParent::set(System::Windows::Forms::ContainerControl^ value)
{
	m_pUserCtrl = value;
}

System::Void CBoundingBoxPropCom::_InitCtrl( void )
{

}

void sqr_tools::CBoundingBoxPropCom::Update( void )
{
	if (!BoundingBoxImp || !m_pUserCtrl)
		return;

	float _xDelta,_yDelta,_zDelta,_xSize,_ySize,_zSize;
	BoundingBoxImp->GetPieceBoxInfo(_xDelta, _yDelta, _zDelta, _xSize, _ySize, _zSize);
	xDelta = _xDelta;
	yDelta = _yDelta;
	zDelta = _zDelta;
	xSize = _xSize;
	ySize = _ySize;
	zSize = _zSize;
	m_pUserCtrl->Refresh();
}