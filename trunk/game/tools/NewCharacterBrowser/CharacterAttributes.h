#pragma once
#include "CharacterAttriTypeDefine.h"
class CPlayerModelPart;
namespace sqr
{
	class CRenderObject;
}

namespace sqr_tools
{
	class CEditModel;
}

namespace CharacterBrowser {

class CCharacterAttributes
{
public:

	struct PieceItem
	{
		string				m_Name;
		string				m_PiecePath;
		CPlayerModelPart*	m_pPartInfo;

		PieceItem(void);
		~PieceItem();
	};

	typedef vector<PieceItem>	ItemList;
	struct SLoadInf
	{
		ECharType			m_Type;
		string				m_TypePath;
		string				m_Name;

// 		ItemList			m_Head;
// 		ItemList			m_Face;
// 		ItemList			m_Hair;
// 		ItemList			m_Shoulder;
// 		ItemList			m_Body;
// 		ItemList			m_Mantle;
// 		ItemList			m_Cuff;
// 		ItemList			m_Boot;
// 		ItemList			m_MainWeapon;
// 		ItemList			m_SubWeapon;

		vector<ItemList *>  m_ItemLists;

		SLoadInf();
		~SLoadInf();
		void Init();
	};

protected:
	vector<SLoadInf*>		m_pLoadInfList;

public:
	CCharacterAttributes(void);
	~CCharacterAttributes(void);

	void Init();
	void Destroy();

	void GetItemListInfos(const int LoadInfIndex, const int itemIndex, vector<string>& itemlists);
	void GetItemModelInfos(const int LoadInfIndex, const int itemIndex, const int pieceItemIndex, string& strPiece, string& rsname);
	void GetItemListEffectInfos(const int LoadInfIndex, const int itemIndex, vector<string>& itemeffctlist);
	void GetItemEffectInfo(const int LoadInfIndex, const int itemIndex, const int pieceItemIndex, string& strEffect);
	CCharacterAttributes::PieceItem GetPieceItem(const int LoadInfIndex, const int itemIndex, const int pieceItemIndex);
	void GetEquipGroupNames(const int LoadInfIndex, vector<string>& equipGroupLists);
	void GetItemNames(vector<string>& names);
	void GetItemName( const int LoadInfIndex, string& name );
	void GetItemTypePath(const int LoadInfIndex, string& path);
	void GetItemType(const int LoadInfIndex, ECharType& type);
	void GetLoadInfTypePath(const int LoadInfIndex, string & path);
	void GetPieceItemIndexByName(const int LoadInfIndex, const vector<int> vecItemType, const string& strName, vector<int>& pieceItemIndex);
};

}
