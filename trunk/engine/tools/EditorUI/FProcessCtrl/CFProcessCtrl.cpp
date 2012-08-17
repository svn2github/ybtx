#include "StdAfx.h"
#include "FProcessCtrl\CFProcessCtrl.h"
#include "FileProcessCtrl\FileProcessCtrlImp.h"
#include "ProgressCtrl/CProgressCtrlImp.h"

void CFProcessCtrl::InitCtrlValue()
{
	CFileProcessCtrlImp *pImp = NULL;
	pImp = (CFileProcessCtrlImp*)m_pCtrl;
	if( pImp )
	{
		vector<string> pkgs;
		pImp->GetProcesserList(pkgs);
		if( pkgs.empty() )
			return;
		
		this->ALL_PROCESSER_ListBox->Items->Clear();
		size_t size = pkgs.size();
		for ( size_t m = 0; m < size; ++m )
		{
			string strPkg  = pkgs[m];
			String^ ClrPkg = gcnew String(strPkg.c_str());
			this->ALL_PROCESSER_ListBox->Items->Add(ClrPkg);
		}
	}
}

System::Void CFProcessCtrl::Add_Button_Click( System::Object^ sender, System::EventArgs^ e )
{
	int nSelectIndex = this->ALL_PROCESSER_ListBox->SelectedIndex;
	if( nSelectIndex == -1 )
		return;

	String^ str = this->ALL_PROCESSER_ListBox->Text;
	this->ALL_PROCESSER_ListBox->Items->RemoveAt(nSelectIndex);
	this->USE_PROCESSER_ListBox->Items->Add(str);
}

System::Void CFProcessCtrl::Remove_Button_Click( System::Object^ sender, System::EventArgs^ e )
{
	int nSelectIndex = this->USE_PROCESSER_ListBox->SelectedIndex;
	if( nSelectIndex == -1 )
		return;

	String^ str = this->USE_PROCESSER_ListBox->Text;
	this->USE_PROCESSER_ListBox->Items->RemoveAt(nSelectIndex);
	this->ALL_PROCESSER_ListBox->Items->Add(str);
}

System::Void CFProcessCtrl::AddAll_Button_Click( System::Object^ sender, System::EventArgs^ e )
{
	int nCount = this->ALL_PROCESSER_ListBox->Items->Count;
	for (int m = 0; m < nCount; ++m)
	{
		this->USE_PROCESSER_ListBox->Items->Add(this->ALL_PROCESSER_ListBox->Items[m]);
	}

	this->ALL_PROCESSER_ListBox->Items->Clear();
}

System::Void CFProcessCtrl::RemoveAll_Button_Click( System::Object^ sender, System::EventArgs^ e )
{
	int nCount = this->USE_PROCESSER_ListBox->Items->Count;
	for (int m = 0; m < nCount; ++m)
	{
		this->ALL_PROCESSER_ListBox->Items->Add(this->USE_PROCESSER_ListBox->Items[m]);
	}

	this->USE_PROCESSER_ListBox->Items->Clear();
}

System::Void CFProcessCtrl::Add_Path_Button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( this->folderBrowserDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		String^ folderName = this->folderBrowserDialog->SelectedPath;
		int nFind = this->Path_ListBox->FindString(folderName);
		if( nFind == -1 )
		{
			this->Path_ListBox->Items->Add(folderName);
		}
	}
}

System::Void CFProcessCtrl::Remove_Path_Button_Click( System::Object^ sender, System::EventArgs^ e )
{
	int nSelectIndex = this->Path_ListBox->SelectedIndex;
	if( nSelectIndex == -1 )
		return;

	this->Path_ListBox->Items->RemoveAt(nSelectIndex);
}

System::Void CFProcessCtrl::Process_Button_Click( System::Object^ sender, System::EventArgs^ e )
{
	vector<string> paths;
	int nCount = this->Path_ListBox->Items->Count;
	for ( int i = 0; i < nCount; ++i )
	{
		String^ clrPath = this->Path_ListBox->GetItemText(this->Path_ListBox->Items[i]);
		string  strPath = MagStringToUnmagString(clrPath);
		if( strPath.empty() )
			continue;

		paths.push_back(strPath);
	}

	if( paths.empty() )
		return;

	vector<string> pkgs;
	nCount = this->USE_PROCESSER_ListBox->Items->Count;
	for ( int i = 0; i < nCount; ++i )
	{
		String^ clrPkg = this->USE_PROCESSER_ListBox->GetItemText(this->USE_PROCESSER_ListBox->Items[i]);
		string  strPkg = MagStringToUnmagString(clrPkg);
		if( strPkg.empty() )
			continue;

		pkgs.push_back(strPkg);
	}

	if( pkgs.empty() )
		return;

	vector<vector<string>> msgInfos;
	msgInfos.resize(2);
	msgInfos[0] = paths;
	msgInfos[1] = pkgs;

	CFileProcessCtrlImp *pImp = NULL;
	pImp = (CFileProcessCtrlImp*)m_pCtrl;
	if( pImp )
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source = MGS_CUSTOM;
		type.custom_type	= MGT_FILES_PROCESS;
		info._iinfo			= 10;
		info._handle		= (void*)&msgInfos;
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

void CFProcessCtrl::Update(void)
{
	static bool bFirst = true;
	if( bFirst )
	{
		this->InitCtrlValue();
		bFirst = false;
	}

	CFileProcessCtrlImp *pImp = (CFileProcessCtrlImp*)m_pCtrl;
	if( pImp )
	{
		string  strText = pImp->GetCurTaskText();
		if( strText.empty() )
			return;

		String^ clrText = gcnew String(strText.c_str());
		this->groupBox3->Text = clrText;
		this->groupBox3->Refresh();
	}
}

System::Void CFProcessCtrl::CFProcessCtrl_Load( System::Object^ sender, System::EventArgs^ e )
{

	CFileProcessCtrlImp *pImp = NULL;
	pImp = (CFileProcessCtrlImp*)m_pCtrl;
	if( pImp )
	{
		CProgressCtrlImp* pTotalProgressBarImp = NULL;
		pTotalProgressBarImp = (CProgressCtrlImp*)(this->cTotal_ProcessCtrlEx->GetCoreCtrl());
		if( pTotalProgressBarImp )
		{
			string strTypeName = pTotalProgressBarImp->GetTypeName();
			pImp->SetTotalProgressImp(pTotalProgressBarImp);
		}

		CProgressCtrlImp* pCurProgressBarImp = NULL;
		pCurProgressBarImp = (CProgressCtrlImp*)(this->cCur_ProcessCtrlEx->GetCoreCtrl());
		if( pCurProgressBarImp )
		{
			string strTypeName = pCurProgressBarImp->GetTypeName();
			pImp->SetCurProgressImp(pCurProgressBarImp);
		}
	}
}

char* CFProcessCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CFileProcessCtrlImp);
}