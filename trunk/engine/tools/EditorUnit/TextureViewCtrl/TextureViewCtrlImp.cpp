#include "StdAfx.h"
#include "TextureViewCtrl/TextureViewCtrlImp.h"
#include "Brush/TerrainTextureBrush.h"
#include "CEditSet.h"
#include "StringHelper.h"
#include "CEditContext.h"

DEF_UI_IMP(CTextureViewCtrlImp);
CTextureViewCtrlImp::CTextureViewCtrlImp(void* param)
{
	this->Init();
}

CTextureViewCtrlImp::~CTextureViewCtrlImp( void )
{
	if( m_pOwnContext )
		m_pOwnContext->UnRegisterDelegate(m_pTerrainTextureBrush);

	SafeDelete(m_pTerrainTextureBrush);

	m_resSets.clear();
}

void CTextureViewCtrlImp::Init()
{
	m_pActiveResSet	= NULL;
	m_pActiveGroup	= NULL;
	m_pActive2DUnit	= NULL;

	m_pTerrainTextureBrush = new CTerrainTextureBrush;
}

void CTextureViewCtrlImp::SelectCurReset( const string& resetName )
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

void CTextureViewCtrlImp::SelectActiveUnit( const string& resetName, const string& strGroupName, const string& strUnitName, const bool bSelectUnit )
{
	ResSetPool::iterator iter = m_resSets.find(resetName);
	if( iter != m_resSets.end() )
	{
		m_pActiveResSet = iter->second;

		size_t size = m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CEditResSetGroup* group = m_pActiveResSet->m_2DResSet.m_resSetGroups[i];
			m_pActiveGroup = group;
			if( group->m_groupName == strGroupName )
			{
				if( !bSelectUnit )//如果不选择unti，则选择group
				{
					vector<string> texnames;
					size_t size = group->m_resSetUnits.size();
					for ( size_t m = 0; m < size; ++m )
					{
						string texname = group->m_resSetUnits[m]->GetNameByIndex(0);
						texnames.push_back(texname);
					}

					if( m_pTerrainTextureBrush )
						m_pTerrainTextureBrush->SetBrushTextureNames(texnames);

					return;
				}
			}

			if ( strGroupName == group->m_groupName )
			{
				size_t size = group->m_resSetUnits.size();
				for ( size_t m = 0; m < size; ++m )
				{
					string strName = group->m_resSetUnits[m]->GetItemName();
					if ( strName == strUnitName )
					{
						m_pActive2DUnit = (CEdit2DResSetUnit*)group->m_resSetUnits[m];
						vector<string> texnames;
						string texname = group->m_resSetUnits[m]->GetNameByIndex(0);
						texnames.push_back(texname);
						if( m_pTerrainTextureBrush )
							m_pTerrainTextureBrush->SetBrushTextureNames(texnames);

						return;
					}
				}
			}
		}
	}
}

void CTextureViewCtrlImp::MakePathToText( const string& Src, string& Dst )
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

void CTextureViewCtrlImp::UpdateTextureViewTree()
{
	CEditResSet* pEditResSet = (CEditResSet*)m_pSubject;
	if (pEditResSet)
	{
		m_pActiveResSet = pEditResSet;
		string strResetName = pEditResSet->m_resSetName;
		m_resSets.insert(make_pair(strResetName, pEditResSet));
	}

	CCtrlBase::Update();
}

bool CTextureViewCtrlImp::_Update( void )
{
	this->UpdateTextureViewTree();

	return true;
}

int CTextureViewCtrlImp::GetReset2DGroupCount()
{
	if( m_pActiveResSet )
	{
		return m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
	}

	return 0;
}

void CTextureViewCtrlImp::GetReset2DGroupNameByIndex( const size_t index, string&name )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
		if( index < size )
			name = m_pActiveResSet->m_2DResSet.m_resSetGroups[index]->m_groupName;
	}
}

int CTextureViewCtrlImp::GetReset2DGroupUnitCount( const size_t index )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
		if( index < size )
		{
			CEditResSetGroup *pEditGroup = m_pActiveResSet->m_2DResSet.m_resSetGroups[index];
			return pEditGroup->m_resSetUnits.size();
		}

		return 0;
	}

	return 0;
}

void CTextureViewCtrlImp::GetReset2DUnitNameByIndex( const size_t groupindex, const size_t unitindex, string&name )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
		if( groupindex < size )
		{
			CEditResSetGroup *pEditGroup = m_pActiveResSet->m_2DResSet.m_resSetGroups[groupindex];
			name = pEditGroup->m_resSetUnits[unitindex]->GetItemName();
		}
	}
}

void CTextureViewCtrlImp::GetResSetName( string& name )
{
	if( m_pActiveResSet )
		name = m_pActiveResSet->m_resSetName;
}

void CTextureViewCtrlImp::SetHotLayer(const int nHotLayer)
{
	if( m_pTerrainTextureBrush )
		m_pTerrainTextureBrush->SetHotLayer(nHotLayer);
}

void CTextureViewCtrlImp::SetContext( CEditContext* pContext )
{
	m_pOwnContext = pContext;
	if( m_pOwnContext )
	{
		m_pOwnContext->RegisterDelegate(m_pTerrainTextureBrush);
		m_pTerrainTextureBrush->SetBaseBrushType(BRUSH_CIRCLE);
		m_pTerrainTextureBrush->SetBrushUnit(1);
		//m_pTerrainTextureBrush->SetBrushIsShowGrid(true);
		m_pTerrainTextureBrush->InitTextureData();
	}
}

void CTextureViewCtrlImp::ActiveBrush()
{
	if( m_pTerrainTextureBrush )
		m_pTerrainTextureBrush->Active();
}

void CTextureViewCtrlImp::SetDirType( const int nType )
{
	if( m_pTerrainTextureBrush )
		m_pTerrainTextureBrush->SetDirType(nType);
}

void CTextureViewCtrlImp::SetWayType( const int nType )
{
	if( m_pTerrainTextureBrush )
		m_pTerrainTextureBrush->SetWayType(nType);
}

void CTextureViewCtrlImp::SetAlphaStrength(const float f)
{
	if( m_pTerrainTextureBrush )
		m_pTerrainTextureBrush->SetAlphaStrength(f);
}