#include "stdafx.h"
#include "TileViewCtrl/TileViewCtrlImp.h"
#include "MapEffectCtrl/MapEffectCtrlImp.h"
#include "CEditSet.h"
#include <fstream>
#include "CXmlConfig.inl"
#include "StringHelper.h"
#include "BaseHelper.h"
#include "CEditorConfig.h"
#include "PathManager/CPathManager.h"
#include "CEditContext.h"
#include "CPkgDirectory.h"

#define USE_OLD_RESOURCEDIR_NOTCODER
const wstring PATH_ALIAS_EFFECT = L"res";

bool modelless_second(CEditResSetEditModelUnit* m1, CEditResSetEditModelUnit* m2)
{
	return m1->m_itemName < m2->m_itemName;
}

bool texless_second(const CEditResSetUnit* m1, const CEditResSetUnit* m2)
{
	return ((CEdit2DResSetUnit*)m1)->GetNameByIndex(0) < ((CEdit2DResSetUnit*)m2)->GetNameByIndex(0);
}

void EditFindSubString(string& mainstr, const string& substr)
{
	string::iterator iter = search(mainstr.begin( ), mainstr.end( ), substr.begin( ), substr.end( ));

	int npos = iter - mainstr.begin();

	mainstr = mainstr.substr(npos + substr.length() + 1, mainstr.length());
}

DEF_UI_IMP(CTileViewCtrlImp);
CTileViewCtrlImp::CTileViewCtrlImp(void* param)
{
	this->Init();
}

CTileViewCtrlImp::~CTileViewCtrlImp( void )
{
	ResSetPool::iterator end = m_resSets.end();
	for ( ResSetPool::iterator beg = m_resSets.begin(); beg != end; ++beg )
	{
		SafeDelete(beg->second);
	}

	m_resSets.clear();
}

void CTileViewCtrlImp::SetSetViewCtrlObserver(CEditObserver* pObserver)
{
	m_pEditResSetObserver = pObserver;
}

void CTileViewCtrlImp::Init()
{
	m_pEditResSetObserver	= NULL;
	m_pActiveResSet			= NULL;
	m_pActiveGroup			= NULL;
	m_pActiveModelGroup		= NULL;
	m_pActiveEditModelUnit	= NULL;
	m_pActive2DUnit			= NULL;
	m_pMapEffectCtrlImp		= NULL;
	m_pImpContext			= NULL;

	m_strResSetPath = "Tools\\MapEditor\\TitleResourceSet\\";
	CEditorConfig::GetInst()->GetEditorConfigValue("MapEditorResSetting", "MapEditorRes", "Path", m_strResSetPath);
	int nCount = CPathSetImp::GetInst()->Count();
	string strEditorResDir = CPathSetImp::GetInst()->GetPath(0);
	size_t npos1 = strEditorResDir.rfind("\\");
	size_t npos2 = strEditorResDir.rfind("/");
	if( npos1 != -1 )
		strEditorResDir = strEditorResDir.substr(0, npos1+1);
	else
	{
		if( npos2 != -1 )
			strEditorResDir = strEditorResDir.substr(0, npos2+1);
	}

	m_strResSetPath = strEditorResDir + m_strResSetPath;
}

void CTileViewCtrlImp::LoadTerrainMaterialConfig(vector<string>& names)
{
	char dir[1024];
	::GetCurrentDirectory(1024,dir);
	string config("音效配置文件\\AudioMaterailPro.xml");

	const string old_locale = setlocale(LC_CTYPE, NULL);
	setlocale(LC_CTYPE, ".936");
	ifstream iXml(config.c_str());
	setlocale(LC_CTYPE, old_locale.c_str());

	if ( !iXml.good() )
	{
		MessageBox(0, "请检查是否有音效配置文件, 请检查或者全部更新", "提示", 0);
		return;
	}

	CXmlConfig *pAudioMaterailProFile = new CXmlConfig( "MaterialProterty", iXml );
	///x property
	TiXmlNode  *pAudioMaterailProNode = pAudioMaterailProFile->GetRootNode();
	if( pAudioMaterailProNode == NULL )
		return;

	TiXmlElement *XMLRoot	= pAudioMaterailProNode->ToElement();
	//开始读取文件
	if(XMLRoot)
	{
		TiXmlElement *pMaterialValueElement = NULL;
		pMaterialValueElement = XMLRoot->FirstChildElement("Material");
		if( pMaterialValueElement )
		{
			TiXmlAttribute *pAttribute =NULL;
			pAttribute = pMaterialValueElement->FirstAttribute();
			if(pAttribute)
			{
				string TempValue = "", strAttName = "";
				int  id = 0;

				strAttName = pAttribute->Name();
				TempValue  = pAttribute->Value();
				sscanf(TempValue.c_str(), "%d", &id);

				m_mapMaterialIndexs.insert(make_pair(strAttName, id));

				names.push_back(strAttName);

				pAttribute = pAttribute->Next();
				while ( pAttribute )
				{
					strAttName = pAttribute->Name();
					TempValue  = pAttribute->Value();
					sscanf(TempValue.c_str(), "%d", &id);

					m_mapMaterialIndexs.insert(make_pair(strAttName, id));
					names.push_back(strAttName);

					pAttribute = pAttribute->Next();
				}
			}

			TiXmlElement *pPhyleNameElement = NULL;
			pPhyleNameElement = pMaterialValueElement->NextSiblingElement("Phyle");
			if( pPhyleNameElement )
			{
				TiXmlAttribute *pAttribute =NULL;
				pAttribute = pPhyleNameElement->FirstAttribute();
				if(pAttribute)
				{
					string strPhyleName = pAttribute->Value();

					pAttribute = pAttribute->Next();
					if(pAttribute)
					{
						string strDefaultMatName = pAttribute->Value();
						//CSceneAudioMgr::GetInst()->SetPhyleName(strPhyleName, strDefaultMatName);
					}
				}
			}
		}

	}
	else
		MessageBox(0, "请地表材质音效配置文件是否正确, 请重现选择", "提示", 0);

	delete pAudioMaterailProFile;
	pAudioMaterailProFile = NULL;

	iXml.close();
}

bool CTileViewCtrlImp::CreateTileReset( const string& resetName )
{
	if ( resetName.empty() )
	{
		MessageBox(0, "新创建模型包名字不能为空", "提示", 0);
		return false;
	}

	ResSetPool::iterator iter = m_resSets.find(resetName);
	if( iter != m_resSets.end() )
	{
		MessageBox(0, "该图素包名已存在，请另取它名", "提示", 0);
		return false;
	}
	else
	{
		m_pActiveResSet = new CEditResSet;

		m_pActiveResSet->m_resSetName = resetName;
		m_resSets.insert(make_pair(resetName, m_pActiveResSet));
	}

	bool isAsyc = CPkgFile::IsEnableAsyncRead();
	if( isAsyc )
		CGraphic::GetInst()->SetAsynSwitch(false);

	return true;
}

void CTileViewCtrlImp::SelectCurReset( const string& resetName )
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

bool CTileViewCtrlImp::DeleteCurReset( const string& resetName )
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

void CTileViewCtrlImp::ImportReset( const vector<string> resetNames )
{
	size_t size = resetNames.size();
	for ( size_t m = 0; m < size; ++m )
	{
		string strResSetFileName = resetNames[m];

		this->LoadReset(strResSetFileName);
	}
}

void CTileViewCtrlImp::LoadReset(const string& resetName)
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
	bool isAsyc = CPkgFile::IsEnableAsyncRead();
	if( isAsyc )
		CGraphic::GetInst()->SetAsynSwitch(false);

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

	int nTemp = 0;
	bool b = false;
	string modelname("");

	TiXmlNode *p3d = pResourceSetNode->FirstChild();
	size_t npos = 0;
	for (TiXmlNode * pXmlChild=p3d->FirstChild();pXmlChild; pXmlChild=pXmlChild->NextSibling())
	{
		if(!pXmlChild->ToElement())
			continue;

		CEditModelResSetGroup*pModelResSetGroup = new CEditModelResSetGroup;

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

			CEditResSetEditModelUnit* pEditModelUnit = new CEditResSetEditModelUnit;
			pModelResSetGroup->m_resModelSetUnits.push_back(pEditModelUnit);
			CEditModel* pParentModel = m_pImpContext->CreateEditModel(NULL);
			pEditModelUnit->m_pEditModel = pParentModel;

			TiXmlAttribute* pk=pj->ToElement()->FirstAttribute();

			int cnt3 = atoi(pk->Value());
			for ( int k = 0; k < cnt3; ++k )
			{
				pk=pk->Next();
				modelname = pk->ValueStr();

				CEditModel* pChildModel = m_pImpContext->CreateEditModel(NULL);
				pChildModel->AddPieceGroup(modelname.c_str());
				pParentModel->AddChild(pChildModel, LT_CENTER, "");
			}

			string itemname;
			if( cnt3 > 1 )
			{
				this->MakePathToText(modelname, itemname);
				itemname = itemname.substr(0, itemname.length() -1 );

				pEditModelUnit->m_itemName = itemname;
				pEditModelUnit->m_bSubUnit = true;
			}
			else
			{
				this->MakePathToText(modelname, itemname);

				pEditModelUnit->m_itemName = itemname;
				pEditModelUnit->m_bSubUnit = false;
			}

			pk=pk->Next();
			string agpname = pk->ValueStr();
			pParentModel->AddAnimationGroup(agpname.c_str());

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

				pParentModel->AddEffect(effectFileName.c_str(), effectname.c_str(), CEffectPlayer::PLAY_LOOP, NULL);
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

	delete pTitleResourceSetFile;
	pTitleResourceSetFile = NULL;
	in.close();
}

void CTileViewCtrlImp::SaveReset(const string& resetName)
{
	char dir[1024];
	::GetCurrentDirectory(1024, dir);

	locale::global(locale(""));
	locale loc;

	string strRootName = "x";
	CXmlConfig *pResourceXmlCfg = new CXmlConfig(strRootName.c_str());
	string temp = "";

	//////////////////////////////////////////////////////////////////////////
	//3d model
	pResourceXmlCfg->AttribSet(strRootName.c_str(), "佣兵天下模型集3D");

	size_t ResSet3DGroupSize = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
	for ( size_t i = 0; i < ResSet3DGroupSize; ++i)
	{
		vector<string>vecAttribValueName, vecAttribValueValue;
		CEditModelResSetGroup* pModelGroup = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[i];

		temp = pModelGroup->m_groupName;
		vecAttribValueName.push_back("ResSetGroupName");
		vecAttribValueValue.push_back(temp);

		temp = format("%d", pModelGroup->m_ResGroupMark);
		vecAttribValueName.push_back("ResGroupMark");
		vecAttribValueValue.push_back(temp);

		temp = format("%d", pModelGroup->m_bVisibleByConfigue);
		vecAttribValueName.push_back("ResSetObjectVisibleMark");
		vecAttribValueValue.push_back(temp);

		pResourceXmlCfg->AttribSet("佣兵天下模型集3D", "ResSetGroup", vecAttribValueName, vecAttribValueValue);

		size_t unit3DSize = pModelGroup->m_resModelSetUnits.size();

		////开始排序
		sort(pModelGroup->m_resModelSetUnits.begin(), pModelGroup->m_resModelSetUnits.end(), modelless_second);
		//////////////////////////////////////////////////////////////////////////

		for ( size_t j = 0; j < unit3DSize; ++j)
		{
			vector<string>vecAttribValueName, vecAttribValueValue;

			CEditResSetEditModelUnit* pEditModelUnit= pModelGroup->m_resModelSetUnits[j];
			CEditModel*  pResSetModelUnit = pEditModelUnit->m_pEditModel;

			size_t size = pResSetModelUnit->GetChildCount();
			temp = format("%d", size);
			vecAttribValueName.push_back("ResSetUnitArpCount");
			vecAttribValueValue.push_back(temp);

			// arps
			for ( size_t k = 0; k < size; ++k )
			{
				string modelname = pResSetModelUnit->GetModelName(k);

				temp = format("Name%d", k);
				vecAttribValueName.push_back(temp);
				vecAttribValueValue.push_back(modelname);
			}

			// ani file name
			string aniname = pResSetModelUnit->GetAnimationGroupName();
			use_facet<ctype<TCHAR> >( loc ).tolower
				( &aniname[0], &aniname[aniname.length()] );

			replace(aniname.begin(), aniname.end(), '\\', '/');

			vecAttribValueName.push_back("AraFileName");
			vecAttribValueValue.push_back(aniname);

			// effect file name]
			string effectFileName;
			string effectname = pResSetModelUnit->GetEffectName(effectFileName);
			if ( !effectname.empty() )
			{
				replace(effectname.begin(), effectname.end(), '\\', '/');

				vecAttribValueName.push_back("EffectBound");
				vecAttribValueValue.push_back("1");

				vecAttribValueName.push_back("EffectName");
				vecAttribValueValue.push_back(effectname);

				
				m_pMapEffectCtrlImp->GetEffectFileName(effectname, effectFileName);

				vecAttribValueName.push_back("EffectFileName");
				vecAttribValueValue.push_back(effectFileName);
			}
			else
			{
				vecAttribValueName.push_back("EffectBound");
				vecAttribValueValue.push_back("0");
			}

			pResourceXmlCfg->AttribSet("佣兵天下模型集3D", "ResSetGroup", i, "ResSetUnit", vecAttribValueName, vecAttribValueValue);
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//2d texture
	pResourceXmlCfg->AttribSet(strRootName.c_str(), "佣兵天下图素集2D");

	size_t ResSet2DGroupSize = m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
	for ( size_t i = 0; i < ResSet2DGroupSize; ++i)
	{
		vector<string>vecAttribValueName, vecAttribValueValue;

		CEditResSetGroup *pResSetGroup = m_pActiveResSet->m_2DResSet.m_resSetGroups[i];
		temp = pResSetGroup->m_groupName;

		vecAttribValueName.push_back("ResSetGroupName");
		vecAttribValueValue.push_back(temp);
		vecAttribValueName.push_back("ResGroupMark");
		vecAttribValueValue.push_back("0");

		pResourceXmlCfg->AttribSet("佣兵天下图素集2D", "ResSetGroup", vecAttribValueName, vecAttribValueValue);

		////开始排序
		sort(pResSetGroup->m_resSetUnits.begin(), pResSetGroup->m_resSetUnits.end(), texless_second);

		int nMaterialIndex = 0;
		size_t uint2DSize = pResSetGroup->m_resSetUnits.size();
		for ( size_t j = 0; j < uint2DSize; j++)
		{
			vector<string>vecAttribValueName, vecAttribValueValue;
			CEdit2DResSetUnit *pResSetUnit = (CEdit2DResSetUnit*)(pResSetGroup->m_resSetUnits[j]);

			temp = pResSetUnit->GetNameByIndex(0);
			use_facet<ctype<TCHAR> >( loc ).tolower
				( &temp[0], &temp[temp.length()] );

			vecAttribValueName.push_back("TextureFileName");
			vecAttribValueValue.push_back(temp);

			nMaterialIndex = pResSetUnit->GetMaterialIndex();
			temp = format("%d", nMaterialIndex);
			vecAttribValueName.push_back("MaterialIndex");
			vecAttribValueValue.push_back(temp);

			pResourceXmlCfg->AttribSet("佣兵天下图素集2D", "ResSetGroup", i, "ResSetUnit", vecAttribValueName, vecAttribValueValue);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	pResourceXmlCfg->SaveConfig(resetName.c_str());
	delete pResourceXmlCfg;
	pResourceXmlCfg = NULL;

	MessageBox(0, "图素包保存完毕", "提示", 0);

	this->UpdateSetViewCtrl();
}

void CTileViewCtrlImp::SaveCurReset( const string& resetName )
{
	if( m_pActiveResSet == NULL )
	{
		MessageBox(0, "请在图素包列表中选择需要图素包名", "提示", 0);
		return;
	}

	if( m_pActiveResSet->m_3DModelResSet.m_resSetGroups.empty() && m_pActiveResSet->m_2DResSet.m_resSetGroups.empty() )
	{
		MessageBox(0, "当前图素包是空的，没必要保存", "提示", 0);
		return;
	}

	string titlname = m_pActiveResSet->m_resSetName;
	string arsstr = "";

	// #ifdef USE_OLD_RESOURCEDIR_NOTCODER
	// 	arsPath = "..\\Tools\\MapEditor\\TitleResourceSet\\";
	// #else
	// 	arsPath = "./src/ResourceSet/" ;
	// #endif

	if( !m_pActiveResSet->m_3DModelResSet.m_resSetGroups.empty() )
	{
		CEditModelResSetGroup * pResSetModelGroup = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[0];
		if( pResSetModelGroup->m_bVisibleByConfigue )
		{
			string path  = m_strResSetPath + "VisibleByConfigure";
			DWORD fileAttrs = GetFileAttributes(path.c_str());
			bool   bExit = ((INVALID_FILE_ATTRIBUTES != fileAttrs) && (0 != (FILE_ATTRIBUTE_DIRECTORY & fileAttrs)));
			if( !bExit )
				::CreateDirectory(path.c_str(), NULL);

			arsstr = path + "\\" + resetName + ".erp";
		}
		else
			arsstr = m_strResSetPath + resetName + ".erp";
	}
	else
		arsstr = m_strResSetPath + resetName + ".erp";

	//判断是否只读
	DWORD fileAttrs = GetFileAttributes(arsstr.c_str());
	bool bReadOnly = (INVALID_FILE_ATTRIBUTES != fileAttrs && (fileAttrs & FILE_ATTRIBUTE_READONLY));
	if (bReadOnly)
	{
		string log = resetName;

		log += "该文件已被他人使用";
		MessageBox(0, log.c_str(), "无法保存", 0);
		return ;
	}

	this->SaveReset(arsstr);
}

void CTileViewCtrlImp::PatchSaveCurReset( const vector<string> resetNames )
{

}

void CTileViewCtrlImp::CreateResSetGroup( const string& strGroupName, const bool bModel, const BYTE bResGroupMark, const bool bVisibleByConfigue )
{
	if( bModel )
	{
		CEdit3DDimSet::EditModelResSetPool::iterator beg = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.begin();
		CEdit3DDimSet::EditModelResSetPool::iterator end = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.end();

		for ( beg; beg != end; ++beg )
		{
			string strName = (*beg)->m_groupName;
			if( strName.compare(strGroupName) == 0 )
			{
				string log = strGroupName;
				log = log + "图素组名已存在，请用新的图素组名";
				MessageBox( 0, log.c_str(), "提示", 0 );
				return;
			}
		}

		CEditModelResSetGroup *pEditModelResSetGroup = new CEditModelResSetGroup;
		pEditModelResSetGroup->m_groupName = strGroupName;
		pEditModelResSetGroup->m_ResGroupMark = bResGroupMark;
		pEditModelResSetGroup->m_bVisibleByConfigue = bVisibleByConfigue;

		m_pActiveResSet->m_3DModelResSet.m_resSetGroups.push_back( pEditModelResSetGroup );
		m_pActiveModelGroup = pEditModelResSetGroup;
	}
	else
	{	
		vector<CEditResSetGroup*>::iterator beg;
		vector<CEditResSetGroup*>::iterator end;
		beg = m_pActiveResSet->m_2DResSet.m_resSetGroups.begin();
		end = m_pActiveResSet->m_2DResSet.m_resSetGroups.end();

		for ( beg; beg != end; ++beg )
		{
			string strName = (*beg)->m_groupName;
			if( strName.compare(strGroupName) == 0 )
			{
				string log = strGroupName;
				log = log + "图素组名已存在，请用新的图素组名";
				MessageBox( 0, log.c_str(), "提示", 0 );
				return;
			}
		}

		CEditResSetGroup *pGroup = new CEditResSetGroup;
		pGroup->m_groupName = strGroupName;

		m_pActiveResSet->m_2DResSet.m_resSetGroups.push_back( pGroup );
		m_pActiveGroup = pGroup;
	}

}

bool CTileViewCtrlImp::DeleteReSetGroup( const string& strGroupName, const bool b3D )
{
	if( m_pActiveResSet )
	{
		if( b3D )
		{
			CEdit3DDimSet DimSet = DimSet = m_pActiveResSet->m_3DModelResSet;
			size_t size = DimSet.m_resSetGroups.size();
			for ( size_t i = 0; i < size; ++i )
			{
				CEditModelResSetGroup* group = DimSet.m_resSetGroups[i];
				if ( strGroupName == group->m_groupName )
				{
					delete group;
					DimSet.m_resSetGroups.erase( DimSet.m_resSetGroups.begin() + i );
					return true;
				}
			}
		}
		else
		{
			CEditDimSet DimSet = m_pActiveResSet->m_2DResSet;

			size_t size = DimSet.m_resSetGroups.size();
			for ( size_t i = 0; i < size; ++i )
			{
				CEditResSetGroup* group = DimSet.m_resSetGroups[i];
				if ( strGroupName == group->m_groupName )
				{
					delete group;
					DimSet.m_resSetGroups.erase( DimSet.m_resSetGroups.begin() + i );
					return true;
				}
			}
		}
	}
	else
	{
		MessageBox( 0, "请选择具体图素包", "", 0 );
		return false;
	}

	return false;
}

bool CTileViewCtrlImp::ModelUnitIsExitInGroup( const string& strModelUnitName )
{
	//该模型是否已经存在
	CEditModel* pEditModelUnit = NULL;
	bool bWrite = true;
	string groupName;

	size_t group3DSize = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
	for ( size_t i = 0; i < group3DSize; ++i)
	{
		CEditModelResSetGroup* pResSetGroup = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[i];
		groupName = pResSetGroup->m_groupName;

		if( pResSetGroup->NameExist(strModelUnitName) )
		{
			bWrite = false;
			string log = strModelUnitName + "该物件已存在Group" + groupName + "中";
			MessageBox( 0, log.c_str(), "", 0 );
		}
	}

	return bWrite;
}

bool CTileViewCtrlImp::TexUnitIsExitInGroup(const string& strTexUnitName)
{
	bool bWrite = true;
	string groupName;

	size_t group3DSize = m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
	for ( size_t i = 0; i < group3DSize; ++i)
	{
		CEditResSetGroup* pResSetGroup = m_pActiveResSet->m_2DResSet.m_resSetGroups[i];
		groupName = pResSetGroup->m_groupName;

		if( pResSetGroup->NameExist(strTexUnitName) )
		{
			bWrite = false;
			string log = strTexUnitName + "该物件已存在" + groupName + "图素包中";
			MessageBox( 0, log.c_str(), "", 0 );
		}
	}

	return bWrite;
}

bool CTileViewCtrlImp::CreateModelUnit( const string& strGroupName, const string& strModelUnitName, string& strItemName )
{
	if( m_pActiveModelGroup == NULL )
	{
		MessageBox(0, "请选择需要添加图素的具体图素组", "错误提示", 0);
		return false;
	}

	string strName =  m_pActiveModelGroup->m_groupName;
	if( strName != strGroupName )
	{
		MessageBox(0, "请选择需要添加图素的具体图素组", "错误提示", 0);
		return false;
	}

	string modelname = strModelUnitName;

#ifdef USE_OLD_RESOURCEDIR_NOTCODER
	int nPos = modelname.find("\\res");
	if ( nPos == -1 )
	{
		MessageBox(0, "请选择res目录下的资源", "错误提示", 0);
		return false;
	}
	else
	{
		nPos = modelname.find("\\resbin");
		if ( nPos != -1 )
		{
			MessageBox(0, "当前路径为resbin，请选择res目录下的资源", "错误提示", 0);
			return false;
		}

		EditFindSubString(modelname, "res");
	}
#else
	int nPos = modelname.find("\\ressrc");
	if ( nPos == -1 )
	{
		MessageBox(0, "请选择ressrc目录下的资源", "错误提示", 0);
		return false;
	}
	else
	{
		nPos = modelname.find("ressrc\\tgt");
		if ( nPos != -1 )
		{
			EditFindSubString(modelname, "ressrc\\tgt");
		}
		else
			EditFindSubString(modelname, "ressrc\\src");
	}
#endif

	nPos = modelname.rfind(".");
	if ( nPos == -1 )
		modelname += ".mod";

	if( !this->ModelUnitIsExitInGroup(strModelUnitName) )
		return false;

	//////////////////////////////////////////////////////////////////////////
	CEditResSetEditModelUnit* pEditModelUnit= new CEditResSetEditModelUnit;
	m_pActiveModelGroup->m_resModelSetUnits.push_back(pEditModelUnit);

	CEditModel* pParentModel = m_pImpContext->CreateEditModel(NULL);
	pEditModelUnit->m_pEditModel = pParentModel;
	this->MakePathToText(modelname, strItemName);
	pEditModelUnit->m_itemName = strItemName;

	CEditModel* pChildModel = m_pImpContext->CreateEditModel(NULL);
	pChildModel->AddPieceGroup(modelname.c_str());
	pParentModel->AddChild(pChildModel, LT_CENTER, "");

	m_pActiveEditModelUnit = pEditModelUnit;

	return true;
}

bool CTileViewCtrlImp::CreateModelUnits( const string& strGroupName, const vector<string>& vecModelUnitNames, string& strItemName )
{
	if( m_pActiveModelGroup == NULL )
	{
		MessageBox(0, "请选择需要添加图素的具体图素组", "错误提示", 0);
		return false;
	}

	string strName =  m_pActiveModelGroup->m_groupName;
	if( strName != strGroupName)
	{
		MessageBox(0, "请选择需要添加图素的具体图素组", "错误提示", 0);
		return false;
	}

	vector<string> modelnamelist;
	size_t size = vecModelUnitNames.size();
	for ( size_t m = 0; m < size; ++m )
	{
		string modelname = vecModelUnitNames[m];

#ifdef USE_OLD_RESOURCEDIR_NOTCODER
		int nPos = modelname.find("\\res");
		if ( nPos == -1 )
		{
			MessageBox(0, "请选择res目录下的资源", "错误提示", 0);
			return false;
		}
		else
		{
			nPos = modelname.find("\\resbin");
			if ( nPos != -1 )
			{
				MessageBox(0, "当前路径为resbin，请选择res目录下的资源", "错误提示", 0);
				return false;
			}

			EditFindSubString(modelname, "res");
		}
#else
		int nPos = modelname.find("\\ressrc");
		if ( nPos == -1 )
		{
			MessageBox(0, "请选择ressrc目录下的资源", "错误提示", 0);
			return false;
		}
		else
		{
			nPos = modelname.find("ressrc\\tgt");
			if ( nPos != -1 )
			{
				EditFindSubString(modelname, "ressrc\\tgt");
			}
			else
				EditFindSubString(modelname, "ressrc\\src");
		}
#endif

		nPos = modelname.rfind(".");
		if ( nPos == -1 )
			modelname += ".mod";

		modelnamelist.push_back(modelname);
	}

	if( !this->ModelUnitIsExitInGroup(modelnamelist[0]) )
		return NULL;

	CEditResSetEditModelUnit* pEditModelUnit= new CEditResSetEditModelUnit;
	m_pActiveModelGroup->m_resModelSetUnits.push_back(pEditModelUnit);
	
	CEditModel* pParentModel = m_pImpContext->CreateEditModel(NULL);
	pEditModelUnit->m_pEditModel = pParentModel;
	size = modelnamelist.size();
	this->MakePathToText(modelnamelist[size-1], strItemName);
	strItemName = strItemName.substr(0, strItemName.length() -1 );

	pEditModelUnit->m_itemName = strItemName;
	pEditModelUnit->m_bSubUnit = true;
	
	for ( size_t m = 0; m < size; ++m )
	{
		CEditModel* pChildModel = m_pImpContext->CreateEditModel(NULL);
		pChildModel->AddPieceGroup(modelnamelist[m].c_str());
		pParentModel->AddChild(pChildModel, LT_CENTER, "");
	}

	m_pActiveEditModelUnit = pEditModelUnit;

	return true;
}

bool CTileViewCtrlImp::CreateTextureUnit( const string& strGroupName, const string& strTexUnitName, string& strItemName )
{
	if( m_pActiveGroup == NULL )
	{
		MessageBox(0, "请选择需要添加图素的具体图素组", "错误提示", 0);
		return false;
	}

	string strName = m_pActiveGroup->m_groupName;
	if( strName != strGroupName )
	{
		MessageBox(0, "请选择需要添加图素的具体图素组", "错误提示", 0);
		return false;
	}

	//得到相对名
	string texname = strTexUnitName;
	size_t nPos = 0;
	if( texname.find(".bmp") != -1  )
	{
		nPos = texname.find("tools\\地表纹理\\terrain");
		if ( nPos == -1 )
		{
			MessageBox(0, "请选择artist\\tools\\地表纹理 下的纹理地表资源", "提示", 0);
			return NULL;
		}
		EditFindSubString(texname, "tools");
		texname = "..\\tools\\" + texname;
	}
	else
	{
		if( texname.find(".dds") != -1 )
		{
			nPos = texname.find("res\\tile\\");
			if ( nPos == -1 )
			{
				MessageBox(0, "如果刷水请选择res\\tile\\下的纹理水资源", "提示", 0);
				return NULL;
			}
			else
				EditFindSubString(texname, "res");
		}
	}

	if( !this->TexUnitIsExitInGroup(texname) )
		return NULL;

	MakePathToText(texname, strItemName);

	CEdit2DResSetUnit *pUnit = new CEdit2DResSetUnit();
	pUnit->SetIsSubUnit(false);
	pUnit->AddName(texname);
	pUnit->SetItemName(strItemName);
	m_pActiveGroup->m_resSetUnits.push_back( pUnit );

	m_pActive2DUnit = pUnit;

	return true;
}

bool CTileViewCtrlImp::DeleteGroupUnit( const string& strGroupName, const string& strUnitName, const bool b3D )
{
	if( b3D )
	{
		if( m_pActiveModelGroup == NULL )
		{
			MessageBox(0, "请选择需要删除图素的具体图素组", "错误提示", 0);
			return false;
		}

		string strName =  m_pActiveModelGroup->m_groupName;
		if( strName != strGroupName)
		{
			MessageBox(0, "请选择需要删除图素的具体图素组", "错误提示", 0);
			return false;
		}

		size_t size = m_pActiveModelGroup->m_resModelSetUnits.size();
		for ( size_t i = 0; i < size; ++i )
		{
			string strName = m_pActiveModelGroup->m_resModelSetUnits[i]->m_itemName;
			if ( strName == strUnitName )
			{
				m_pActiveModelGroup->m_resModelSetUnits.erase( m_pActiveModelGroup->m_resModelSetUnits.begin() + i );
				return true;
			}
		}
	}
	else
	{
		if( m_pActiveGroup == NULL )
		{
			MessageBox(0, "请选择需要删除图素的具体图素组", "错误提示", 0);
			return false;
		}

		string strName =  m_pActiveGroup->m_groupName;
		if( strName != strGroupName)
		{
			MessageBox(0, "请选择需要删除图素的具体图素组", "错误提示", 0);
			return false;
		}

		size_t size = m_pActiveGroup->m_resSetUnits.size();
		for ( size_t i = 0; i < size; ++i )
		{
			string strName = m_pActiveGroup->m_resSetUnits[i]->GetNameByIndex(0);
			if ( strName == strUnitName )
			{
				m_pActiveGroup->m_resSetUnits.erase( m_pActiveGroup->m_resSetUnits.begin() + i );
				return true;
			}
		}
	}

	return false;
}

void CTileViewCtrlImp::BindUnitAnimationGroup( const string& strUnitName, const string& strAgpFile )
{
	if( m_pActiveEditModelUnit == NULL )
		return;

	string unitname = m_pActiveEditModelUnit->m_itemName;
	if( unitname != strUnitName)
	{
		MessageBox(0, "请选择需要绑定骨骼的图素", "错误提示", 0);
		return ;
	}

	string aniName = strAgpFile;
#ifdef USE_OLD_RESOURCEDIR_NOTCODER
	int nPos = aniName.find("\\res");
	if ( nPos == -1 )
	{
		MessageBox(0, "请重新选择res下的动作资源文件", "错误提示", 0);
		return;
	}
	else
	{
		nPos = aniName.find("\\resbin");
		if ( nPos != -1 )
		{
			MessageBox(0, "当前路径为resbin，请选择res目录下的资源", "错误提示", 0);
			return;
		}

		EditFindSubString(aniName, "res");
	}
#else
	int nPos = aniName.find("\\ressrc");
	if ( nPos == -1 )
	{
		MessageBox("请选择ressrc目录下的资源", "错误提示");
		return;
	}
	else
	{
		nPos = aniName.find("ressrc\\tgt");
		if ( nPos != -1 )
		{
			EditFindSubString(aniName, "ressrc\\tgt");
		}
		else
			EditFindSubString(aniName, "ressrc\\src");
	}
#endif

	if ( !aniName.empty() )
	{
		unitname = m_pActiveEditModelUnit->m_itemName;
		if( unitname != strUnitName)
		{
			MessageBox(0, "请选择需要绑定骨骼的图素", "错误提示", 0);
			return ;
		}
		m_pActiveEditModelUnit->m_pEditModel->AddAnimationGroup(aniName.c_str());
	}
}

void CTileViewCtrlImp::BindGroupAnimationGroup(const string& strGroupName, const string& strAgpFile)
{
	if( m_pActiveModelGroup == NULL )
	{
		MessageBox(0, "请选择需要整体绑定动画场景物件图素组包文件", "错误提示", 0);
		return;
	}

	if( m_pActiveModelGroup->m_groupName != strGroupName )
	{
		MessageBox(0, "请选择需要整体绑定动画场景物件图素组包文件", "错误提示", 0);
		return;
	}

	string aniName = strAgpFile;
#ifdef USE_OLD_RESOURCEDIR_NOTCODER
	int nPos = aniName.find("\\res");
	if ( nPos == -1 )
	{
		MessageBox(0, "请重新选择res下的动作资源文件", "错误提示", 0);
		return;
	}
	else
	{
		nPos = aniName.find("\\resbin");
		if ( nPos != -1 )
		{
			MessageBox(0, "当前路径为resbin，请选择res目录下的资源", "错误提示", 0);
			return;
		}

		EditFindSubString(aniName, "res");
	}
#else
	int nPos = aniName.find("\\ressrc");
	if ( nPos == -1 )
	{
		MessageBox("请选择ressrc目录下的资源", "错误提示");
		return;
	}
	else
	{
		nPos = aniName.find("ressrc\\tgt");
		if ( nPos != -1 )
		{
			EditFindSubString(aniName, "ressrc\\tgt");
		}
		else
			EditFindSubString(aniName, "ressrc\\src");
	}
#endif

	if ( !aniName.empty() )
	{
		bool bSuccess = true;//CMapEditApp::GetInst()->GetOperator()->GetTitleActiveObject()->GetModelGroup()->SetTitleAnimationByAgpFileName(aniName);
		if( bSuccess )
		{
			size_t size = m_pActiveModelGroup->m_resModelSetUnits.size();
			for ( size_t i = 0; i < size; ++i )
			{
				CEditResSetEditModelUnit* pEditModelUnit = m_pActiveModelGroup->m_resModelSetUnits[i];
				CEditModel* pEditModel = pEditModelUnit->m_pEditModel;
				if( pEditModel )
					pEditModel->AddAnimationGroup(aniName.c_str());
			}
		}
		else
		{
			MessageBox(0, "骨骼绑定不匹配，请重新绑定", "提示", 0);
		}
	}
}

void CTileViewCtrlImp::SetUnitMaterialCueIndex(const string& unitName, string& strMaterialIndex)
{
	string strName =  m_pActive2DUnit->GetNameByIndex(0);
	if( m_pActive2DUnit == NULL || strName != unitName)
	{
		MessageBox(0, "请选择需要绑定音效材质地表文件图素", "错误提示", 0);
		return ;
	}

	int nMaterialIndex  = -1;//CSceneAudioMgr::GetInst()->GetMaterialIndex(strMaterialIndex);
	m_pActive2DUnit->SetMaterialIndex(nMaterialIndex);
}

void CTileViewCtrlImp::SetGroupMaterialCueIndex(const string& strGroupName, string& strMaterialIndex)
{
	string strName = m_pActiveGroup->m_groupName;
	if( m_pActiveGroup == NULL || strName != strGroupName)
	{
		MessageBox(0, "请选择需要绑定音效材质地表文件图素组", "错误提示", 0);
		return ;
	}

	int nMaterialIndex  = -1;//CSceneAudioMgr::GetInst()->GetMaterialIndex(strMaterialIndex);
	terrainMatCuePool::iterator iter = m_mapMaterialIndexs.find(strMaterialIndex);
	if( iter != m_mapMaterialIndexs.end() )
		nMaterialIndex = iter->second;

	size_t size = m_pActiveGroup->m_resSetUnits.size();
	for ( size_t i = 0; i < size; ++i )
	{
		CEditResSetUnit *pSetUnit = m_pActiveGroup->m_resSetUnits[i];
		pSetUnit->SetMaterialIndex(nMaterialIndex);
	}
}

int CTileViewCtrlImp::GetMaterialIndex( string& name )
{
	terrainMatCuePool::iterator iter = m_mapMaterialIndexs.find(name);
	if( iter != m_mapMaterialIndexs.end() )
		return iter->second;

	return -1;
}

bool CTileViewCtrlImp::ResetIsExit( const string& resetName )
{
	ResSetPool::iterator iter = m_resSets.find(resetName);
	if( iter == m_resSets.end() )
		return false;

	return true;
}

void CTileViewCtrlImp::SelectActiveUnit( const string& resetName, const string& strGroupName, const string& strUnitName, const bool b3D, const bool bSelectUnit )
{
	ResSetPool::iterator iter = m_resSets.find(resetName);
	if( iter != m_resSets.end() )
	{
		m_pActiveResSet = iter->second;

		if( b3D )
			this->SelectActive3DGroupUnit(strGroupName, strUnitName, bSelectUnit);
		else
			this->SelectActive2DGroupUnit(strGroupName, strUnitName, bSelectUnit);
	}
}

int CTileViewCtrlImp::GetUnitMaterialIndex( const string& unitName )
{
	if( m_pActive2DUnit == NULL )
	{
		MessageBox(0, "请选择具体的纹理图素组", "错误提示", 0);
		return -1;
	}

	return m_pActive2DUnit->GetMaterialIndex();
}

void CTileViewCtrlImp::BindUnitEffectName( const string& strUnitName, const string& strEffectName )
{
	if( m_pActiveEditModelUnit == NULL )
	{
		MessageBox(0, "请选择需要绑定特效的图素", "错误提示", 0);
		return ;
	}

	string unitname = m_pActiveEditModelUnit->m_itemName;
	if( unitname != strUnitName)
	{
		MessageBox(0, "请选择需要绑定骨骼的图素", "错误提示", 0);
		return ;
	}
	if( strEffectName.empty() )
		m_pActiveEditModelUnit->m_pEditModel->ClearAllEfx();
}

void CTileViewCtrlImp::MakePathToText( const string& Src, string& Dst )
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

void CTileViewCtrlImp::UpdateSetViewCtrl()
{
	if (m_pEditResSetObserver)
	{
		m_pEditResSetObserver->SetSubject(m_pActiveResSet);
	}
}

int CTileViewCtrlImp::GetReset3DGroupCount()
{
	if( m_pActiveResSet )
	{
		return m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
	}

	return 0;
}

void CTileViewCtrlImp::GetReset3DGroupNameByIndex( const size_t index, string&name )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
		if( index < size )
			name = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[index]->m_groupName;
	}
}

int CTileViewCtrlImp::GetReset3DGroupUnitCount( const size_t index )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
		if( index < size )
		{
			CEditModelResSetGroup *pEditGroup = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[index];
			return pEditGroup->m_resModelSetUnits.size();
		}

		return 0;
	}

	return 0;
}

void CTileViewCtrlImp::GetReset3DUnitNameByIndex( const size_t groupindex, const size_t unitindex, string&name )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
		if( groupindex < size )
		{
			CEditModelResSetGroup *pEditGroup = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[groupindex];
			name = pEditGroup->m_resModelSetUnits[unitindex]->m_itemName;
		}
	}
}

int CTileViewCtrlImp::GetReset2DGroupCount()
{
	if( m_pActiveResSet )
	{
		return m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
	}

	return 0;
}

void CTileViewCtrlImp::GetReset2DGroupNameByIndex( const size_t index, string&name )
{
	if( m_pActiveResSet )
	{
		size_t size = m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
		if( index < size )
			name = m_pActiveResSet->m_2DResSet.m_resSetGroups[index]->m_groupName;
	}
}

int CTileViewCtrlImp::GetReset2DGroupUnitCount( const size_t index )
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

void CTileViewCtrlImp::GetReset2DUnitNameByIndex( const size_t groupindex, const size_t unitindex, string&name )
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

void CTileViewCtrlImp::SelectActive3DGroupUnit( const string& strGroupName, const string& strUnitName, const bool bSelectUnit )
{
	size_t size = m_pActiveResSet->m_3DModelResSet.m_resSetGroups.size();
	for ( size_t i = 0; i < size; ++i )
	{
		CEditModelResSetGroup* pModelGroup = m_pActiveResSet->m_3DModelResSet.m_resSetGroups[i];

		if ( strGroupName == pModelGroup->m_groupName )
		{
			if( !bSelectUnit )
			{
				m_pActiveModelGroup = pModelGroup;

				return;
			}
			else
			{
				size_t size = pModelGroup->m_resModelSetUnits.size();
				for ( size_t m = 0; m < size; ++m )
				{
					m_pActiveEditModelUnit = pModelGroup->m_resModelSetUnits[m];
					string unitName = m_pActiveEditModelUnit->m_itemName;
					if ( unitName == strUnitName )
					{
						m_pImpContext->SetActiveObj(m_pActiveEditModelUnit->m_pEditModel);
						return;
					}
				}

			}

		}
	}
}

void CTileViewCtrlImp::SelectActive2DGroupUnit( const string& strGroupName, const string& strUnitName, const bool bSelectUnit )
{
	size_t size = m_pActiveResSet->m_2DResSet.m_resSetGroups.size();
	for ( size_t i = 0; i < size; ++i )
	{
		CEditResSetGroup* group = m_pActiveResSet->m_2DResSet.m_resSetGroups[i];

		if ( strGroupName == group->m_groupName )
		{
			if( !bSelectUnit )
			{
				m_pActiveGroup = group;
				return;
			}
			else
			{
				size_t size = group->m_resSetUnits.size();
				for ( size_t m = 0; m < size; ++m )
				{
					string unitName = group->m_resSetUnits[m]->GetItemName();
					if ( unitName == strUnitName )
					{
						m_pActive2DUnit = (CEdit2DResSetUnit*)group->m_resSetUnits[m];

						return;
					}
				}
			}

		}
	}
}

void CTileViewCtrlImp::GetResetResPath( string& path )
{
	path = m_strResSetPath;
}

void CTileViewCtrlImp::SetMapEffectCtrlImp( CMapEffectCtrlImp* pImp )
{
	m_pMapEffectCtrlImp = pImp;
}

bool CTileViewCtrlImp::_ProcMsg( const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_CUSTOM:
		{
			switch(msg.msgType.custom_type)
			{
			case MGT_UPDATE_EFFECT:
				{
					// 					MSG_INFO info = msg.msgInfo;
					// 					DWORD color = info._iinfo;
					// 					m_pFogColorCtrlImp->SetColor(color);
					// 					this->UpdateFog();

					break;
				}

			default:
				break;
			}
		}
	}
	return false;
}

void CTileViewCtrlImp::Update()
{
	CCtrlBase::Update();
}

void CTileViewCtrlImp::ActiveNode( CRenderNode* pNode )
{
	CCtrlBase::Update();
}

void CTileViewCtrlImp::SetContext( CEditContext* pContext )
{
	m_pOwnContext = pContext;
}

void CTileViewCtrlImp::SetImpContext( CEditContext* pContext )
{
	m_pImpContext = pContext;
}

void CTileViewCtrlImp::LoadOldReset( const string& resetName )
{
	char dir[1024];
	::GetCurrentDirectory(1024, dir);

	string strResSetFileName = resetName;
	locale loc;
	use_facet< ctype<TCHAR> >( loc ).tolower
		( &strResSetFileName[0], &strResSetFileName[strResSetFileName.length()] );

	CEditResSet * pResSet = new CEditResSet;
	int i = strResSetFileName.find_last_of('.');
	int nPos = strResSetFileName.find("titleresourceset");
	pResSet->m_resSetName =	strResSetFileName.substr(nPos+17,i-nPos-17);


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

	int nTemp = 0;
	bool b = false;
	string modelname("");

	TiXmlNode *p3d = pResourceSetNode->FirstChild();
	size_t npos = 0;
	for (TiXmlNode * pXmlChild=p3d->FirstChild();pXmlChild; pXmlChild=pXmlChild->NextSibling())
	{
		if(!pXmlChild->ToElement())
			continue;

		CEditModelResSetGroup*pModelResSetGroup = new CEditModelResSetGroup;

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

			CEditResSetEditModelUnit* pEditModelUnit = new CEditResSetEditModelUnit;
			pModelResSetGroup->m_resModelSetUnits.push_back(pEditModelUnit);
			CEditModel* pParentModel = m_pImpContext->CreateEditModel(NULL);
			pEditModelUnit->m_pEditModel = pParentModel;

			TiXmlAttribute* pk=pj->ToElement()->FirstAttribute();

			int cnt3 = atoi(pk->Value());
			for ( int k = 0; k < cnt3; ++k )
			{
				pk=pk->Next();
				modelname = pk->ValueStr();

				CEditModel* pChildModel = m_pImpContext->CreateEditModel(NULL);
				pChildModel->AddPieceGroup(modelname.c_str());
				pParentModel->AddChild(pChildModel, LT_CENTER, "");
			}

			string itemname;
			if( cnt3 > 1 )
			{
				this->MakePathToText(modelname, itemname);
				itemname = itemname.substr(0, itemname.length() -1 );

				pEditModelUnit->m_itemName = itemname;
				pEditModelUnit->m_bSubUnit = true;
			}
			else
			{
				this->MakePathToText(modelname, itemname);

				pEditModelUnit->m_itemName = itemname;
				pEditModelUnit->m_bSubUnit = false;
			}

			pk=pk->Next();
			pk=pk->Next();
			pk=pk->Next();
			pk=pk->Next();
			pk=pk->Next();
			pk=pk->Next();
			pk=pk->Next();
			pk=pk->Next();

			pk=pk->Next();
			string agpname = pk->ValueStr();
			if( !agpname.empty() )
				pParentModel->AddAnimationGroup(agpname.c_str());

			string effectname("");
			pk=pk->Next();
			nTemp = atoi(pk->Value());
			b = nTemp == 1 ? true : false;
			if (b)
			{
				pk=pk->Next();
				effectname = pk->ValueStr();
				replace(effectname.begin(), effectname.end(), '\\', '/');
			
				string effectFileName;
				m_pMapEffectCtrlImp->GetEffectFileName(effectname, effectFileName);
				pParentModel->AddEffect(effectFileName.c_str(), effectname.c_str(), CEffectPlayer::PLAY_LOOP, NULL);
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

	delete pTitleResourceSetFile;
	pTitleResourceSetFile = NULL;
	in.close();
}

void CTileViewCtrlImp::PathResetFile()
{
	vector<string> fileList;
	string fileName;
	CPkgDir pkgDir;
	int nFileNum = pkgDir.Open(PATH_ALIAS_EFFECT.c_str(), m_strResSetPath.c_str());
	for ( int m = 0; m < nFileNum; ++m )
	{
		pkgDir.GetFileName(m, fileName);
		fileList.push_back(fileName);
	}
	
	if( fileList.empty() )
		return;

	size_t size = fileList.size();
	for ( size_t m = 0; m < size; ++m )
	{
		fileName = fileList[m];
		fileName = m_strResSetPath + fileName;

		LoadOldReset(fileName);
		SaveReset(fileName);
	}
}