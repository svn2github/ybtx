#include "StdAfx.h"
#include "SetViewCtrl/SetViewCtrlImp.h"
#include "Brush/TerrainModelBrush.h"
#include "CEditSet.h"
#include <fstream>
#include "CXmlConfig.inl"
#include "CEditContext.h"
#include "StringHelper.h"
#include "CGraphic.h"
#include "BaseHelper.h"

DEF_UI_IMP(CSetViewCtrlImp);
CSetViewCtrlImp::CSetViewCtrlImp(void* param)
{
	this->Init();
	m_pTerrainModelBrush = new CTerrainModelBrush;
}

CSetViewCtrlImp::~CSetViewCtrlImp( void )
{
	m_pOwnContext->UnRegisterDelegate(m_pTerrainModelBrush);
	SafeDelete(m_pTerrainModelBrush);

	ResSetPool::iterator end = m_resSets.end();
	for ( ResSetPool::iterator beg = m_resSets.begin(); beg != end; ++beg )
	{
		SafeDelete(beg->second);
	}

	m_resSets.clear();
}

void CSetViewCtrlImp::SetTextureViewCtrlObserver( CEditObserver* pObserver )
{
	m_pEditResSetObserver = pObserver;
}

void CSetViewCtrlImp::Init()
{
	m_pEditResSetObserver	= NULL;
	m_pActiveResSet			= NULL;
	m_pActiveModelGroup		= NULL;
	m_pActiveEditModelUnit	= NULL;
}

void CSetViewCtrlImp::SelectCurReset( const string& resetName )
{
	ResSetPool::iterator iter = m_resSets.find(resetName);
	if( iter == m_resSets.end() )
	{
		this->Init();
		MessageBox(0, "请在图素包列表中选择正确的图素包名", "提示", 0);
	}
	else
	{
		ResSetPool::iterator end = m_resSets.begin();
		m_pActiveResSet = iter->second;
	}
}

bool CSetViewCtrlImp::DeleteCurReset( const string& resetName )
{
	ResSetPool::iterator iter = m_resSets.find(resetName);
	if( iter == m_resSets.end() )
	{
		MessageBox(0, "请在图素包列表中选择需要删除的图素包名", "提示", 0);
		return false;
	}
	else
	{
		ResSetPool::iterator end = m_resSets.begin();
		SafeDelete(iter->second);
		m_resSets.erase(iter);
		
		this->Init();
	}

	return true;
}

void CSetViewCtrlImp::ImportReset( const vector<string> resetNames )
{
	size_t size = resetNames.size();
	for ( size_t m = 0; m < size; ++m )
	{
		string strResSetFileName = resetNames[m];

		this->LoadReset(strResSetFileName);
	}
}


bool CSetViewCtrlImp::ResetIsExit( const string& resetName )
{
	ResSetPool::iterator iter = m_resSets.find(resetName);
	if( iter == m_resSets.end() )
		return false;

	return true;
}

void CSetViewCtrlImp::LoadReset(const string& resetName)
{
	char dir[1024];
	::GetCurrentDirectory(1024, dir);

	string strResSetFileName = resetName;
	locale loc;
	use_facet< ctype<TCHAR> >( loc ).tolower
		( &strResSetFileName[0], &strResSetFileName[strResSetFileName.length()] );


	int i = strResSetFileName.find_last_of('.');
	int nPos = strResSetFileName.find("titleresourceset");
	CEditResSet * pResSet = new CEditResSet;

	pResSet->m_resSetName =	strResSetFileName.substr(nPos+17,i-nPos-17);
	if( this->ResetIsExit(pResSet->m_resSetName) )
	{
		ResSetPool::iterator iter = m_resSets.find(pResSet->m_resSetName);
		if( iter != m_resSets.end() )
		{
			delete iter->second;

			m_resSets.erase(iter);
		}
	}
	m_resSets.insert(make_pair(pResSet->m_resSetName, pResSet));

	//////////////////////////////////////////////////////////////////////////

	locale::global(locale(""));
	ifstream in(strResSetFileName.c_str());
	CXmlConfig *pTitleResourceSetFile = new CXmlConfig( "x", in );
	///x property
	TiXmlNode  *pResourceSetNode = pTitleResourceSetFile->GetRootNode();
	if( pResourceSetNode == NULL )
	{
		MessageBox(0, "请确定是否设置了工作路径", "提示", 0);
		return;
	}

	bool isAsyc = CPkgFile::IsEnableAsyncRead();
	if( isAsyc )
		CGraphic::GetInst()->SetAsynSwitch(false);

	int nTemp = 0;
	bool b = false;

	TiXmlNode *p3d = pResourceSetNode->FirstChild();
	size_t npos = 0;
	for (TiXmlNode * pXmlChild=p3d->FirstChild();pXmlChild; pXmlChild=pXmlChild->NextSibling())
	{
		if(!pXmlChild->ToElement())
			continue;
		
		CEditModelResSetGroup* pModelResSetGroup = new CEditModelResSetGroup;;

		TiXmlAttribute* pk=NULL;
		pk = pXmlChild->ToElement()->FirstAttribute();

		pModelResSetGroup->m_groupName = pk->ValueStr();

		pk=pk->Next();
		nTemp = atoi(pk->Value());
		pModelResSetGroup->m_ResGroupMark = nTemp;

		pk=pk->Next();
		if( pk )
		{
			nTemp = atoi(pk->Value());
			b = nTemp == 1 ? true : false;
			pModelResSetGroup->m_bVisibleByConfigue = b;
		}

		for (TiXmlNode * pj=pXmlChild->FirstChild(); pj; pj=pj->NextSibling() )
		{
			if (!pj->ToElement())
				continue;

			CEditResSetEditModelUnit* pEditModelUnit= new CEditResSetEditModelUnit;
			pModelResSetGroup->m_resModelSetUnits.push_back(pEditModelUnit);

			TiXmlAttribute* pk=pj->ToElement()->FirstAttribute();

			string modelname, itemname;
			int cnt3 = atoi(pk->Value());
			for ( int k = 0; k < cnt3; ++k )
			{
				pk=pk->Next();
				modelname = pk->ValueStr();

				pEditModelUnit->m_strModelNames.push_back(modelname);
			}

			if( cnt3 > 1 )
			{
				MakePathToText(modelname, itemname);
				itemname = itemname.substr(0, itemname.length() -1 );

				pEditModelUnit->m_itemName = itemname;
				pEditModelUnit->m_bSubUnit = true;
			}
			else
			{
				MakePathToText(modelname, itemname);

				pEditModelUnit->m_itemName = itemname;
				pEditModelUnit->m_bSubUnit = false;
			}

			pk=pk->Next();
			string agpname = pk->ValueStr();
			pEditModelUnit->m_strAgpName = agpname;

			string effectname("");
			pk=pk->Next();
			nTemp = atoi(pk->Value());
			b = nTemp == 1 ? true : false;
			if (b)
			{
				pk=pk->Next();
				effectname = pk->ValueStr();

				string effectFileName;
				pk=pk->Next();
				effectFileName = pk->ValueStr();

				pEditModelUnit->m_strEffectFileName = effectFileName;
				pEditModelUnit->m_strEffectName		= effectname;
			}
		}

		pResSet->m_3DModelResSet.m_resSetGroups.push_back(pModelResSetGroup);
	}

	//////////////////////////////////////////////////////////////////////////
	int nMaterialIndex = -1;

	string strGroupName = "", strTexName = "";
	TiXmlNode *p2D = pResourceSetNode->FirstChild("佣兵天下图素集2D");
	for (TiXmlNode* pNode = p2D->FirstChild(); pNode; pNode = pNode->NextSibling())
	{
		CEditResSetGroup * pGroup = new CEditResSetGroup;

		pGroup->m_groupName = pNode->ToElement()->Attribute("ResSetGroupName");
		strGroupName		= pGroup->m_groupName;

		for (TiXmlNode* pSubNode = pNode->FirstChild(); pSubNode; pSubNode=pSubNode->NextSibling())
		{
			if(!pSubNode->ToElement())
				continue;

			TiXmlAttribute* pk = pSubNode->ToElement()->FirstAttribute();

			CEdit2DResSetUnit *pResSetUnit  = new CEdit2DResSetUnit;

			string str = pk->Value();

			strTexName = str;
			pResSetUnit->AddName(str);
			string itemName;
			MakePathToText(strTexName, itemName);
			pResSetUnit->SetItemName(itemName);

			pGroup->m_resSetUnits.push_back(pResSetUnit);

			pk=pk->Next();
			if( pk )
			{
				str = pk->Value();
				nMaterialIndex = atoi(str.c_str());

				pResSetUnit->SetMaterialIndex(nMaterialIndex);
			}
		}

		pResSet->m_2DResSet.m_resSetGroups.push_back(pGroup);
	}

	m_pActiveResSet = pResSet;
	this->UpdateTextureViewCtrl();

	delete pTitleResourceSetFile;
	pTitleResourceSetFile = NULL;
	in.close();
}

void CSetViewCtrlImp::SelectActiveUnit( const string& resetName, const string& strGroupName, const string& strUnitName, const bool bSelectUnit )
{
	ResSetPool::iterator iter = m_resSets.find(resetName);
	if( iter != m_resSets.end() )
	{
		m_pActiveResSet = iter->second;

		size_t size = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CEditModelResSetGroup* pEditModelGroup = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[i];
			m_pActiveModelGroup = pEditModelGroup;
			if( !bSelectUnit )//如果不选择unti，则选择group
				return;

			if ( strGroupName == pEditModelGroup->m_groupName )
			{
				size_t size = pEditModelGroup->m_resModelSetUnits.size();
				for ( size_t m = 0; m < size; ++m )
				{
					string strName = pEditModelGroup->m_resModelSetUnits[m]->m_itemName;
					if ( strName == strUnitName )
					{
						m_pActiveEditModelUnit = pEditModelGroup->m_resModelSetUnits[m];
						

						bool bVisibleByConfigure = pEditModelGroup->m_bVisibleByConfigue;
						string aniname			 = m_pActiveEditModelUnit->m_strAgpName;
						string effectfilename	= m_pActiveEditModelUnit->m_strEffectFileName;
						string effectname		= m_pActiveEditModelUnit->m_strEffectName;
						m_pTerrainModelBrush->CreateActiveModel(m_pActiveEditModelUnit->m_strModelNames, aniname, effectfilename, effectname, bVisibleByConfigure);

						return;
					}
				}
			}
		}
	}
}

void CSetViewCtrlImp::MakePathToText( const string& Src, string& Dst )
{
	Dst = Src;
	size_t npos = Dst.rfind("\\");
	if( npos != -1 )
	{
		Dst = Dst.substr(npos+1, Dst.length());
		npos = Dst.find(".");
		if( npos != -1 )
			Dst = Dst.substr(0, npos);
	}
}

bool CSetViewCtrlImp::_Update( void )
{
	this->UpdateSetViewTree();
	
	return true;
}

void CSetViewCtrlImp::UpdateSetViewTree()
{
	CEditResSet* pEditResSet = (CEditResSet*)m_pSubject;
	if (pEditResSet)
	{
		string  strRessetFile = pEditResSet->m_resSetName;
		strRessetFile = m_strResetFilePath + strRessetFile + ".erp";
		
		this->LoadReset(strRessetFile);
	}

	CCtrlBase::Update();
}

void CSetViewCtrlImp::UpdateTextureViewCtrl()
{
	if (m_pEditResSetObserver)
	{
		m_pEditResSetObserver->SetSubject(m_pActiveResSet);
	}
}

int CSetViewCtrlImp::GetReset3DGroupCount()
{
	if( m_pActiveResSet )
	{
		return m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
	}

	return 0;
}

void CSetViewCtrlImp::GetReset3DGroupNameByIndex( const size_t index, string&name )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
		if( index < size )
			name = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[index]->m_groupName;
	}
}

int CSetViewCtrlImp::GetReset3DGroupUnitCount( const size_t index )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
		if( index < size )
		{
			CEditModelResSetGroup* pEditModelGroup = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[index];
			return pEditModelGroup->m_resModelSetUnits.size();
		}

		return 0;
	}

	return 0;
}

void CSetViewCtrlImp::GetReset3DUnitNameByIndex( const size_t groupindex, const size_t unitindex, string&name )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
		if( groupindex < size )
		{
			CEditModelResSetGroup* pEditModelGroup = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[groupindex];
			name = pEditModelGroup->m_resModelSetUnits[unitindex]->m_itemName;
		}
	}
}

void CSetViewCtrlImp::GetResSetName( string& name )
{
	if( m_pActiveResSet )
		name = m_pActiveResSet->m_resSetName;
}

void CSetViewCtrlImp::SetResSetResPath( const string& path )
{
	m_strResetFilePath = path;
}

void CSetViewCtrlImp::SetContext( CEditContext* pContext )
{
	m_pOwnContext = pContext;
	m_pOwnContext->RegisterDelegate(m_pTerrainModelBrush);
	m_pTerrainModelBrush->SetBaseBrushType(BRUSH_REGULAR);
	m_pTerrainModelBrush->SetBrushUnit(2);
	m_pTerrainModelBrush->ShowLockModeWireBox();
}

void CSetViewCtrlImp::ActiveBrush()
{
	if( m_pTerrainModelBrush )
		m_pTerrainModelBrush->Active();
}