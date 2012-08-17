#include "StdAfx.h"
#include "CGridPanel.h"
#include "GridCtrl\CGridCtrlImp.h"

void CGridPanel::Update(void)
{
	CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
	if(!pImp)
		return ;
}

char* CGridPanel::GetImpTypeName(void)
{
	return GET_IMP_NAME(CGridCtrlImp);
}

System::Void CGridPanel::rbTerrain_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->rbTerrain->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetTerrainLevel(true);
	}
}
System::Void CGridPanel::rbWater_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->rbWater->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetWaterLevel(true);
	}
}
System::Void CGridPanel::checkWave_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkWave->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetWaterWave(true);
	}
}
System::Void CGridPanel::rbPhysicHeight_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
}
System::Void CGridPanel::rbLogicHeight_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
}
System::Void CGridPanel::rbLine_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->rbLine->Checked)
	{
		this->rbPull->Enabled=true;
		this->rbPush->Enabled=true;
		this->rbDirX->Enabled=true;
		this->rbDirZ->Enabled=true;
		this->rbDirBoth->Enabled=true;

		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetBrushLine(true);
	}
}
System::Void CGridPanel::rbGaosi_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->rbGaosi->Checked)
	{
		this->rbPull->Enabled=true;
		this->rbPush->Enabled=true;
		this->rbDirX->Enabled=false;
		this->rbDirZ->Enabled=false;
		this->rbDirBoth->Enabled=false;

		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetBrushGauss(true);
	}
}
System::Void CGridPanel::rbSmooth_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->rbSmooth->Checked)
	{
		this->rbPull->Enabled=false;
		this->rbPush->Enabled=false;
		this->rbDirX->Enabled=false;
		this->rbDirZ->Enabled=false;
		this->rbDirBoth->Enabled=false;

		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetBrushSlip(true);
	}
}
System::Void CGridPanel::rbAlign_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->rbAlign->Checked)
	{
		this->rbPull->Enabled=false;
		this->rbPush->Enabled=false;
		this->rbDirX->Enabled=false;
		this->rbDirZ->Enabled=false;
		this->rbDirBoth->Enabled=false;

		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->GetBrushAlign(true);
	}
}
System::Void CGridPanel::rbPull_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->rbPull->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetBrushIncrease(true);
	}
}
System::Void CGridPanel::rbPush_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->rbPush->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetBrushDecrease(true);
	}
}
System::Void CGridPanel::rbDirX_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->rbDirX->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetBrushDirX(true);
	}
}
System::Void CGridPanel::rbDirZ_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->rbDirZ->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetBrushDirZ(true);
	}
}
System::Void CGridPanel::rbDirBoth_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->rbDirBoth->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetBrushDirB(true);
	}
}
System::Void CGridPanel::btLogicHeight_Click(System::Object^  sender, System::EventArgs^  e)
{
}

System::Void CGridPanel::rbGaosiHead_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->rbGaosiHead->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetTemplateType(0);
	}
}

System::Void CGridPanel::rbVolumnHead_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->rbVolumnHead->Checked)
	{
		CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
		pImp->SetTemplateType(1);
	}
}

System::Void CGridPanel::rbLine_EnabledChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(!this->rbLine->Enabled)
	{
		this->rbDirX->Enabled=false;
		this->rbDirZ->Enabled=false;
		this->rbDirBoth->Enabled=false;
	}
	else
	{
		if(this->rbLine->Checked)
		{
			this->rbDirX->Enabled=true;
			this->rbDirZ->Enabled=true;
			this->rbDirBoth->Enabled=true;
		}
	}

}
System::Void CGridPanel::rbGaosi_EnabledChanged(System::Object^  sender, System::EventArgs^  e)
{
}

System::Void CGridPanel::trackBarBrush_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	float fHeightStrength = float(this->trackBarBrush->Value);
	CGridCtrlImp *pImp = (CGridCtrlImp*)m_pCtrl;
	pImp->SetBrushHeightStrength(fHeightStrength);
}