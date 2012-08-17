#include "stdafx.h"
#include "SingleStringEditCtrlImp.h" 
#include "IDName.h"
#include "CEffectMallocObject.h"
#include "PathManager/CPathManager.h"

namespace sqr_tools
{
	class CSingleStrCmd : public CPropBaseCmd<SingleStringEditCtrlImp>
	{
	public:
		CSingleStrCmd( SingleStringEditCtrlImp* pCtrl,const char* Str )
			:CPropBaseCmd(pCtrl)
		{
			m_strOld = GetCtrl()->GetImpSingleStrValue();
			m_strData = Str;
			Execute();
		}

		void _Execute(void)
		{
			GetCtrl()->_SetImpSingleStrValue(m_strData.c_str());
		}

		void _UnExecute(void)
		{
			GetCtrl()->_SetImpSingleStrValue(m_strOld.c_str());
		}

	protected:
		string	m_strOld;
		string	m_strData;
	};

	DEF_UI_IMP(SingleStringEditCtrlImp);

	SingleStringEditCtrlImp::SingleStringEditCtrlImp(void* param)
	{
	}

	SingleStringEditCtrlImp::~SingleStringEditCtrlImp(void)
	{
	}

	void SingleStringEditCtrlImp::SetImpStrWithFullPath(const char* c)
	{
		string workDir;
		string strCPlus = c;
		if( CPathSetImp::GetInst()->GetResFileName(workDir,strCPlus) )
			SetImpSingleStrValue(strCPlus.c_str());	
	}

	void SingleStringEditCtrlImp::SetImpSingleStrValue(const char* c)
	{
		if (m_pCtrlData && string(GetImpSingleStrValue())!=c)
		{
			new CSingleStrCmd(this,c);
		}
	}

	void SingleStringEditCtrlImp::_SetImpSingleStrValue(const char* c)
	{
		if (m_pCtrlData && string(GetImpSingleStrValue())!=c)
		{
			if (m_pCtrlData->m_eDataType == E_STRING || m_pCtrlData->m_eDataType == E_STRINGBUTTON)
			{
				string tempStr = c;
				(*((string*)(m_pCtrlData->m_pData))) = tempStr;
			}
			else if(m_pCtrlData->m_eDataType == E_ESTRING 
				|| m_pCtrlData->m_eDataType == E_ESTRINGBUTTON
				|| m_pCtrlData->m_eDataType == E_CUEESTRING)
			{
				EString tempStr = c;
				(*((EString*)(m_pCtrlData->m_pData))) = tempStr;
			}
			else if(m_pCtrlData->m_eDataType == E_ESETESTRING)
			{
				ESet<EString>* tempSet = (ESet<EString>*)(m_pCtrlData->m_pData);
				tempSet->clear();
				EString str = c;
				char* pStr = (char*)str.c_str();
				for( int32 i = strlen( pStr) - 1; i > 0; --i )
				{
					if( pStr[i] == ',' )
					{
						pStr[i] = 0;
						EString Temp = pStr + i + 1;
						if( !Temp.empty() )
							tempSet->insert( Temp );
					}
				}
				tempSet->insert( pStr );
			}
			else if(m_pCtrlData->m_eDataType == E_EVECESTRING)
			{
				EVector<EString>* tempSet = (EVector<EString>*)(m_pCtrlData->m_pData);
				tempSet->clear();
				EString str = c;
				char* pStr = (char*)str.c_str();
				for( int32 i = strlen( pStr) - 1; i > 0; --i )
				{
					if( pStr[i] == ',' )
					{
						pStr[i] = 0;
						EString Temp = pStr + i + 1;
						if( !Temp.empty() )
							tempSet->push_back( Temp );
					}
				}
				tempSet->push_back( pStr );
			}
			else if(m_pCtrlData->m_eDataType == E_COLORSTRING)
			{
				uint32* tempColor = (uint32*)(m_pCtrlData->m_pData);
				(*tempColor) = strtoul( ( "0x" + EString(c) ).c_str(), 0, 0 );
			}
			else if(m_pCtrlData->m_eDataType == E_CHARPTR)
			{
				IDNAME* tempName = (IDNAME*)(m_pCtrlData->m_pData);
				(*tempName) = c;
			}
			OnDataChanged();
		}
	}

	const char* SingleStringEditCtrlImp::GetImpSingleStrValue()
	{
		if (m_pCtrlData)
		{
			if (m_pCtrlData->m_eDataType == E_STRING || m_pCtrlData->m_eDataType == E_STRINGBUTTON)
			{
				return (*((string*)(m_pCtrlData->m_pData))).c_str();
			}
			else if(m_pCtrlData->m_eDataType == E_ESTRING 
				|| m_pCtrlData->m_eDataType == E_ESTRINGBUTTON
				|| m_pCtrlData->m_eDataType == E_CUEESTRING)
			{
				return (*((EString*)(m_pCtrlData->m_pData))).c_str();
			}
			else if(m_pCtrlData->m_eDataType == E_ESETESTRING)
			{
				m_sTempStr = "";
				ESet<EString>* tempSet = (ESet<EString>*)(m_pCtrlData->m_pData);
				for( ESet<EString>::iterator it = tempSet->begin(); it != tempSet->end(); ++it )
				{
					if( it != tempSet->begin() )
						m_sTempStr += ",";
					m_sTempStr += (*it).c_str();
				}
				return m_sTempStr.c_str();
			}
			else if(m_pCtrlData->m_eDataType == E_EVECESTRING)
			{
				m_sTempStr = "";
				EVector<EString>* tempSet = (EVector<EString>*)(m_pCtrlData->m_pData);
				for( EVector<EString>::iterator it = tempSet->begin(); it != tempSet->end(); ++it )
				{
					if( it != tempSet->begin() )
						m_sTempStr += ",";
					m_sTempStr += (*it).c_str();
				}
				return m_sTempStr.c_str();
			}
			else if(m_pCtrlData->m_eDataType == E_COLORSTRING)
			{
				uint32* tempColor = (uint32*)(m_pCtrlData->m_pData);
				char Buf[32];
				m_sTempStr = _ultoa( *tempColor, Buf, 16 );
				return m_sTempStr.c_str();
			}
			else if(m_pCtrlData->m_eDataType == E_CHARPTR)
			{
				return *(const char**)(m_pCtrlData->m_pData);
			}
		}
		return NULL;
	}
}
