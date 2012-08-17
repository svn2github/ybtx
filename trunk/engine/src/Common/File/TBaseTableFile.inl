#pragma once 
#include "CVariant.h"
#include "CodeCvs.h"

namespace sqr 
{

template<typename ImpType>
TBaseTableFile<ImpType>::TBaseTableFile()
: m_pImp(NULL)
{

}

template<typename ImpType>
TBaseTableFile<ImpType>::~TBaseTableFile()
{
	if (m_pImp)
	{
		ImpType::Lock();
		m_pImp->DelRef();
		ImpType::UnLock();
	}
}

inline bool LessFileName(const char* szLeft, const char* szRight)
{
	return (strcmp(szLeft, szRight) < 0) ? true : false;
}

template<typename ImpType>
bool TBaseTableFile<ImpType>::IntLoad( const wchar_t* alias, VecFileName_t& vecFileName )
{
	sort(vecFileName.begin(), vecFileName.end(), LessFileName);

	ImpType::Lock();

	pair<ImpType*, bool> PairRet = ImpType::GetImp(alias, vecFileName);
	
	ImpType* pImp = PairRet.first;
	bool bNewImp = PairRet.second;

	if (m_pImp)
	{
		if(m_pImp != pImp)
		{
			m_pImp->DelRef();
			m_pImp = pImp;
			m_pImp->AddRef();
		}
	}
	else
	{
		m_pImp = pImp;
		m_pImp->AddRef();
	}

	bool bLoad = true;
	if (bNewImp)
	{
		bLoad = m_pImp->Load();
	}

	ImpType::UnLock();

	return bLoad;
}

template<typename ImpType>
bool TBaseTableFile<ImpType>::Load( const char* alias, vector<CVariant*>* pArgList )
{
	string sAliasPath = alias ? alias : "";
	return Load(utf8_to_utf16(sAliasPath).c_str(), pArgList);
}

template<typename ImpType>
bool TBaseTableFile<ImpType>::Load( const wchar_t* szAliasPath, vector<CVariant*>* pArgList )
{
	wstring sAliasPath = szAliasPath ? szAliasPath : L"";

	size_t stSize = pArgList->size();
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
	}

	return IntLoad(sAliasPath.c_str(), vecFileName);
}

template<typename ImpType>
bool TBaseTableFile<ImpType>::Load( const char* alias, const char* szFileName )
{
	string sAliasPath = alias ? alias : "";
	return Load(utf8_to_utf16(sAliasPath).c_str(), szFileName);
}

template<typename ImpType>
bool TBaseTableFile<ImpType>::Load( const wchar_t* szAliasPath, const char* szFileName )
{
	wstring sAliasPath = szAliasPath ? szAliasPath : L"";
	
	VecFileName_t vecFileName(1, szFileName);

	return IntLoad(sAliasPath.c_str(), vecFileName);
}

template<typename ImpType>
bool TBaseTableFile<ImpType>::Reload(const char* szFileName)
{
	Ast(m_pImp);
	VecFileName_t vecFileName(1, szFileName);
	
	return m_pImp->Reload(vecFileName);
}

template<typename ImpType>
bool TBaseTableFile<ImpType>::Reload(vector<CVariant*>* pArgList)
{
	Ast(m_pImp);
	size_t stSize = pArgList->size();

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
	}

	return m_pImp->Reload(vecFileName);
}

}
