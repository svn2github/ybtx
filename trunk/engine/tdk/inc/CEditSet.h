#pragma once
#include "CEditSubject.h"

namespace sqr_tools
{
	#define USE_OLD_RESOURCEDIR_NOTCODER

	class CEditModel;

	enum RESSETGROUP_TYPE
	{
		RSG_COMMON = 0, //普通图素包
		RSG_NPC    = 1, //npc图素包
		RSG_FRUTEX = 2, //树图素包
		RSG_TREE   = 3, //灌木图素包
		RSG_GRASS  = 4, //草图素包
		RSG_CNT    = 5, 
	};

	enum ETRANS_TYPE
	{
		ETT_VERTEX = 0,
		ETT_TEXTURE,
		ETT_NONE
	};

	enum ETrans_Type
	{
		ETT_TRANSLUCENT = 0,
		ETT_SOLID,
		ETT_COUNT
	};

	enum ECenter_Type
	{
		ECT_CENTER = 0,
		ECT_CORNER,
		ECT_XMIDWAY,
		ECT_ZMIDWAY,
		ECT_NONE
	};

	class CEditResSetUnit
	{
		vector<string> m_unitNames;
		bool	m_bIsEdited;
		bool	m_bSubUnit;
		string  m_strItemName;
		int		m_nMaterialIndex;
	public:
		bool m_bAmbientInUse;
		int m_nProbability;
		string m_strAmbientEffectSkyTextureName;
	public:

		// for effect
		void * p; // object file
		CEditResSetUnit() : m_bIsEdited(false), m_nMaterialIndex(-1), m_nProbability(100),
				m_bAmbientInUse(false), m_strAmbientEffectSkyTextureName(""), p(NULL)
		{
			;
		}

		//添加析构函数并资源释放，baozi
		~CEditResSetUnit() 
		{
		}

		void AddName(string s)
		{
			m_unitNames.push_back(s);
		}

		void AddName(string s, size_t i)
		{
			if ( i < m_unitNames.size() )
				m_unitNames[i] = s;
		}

		bool NameExist(const string& s);

		vector<string> & GetName()
		{
			return m_unitNames;
		}

		int GetNameSize()
		{
			return m_unitNames.size();
		}

		string & GetNameByIndex(int n)
		{
			return m_unitNames[n];
		}

		vector<string> GetUnitNames() const
		{
			return m_unitNames;
		}

		string & GetItemName()
		{
			return m_strItemName;
		}

		void SetItemName(string & str)
		{
			m_strItemName = str;
		}

		void SetIsSubUnit(bool b)
		{
			m_bSubUnit = b;
		}

		bool GetIsSubUnit()
		{
			return m_bSubUnit;
		}

		bool GetIsEdited()
		{
			return m_bIsEdited;
		}

		void SetIsEdited(bool b)
		{
			m_bIsEdited = b;
		}

		void SetMaterialIndex(int index)
		{
			m_nMaterialIndex = index;
		}

		int GetMaterialIndex()
		{
			return m_nMaterialIndex;
		}
	};

	struct CEditResSetEditModelUnit
	{
		string			m_itemName;
		vector<string> m_strModelNames;
		string			m_strAgpName;
		string			m_strEffectFileName;
		string			m_strEffectName;
		bool			m_bSubUnit;

		CEditModel*		m_pEditModel;

		CEditResSetEditModelUnit() : m_bSubUnit(false), m_pEditModel(NULL){};
		~CEditResSetEditModelUnit();
	};

	struct CEditResSetGroup
	{
		string						m_groupName;
		BYTE						m_ResGroupMark;///m_ResGroupMark 
		bool						m_bVisibleByConfigue;
		vector<CEditResSetUnit*>	m_resSetUnits;

		CEditResSetGroup() : m_groupName(""), m_ResGroupMark(0), m_bVisibleByConfigue(false) {}

		~CEditResSetGroup();

		bool NameExist( const string& s );
	};

	struct CEditModelResSetGroup
	{
		string								m_groupName;
		BYTE								m_ResGroupMark;///m_ResGroupMark 
		bool								m_bVisibleByConfigue;
		vector<CEditResSetEditModelUnit*>	m_resModelSetUnits;

		CEditModelResSetGroup() : m_groupName(""), m_ResGroupMark(0), m_bVisibleByConfigue(false) {}

		~CEditModelResSetGroup();

		bool NameExist( const string& s );
	};

	struct CEditDimSet
	{
		vector<CEditResSetGroup*> m_resSetGroups;

		~CEditDimSet();
	};

	struct CEdit3DDimSet
	{
		typedef vector<CEditModelResSetGroup*> EditModelResSetPool;

		EditModelResSetPool m_resSetGroups;

		~CEdit3DDimSet();
	};

	class CEditResSet : public CEditSubject 
	{
	public:
		string			m_resSetName;
		CEditDimSet		m_2DResSet;
		CEdit3DDimSet	m_3DModelResSet;
		void			*m_pEffectGroup;
		bool			m_bAmbientEffect;

		CEditResSet();
		~CEditResSet();
	};

	class CEdit2DResSetUnit : public CEditResSetUnit
	{
		ETRANS_TYPE m_eTransType;

	public:
		CEdit2DResSetUnit();
		~CEdit2DResSetUnit();

		ETRANS_TYPE GetTransType();
		void SetTransType(ETRANS_TYPE e);
	};
}