#include "stdafx.h"
#include "CCharacter.h"
#include "BaseHelper.h"
#include "StringHelper.h"

void CCharacter::AddModelPiece(CModelPiece* pModelPiece)
{
	if(!pModelPiece)
		return;
	m_mapModelPiece.insert(make_pair(pModelPiece->GetIndexName(),pModelPiece));
	AdvAddRef(pModelPiece);
}

// CModelPiece* CCharacter::GetModelPiece(string PieceIndexName)
// {
// 	MODELPIECE_MAP::iterator itor = m_mapModelPiece.find(PieceIndexName);
// 	if(itor != m_mapModelPiece.end())
// 	{
// 		return itor->second;
// 	}
// 	return NULL;
// }

void CModelPiece::AddHidePiece(const TCHAR *pieceName)
{
	string strHide = pieceName;
	replace(strHide,"\"","");
	m_vHidePiece.push_back(pieceName);
}

string CCharacter::GetBodyModFileName()
{
	if (m_BodyModelPiece)
		return m_BodyModelPiece->GetModelFileName();
	else
		return "";
}

void	CPlayerModelPart::SetModelFileName(const TCHAR* name)
{
	string strTemp = name;
	int pos = strTemp.find(',');
	m_mapModFile["人类男"] = strTemp.substr(0,pos);
	int secondPos = strTemp.find(',',pos+1);
	m_mapModFile["人类女"] = strTemp.substr(pos+1,secondPos-pos-1);
	m_mapModFile["兽人男"] = strTemp.substr(secondPos+1,strTemp.length()-secondPos-1);
}

void CPlayerModelPart::SetEveryModelFileName(const string& strType,const string& strModel)
{
	m_mapModFile[strType] = strModel;
}

string	CPlayerModelPart::GetModelFileName(string strType)
{
	if (!m_mapModFile.count(strType))
		return "";
	else
		return m_mapModFile[strType];
}
