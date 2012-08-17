#include "stdafx.h"
#include "CCtrlBase.h"
#include "CEditObserver.h"
#include "CEditContext.h"
#include "CEventDelegate.h"
#include "CCtrlBaseObserver.h"
#include "PieceGroupEditCtrl\CPieceGroupEditImp.h"
#include "BaseHelper.h"
#include "CEditModel.h"
#include "CEditPieceGroup.h"
#include "PathManager/CPathManager.h"
#include "ContextMsg.h"
#include "CEditorConfig.h"

DEF_UI_IMP(CPieceGroupEditImp);

#define MODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 

CPieceGroupEditImp::CPieceGroupEditImp(void* param)
: m_pModel(NULL)
, m_pLastEditPieceGroup(NULL)
{

}

CPieceGroupEditImp::~CPieceGroupEditImp(void)
{

}

void sqr_tools::CPieceGroupEditImp::SetContext( CEditContext* pContext )
{
	if(pContext == m_pOwnContext)
		return;

	m_pOwnContext = pContext;
	CPkgFile::EnableAsyncRead(false);
	//SetModel(pModel);
}

void sqr_tools::CPieceGroupEditImp::SetModel( CEditModel* pModel )
{
	m_pModel = pModel;
}

void CPieceGroupEditImp::AddPieceGroup( std::string name, STRINGVECTOR& pieceGroupNames )
{
	std::vector<std::string>::iterator itr = m_vtModelList.begin();
	while (itr != m_vtModelList.end())
	{
		if ((*itr).compare(name) == 0)
		{
			return;
		}
		itr++;
	}
	m_vtModelList.push_back(name);

	STRINGVECTOR strVt;
	m_mpNodes.insert(NODESMAP::value_type(name, strVt));
	pieceGroupNames.push_back(name);

	CEditPieceGroup* pPieceGroup = NULL;
	pPieceGroup = new CEditPieceGroup;
	pPieceGroup->AddRef();
	m_PieceGroupList.push_back(pPieceGroup);

	if (name.rfind(".mod") != -1)
	{
		std::string scenePath;
		CPathSetImp::GetInst()->GetResFileName(scenePath, name);
		if( FAILED(pPieceGroup->ReLoad( name ) ) )
		{
			m_PieceGroupList.remove(pPieceGroup);
			delete pPieceGroup;
			pPieceGroup = NULL;
			return;
		}
	}
	else
	{
		MessageBox(NULL, "请选择.mod的模型文件，请仔细检查" ,"警告",MB_OK );
		return;
	}
	
	for( int i = 0; i< (int)pPieceGroup->GetPieceClassNum(); i++ )
	{

		CPieceClass& Pc = *pPieceGroup->GetPieceClass( i );

		for( int j = 0;j < (int)Pc.size(); ++j )
		{
			m_mpNodes[name].push_back(Pc[j]->GetPieceName());
			pieceGroupNames.push_back(Pc[j]->GetPieceName());
		}
	}

	Update();
}

void sqr_tools::CPieceGroupEditImp::SaveMesh(std::string name)
{
	bool result = MODEL->BinSave(name);
	if (result)
		MessageBox(NULL, "保存成功" ,"警告",MB_OK );
}

void sqr_tools::CPieceGroupEditImp::DeleteAllGroup( void )
{
	SelectChanged("");

	m_pLastEditPieceGroup = NULL;

	m_vtModelList.clear();
	m_mpNodes.clear();

	PieceGroupList::iterator itr = m_PieceGroupList.begin();
	while (itr != m_PieceGroupList.end())
	{
		CEditPieceGroup* pPieceGroup = (CEditPieceGroup*)*itr;
		SafeRelease(pPieceGroup);
		itr++;
	}
	m_PieceGroupList.clear();
}

CEditPieceGroup* CPieceGroupEditImp::GetSelPieceGroup( std::string name )
{
	PieceGroupList::iterator itr = m_PieceGroupList.begin();
	while (itr != m_PieceGroupList.end())
	{
		CEditPieceGroup* pPieceGroup = (CEditPieceGroup*)*itr;
		std::string strName = pPieceGroup->GetName();
		int index = strName.find_last_of("/") + 1;
		strName = strName.substr(index);
		if(0 == strName.compare(name.c_str()))
		{
			return pPieceGroup;
		}
		itr++;
	}

	return NULL;
}

void sqr_tools::CPieceGroupEditImp::DeletePieces()
{
	std::vector<std::string>::iterator itr = m_vtPieceNames.begin();
	while(itr != m_vtPieceNames.end())
	{
		MODEL->DeleteMeshPiece((*itr).c_str());
		itr++;
	}
	m_vtPieceNames.clear();
	MODEL->ClearMeshPiece();
}

void CPieceGroupEditImp::SelectChanged( std::string name )
{
	CEditPieceGroup* pieceGroup = GetSelPieceGroup(name);

	if (m_pModel)
	{
		SafeRelease(m_pModel);
	}

	m_pModel = m_pOwnContext->CreateEditModel(NULL);
	m_pOwnContext->SetActiveObj(m_pModel);
	//if (pieceGroup != m_pLastEditPieceGroup)
	{
		m_pLastEditPieceGroup = pieceGroup;
		//DeletePieces();
		if (pieceGroup)
		{
			for( unsigned int i = 0; i< pieceGroup->GetPieceClassNum(); i++ )
			{
				CPieceClass& Pc = *pieceGroup->GetPieceClass( i );
				for( unsigned int j = 0;j < Pc.size(); ++j )
				{
					m_vtPieceNames.push_back(Pc[j]->GetPieceName());
					MODEL->AddMeshPieceEx((CEditPiece*)Pc[j],0);
				}
			}
		}

		MSG_SYS_SELCHANGE msg(NULL);
		m_pOwnContext->EventCollect(msg);
	}
}

std::string sqr_tools::CPieceGroupEditImp::GetLastModelFile( void )
{
	std::string keyName = "AddModBt";
	std::string pathFileValue;
	TiXmlNode* pConfigXml = CEditorConfig::GetInst()->GetEditorConfig("FileDialogSetting");
	TiXmlElement* pElement = pConfigXml->ToElement();
	TiXmlNode* pNode = NULL;

	pNode = pConfigXml->FirstChild(keyName);
	if(pNode!=NULL)
	{ 
		//结点存在 获取属性值
		pElement=pNode->ToElement();
		pathFileValue = pElement->Attribute("Path");
	}
	else
	{
		std::string temp = CPathSetImp::GetInst()->GetPath(0);
		TiXmlElement newNode(keyName);
		if (!temp.empty())
			newNode.SetAttribute("Path", temp);
		else
			newNode.SetAttribute("Path", "c:");
		pConfigXml->InsertEndChild(newNode);
		CEditorConfig::GetInst()->SaveEditorConfig();
	}
	return pathFileValue;
}

void sqr_tools::CPieceGroupEditImp::SaveNewModelFile( std::string name )
{
	std::string keyName = "AddModBt";
	string pathFileValue;
	TiXmlNode* pConfigXml = CEditorConfig::GetInst()->GetEditorConfig("FileDialogSetting");
	TiXmlElement* pElement = pConfigXml->ToElement();
	TiXmlNode* pNode = NULL;
	pNode = pConfigXml->FirstChild(keyName);
	if(pNode)
	{
		pElement=pNode->ToElement();
		pElement->SetAttribute("Path",name);
		CEditorConfig::GetInst()->SaveEditorConfig();
	}
}