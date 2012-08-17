#include "stdafx.h"
#include "CBaseUnit.h"
#include "StringHelper.h"
#include "CDataSerializer.h"
#include "tinyxml/tinyxml.h"

#include "MainForm.h"
#include "IApplication.h"
#include "CCoreViewImp.h"
#include "CtrlImpHelper.h"
#include "LogImp/CLogImp.h"
#include "PathManager/CPathManager.h"
#include "ContextCtrl/CContextCtrl.h"
#include "TSqrAllocator.inl"
#include "dthelp.h"


namespace GraphicTester
{
	typedef vector<CBaseUnit*> UNIT_POOL;
	class MainFormImp
	{
	public:
		MainFormImp()
		{
			max_count = 0;
		}

		~MainFormImp()
		{
			UNIT_POOL::iterator it,eit = unit_pool.end();
			for(it = unit_pool.begin(); it!=eit; ++it)
			{
				CBaseUnit::DestroyUnit(*it);
			}
		}

		ModelPool			model_pool;
		unsigned int		max_count;
		UNIT_POOL			unit_pool;
	};

	System::Void MainForm::ExitMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		this->Close();
	}

	System::Void MainForm::MainForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) 
	{
		delete m_pImp;
		IApplication::GetInst()->Close();
	}

	System::Void MainForm::MainForm_Load( System::Object^ sender, System::EventArgs^ e )
	{
		m_pImp = new MainFormImp;
		m_UnitCont = (int32)CBaseUnit::InitUnit();
	}

	System::Void MainForm::SetPathBrowser_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CPathSetDlg Dlg;
		Dlg.ShowDialog();
	}

	System::Void MainForm::CtrlTab_Validated( System::Object^ sender, System::EventArgs^ e )
	{
	
	}

	System::Void MainForm::CtrlTab_Enter( System::Object^ sender, System::EventArgs^ e )
	{
		TestUnitSelector->Items->Clear();
		for(int i = 0; i<m_UnitCont; ++i)
		{
			TestUnitSelector->Items->Insert(i,UnmagStringToMagString(CBaseUnit::GetUnitName(i)));
		}
		TestUnitSelector->Text = UnmagStringToMagString(CBaseUnit::GetUnitName(0));
	}

	CBaseUnit*		MainForm::CreateTestUnit(const char* name)
	{
		CContextCtrlImp* ContextImp = CtrlImpPtr<CContextCtrlImp>(TestMainContext->GetCoreCtrl());
		CBaseUnit* pUnit = CBaseUnit::CreateUnit(name , ContextImp->GetContext() );

		if(pUnit == NULL)
			return NULL;

		pUnit->SetModelPool(&m_pImp->model_pool);
		pUnit->SetTick(60);
		++m_pImp->max_count;
		m_pImp->unit_pool.push_back(pUnit);

		string UnitTile;
		format(UnitTile,"<%d>%s",m_pImp->max_count,pUnit->GetName().c_str());

		TestUnitList->Items->Add(UnmagStringToMagString(UnitTile.c_str()) ,true);
		return pUnit;
	}

	System::Void MainForm::AddNewTestUnit_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CreateTestUnit( MagStringToUnmagString( TestUnitSelector->Text ));		
	}

	System::Void MainForm::TestUnitList_ItemCheck( System::Object^ sender, System::Windows::Forms::ItemCheckEventArgs^ e )
	{
		if(e->NewValue == System::Windows::Forms::CheckState::Checked)
			m_pImp->unit_pool[e->Index]->SetTick();
		else
			m_pImp->unit_pool[e->Index]->SetTick(0);
	}

	System::Void MainForm::UnitDelete_Click( System::Object^ sender, System::EventArgs^ e )
	{
		int32 idx = TestUnitList->SelectedIndex;
		if(idx > -1 && idx < (int32)m_pImp->unit_pool.size())
		{	
			TestUnitList->Items->RemoveAt(idx);
			UNIT_POOL::iterator it = m_pImp->unit_pool.begin();
			advance(it, idx);
			CBaseUnit::DestroyUnit(*it);
			m_pImp->unit_pool.erase(it);	
		}
	}

	System::Void MainForm::UnitEdit_Click( System::Object^ sender, System::EventArgs^ e )
	{

	}

	System::Void MainForm::UnitEditTab_Enter( System::Object^ sender, System::EventArgs^ e )
	{
		
	}

	System::Void MainForm::UnitEditTab_Layout( System::Object^ sender, System::Windows::Forms::LayoutEventArgs^ e )
	{
		int32 idx = TestUnitList->SelectedIndex;
		if(idx > -1 && idx < (int32)m_pImp->unit_pool.size())
		{	
			CBaseUnit* pUnit = m_pImp->unit_pool[idx];
			TestUnitTitle->Text = TestUnitList->Items[idx]->ToString();
			AutoUnitEditor->SetPropContent(m_pImp->unit_pool[idx]);	
		}
	}

	void InsertUnitToDoc(TiXmlNode& XmlDoc,CBaseUnit* pUnit)
	{
		CXMLDataSerializer Serial;
		TiXmlElement tmpElement(pUnit->GetName());
		Serial.ToXml(pUnit,&tmpElement);
		XmlDoc.InsertEndChild(tmpElement);
	}

	System::Void MainForm::UnitSave_Click( System::Object^ sender, System::EventArgs^ e )
	{
		saveTestUnit->Filter = "测试用例|*.TSU";
		if( System::Windows::Forms::DialogResult::OK == saveTestUnit->ShowDialog() )
		{
			String^ filename = saveTestUnit->FileName;

			TiXmlDocument XmlDoc;
			int32 idx = TestUnitList->SelectedIndex;
			if(idx > -1 && idx < (int32)m_pImp->unit_pool.size())
				InsertUnitToDoc( XmlDoc,m_pImp->unit_pool[idx] );	
			XmlDoc.SaveFile(MagStringToUnmagString(filename));	
		}
	}

	System::Void MainForm::UnitLoad_Click( System::Object^ sender, System::EventArgs^ e )
	{
		openTestUnit->Filter = "测试用例|*.TSU";
		if( System::Windows::Forms::DialogResult::OK == openTestUnit->ShowDialog() )
		{
			String^ filename = openTestUnit->FileName;

			TiXmlDocument XmlDoc(MagStringToUnmagString(filename));
			XmlDoc.LoadFile();
			int32 idx = TestUnitList->SelectedIndex;
			if(idx > -1 && idx < (int32)m_pImp->unit_pool.size())
			{
				TiXmlElement* pUnitNode = XmlDoc.FirstChildElement( m_pImp->unit_pool[idx]->GetName() );
				if(pUnitNode)
				{
					CXMLDataSerializer Serial;
					Serial.FromXml(m_pImp->unit_pool[idx],pUnitNode);
				}
			}
		}
	}

	void MainForm::FillProjForXml(TiXmlDocument& XmlDoc)
	{
		CXMLDataSerializer Serial;
		XmlDoc.LoadFile();
		TiXmlNode* pElement = XmlDoc.RootElement()->FirstChild();
		while(pElement)
		{
			CBaseUnit* pUnit = CreateTestUnit(pElement->Value());
			Serial.FromXml(pUnit,pElement);
			pElement = pElement->NextSiblingElement();
		}

	}

	System::Void MainForm::LoadProj_Click( System::Object^ sender, System::EventArgs^ e )
	{
		openTestUnit->Filter = "测试工程|*.TSP";
		if( System::Windows::Forms::DialogResult::OK == openTestUnit->ShowDialog() )
		{
			TiXmlDocument XmlDoc(MagStringToUnmagString(openTestUnit->FileName));
			TestUnitClear_Click(sender,e);
			FillProjForXml(XmlDoc);
		}
	}

	System::Void MainForm::ImportTestProj_Click( System::Object^ sender, System::EventArgs^ e )
	{
		openTestUnit->Filter = "测试工程|*.TSP";
		if( System::Windows::Forms::DialogResult::OK == openTestUnit->ShowDialog() )
		{
			TiXmlDocument XmlDoc(MagStringToUnmagString(openTestUnit->FileName));
			FillProjForXml(XmlDoc);
		}
	}


	System::Void MainForm::SaveProj_Click( System::Object^ sender, System::EventArgs^ e )
	{
		saveTestUnit->Filter = "测试工程|*.TSP";
		if( System::Windows::Forms::DialogResult::OK == saveTestUnit->ShowDialog() )
		{
			String^ filename = saveTestUnit->FileName;

			TiXmlDocument XmlDoc;
			TiXmlElement tsp("测试工程");
			
			for(size_t i = 0; i< m_pImp->unit_pool.size(); ++i)
			{
				InsertUnitToDoc( tsp,m_pImp->unit_pool[i] );
			}
			XmlDoc.InsertEndChild(tsp);
			XmlDoc.SaveFile(MagStringToUnmagString(filename));	
		}
	}

	System::Void MainForm::TestUnitClear_Click( System::Object^ sender, System::EventArgs^ e )
	{
		UNIT_POOL::iterator it,eit = m_pImp->unit_pool.end();
		for(it = m_pImp->unit_pool.begin(); it!=eit; ++it)
		{
			CBaseUnit::DestroyUnit(*it);
		}
		TestUnitList->Items->Clear();
		 m_pImp->unit_pool.clear();
		 m_pImp->max_count = 0;
	}
}
