#include "stdafx.h"
#include "AutoPropData.h"
#include "CEffectMallocObject.h"
#include "TSqrAllocator.inl"
#include "CVector3.h"

namespace sqr_tools
{
	ECtrlData::ECtrlData(ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj,  void* data)
	{
		m_eDataType = tp;
		m_pFunc		= fun;
		m_pObj		= obj;
		m_pData		= data;
		m_pHelpInfo	= NULL;
		m_pAssistInfo = NULL;
	}

	ECtrlData::ECtrlData(ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj,  const void* helpinfo, void* data)
	{
		m_eDataType = tp;
		m_pFunc		= fun;
		m_pObj		= obj;
		m_pData		= data;
		if(helpinfo)
			m_pHelpInfo	= new string(static_cast<const char*>(helpinfo));
		else 
			m_pHelpInfo	= NULL;

		m_pAssistInfo = NULL;
	}

	ECtrlData::ECtrlData( ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj, const void* helpinfo, void* assitInfo, void* data )
	{
		m_eDataType = tp;
		m_pFunc		= fun;
		m_pObj		= obj;
		m_pData		= data;
		if(helpinfo)
			m_pHelpInfo	= new string(static_cast<const char*>(helpinfo));
		else 
			m_pHelpInfo	= NULL;

		m_pAssistInfo = assitInfo;
	}

	ECtrlData::~ECtrlData()
	{
		if (m_pHelpInfo)
			delete m_pHelpInfo;	
			
	}

	ECtrlData* ECtrlData::CreateCtrlData(ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj
		, const void* helpinfo, void* data)
	{
		ECtrlData*	tempData = NULL;
		switch(tp)
		{
		case E_INTEGER:
		case E_FLOAT:
		case E_BYTE:
		case E_BOOLValue:
		case E_DBWORD:
		case E_VECTOR:
		case E_VECTOR2F:
		case E_STRING:
		case E_STRINGBUTTON:
		case E_ESTRING:
		case E_ESTRINGBUTTON:
		case E_ESETESTRING:
		case E_EVECESTRING:
		case E_COLORSTRING:
		case E_CHARPTR:
		case E_CUEESTRING:
		case E_COLOR4Map:
		case E_COLORMap:
		case E_UINT32CLMap:
		case E_FLOATCLMap:
		case E_ECOLORMap:
			{
				tempData = new ECtrlData(tp, fun, obj,  helpinfo, data);
			}
			break;
		case E_COMBO:
			{
				CComboCtrlData* tempCom = new CComboCtrlData(data);
				tempData = new ECtrlData(tp, fun, obj, NULL, tempCom->m_pHelper, tempCom);
			}
			break;
		case E_FloatMap:
		case E_EFloatMap:
		case E_BYTEMap:
		case E_EBYTEMap:
			{
				CFloatMapData* temNumMap = new CFloatMapData(tp, data);
				tempData = new ECtrlData(tp, fun, obj,  helpinfo, temNumMap);
			}
			break;
		case E_VectorMap:
		case E_EVectorMap:
			{
				CVector3fMapData* temVecMap = new CVector3fMapData(tp, data);
				tempData = new ECtrlData(tp, fun, obj,  helpinfo, temVecMap);
			}
			break;
		case E_EEStrMap:
			{
				CEStrMapData* temESMap = new CEStrMapData(tp, data);
				tempData = new ECtrlData(tp, fun, obj,  helpinfo, temESMap);
			}
			break;
		default:
			return NULL;
		}

		return tempData;
	}

	EAutoCtrlData::EAutoCtrlData(const char* title, ECtrlData* data)
	{
		m_Title	= title;
		m_pCtrlData		= data; 
	}

	EAutoCtrlData::~EAutoCtrlData()
	{
		if(m_pCtrlData)
			delete m_pCtrlData;
	}

	CFloatMapData::CFloatMapData(ECtrlDataType tp, void* pMap)
	{
		m_eMapType = tp;
		m_pMap = pMap;
	}

	void CFloatMapData::SetMapData(float idx, float val)
	{
		switch(m_eMapType)
		{
		case E_FloatMap:
			(*(static_cast<map<float,float>*>(m_pMap)))[idx] = val;
			break;
		case E_EFloatMap:
			(*(static_cast<EMap<float,float>*>(m_pMap)))[idx] = val;
			break;
		case E_BYTEMap:
			(*(static_cast<map<float,BYTE>*>(m_pMap)))[idx] = (BYTE)val;
			break;
		case E_EBYTEMap:
			(*(static_cast<EMap<float,BYTE>*>(m_pMap)))[idx] = (BYTE)val;
			break;
		default:
			return;
		}
	}

	void CFloatMapData::EraseMapData(float idx)
	{
		switch(m_eMapType)
		{
		case E_FloatMap:
			(*(static_cast<map<float,float>*>(m_pMap))).erase( idx );
			break;
		case E_EFloatMap:
			(*(static_cast<EMap<float,float>*>(m_pMap))).erase( idx );
			break;
		case E_BYTEMap:
			(*(static_cast<map<float,BYTE>*>(m_pMap))).erase( idx );
			break;
		case E_EBYTEMap:
			(*(static_cast<EMap<float,BYTE>*>(m_pMap))).erase( idx );
			break;
		default:
			return;
		}
	}

	int CFloatMapData::GetMapCount()
	{
		switch(m_eMapType)
		{
		case E_FloatMap:
			return (int)((*(static_cast<map<float,float>*>(m_pMap))).size());
			break;
		case E_EFloatMap:
			return (int)((*(static_cast<EMap<float,float>*>(m_pMap))).size());
			break;
		case E_BYTEMap:
			return (int)((*(static_cast<map<float,BYTE>*>(m_pMap))).size());
			break;
		case E_EBYTEMap:
			return (int)((*(static_cast<EMap<float,BYTE>*>(m_pMap))).size());
			break;
		default:
			return 0;
		}
	}

	float CFloatMapData::GetMapIndex(int n)
	{
		switch(m_eMapType)
		{
		case E_FloatMap:
			{
				map<float,float>* tempMap = static_cast<map<float,float>*>(m_pMap);
				map<float,float>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return itr->first;
				return 0.0f;
			}
		case E_EFloatMap:
			{
				EMap<float,float>* tempMap = static_cast<EMap<float,float>*>(m_pMap);
				EMap<float,float>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return itr->first;
				return 0.0f;
			}
		case E_BYTEMap:
			{
				map<float,BYTE>* tempMap = static_cast<map<float,BYTE>*>(m_pMap);
				map<float,BYTE>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return itr->first;
				return 0.0f;
			}
			break;
		case E_EBYTEMap:
			{
				EMap<float,BYTE>* tempMap = static_cast<EMap<float,BYTE>*>(m_pMap);
				EMap<float,BYTE>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return itr->first;
				return 0.0f;
			}
			break;
		default:
			return 0.0f;
		}
	}

	float CFloatMapData::GetMapValue(int n)
	{
		switch(m_eMapType)
		{
		case E_FloatMap:
			{
				map<float,float>* tempMap = static_cast<map<float,float>*>(m_pMap);
				map<float,float>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return itr->second;
				return 0.0f;
			}
		case E_EFloatMap:
			{
				EMap<float,float>* tempMap = static_cast<EMap<float,float>*>(m_pMap);
				EMap<float,float>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return itr->second;
				return 0.0f;
			}
		case E_BYTEMap:
			{
				map<float,BYTE>* tempMap = static_cast<map<float,BYTE>*>(m_pMap);
				map<float,BYTE>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return float(itr->second);
				return 0.0f;
			}
			break;
		case E_EBYTEMap:
			{
				EMap<float,BYTE>* tempMap = static_cast<EMap<float,BYTE>*>(m_pMap);
				EMap<float,BYTE>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return float(itr->second);
				return 0.0f;
			}
			break;
		default:
			return 0.0f;
		}
	}

	bool CFloatMapData::GetMapValue(float idx,float& value)
	{
		switch(m_eMapType)
		{
		case E_FloatMap:
			{
				map<float,float>* tempMap = static_cast<map<float,float>*>(m_pMap);
				map<float,float>::iterator itr = tempMap->find(idx);
				if (itr != tempMap->end())
				{
					value = itr->second;
					return true;
				}
				return false;
			}
		case E_EFloatMap:
			{
				EMap<float,float>* tempMap = static_cast<EMap<float,float>*>(m_pMap);
				EMap<float,float>::iterator itr = tempMap->find(idx);
				if (itr != tempMap->end())
				{
					value = itr->second;
					return true;
				}
				return false;
			}
		case E_BYTEMap:
			{
				map<float,BYTE>* tempMap = static_cast<map<float,BYTE>*>(m_pMap);
				map<float,BYTE>::iterator itr = tempMap->find(idx);
				if (itr != tempMap->end())
				{
					value = float(itr->second);
					return true;
				}
				return false;
			}
			break;
		case E_EBYTEMap:
			{
				EMap<float,BYTE>* tempMap = static_cast<EMap<float,BYTE>*>(m_pMap);
				EMap<float,BYTE>::iterator itr = tempMap->find(idx);
				if (itr != tempMap->end())
				{
					value = float(itr->second);
					return true;
				}
				return false;
			}
			break;
		default:
			return 0.0f;
		}
	}

	CVector3fMapData::CVector3fMapData(ECtrlDataType tp, void* pMap)
	{
		m_eMapType = tp;
		m_pMap = pMap;
	}

	int CVector3fMapData::GetVector3MapCount()
	{
		switch(m_eMapType)
		{
		case E_VectorMap:
			return (int)((*(static_cast<map<float,CVector3f>*>(m_pMap))).size());
			break;
		case E_EVectorMap:
			return (int)((*(static_cast<EMap<float,CVector3f>*>(m_pMap))).size());
			break;
		default:
			return 0;
		}
	}

	float CVector3fMapData::GetVector3MapIndex(int n)
	{
		switch(m_eMapType)
		{
		case E_VectorMap:
			{
				map<float,CVector3f>* tempMap = static_cast<map<float,CVector3f>*>(m_pMap);
				map<float,CVector3f>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return itr->first;
				return 0.0f;
			}
		case E_EVectorMap:
			{
				EMap<float,CVector3f>* tempMap = static_cast<EMap<float,CVector3f>*>(m_pMap);
				EMap<float,CVector3f>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return itr->first;
				return 0.0f;
			}
		default:
			return 0.0f;
		}
	}

	float CVector3fMapData::GetVector3MapXVal(float idx)
	{
		switch(m_eMapType)
		{
		case E_VectorMap:
			{
				map<float,CVector3f>* tempMap = static_cast<map<float,CVector3f>*>(m_pMap);
				map<float,CVector3f>::iterator itr = tempMap->find(idx);

				if (itr != tempMap->end())
				{
					CVector3f* vec = (CVector3f*)(&(itr->second));
					return vec->x;
				}
				return NULL;
			}
		case E_EVectorMap:
			{
				EMap<float,CVector3f>* tempMap = static_cast<EMap<float,CVector3f>*>(m_pMap);
				EMap<float,CVector3f>::iterator itr = tempMap->find(idx);

				if (itr != tempMap->end())
				{
					CVector3f* vec = (CVector3f*)(&(itr->second));
					return vec->x;
				}
				return NULL;
			}
		default:
			return NULL;
		}
	}

	float CVector3fMapData::GetVector3MapYVal(float idx)
	{
		switch(m_eMapType)
		{
		case E_VectorMap:
			{
				map<float,CVector3f>* tempMap = static_cast<map<float,CVector3f>*>(m_pMap);
				map<float,CVector3f>::iterator itr = tempMap->find(idx);

				if (itr != tempMap->end())
				{
					CVector3f* vec = (CVector3f*)(&(itr->second));
					return vec->y;
				}
				return NULL;
			}
		case E_EVectorMap:
			{
				EMap<float,CVector3f>* tempMap = static_cast<EMap<float,CVector3f>*>(m_pMap);
				EMap<float,CVector3f>::iterator itr = tempMap->find(idx);

				if (itr != tempMap->end())
				{
					CVector3f* vec = (CVector3f*)(&(itr->second));
					return vec->y;
				}
				return NULL;
			}
		default:
			return NULL;
		}
	}

	float CVector3fMapData::GetVector3MapZVal(float idx)
	{
		switch(m_eMapType)
		{
		case E_VectorMap:
			{
				map<float,CVector3f>* tempMap = static_cast<map<float,CVector3f>*>(m_pMap);
				map<float,CVector3f>::iterator itr = tempMap->find(idx);

				if (itr != tempMap->end())
				{
					CVector3f* vec = (CVector3f*)(&(itr->second));
					return vec->z;
				}
				return NULL;
			}
		case E_EVectorMap:
			{
				EMap<float,CVector3f>* tempMap = static_cast<EMap<float,CVector3f>*>(m_pMap);
				EMap<float,CVector3f>::iterator itr = tempMap->find(idx);

				if (itr != tempMap->end())
				{
					CVector3f* vec = (CVector3f*)(&(itr->second));
					return vec->z;
				}
				return NULL;
			}
		default:
			return NULL;
		}
	}

	void CVector3fMapData::InsertMapNode(float idx, CVector3f& vec3f)
	{
		switch(m_eMapType)
		{
		case E_VectorMap:
			(*(static_cast<map<float,CVector3f>*>(m_pMap)))[idx] = vec3f;
			break;
		case E_EVectorMap:
			(*(static_cast<EMap<float,CVector3f>*>(m_pMap)))[idx] = vec3f;
			break;
		default:
			return;
		}
	}

	void CVector3fMapData::EraseMapNode(float idx)
	{
		switch(m_eMapType)
		{
		case E_VectorMap:
			(*(static_cast<map<float,CVector3f>*>(m_pMap))).erase( idx );
			break;
		case E_EVectorMap:
			(*(static_cast<EMap<float,CVector3f>*>(m_pMap))).erase( idx );
			break;
		default:
			return;
		}
	}

	CVector3f* CVector3fMapData::GetVector3Node(float idx)
	{
		switch(m_eMapType)
		{
		case E_VectorMap:
			{
				map<float,CVector3f>* tempMap = static_cast<map<float,CVector3f>*>(m_pMap);
				map<float,CVector3f>::iterator itr = tempMap->find(idx);
				if (itr != tempMap->end())
				{
					CVector3f* vec = (CVector3f*)(&(itr->second));
					return vec;
				}
				return NULL;
			}
		case E_EVectorMap:
			{
				EMap<float,CVector3f>* tempMap = static_cast<EMap<float,CVector3f>*>(m_pMap);
				EMap<float,CVector3f>::iterator itr = tempMap->find(idx);

				if (itr != tempMap->end())
				{
					CVector3f* vec = (CVector3f*)(&(itr->second));
					return vec;
				}
				return NULL;
			}
		default:
			return NULL;
		}
	}

	CEStrMapData::CEStrMapData(ECtrlDataType tp, void* pMap)
	{
		m_eMapType = tp;
		m_pMap = pMap;
	}

	int	CEStrMapData::GetEStrMapCount()
	{
		switch(m_eMapType)
		{
		case E_EEStrMap:
			return (int)((*(static_cast<EMap<float,EString>*>(m_pMap))).size());
			break;
		default:
			return 0;
		}
	}

	float	CEStrMapData::GetEStrMapIndex(int n)
	{
		switch(m_eMapType)
		{
		case E_EEStrMap:
			{
				EMap<float,EString>* tempMap = static_cast<EMap<float,EString>*>(m_pMap);
				EMap<float,EString>::iterator itr = tempMap->begin();
				int i = 0;
				while (i < n && itr != tempMap->end())
				{
					++itr;
					++i;
				}
				if (itr != tempMap->end())
					return itr->first;
				return 0.0f;
			}
		default:
			return 0.0f;
		}
	}

	void	CEStrMapData::InsertMapNode(float idx, const char* estr)
	{
		if(!estr)
			return;
		switch(m_eMapType)
		{
		case E_EEStrMap:
			{
				EString str = estr;
				(*(static_cast<EMap<float,EString>*>(m_pMap)))[idx] = str;
			}
			break;
		default:
			return;
		}
	}

	void	CEStrMapData::SetNodeEStr(float idx, const char* tex)
	{
		switch(m_eMapType)
		{
		case E_EEStrMap:
			{
				EString str = tex;
				(*(static_cast<EMap<float,EString>*>(m_pMap)))[idx] = str;
			}
			break;
		default:
			return;
		}
	}

	void	CEStrMapData::EraseMapNode(float idx)
	{
		switch(m_eMapType)
		{
		case E_EEStrMap:
			(*(static_cast<EMap<float,EString>*>(m_pMap))).erase( idx );
			break;
		default:
			return;
		}
	}

	const char*	CEStrMapData::GetEStrNode(float idx)
	{
		switch(m_eMapType)
		{
		case E_EEStrMap:
			{
				EMap<float,EString>* tempMap = static_cast<EMap<float,EString>*>(m_pMap);
				EMap<float,EString>::iterator itr = tempMap->find(idx);
				if (itr != tempMap->end())
				{
					EString* str = (EString*)(&(itr->second));
					return str->c_str();
				}
				return NULL;
			}
		default:
			return NULL;
		}
	}

	CCombDataHelper::CombData::CombData( const string& name_in, int value_in )
		: name(name_in)
		, value(value_in)
	{
	}

	void CCombDataHelper::PushItem( string name, int value )
	{
		m_data.push_back(CombData(name, value));
	}

	int CCombDataHelper::GetValueByIdx( int selIdx )
	{
		if ( selIdx>=0 && (size_t)selIdx < m_data.size() )
			return m_data[selIdx].value;
		else
			return NULL;
	}

	int CCombDataHelper::GetIdxByValue( int value )
	{
		int i = 0;
		for ( ; (size_t)i<m_data.size(); ++i )
		{
			if ( m_data[i].value == value )
				return i;
		}

		return -1;
	}

	int CCombDataHelper::GetItemNum() const
	{
		return m_data.size();
	}

	const char* CCombDataHelper::GetItemName( int idx ) const
	{
		if ( idx>=0 && (size_t)idx < m_data.size() )
			return m_data[idx].name.c_str();
		else
			return "";
	}

	void CCombDataHelper::Reset()
	{
		m_data.clear();
	}

	CComboCtrlData::CComboCtrlData( void* val )
		: m_pVal(val)
		, m_pHelper(new CCombDataHelper())
	{
	};

	CComboCtrlData::~CComboCtrlData()
	{
		if ( m_pHelper )
			delete m_pHelper;
	}

	void CComboCtrlData::ResetHelper()
	{
		if ( m_pHelper )
			m_pHelper->Reset();
		else
			m_pHelper = new CCombDataHelper();
	}
}