#pragma once
#include "BaseHelper.h"
#include "CVector3.h"
#include "CEffectMallocObject.h"

namespace sqr
{

	class EFFECT_DATA : public CEffectMallocObject
	{
	public:

		// Enumerations
		enum eDataType
		{
			STRING,
			INTEGER,
			FLOAT,
			VECTOR,
			COMBO,
			DBWORD,
			CSTRING,
			FloatMap,
			BOOLValue,
		};

		struct STRINGDATA : public CEffectMallocObject
		{
			BYTE		nFlag;
			EString		szStr;
		};

		struct COMBODATA : public CEffectMallocObject
		{
			BYTE		nNum;
			EString		szItem;
		};

		struct VECTORDATA 
			: public CVector3f
		{
		public:
			VECTORDATA(const CVector3f& vec) : CVector3f(vec){}
		};

		// Constructor
		EFFECT_DATA() : m_nType(INTEGER), strValue(NULL)
		{
		}

		EFFECT_DATA( const EFFECT_DATA& a )
		{
			m_nType = a.m_nType;
			switch (m_nType)
			{
			case STRING:
				this->strValue = new STRINGDATA();
				this->strValue->nFlag = a.strValue->nFlag;
				this->strValue->szStr = a.strValue->szStr;
				break;
			case INTEGER:
				this->intValue = a.intValue;
				break;
			case FLOAT:
				this->floatValue = a.floatValue;
				break;
			case VECTOR:
				this->vecValue = new VECTORDATA(*a.vecValue);
				break;
			case COMBO:
				this->comboValue = new COMBODATA();
				this->comboValue->nNum = a.comboValue->nNum;
				this->comboValue->szItem = a.comboValue->szItem;
				break;
			case DBWORD:
				this->dwValue = a.dwValue;
				break;
			case CSTRING:
				this->cstrValue = new EString(*a.cstrValue);
				break;
			case FloatMap:
				this->floatMap = new EMap<float, float>(*a.floatMap);
				break;
			case BOOLValue:
				this->boolValue = a.boolValue;
				break;
			}
		}

		~EFFECT_DATA()
		{
			Clear();
		}

		void Clear()
		{
			switch (m_nType)
			{
			case STRING:
				SafeDelete(this->strValue);
				break;
			case VECTOR:
				SafeDelete(this->vecValue);
				break;
			case COMBO:
				SafeDelete(this->comboValue);
				break;
			case CSTRING:
				SafeDelete(this->cstrValue);
				break;
			case FloatMap:
				SafeDelete(this->floatMap);
				break;
			}
			m_nType = INTEGER;
		}

		const EFFECT_DATA& operator= ( const STRINGDATA& str )
		{
			this->Clear();
			m_nType = STRING;
			this->strValue = new STRINGDATA();
			this->strValue->nFlag = str.nFlag;
			this->strValue->szStr = str.szStr;
			return *this;
		}

		const EFFECT_DATA& operator= ( int32 nValue )
		{
			this->Clear();
			m_nType = INTEGER;
			this->intValue = nValue;
			return *this;
		}

		const EFFECT_DATA& operator= ( float fValue )
		{
			this->Clear();
			m_nType = FLOAT;
			this->floatValue = fValue;
			return *this;
		}

		const EFFECT_DATA& operator= ( CVector3f vec )
		{
			this->Clear();
			m_nType = VECTOR;
			this->vecValue = new VECTORDATA(vec);
			return *this;
		}

		const EFFECT_DATA& operator= ( const COMBODATA& comValue )
		{
			this->Clear();
			m_nType = COMBO;
			this->comboValue = new COMBODATA();
			this->comboValue->nNum = comValue.nNum;
			this->comboValue->szItem = comValue.szItem;
			return *this;
		}

		const EFFECT_DATA& operator= ( const char* str )
		{
			this->Clear();
			m_nType = CSTRING;
			this->cstrValue = new EString();
			*this->cstrValue = str;
			return *this;
		}

		const EFFECT_DATA& operator= ( uint32 dw)
		{
			this->Clear();
			m_nType = DBWORD;
			this->dwValue = dw;
			return *this;
		}

		const EFFECT_DATA& operator= ( bool b)
		{
			this->Clear();
			m_nType = BOOLValue;
			this->boolValue = b;
			return *this;
		}

		const EFFECT_DATA& operator=(const EMap<float, float> other)
		{
			this->Clear();
			m_nType = FloatMap;
			this->floatMap = new EMap<float, float>(other);
			return *this;
		}

		const EFFECT_DATA& operator= ( const EFFECT_DATA& a )
		{
			this->Clear();
			m_nType = a.m_nType;
			switch (m_nType)
			{
			case STRING:
				this->strValue = new STRINGDATA();
				this->strValue->nFlag = a.strValue->nFlag;
				this->strValue->szStr = a.strValue->szStr;
				break;
			case INTEGER:
				this->intValue = a.intValue;
				break;
			case FLOAT:
				this->floatValue = a.floatValue;
				break;
			case VECTOR:
				this->vecValue = new VECTORDATA(*a.vecValue);
				break;
			case COMBO:
				this->comboValue = new COMBODATA();
				this->comboValue->nNum = a.comboValue->nNum;
				this->comboValue->szItem = a.comboValue->szItem;
				break;
			case DBWORD:
				this->dwValue = a.dwValue;
				break;
			case CSTRING:
				this->cstrValue = new EString(*a.cstrValue);
				break;
			case FloatMap:
				this->floatMap = new EMap<float, float>(*a.floatMap);
				break;
			case BOOLValue:
				this->boolValue = a.boolValue;
				break;
			}
			return a;
		}

		EFFECT_DATA::eDataType	GetType() const
		{
			return m_nType;
		}
		const char*				CStr() const
		{
			return this->cstrValue->c_str();
		}
		int32					Int() const
		{
			return this->intValue;
		}
		float					Float() const
		{
			return this->floatValue;
		}
		uint32					DWord() const
		{
			return this->dwValue;
		}
		const float*			Vec() const
		{
			return &this->vecValue->x;
		}
		bool					IsFile() const
		{
			return this->strValue->nFlag == 1;
		}
		bool					IsAudio() const
		{
			return this->strValue->nFlag == 2;
		}
		bool					IsEffect() const
		{
			return this->strValue->nFlag == 3;
		}
		const char*				Str() const
		{
			return this->strValue->szStr.c_str();
		}
		BYTE					Index() const
		{
			return this->comboValue->nNum;
		}
		const bool				Bool() const
		{
			return this->boolValue;
		}
		const char*				ComItem() const
		{
			return this->comboValue->szItem.c_str();
		}
		const EMap<float, float>* FloatList() const
		{
			return this->floatMap;
		}

	private:
		eDataType	m_nType;
		union
		{
			bool  boolValue;
			int32 intValue;
			float floatValue;
			DWORD dwValue;

			VECTORDATA* vecValue;
			STRINGDATA* strValue;
			COMBODATA* comboValue;
			EString* cstrValue;
			EMap<float, float>* floatMap;
		};
	};

	struct EFFECT_PROP  : public CEffectMallocObject
	{
		EFFECT_PROP() {}

		EFFECT_PROP( const char* Name, const EFFECT_DATA::STRINGDATA& Data  )
		{
			memset( PropName, 0, sizeof( PropName ) );
			strncpy( PropName, Name, sizeof( PropName ) - 1 );
			Prop		= Data;
		}
		EFFECT_PROP( const char* Name, int32 Data )
		{
			memset( PropName, 0, sizeof( PropName ) );
			strncpy( PropName, Name, sizeof( PropName ) - 1 );
			Prop		= Data;
		}
		EFFECT_PROP( const char* Name, float Data )
		{
			memset( PropName, 0, sizeof( PropName ) );
			strncpy( PropName, Name, sizeof( PropName ) - 1 );
			Prop		= Data;
		}

		EFFECT_PROP( const char* Name, bool Data )
		{
			memset( PropName, 0, sizeof( PropName ) );
			strncpy( PropName, Name, sizeof( PropName ) - 1 );
			Prop		= Data;
		}

		EFFECT_PROP( const char* Name, CVector3f Data )
		{
			memset( PropName, 0, sizeof( PropName ) );
			strncpy( PropName, Name, sizeof( PropName ) - 1 );
			Prop		= Data;
		}
		EFFECT_PROP( const char* Name, const EFFECT_DATA::COMBODATA& Data )
		{
			memset( PropName, 0, sizeof( PropName ) );
			strncpy( PropName, Name, sizeof( PropName ) - 1 );
			Prop		= Data;
		}
		EFFECT_PROP(const char* Name, const EMap<float, float>& Data)
		{
			memset( PropName, 0, sizeof( PropName ) );
			strncpy( PropName, Name, sizeof( PropName ) - 1 );
			Prop		= Data;
		}

		EString				Name()	const
		{
			return PropName;
		}
		const EFFECT_DATA&	Data()	const
		{
			return Prop;
		}
		EFFECT_DATA&		Get()
		{
			return Prop;
		}
	private:
		char PropName[32];
		EFFECT_DATA	Prop;
	};

	struct ADVANCE_PROP
	{
		float		fPercent;
		EFFECT_DATA Prop;
	};

	class CComboData : public EFFECT_DATA::COMBODATA
	{
	public:
		CComboData(){};
		CComboData( BYTE nN, const EString Items )
		{
			nNum	= nN;
			szItem	= Items;
		};
	};

	class CStringData : public EFFECT_DATA::STRINGDATA
	{
	public:
		CStringData(){};
		CStringData( BYTE flag, const EString str )
		{
			nFlag = flag;
			szStr = str;
		};
	};



	inline bool operator < ( const GUID& arg1, const GUID& arg2 )
	{
		const char* pA1 = (const char*)&arg1;
		const char* pA2 = (const char*)&arg2;

		return memcmp( pA1, pA2, 16 ) < 0;
	}

	inline bool operator > ( const GUID& arg1, const GUID& arg2 )
	{
		const char* pA1 = (const char*)&arg1;
		const char* pA2 = (const char*)&arg2;

		return memcmp( pA1, pA2, 16 ) > 0;
	}

}
