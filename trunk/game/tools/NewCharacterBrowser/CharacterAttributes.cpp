#include "StdAfx.h"
#include "CharacterAttributes.h"
#include "CDataCache.h"
#include "CGraphic.h"
#include "CDataCache.h"
#include "ScriptMgr.h"
#include "CSkeletalsMgr.h"
#include "CEditModel.h"
#include "CCharacter.h"
#include "CCharacter.inl"

#define	INIT_LOADINF(type,path,name) m_pLoadInfList[type]->m_Type = type; m_pLoadInfList[type]->m_TypePath = path; m_pLoadInfList[type]->m_Name = name;

namespace CharacterBrowser {

CCharacterAttributes::PieceItem::PieceItem( void )
{
	m_pPartInfo = NULL;
}

CCharacterAttributes::PieceItem::~PieceItem()
{
	m_pPartInfo = NULL;
}

CCharacterAttributes::SLoadInf::SLoadInf()
{
	m_ItemLists.resize(ITEM_COUNT);
}

CCharacterAttributes::SLoadInf::~SLoadInf()
{
	for ( int i = 0; i < ITEM_COUNT; ++i )
	{
		ItemList *pItem = m_ItemLists[i];
		delete pItem;
		pItem = NULL;
	}
	m_ItemLists.clear();
}

void CCharacterAttributes::SLoadInf::Init()
{
	for ( int i = 0; i < ITEM_COUNT; ++i )
	{
		ItemList *pItemList = new ItemList;
		m_ItemLists[i] = pItemList;
	}
}

CCharacterAttributes::CCharacterAttributes(void)
{
}

CCharacterAttributes::~CCharacterAttributes( void )
{
	this->Destroy();
}

void CCharacterAttributes::Init()
{
	m_pLoadInfList.resize(eCharTypeCount);
	for ( int i = 0; i < eCharTypeCount; ++i )
	{
		SLoadInf *pNewLoadInf = new SLoadInf;
		pNewLoadInf->Init();
		m_pLoadInfList[i] = pNewLoadInf;
	}

	INIT_LOADINF(eRLM, "rlm", "人类男");
	INIT_LOADINF(eRLW, "rlw", "人类女");
	INIT_LOADINF(eSRM, "srm", "兽人男");
	INIT_LOADINF(eSRW, "srw", "兽人女");
	INIT_LOADINF(eJLM, "jlm", "精灵男");
	INIT_LOADINF(eJLW, "jlw", "精灵女");
	INIT_LOADINF(eARM, "arm", "矮人男");
	INIT_LOADINF(eARW, "arw", "矮人女");
	INIT_LOADINF(eOTHER,"",   "其它");

 	const ModelPART_VEC& tModelPartVec = gs_pCahce->GetPlayerModelPart();
 	ModelPART_VEC::const_iterator it,eit = tModelPartVec.end();
 
 	for( uint i = 0; i < eCharTypeCount; ++i )
 	{
 		SLoadInf *pInf = m_pLoadInfList[i];
 		for( it = tModelPartVec.begin(); it!=eit; ++it )
 		{
 			ItemList* pItem = NULL;
 			CPlayerModelPart* pPart = *it;
 
 			if( pPart->GetPieceName() == "armet" )
 				pItem = pInf->m_ItemLists[ITEM_HEAD];
 			else if( pPart->GetPieceName() == "face" )
 				pItem = pInf->m_ItemLists[ITEM_FACE];
 			else if( pPart->GetPieceName() == "hair" )
 				pItem = pInf->m_ItemLists[ITEM_HAIR];
 			else if( pPart->GetPieceName() == "shoulder" )
 				pItem = pInf->m_ItemLists[ITEM_SHOULDER];
 			else if( pPart->GetPieceName() == "body" )
 				pItem = pInf->m_ItemLists[ITEM_BODY];
 			else if( pPart->GetPieceName() == "back" )
 				pItem = pInf->m_ItemLists[ITEM_MANTLE];
 			else if( pPart->GetPieceName() == "arm" )
 				pItem = pInf->m_ItemLists[ITEM_CUFF];
 			else if( pPart->GetPieceName() == "shoe" )
 				pItem = pInf->m_ItemLists[ITEM_BOOT];
 			else if( pPart->GetPieceName() == "weapon" )
 				pItem = pInf->m_ItemLists[ITEM_MAINWEAPON];
 			else if( pPart->GetPieceName() == "offweapon" )
 				pItem = pInf->m_ItemLists[ITEM_SUBWEAPON];
 
 			if( pItem!=NULL )
 			{
 				string FileName = pPart->GetModelFileName(pInf->m_Name);
 				//CPieceGroup* pGroup = NULL;
  				//if( 0 == CGraphic::GetInst()->CreatePieceGroup(FileName.c_str(),&pGroup) )
  				{
  					PieceItem Item;
  					Item.m_Name = pPart->GetName();
  					Item.m_PiecePath = FileName;
  					Item.m_pPartInfo = pPart;
  					pItem->push_back(Item);
  					//pGroup->Release();
  				}
 			}			
 		}
 	}
}

void CCharacterAttributes::Destroy()
{
	size_t size = m_pLoadInfList.size();
	for ( size_t i = 0; i < size; ++i )
	{
		SLoadInf* ptemp = m_pLoadInfList[i];
		delete ptemp;
		ptemp = NULL;
	}

	m_pLoadInfList.clear();
}

void CCharacterAttributes::GetItemListInfos( const int LoadInfIndex, const int itemIndex, vector<string>& itemlists )
{
	if( LoadInfIndex > eCharTypeCount )
		return;

	if( itemIndex > ITEM_COUNT )
		return;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	string strTypePath = pInf->m_TypePath;
	ItemList *il = pInf->m_ItemLists[itemIndex];
	ItemList::iterator it,eit = il->end();
	for( it = il->begin(); it != eit; ++it )
	{
		itemlists.push_back((*it).m_Name.c_str());
	}
}

void CCharacterAttributes::GetItemListEffectInfos( const int LoadInfIndex, const int itemIndex, vector<string>& itemeffctlists )
{
	if( LoadInfIndex > eCharTypeCount )
		return;

	if( itemIndex > ITEM_COUNT )
		return;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	string strTypePath = pInf->m_TypePath;
	ItemList *il = pInf->m_ItemLists[itemIndex];
	ItemList::iterator it,eit = il->end();
	for( it = il->begin(); it != eit; ++it )
	{
		PieceItem item = (*it);

		string FileName = item.m_pPartInfo->GetModelFileName();
		int Off = FileName.find_last_of("//");
		FileName = FileName.substr(Off+1,FileName.find_last_of(".") - Off);

		string tPath = item.m_pPartInfo->GetEffectFileName();
		tPath += strTypePath + "/" + FileName + "efx";
		itemeffctlists.push_back(tPath);
	}
}

void CCharacterAttributes::GetItemModelInfos(const int LoadInfIndex, const int itemIndex, const int pieceItemIndex, string& strPiece, string& rsname)
{
	if( LoadInfIndex > eCharTypeCount )
		return;

	if( itemIndex > ITEM_COUNT )
		return;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	ItemList *il = pInf->m_ItemLists[itemIndex];

	int size = (*il).size();
	if( pieceItemIndex > size )
		return;

	PieceItem Item = (*il)[pieceItemIndex];
	strPiece = Item.m_pPartInfo->GetPieceName();
	rsname   = Item.m_pPartInfo->GetRenderStyle();
}

void CCharacterAttributes::GetItemEffectInfo(const int LoadInfIndex, const int itemIndex, const int pieceItemIndex, string& strEffect)
{
	if( LoadInfIndex > eCharTypeCount )
		return;

	if( itemIndex > ITEM_COUNT )
		return;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	ItemList *il = pInf->m_ItemLists[itemIndex];

	int size = (*il).size();
	if( pieceItemIndex > size )
		return;

	PieceItem Item = (*il)[pieceItemIndex];
	string tPath = Item.m_pPartInfo->GetEffectFileName();
	string FileName = Item.m_pPartInfo->GetModelFileName();
	int Off = FileName.find_last_of("//");
	FileName = FileName.substr(Off+1,FileName.find_last_of(".") - Off);
	tPath += pInf->m_TypePath + "/" + FileName + "efx";
}

void CCharacterAttributes::GetEquipGroupNames(const int LoadInfIndex, vector<string>& equipGroupLists)
{
	if( LoadInfIndex > eCharTypeCount )
		return;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	ItemList *il   = pInf->m_ItemLists[ITEM_HEAD];
	for (ItemList::iterator iter = il->begin();iter != il->end();++iter)
	{
		string strEquipHead = (*iter).m_Name;
		int32 iLength = strEquipHead.length();
		string strGroup = strEquipHead.substr(0,iLength-4);
		vector<ItemList*> vecItemList;
		vecItemList.push_back(pInf->m_ItemLists[ITEM_SHOULDER]);
		vecItemList.push_back(pInf->m_ItemLists[ITEM_BODY]);
		vecItemList.push_back(pInf->m_ItemLists[ITEM_MANTLE]);
		vecItemList.push_back(pInf->m_ItemLists[ITEM_CUFF]);
		vecItemList.push_back(pInf->m_ItemLists[ITEM_BOOT]);
		for (vector<ItemList*>::iterator iterVec = vecItemList.begin();iterVec != vecItemList.end();++iterVec)
		{
			bool bResult = false;
			for (ItemList::iterator iterIT = (*iterVec)->begin();iterIT != (*iterVec)->end();++iterIT)
			{
				string strTemp = (*iterIT).m_Name;
				int32 iLengthTemp = strTemp.length();
				if(iLengthTemp>4)
				{
					string strNameTemp = strTemp.substr(0,iLengthTemp-4);
					if (strNameTemp == strGroup)
					{
						equipGroupLists.push_back(strGroup);
						bResult = true;
						break;
					}
				}
			}
			if(bResult)
				break;
		}
	}
}

void CCharacterAttributes::GetItemNames(vector<string>& names)
{
	for( uint i = 0; i < eCharTypeCount; ++i )
	{
		SLoadInf *pInf = m_pLoadInfList[i];
		names.push_back(pInf->m_Name);
	}
}

void CCharacterAttributes::GetItemName( const int LoadInfIndex, string& name )
{
	if( LoadInfIndex > eCharTypeCount )
		return;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	name = pInf->m_Name;
}

void CCharacterAttributes::GetItemTypePath( const int LoadInfIndex,  string& path )
{
	if( LoadInfIndex > eCharTypeCount )
		return;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	path = pInf->m_TypePath;
}

void CCharacterAttributes::GetItemType( const int LoadInfIndex, ECharType& type )
{
	if( LoadInfIndex > eCharTypeCount )
		return;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	type = pInf->m_Type;
}

CCharacterAttributes::PieceItem CCharacterAttributes::GetPieceItem( const int LoadInfIndex, const int itemIndex, const int pieceItemIndex )
{
	PieceItem Item;

	if( LoadInfIndex > eCharTypeCount )
		return Item;

	if( itemIndex > ITEM_COUNT )
		return Item;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	ItemList *il = pInf->m_ItemLists[itemIndex];

	int size = (*il).size();
	if( pieceItemIndex > size )
		return Item;

	Item = (*il)[pieceItemIndex];
	return Item;
}

void CCharacterAttributes::GetLoadInfTypePath( const int LoadInfIndex, string & path )
{
	if( LoadInfIndex > eCharTypeCount )
		return;

	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];
	path = pInf->m_TypePath;
}

void CCharacterAttributes::GetPieceItemIndexByName( const int LoadInfIndex, const vector<int> vecItemType, const string& strName, vector<int>& pieceItemIndex )
{
	if( LoadInfIndex > eCharTypeCount )
		return;
	SLoadInf *pInf = m_pLoadInfList[LoadInfIndex];

	size_t size = vecItemType.size();
	for ( size_t m = 0; m < size; ++m )
	{
		int  i = 0;
		bool bResult = false;
		int itemIndex = vecItemType[m];
		ItemList *il = pInf->m_ItemLists[itemIndex];
		ItemList::iterator it,eit = il->end();
		for( it = il->begin(); it != eit; ++it, ++i )
		{
			string strTemp = (*it).m_Name;
			int32 iLengthTemp = strTemp.length();
			if(iLengthTemp>4)
			{
				string strNameTemp = strTemp.substr(0,iLengthTemp-4);
				if (strNameTemp == strName)
				{
					pieceItemIndex.push_back(i);
					bResult = true;
					break;
				}
			}
		}
		if(!bResult)
			pieceItemIndex.push_back(0);
	}
}
}