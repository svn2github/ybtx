#include "stdafx.h"
#include "CCypherVariant.h"
#include "CypherVariantDef.h"
#include "CCypherVariantCodeTable.h"
#include "ICypherVariantHolderHandler.h"
#include "CCypherVariantHolder.h"
#include "TSqrAllocator.inl"
#include "TVariantStream.h"
#include "TVariantStream.inl"

namespace sqr
{
	struct CCypherVariantMemberAdder
	{
		static bool OnMemberAdded(const void* pHolder, const char* szCypherVariantCodeName, CCypherVariant& variant)
		{
			reinterpret_cast<CCypherVariantHolder*>(const_cast<void*>(pHolder))->OnMemberAdded(&variant);
			return true;
		}
	};
}

void CCypherVariantHolder::Reset()
{
	m_pRootVariant->ResetDataMember();
}

CCypherVariant* CCypherVariantHolder::BuildSubTree(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex, const char* szNodeName)
{
	CCypherVariantDynCode* pParentCode = class_cast<CCypherVariantDynCode*>(const_cast<CBaseCypherVariantCode*>(pRoot->GetCode()));
	CCypherVariantCode* pDynRootCodeInfo = class_cast<CCypherVariantCode*>(m_pCodeTable->m_DynamicMap[uSubTreeType]);

	size_t uSubTreeRootId = CCypherVariantDynCode::GenerateSubTreePath(pParentCode->GetID(), uSubTreeIndex, uSubTreeType, 0);

	CCypherVariantDynCodeForEncode* pNewSubTreeRoot = new CCypherVariantDynCodeForEncode(uint32(0), uint32(uSubTreeRootId),
		szNodeName, true, pParentCode);
	pParentCode->AddEntity(pNewSubTreeRoot, uSubTreeIndex);
	CCypherVariant* pSubTreeRootVar = pRoot->SetMemberWithCode(pNewSubTreeRoot);

	for (size_t i = 0; i < pDynRootCodeInfo->m_IDMap.size(); ++i)
	{
		uint32 uDynamicId = (uint32)uSubTreeRootId+i+1;
		const char* varName = pDynRootCodeInfo->m_IDMap[i]->m_szName.c_str();
		CCypherVariantDynCodeForEncode* pNewSubCode = new CCypherVariantDynCodeForEncode(uint32(0), 
			uDynamicId, varName, false, pNewSubTreeRoot);
		pNewSubTreeRoot->AddEntity(pNewSubCode, uint32(i));
		pSubTreeRootVar->SetMemberWithCode(pNewSubCode);
	}
	OnMemberAdded(pSubTreeRootVar);
	pSubTreeRootVar->EnumMember(this, &CCypherVariantMemberAdder::OnMemberAdded);
	return pSubTreeRootVar;
}

CCypherVariant* CCypherVariantHolder::CreateVariant()const
{
	return new CCypherVariant;
}

CCypherVariantHolder::CCypherVariantHolder(CCypherVariantCodeTable* pCodeTable)
:m_pCodeTable(pCodeTable)
,m_pDynCodeRoot(NULL)
,m_pRootVariant(NULL)
{
}

CCypherVariantHolder::~CCypherVariantHolder()
{
	//delete m_pDynCodeRoot;
	//m_pDynCodeRoot=NULL;
}

CCypherVariant* CCypherVariantHolder::IntGetRootVariant()const
{
	return m_pRootVariant;
}

CCypherVariant* CCypherVariantHolder::GetRootVariant() const
{
	if( !m_pRootVariant )
	{
		//换成从RootVariant池中拿对象
		m_pRootVariant = CreateVariant();
		if (m_pCodeTable != NULL)
		{
			m_pRootVariant->m_pCode = m_pCodeTable->GetRoot();
		}
	}
	return m_pRootVariant;
}

CCypherVariant* CCypherVariantHolder::IntAddSubTree(CCypherVariant* pRoot, const char* newName, uint32 uSubTreeType)
{
	CCypherVariant* pVar = pRoot->GetMember(newName);
	if (pVar)
		return pVar;

	CBaseCypherVariantCode* pParent = const_cast<CBaseCypherVariantCode*>(pRoot->GetCode());
	CCypherVariantTreeRootCodeForEncode* pCode = static_cast<CCypherVariantTreeRootCodeForEncode*>(pParent);
	uint32 uSubTreeIndex = pCode->GetNewChildId();
	Ast(pParent->GetID() < 256);

	return BuildSubTree(pRoot, uSubTreeType, uSubTreeIndex, newName);
}

void CCypherVariantHolder::InitSubTree(CCypherVariant* pRoot, CCypherVariant* pSubTree, const char* szSubTreeRootName, uint32 uSubTreeRootCodeIndex, uint32 uSubTreeType)
{
	Ast(pRoot);
	Ast(pSubTree);

	CCypherVariantDynCode* pParentCode = static_cast<CCypherVariantDynCode*>(const_cast<CBaseCypherVariantCode*>(pRoot->GetCode()));

	size_t uSubTreeRootId = CCypherVariantDynCode::GenerateSubTreePath(pParentCode->GetID(), uSubTreeRootCodeIndex, uSubTreeType, 0);	

	CCypherVariantDynCodeForEncode* pSubTreeRootCode = static_cast<CCypherVariantDynCodeForEncode*>(const_cast
<CBaseCypherVariantCode*>(pSubTree->GetCode()));
	pSubTreeRootCode->m_szName = szSubTreeRootName;
	pSubTreeRootCode->m_uDynamicId = uint32(uSubTreeRootId);
	pSubTreeRootCode->m_pParent = pParentCode;
	for(CBaseCypherVariantCode::NameMapType::iterator it = pSubTreeRootCode->m_NameMap.begin(); it != pSubTreeRootCode->m_NameMap.end(); ++it)
	{
		CCypherVariantDynCodeForEncode* pChildCode = static_cast<CCypherVariantDynCodeForEncode*>(it->second); 
		pChildCode->m_uDynamicId = (pChildCode->m_uDynamicId & 0xf) | uSubTreeRootId;
	}

	pParentCode->AddEntity(pSubTreeRootCode, uSubTreeRootCodeIndex);
	pRoot->SetMemberWithVariant(pSubTree);

	OnMemberAdded(pSubTree);

	pSubTree->EnumMember(this, &CCypherVariantMemberAdder::OnMemberAdded);
}

CCypherVariant* CCypherVariantHolder::AddSubTree(const char* path, const char* subTreeName, const char* newName)
{
	CCypherVariant* pVariant = GetMemberNoExp(path);
	if (!pVariant)
		pVariant = SetMember(path, NULL);
	
	CBaseCypherVariantCode* pParent = const_cast<CBaseCypherVariantCode*>(pVariant->GetCode());
	CCypherVariantTreeRootCodeForEncode* pCode = static_cast<CCypherVariantTreeRootCodeForEncode*>(pParent);

	if (!pCode || !pCode->ValidateSubTreeType(subTreeName))
	{
		GenErr("subTreeName can't be added under pCode\n");
	}

	uint32 uSubTreeType = uint32(-1);
	for (size_t i = 0; i < m_pCodeTable->m_DynamicMap.size(); ++i)
	{
		if (strcmp(m_pCodeTable->m_DynamicMap[i]->GetName(), subTreeName)==0)
		{
			uSubTreeType = (uint32)i;
			break;
		}
	}
	Ast(uSubTreeType != uint32(-1));

	return IntAddSubTree(pVariant, newName, uSubTreeType);
}

CCypherVariant* CCypherVariantHolder::GetMember( const char* szPathName )const
{
	if( ! szPathName )
		GenErr("CCypherVariantHolder::GetMember: Invalid nember pathname!");

	string sPathName = szPathName;

	char* szBase = const_cast<char*>( sPathName.c_str() );

	char* szNodeName = szBase;

	CCypherVariant* pVariant = m_pRootVariant;

	for( char* szPos = szBase; *szPos != '\0' ; ++szPos )
	{
		if( *szPos != '/' )
			continue;

		*szPos = '\0';

		pVariant = pVariant->GetMember( szNodeName );
		if( !pVariant )
		{
			ostringstream strm;
			string sErrPath( szPathName, szPos- szBase );
			strm<<"CCypherVariantHolder::GetMember: Can't get memeber \""<<sErrPath<<"\"!";
			GenErr( strm.str() );			
		}

		szNodeName = szPos+1;
	}

	pVariant = pVariant->GetMember( szNodeName );
	if( !pVariant )
	{
		ostringstream strm;
		strm<<"CCypherVariantHolder::GetMember:Can't get memeber "<<szPathName<<"!";
		GenErr( strm.str() );			
	}


	return pVariant;
}

CCypherVariant* CCypherVariantHolder::GetMemberNoExp( const char* szPathName )const
{
	if( ! szPathName )
		GenErr("CCypherVariantHolder::GetMember: Invalid nember pathname!");

	string sPathName = szPathName;
	char* szBase = const_cast<char*>( sPathName.c_str() );
	char* szNodeName = szBase;
	CCypherVariant* pVariant = m_pRootVariant;

	for( char* szPos = szBase; *szPos != '\0' ; ++szPos )
	{
		if( *szPos != '/' )
			continue;
		*szPos = '\0';
		pVariant = pVariant->GetMember( szNodeName );
		if( !pVariant )
		{
			return NULL;
		}

		szNodeName = szPos+1;
	}

	pVariant = pVariant->GetMember( szNodeName );
	if( !pVariant )
	{
		return NULL;
	}
	return pVariant;
}

CCypherVariant* CCypherVariantHolder::IntSetMember( CCypherVariant* pVariant, const char* szNodeName, CBaseCypherVariantInputStream* pStream )
{
	return pVariant->SetMember( szNodeName, pStream );
}

CCypherVariant* CCypherVariantHolder::SetMember( const char* szPathName, CBaseCypherVariantInputStream* pStream )
{
	if( ! szPathName )
		GenErr("CCypherVariantHolder::SetMember:Invalid nember pathname!");

	string sPathName = szPathName;

	char* szBase = const_cast<char*>( sPathName.c_str() );

	char* szNodeName = szBase;

	CCypherVariant* pVariant = m_pRootVariant;

	for( char* szPos = szBase; *szPos != '\0' ; ++szPos )
	{
		if( *szPos != '/' )
			continue;

		*szPos = '\0';

		CCypherVariant * pMember= pVariant->GetMember( szNodeName );

		if( !pMember )
		{
			pVariant = IntSetMember(pVariant, szNodeName, pStream);
		}
		else
		{
			pVariant = pMember;
			if(pStream)
				pVariant->WriteHeaderAndPath( *pStream, eCVA_Set_Table );
		}
			
		szNodeName = szPos+1;
	}

	pVariant = IntSetMember(pVariant, szNodeName, pStream);

	return pVariant;
}

bool CCypherVariantHolder::DelMember( const char* szPathName,CBaseCypherVariantInputStream* pStream )
{
	if( ! szPathName )
		GenErr("CCypherVariantHolder::DelMember:Invalid nember pathname!");

	string sPathName = szPathName;

	char* szBase = const_cast<char*>( sPathName.c_str() );

	char* szNodeName = szBase;

	CCypherVariant* pVariant = m_pRootVariant;

	for( char* szPos = szBase; *szPos != '\0' ; ++szPos )
	{
		if( *szPos != '/' )
			continue;

		*szPos = '\0';

		CCypherVariant * pMember= pVariant->GetMember( szNodeName );

		if( !pMember )
		{
			ostringstream strm;
			string sErrPath( szPathName, szPos- szBase );
			strm<<"CCypherVariantHolder::DelMember:Can't get memeber "<<sErrPath<<"!";
			GenErr( strm.str() );
		}
		else
		{
			pVariant = pMember;
		}

		szNodeName = szPos+1;
	}
	return pVariant->DelMember( szNodeName  , pStream );
}

const char* CCypherVariantHolder::GetString( const char* szPathName )const
{
	CCypherVariant* pVariant = GetMember( szPathName );

	return pVariant->GetString();
}

CCypherVariant* CCypherVariantHolder::SetString( const char* szPathName, const char* szValue, CBaseCypherVariantInputStream* pStream )
{
	CCypherVariant* pVariant = SetMember( szPathName, pStream );
	pVariant->SetString( szValue, pStream );
	return pVariant;
}

template<typename ValueType>
ValueType CCypherVariantHolder::GetNumber( const char* szPathName ) const
{
	CCypherVariant* pVariant = GetMember( szPathName );
	return pVariant->GetNumber<ValueType>();
}

template<typename ValueType>
CCypherVariant* CCypherVariantHolder::SetNumber( const char* szPathName, ValueType Value,CBaseCypherVariantInputStream* pStream )
{
	CCypherVariant* pVariant = SetMember( szPathName, pStream );
	pVariant->SetNumber<ValueType>( Value, pStream );
	return pVariant;
}

void CCypherVariantHolder::SetDynRoot( const char* szPathName, CBaseCypherVariantInputStream* pStream)
{
	CCypherVariant* pVariant = SetMember( szPathName, pStream);
	pVariant->SetEmptyTable(pStream);
	//m_pDynCodeRoot = const_cast<CBaseCypherVariantCode*>(pVariant->GetCode());
}

void CCypherVariantHolder::SetEmptyTable( const char* szPathName, CBaseCypherVariantInputStream* pStream )
{
	CCypherVariant* pVariant = SetMember( szPathName, pStream );
	pVariant->SetEmptyTable( pStream );
}

bool CCypherVariantHolder::IsEmptyTable( const char* szPathName )const
{
	CCypherVariant* pVariant = GetMember( szPathName );
	return pVariant->IsEmptyTable();
}

bool CCypherVariantHolder::EnumMember(CCypherVariant* pVar, void* ptr,bool(*CallBack)(const void* pContext,const char*,CCypherVariant&))const
{
	if (!pVar->GetMemberMap())
		return true;
	CCypherVariant::MemberMap_t& theMap=*pVar->GetMemberMap();
	
	CCypherVariant::MemberMap_t::iterator it=theMap.begin();
	CCypherVariant::MemberMap_t::iterator itEnd=theMap.end();
	
	for( ;it!=itEnd ; ++it ){
		CCypherVariant* pVariant = *it;
		if (!pVariant)
			continue;
		if (pVariant->GetType() == eVT_Table && pVariant->GetMemberMap())
			EnumMember(pVariant, ptr, CallBack);
		
		if( !CallBack( ptr, pVariant->GetCode()->GetName(),*pVariant ) )
		{
			return false;
		}
	}

	return true;
}

bool CCypherVariantHolder::EnumMember(const char* path,void* ptr,bool(*CallBack)(const void* pContext,const char*,CCypherVariant&))const
{
	return EnumMember(GetMember(path), ptr, CallBack);
}

void CCypherVariantHolder::GetNameFromIndex(string& strSubTreeRootName, uint32 uSubTreeRootCodeIndex)const
{
	strSubTreeRootName.clear();
	strSubTreeRootName.append("SubTree");
	Ast(uSubTreeRootCodeIndex < 1000);
	char szIndex[4];
#ifdef _WIN32
	_snprintf(szIndex, 3, "%d", uSubTreeRootCodeIndex);
#else
	snprintf(szIndex, 3, "%d", uSubTreeRootCodeIndex);
#endif
	strSubTreeRootName.append(szIndex);
	//cout << "GetNameFromIndex " << uSubTreeRootCodeIndex << " " << strSubTreeRootName << endl;
}

void CCypherVariantHolder::OnMemberAdded(CCypherVariant* pVariant)
{
}

CCypherVariantHolderForDecode::CCypherVariantHolderForDecode(CCypherVariantCodeTable* pCodeTable)
:CCypherVariantHolder(pCodeTable)
,m_pHandler(NULL)
{
	m_VariantContainer.resize(m_pCodeTable->m_IDMap.size());
}

CCypherVariantHolderForDecode::~CCypherVariantHolderForDecode()
{
}

void CCypherVariantHolderForDecode::OnMemberAdded(CCypherVariant* pVariant)
{
	Ast(pVariant);

	if( GetHandler() )
		GetHandler()->OnMemberAdded( pVariant );
	
	const CBaseCypherVariantCode* pCode = pVariant->GetCode();
	if(!pCode->IsDynCode() || pCode->IsTreeRoot())
	{
		uint32 uID = pCode->GetID();
		if(uID >= m_VariantContainer.size())
		{
			m_VariantContainer.resize(uID+1, NULL);
		}
		Ast(m_VariantContainer[uID] == NULL || m_VariantContainer[uID] == pVariant);
		m_VariantContainer[uID] = pVariant;
	}
}

void CCypherVariantHolderForDecode::Decode(CBaseCypherVariantOutputStream& strm)
{
	CCVCFirstByte* Byte;

	//逐行解析
	for (;;)
	{	
		if( (size_t)strm.RemainCount()< sizeof(CCVCFirstByte) )
			return;
		strm.Read(Byte, sizeof(CCVCFirstByte) );

		if ( Byte->m_uAction == eCVA_Del_Member )
		{
			CCypherVariant* pVariant = NULL;
			GetDecodeTarget(strm, ECypherVariantPathType(Byte->m_uPathType), pVariant);
			//cout << "del mem " << pVariant->GetCode()->GetName() << endl;

			if( !pVariant )
			{
				GenErr("CCypherVariantHolder::Decode: eCVA_Del_Member failed");
			}

			if (GetHandler())
				GetHandler()->OnDeleteMember( pVariant );

			uint32 uMsgLen = 0;
			switch ( ECypherVariantPathType(Byte->m_uPathType) )
			{
			case eCVPT_Code8:
				uMsgLen = sizeof(bool);
				break;
			case eCVPT_Code16:
				uMsgLen = sizeof(uint16);
				break;
			case eCVPT_Code32:
			case eCVPT_Path:
				uMsgLen = sizeof(uint32);
				break;
			default:
				break;
			}
			LogVariantValueChanged(pVariant->GetName(), uMsgLen);
			const CBaseCypherVariantCode* pCode = pVariant->GetCode();
			if(!pCode->IsDynCode() || pCode->IsTreeRoot())
			{
				uint32 uID = pCode->GetID();
				Ast(uID < m_VariantContainer.size());
				Ast(m_VariantContainer[uID] == pVariant);
				m_VariantContainer[uID] = NULL;
			}

			const CBaseCypherVariantCode* pParentCode = pCode->GetParent();
			if(pParentCode->IsTreeRoot() && pParentCode->IsCloned())
			{
				pVariant->RecycleSubTree();
			}
			else
			{
				delete pVariant;
			}
		}
		else
		{
			CCypherVariant* pVariant = NULL;
			ECypherVariantPathType ePathType = ECypherVariantPathType(Byte->m_uPathType);
			uint32 uPath = GetDecodeTargetPathFromStrm(strm, ePathType);
			Ast(uPath != uint32(-1));

			if(uPath != 0)	//不是根路徑
			{
				if(ePathType == eCVPT_Path)
				{
					uint32 uSubTreeTransitID;
					uint32 uSubTreeIndex;
					uint32 uSubTreeType;
					uint32 uIndex;
					CCypherVariantDynCode::InterpretSubTreePath(uPath, uSubTreeTransitID, uSubTreeIndex, uSubTreeType, uIndex);
					CCypherVariant* pSubTreeTransit = m_VariantContainer[uSubTreeTransitID];
					if(!pSubTreeTransit)
					{
						const CBaseCypherVariantCode *pSubTreeTransitCode = m_pCodeTable->GetEntity(uSubTreeTransitID);
						const CBaseCypherVariantCode *pParentCode = pSubTreeTransitCode->GetParent();
						CCypherVariant* pParent = m_VariantContainer[pParentCode->GetID()];
						pSubTreeTransit = pParent->SetMemberForDecode(const_cast<CBaseCypherVariantCode*>(pSubTreeTransitCode), NULL);
						OnMemberAdded(pSubTreeTransit);
					}

					pVariant = pSubTreeTransit->GetMemberByIndex(uSubTreeIndex);

					if(!pVariant)
					{
						pVariant = IntAddSubTreeForDecode(pSubTreeTransit,uSubTreeType,uSubTreeIndex);
					}

					if(uIndex != 0)
					{
						pVariant = pVariant->GetMemberByIndex(uIndex - 1);
						if(!pVariant)
						{
							ostringstream oss;
							oss << "找不到SubTree子节点 " << uIndex << endl;
							GenErr(oss.str().c_str());
						}
					}
				}
				else
				{
					if(uPath < m_VariantContainer.size())
					{
						pVariant = m_VariantContainer[uPath];
					}

					if(!pVariant)
					{
						CBaseCypherVariantCode *pCode = m_pCodeTable->GetEntity(uPath);
						if (pCode == NULL)
						{
							ostringstream oss;
							oss << "CodeTable中找不到ID为" << uPath << "的记录";
							GenErr(oss.str().c_str());
						}
						CBaseCypherVariantCode* pParentCode = pCode->GetParent();
						CCypherVariant* pParent = m_VariantContainer[pParentCode->GetID()];
						if( !pParent )
						{
							ostringstream strm;
							strm<<"CCypherVariantHolder::Decode: Setting child member failed, the parent member of \""<<pCode->GetName()<<"\" not exist!" << " Byte.m_uAction = " << static_cast<uint32>(Byte->m_uAction);
							GenErr(strm.str());
						}
						pVariant = pParent->SetMemberForDecode( pCode, NULL );
						OnMemberAdded(pVariant);
					}
				}
				uint32 uMsgLen = 0;
				switch ( ECypherVariantPathType(Byte->m_uPathType) )
				{
				case eCVPT_Code8:
					uMsgLen = sizeof(bool);
					break;
				case eCVPT_Code16:
					uMsgLen = sizeof(uint16);
					break;
				case eCVPT_Code32:
				case eCVPT_Path:
					uMsgLen = sizeof(uint32);
					break;
				default:
					break;
				}
				LogVariantValueChanged(pVariant->GetName(), uMsgLen);
			}
			else
			{
				//根路徑
				pVariant=IntGetRootVariant();
			}
			#ifdef __JIABEN_DEBUG
			cerr << static_cast<CCypherVariant*>(pVariant)->GetName() << " " << (uint32)Byte->m_uAction << endl;
			#endif
			//cout << "action " << uint32(Byte.m_uAction) << " " << pVariant->GetCode()->GetName() << endl;
			switch ( Byte->m_uAction )
			{
			case eCVA_Set_Bool:
				pVariant->SetNumber(ReadNumber<bool>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(bool));
				break;
			case eCVA_Set_Int8:
				pVariant->SetNumber(ReadNumber<int8>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(int8));
				break;
			case eCVA_Set_Uint8:
				pVariant->SetNumber(ReadNumber<uint8>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(uint8));
				break;
			case eCVA_Set_Int16:
				pVariant->SetNumber(ReadNumber<int16>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(int16));
				break;
			case eCVA_Set_Uint16:
				pVariant->SetNumber(ReadNumber<uint16>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(uint16));
				break;
			case eCVA_Set_Int32:
				pVariant->SetNumber(ReadNumber<int32>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(int32));
				break;
			case eCVA_Set_Uint32:
				pVariant->SetNumber(ReadNumber<uint32>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(uint32));
				break;
			case eCVA_Set_Int64:
				pVariant->SetNumber(ReadNumber<int64>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(int64));
				break;
			case eCVA_Set_Uint64:
				pVariant->SetNumber(ReadNumber<uint64>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(uint64));
				break;
			case eCVA_Set_Float:
				pVariant->SetNumber(ReadNumber<float>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(float));
				break;
			case eCVA_Set_Double:
				pVariant->SetNumber(ReadNumber<double>(strm));
				CallValueChangedCallback( pVariant );
				LogVariantValueChanged(pVariant->GetName(), sizeof(double));
				break;
			case eCVA_Set_String:
				{
					string str;
					//getline(strm, str, '\0');
					strm.GetLine(str);
					pVariant->SetString(str.c_str());
					CallValueChangedCallback( pVariant );
					LogVariantValueChanged(pVariant->GetName(), (uint32)str.size());
				}
				break;
			case eCVA_Set_Table:
				pVariant->m_bDataNotInit = false;
				break;
			case eCVA_Clr_Table:
				pVariant->SetEmptyTableForDecode(NULL);
				if (GetHandler())
					GetHandler()->OnClearTable( pVariant );
				LogVariantValueChanged(pVariant->GetName(), 0);
				break;
			default:		
				{
					ostringstream strm;
					strm<<"Invalid Variant type:"<<uint32(Byte->m_uAction);
					GenErr(strm.str());
				}
			}

		}
	}
}

void CCypherVariantHolderForDecode::LogVariantValueChanged(const char* szPathName, uint32 uMsgLen)
{
}

uint32 CCypherVariantHolderForDecode::GetDecodeTargetPathFromStrm(CBaseCypherVariantOutputStream& strm, ECypherVariantPathType eType)const
{
	uint32 uPath = uint32(-1);
	switch ( eType )
	{
	case eCVPT_Code8:
		uPath = ReadNumber<uint8>(strm);
		break;
	case eCVPT_Code16:
		uPath = ReadNumber<uint16>(strm);
		break;
	case eCVPT_Code32:
	case eCVPT_Path:
		uPath = ReadNumber<uint32>(strm);
		break;
	default:
		{
			ostringstream strm;
			strm<<"Invalid Variant type:"<<eType;
			GenErr(strm.str());
		}
		break;
	}
	return uPath;
}

void CCypherVariantHolderForDecode::GetDecodeTarget(CBaseCypherVariantOutputStream& strm, ECypherVariantPathType eType,CCypherVariant*& pVariant)const
{
	switch ( eType )
	{
	case eCVPT_Code8:
		pVariant = m_VariantContainer[ReadNumber<uint8>(strm)];
		break;
	case eCVPT_Code16:
		pVariant = m_VariantContainer[ReadNumber<uint16>(strm)];
		break;
	case eCVPT_Code32:
		pVariant = m_VariantContainer[ReadNumber<uint32>(strm)];
		break;
	case eCVPT_Path:
		{
			uint32 uFullPath = ReadNumber<uint32>(strm);
			uint32 uParentID;
			uint32 uSubTreeIndex;
			uint32 uSubTreeType;
			uint32 uIndex;
			CCypherVariantDynCode::InterpretSubTreePath(uFullPath, uParentID, uSubTreeIndex, uSubTreeType, uIndex);
			pVariant = m_VariantContainer[uParentID];
			pVariant = pVariant->IntGetMemberByIndex(uSubTreeIndex);

			if(uIndex)
			{
				pVariant = pVariant->IntGetMemberByIndex(uIndex);
			}
		}
		break;
	default:
		{
			ostringstream strm;
			strm<<"Invalid Variant type:"<<eType;
			GenErr(strm.str());
		}
		break;
	}
}

void CCypherVariantHolderForDecode::GetPathElemsById( uint32 uId , PathContainer& PathElems ) const
{
	CBaseCypherVariantCode *pCode = m_pCodeTable->GetEntity( uId );
	if (pCode == NULL)
	{
		ostringstream oss;
		oss << "CodeTable中找不到ID为" << uId << "的记录";
		GenErr(oss.str().c_str());
	}
	pCode->GetPath( PathElems );
}

CCypherVariant* CCypherVariantHolderForDecode::IntAddSubTreeForDecode
(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex)
{
	string strNodeName;
	GetNameFromIndex(strNodeName, uSubTreeIndex);
	return BuildSubTree(pRoot, uSubTreeType, uSubTreeIndex, strNodeName.c_str());
}

void CCypherVariantHolderForDecode::InitVariantContainer()
{
	CCypherVariant* pRootVariant = IntGetRootVariant();
	Ast(pRootVariant);
	struct CVariantContainerInitializer
	{
		static bool Init(const void* pHolder, const char* pVariantCodeName, CCypherVariant& variant)
		{
			Ast(pHolder);
			const CBaseCypherVariantCode* pCode = variant.GetCode();
			reinterpret_cast<CCypherVariantHolderForDecode*>(const_cast<void*>(pHolder))->m_VariantContainer[pCode->GetID()] = &variant;
			return true;
		}
	};
	EnumMember(pRootVariant, this, &CVariantContainerInitializer::Init);
}

void CCypherVariantHolderForDecode::CallValueChangedCallback( CCypherVariant* pVariant )
{
	if ( !GetHandler() )
		return;
	GetHandler()->OnValueChanged( pVariant);
}

template<typename ValueType>
ValueType CCypherVariantHolderForDecode::ReadNumber(CBaseCypherVariantOutputStream& strm)const
{
	ValueType* Number;
	if( (size_t)strm.RemainCount() < sizeof(ValueType) )
		GenErr("Unexpected eos when reading variant number value!");
	//strm.read(  reinterpret_cast<char*>(&Number) , sizeof(ValueType) );
	strm.Read( Number, sizeof(ValueType) );
	return *Number;
}

CCypherVariant* CCypherVariantHolderForDecode::GetMemberParent( PathContainer& PathElems )
{
	Ast( !PathElems.empty() );
	CCypherVariant *pVariant = IntGetRootVariant();
	PathContainer::iterator itEnd = --PathElems.end();

	for (PathContainer::iterator it = PathElems.begin(); it != itEnd; ++it)
		pVariant = pVariant->GetMember(it->c_str());

	return pVariant;
}

CCypherVariant* CCypherVariantHolderForDecode::IntSetMember( CCypherVariant* pVariant, const char* szNodeName, CBaseCypherVariantInputStream* pStream )
{
	return pVariant = pVariant->SetMemberForDecode( szNodeName, pStream );
}

template CCypherVariant* CCypherVariantHolder::SetNumber<bool>(const char*,bool Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<int8>(const char*,int8 Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<uint8>(const char*,uint8 Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<int16>(const char*,int16 Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<uint16>(const char*,uint16 Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<int32>(const char*,int32 Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<uint32>(const char*,uint32 Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<int64>(const char*,int64 Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<uint64>(const char*,uint64 Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<float>(const char*,float Value,CBaseCypherVariantInputStream*);
template CCypherVariant* CCypherVariantHolder::SetNumber<double>(const char*,double Value,CBaseCypherVariantInputStream*);

template bool CCypherVariantHolder::GetNumber<bool>(const char*)const;
template int8 CCypherVariantHolder::GetNumber<int8>(const char*)const;
template uint8 CCypherVariantHolder::GetNumber<uint8>(const char*)const;
template int16 CCypherVariantHolder::GetNumber<int16>(const char*)const;
template uint16 CCypherVariantHolder::GetNumber<uint16>(const char*)const;
template int32 CCypherVariantHolder::GetNumber<int32>(const char*)const;
template uint32 CCypherVariantHolder::GetNumber<uint32>(const char*)const;
template int64 CCypherVariantHolder::GetNumber<int64>(const char*)const;
template uint64 CCypherVariantHolder::GetNumber<uint64>(const char*)const;
template float CCypherVariantHolder::GetNumber<float>(const char*)const;
template double CCypherVariantHolder::GetNumber<double>(const char*)const;
