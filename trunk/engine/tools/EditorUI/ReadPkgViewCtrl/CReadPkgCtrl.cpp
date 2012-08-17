#include "StdAfx.h"
#include "CReadPkgCtrl.h"
#include "ReadPkgCtrl\ReadPkgCtrlImp.h"

void CReadPkgCtrl::InitListViewCtrl()
{
	this->fileInfosListView->Columns->Add("文件名",		270, HorizontalAlignment::Center);
	this->fileInfosListView->Columns->Add("起始位置",	100, HorizontalAlignment::Center);
	this->fileInfosListView->Columns->Add("文件长度",	100, HorizontalAlignment::Center);
	this->fileInfosListView->Columns->Add("特征码",		100, HorizontalAlignment::Center);

//  		ListViewItem^ item = gcnew ListViewItem("ffffffffffffffff");
//  		item->SubItems->Add("100");
//  		item->SubItems->Add("0");
//  		item->SubItems->Add("GF43");
//  		this->listView1->Items->Add(item);
}

System::Void CReadPkgCtrl::fileInfoButton_Click( System::Object^ sender, System::EventArgs^ e )
{
	string  strPkg = MagStringToUnmagString(this->searchFilePathTextBox->Text);
	if( !strPkg.empty() )
	{

	}
}

System::Void CReadPkgCtrl::readPkgButton_Click( System::Object^ sender, System::EventArgs^ e )
{
	openFileDialog->Title			 = "选择包文件";
	openFileDialog->Filter			 = "piece Files (*.pkg)|*.pkg";
	openFileDialog->Multiselect		 = true;
	openFileDialog->RestoreDirectory = true;
	openFileDialog->ShowDialog();

	vector<string> pkgs;
	array<String^>^ resultFiles = openFileDialog->FileNames;
	int nLen = resultFiles->Length;
	for ( int i = 0; i < nLen; ++i )
	{
		String^ clrPkg = resultFiles[i];
		string  strPkg = MagStringToUnmagString(clrPkg);
		pkgs.push_back(strPkg);
	}
}


char* CReadPkgCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CReadPkgCtrlImp);
}