#include "stdafx.h"
#include "CMultiLangTxtTableFile.h"
#include "CMultiLangTxtTableFileImp.h"
#include "CCurLangTxtTableFileImp.h"
#include "ErrLogHelper.h"
#include "CodeCvs.h"
#include "CVariant.h"
#include "ExpHelper.h"
#include "CTableFileValue.h"
#include "CAppConfigServer.h"
#include "CLangCfg.h"

string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value,string::size_type pos)   
{   
	for( ;   pos!=string::npos;   pos+=new_value.length())  
	{   
		if( (pos=str.find(old_value,pos)) != string::npos )   
			str.replace(pos,old_value.length(),new_value);   
		else
			break;   
	}   
	return   str;   
}

CMultiLangTxtTableFile::CMultiLangTxtTableFile(void)
: Parent_t()
, m_pCurLangFile(NULL)
{
	m_pCurLangFile = new CurLangFile_t();
}

CMultiLangTxtTableFile::~CMultiLangTxtTableFile(void)
{
	delete m_pCurLangFile;
	m_pCurLangFile = NULL;
}

bool CMultiLangTxtTableFile::Load( const char* alias, vector<CVariant*>* pArgList )
{
	string sAliasPath = alias ? alias : "";
	return Load(utf8_to_utf16(sAliasPath).c_str(), pArgList);
}

bool CMultiLangTxtTableFile::Load( const wchar_t* szAliasPath, vector<CVariant*>* pArgList )
{
	const string& strOriLangPath = GetOriLangPath();
	//CLangCfg::GetInst()->GetOriLanFolder();
	//CAppConfigServer::Inst()->GetOriLanFolder();

	size_t stSize = pArgList->size();
	if(stSize == 0)
		return false;

	CVariant* pVariant = pArgList->at(0);
	string strCurLangPath = pVariant->GetType() ==
		eVVT_CStr ? pVariant->GetCStr() : pVariant->GetString();

	size_t uPos = strCurLangPath.find('/');
	if(uPos != string::npos)	
	{
		strCurLangPath = strCurLangPath.substr(0, uPos);
	}

	bool bUseMultiLang = bool(strOriLangPath != strCurLangPath);

	if(!bUseMultiLang)
	{
		return Parent_t::Load( szAliasPath, pArgList );
	}

	wstring sAliasPath = szAliasPath ? szAliasPath : L"";

	VecFileName_t vecFileName(stSize);

	for (size_t i = 0; i < stSize; i++)
	{
		CVariant* pVariant = pArgList->at(i);
		EValueVariantType eType = pVariant->GetType();
		if (eType == eVVT_CStr)
		{
			vecFileName[i] = pVariant->GetCStr();	
		}
		else
		{
			vecFileName[i] = pVariant->GetString();	
		}
		//cout << "vecFileName[" << i << "] = " << vecFileName[i] << endl;
	}

	m_pCurLangFile->IntLoad(sAliasPath.c_str(), vecFileName);
	//bool bPrintLog = false;
	//if(strstr(vecFileName[0], "Lan_AttributeOfJingLingSkill_Common"))
	//{
	//	bPrintLog = true;
	//}

	m_pCurLangFile->m_pImp->BuildKeyMap();

	VecFileName_t vecSzOriFileName;
	vector<string> vecStrOriFileName;
	for(size_t i = 0; i < stSize; ++i)
	{
		vecStrOriFileName.push_back(vecFileName[i]);
		replace_all_distinct(vecStrOriFileName[i], strCurLangPath, strOriLangPath, 0);
		vecSzOriFileName.push_back(vecStrOriFileName[i].c_str());
	}

	bool bRet = this->IntLoad(sAliasPath.c_str(), vecSzOriFileName);

	m_pImp->SetUseMultiLang(bUseMultiLang);

	if(!bRet)
		return false;
	else
	{
		m_pImp->RedirectOffset(m_pCurLangFile->m_pImp);
		return true;
	}

}





void CMultiLangTxtTableFile::Release()
{
	delete this;
}

int32 CMultiLangTxtTableFile::GetHeight()
{
	Ast(m_pImp);
	return m_pImp->GetHeight();	
}

int32 CMultiLangTxtTableFile::GetWidth()
{
	Ast(m_pImp);
	return m_pImp->GetWidth();
}

ETableFileFieldType CMultiLangTxtTableFile::GetType( int32 nColumn )
{
	Ast(m_pImp);
	return m_pImp->GetType(nColumn);
}

ETableFileFieldType CMultiLangTxtTableFile::GetType( const char* szColumnName )
{
	Ast(m_pImp);
	return m_pImp->GetType(szColumnName);
}

const char* CMultiLangTxtTableFile::GetString( int32 nRow, int32 nColumn )
{
	Ast(m_pImp);
	return m_pImp->GetString(nRow, nColumn);
}

const char* CMultiLangTxtTableFile::GetString( int32 nRow, const char* szColumnName )
{
	Ast(m_pImp);
	return m_pImp->GetString(nRow, szColumnName);
}

int32 CMultiLangTxtTableFile::GetInteger( int32 nRow, int32 nColumn, int32 defaultvalue )
{
	const char* szStr = GetString( nRow, nColumn );
	return szStr[0] ? atoi( szStr ) : defaultvalue;
}

int32 CMultiLangTxtTableFile::GetInteger( int32 nRow, const char* szColumnName, int32 defaultvalue )
{
	const char* szStr = GetString( nRow, szColumnName );
	return szStr[0] ? atoi( szStr ) : defaultvalue;
}

float CMultiLangTxtTableFile::GetFloat( int32 nRow, int32 nColumn, float defaultvalue )
{
	const char* szStr = GetString( nRow, nColumn );
	return szStr[0] ? (float)atof( szStr ) : defaultvalue;
}

float CMultiLangTxtTableFile::GetFloat( int32 nRow, const char* szColumnName, float defaultvalue )
{
	const char* szStr = GetString( nRow, szColumnName );
	return szStr[0] ? (float)atof( szStr ) : defaultvalue;
}

template<typename T>
T CMultiLangTxtTableFile::GetNumber( int32 nRow, int32 nColumn, T defaultvalue)
{
	const char* szStr = GetString( nRow, nColumn );
	return szStr[0] ? static_cast<T>(atof( szStr )) : defaultvalue;
}

template<typename T>
T CMultiLangTxtTableFile::GetNumber( int32 nRow, const char* szColumnName, T defaultvalue )
{
	const char* szStr = GetString( nRow, szColumnName );
	return szStr[0] ? static_cast<T>(atof( szStr )) : defaultvalue;
}

template int8 CMultiLangTxtTableFile::GetNumber<int8>(int32, int32, int8);
template uint8 CMultiLangTxtTableFile::GetNumber<uint8>(int32, int32, uint8);
template int16 CMultiLangTxtTableFile::GetNumber<int16>(int32, int32, int16);
template uint16 CMultiLangTxtTableFile::GetNumber<uint16>(int32, int32, uint16);
template int32 CMultiLangTxtTableFile::GetNumber<int32>(int32, int32, int32);
template uint32 CMultiLangTxtTableFile::GetNumber<uint32>(int32, int32, uint32);
template int64 CMultiLangTxtTableFile::GetNumber<int64>(int32, int32, int64);
template uint64 CMultiLangTxtTableFile::GetNumber<uint64>(int32, int32, uint64);
template float CMultiLangTxtTableFile::GetNumber<float>(int32, int32, float);
template double CMultiLangTxtTableFile::GetNumber<double>(int32, int32, double);

template int8 CMultiLangTxtTableFile::GetNumber<int8>(int32, const char*, int8);
template uint8 CMultiLangTxtTableFile::GetNumber<uint8>(int32, const char*, uint8);
template int16 CMultiLangTxtTableFile::GetNumber<int16>(int32, const char*, int16);
template uint16 CMultiLangTxtTableFile::GetNumber<uint16>(int32, const char*, uint16);
template int32 CMultiLangTxtTableFile::GetNumber<int32>(int32, const char*, int32);
template uint32 CMultiLangTxtTableFile::GetNumber<uint32>(int32, const char*, uint32);
template int64 CMultiLangTxtTableFile::GetNumber<int64>(int32, const char*, int64);
template uint64 CMultiLangTxtTableFile::GetNumber<uint64>(int32, const char*, uint64);
template float CMultiLangTxtTableFile::GetNumber<float>(int32, const char*, float);
template double CMultiLangTxtTableFile::GetNumber<double>(int32, const char*, double);

CVariant* CMultiLangTxtTableFile::GetValue(CTableFileValue* pValue, int32 nRow, const char* szColumnName)
{
	ETableFileFieldType eType = GetType(szColumnName);

	switch(eType)
	{
	case eTFFT_String:
		{
			pValue->SetCStr(GetString(nRow, szColumnName));
			return pValue->GetValue();
		}
		break;
	case eTFFT_Number:
		{
			pValue->SetNumber(GetNumber<double>(nRow, szColumnName, 0));
			return pValue->GetValue();
		}
		break;
	default:
		break;
	}
	return NULL;

}

CVariant* CMultiLangTxtTableFile::GetValue(CTableFileValue* pValue, int32 nRow, int32 nCol)
{
	ETableFileFieldType eType = GetType(nCol);

	switch(eType)
	{
	case eTFFT_String:
		{
			pValue->SetCStr(GetString(nRow, nCol));
			return pValue->GetValue();
		}
		break;
	case eTFFT_Number:
		{
			pValue->SetNumber(GetNumber<double>(nRow, nCol, 0));
			return pValue->GetValue();
		}
		break;
	default:
		break;
	}
	return NULL;

}
