#pragma once
#include "CDynamicObject.h"

class CModelPiece;
using namespace sqr;
typedef	 std::map<string,CModelPiece*>		MODELPIECE_MAP;
typedef	 std::vector<string>				HIDEPIECE_VECTOR;
class CCharacter
	:public virtual CDynamicObject
{
	friend class CDataCache;
public:
	CCharacter();

	void	SetName(const TCHAR* name)		{m_sName = name;};
	string	GetName()						{return m_sName;};
	void	SetScaling(float scaling)		{m_fScaling = scaling;};
	float	GetScaling()					{return m_fScaling;};
	void	SetAniFileName(const TCHAR* name){m_sBAniFileName = name;};
	string	GetAniFileName()				{return m_sBAniFileName;}
	//string	
	void	AddModelPiece(CModelPiece*	pModelPiece);
	//CModelPiece*	GetModelPiece(string PieceIndexName);	
	string GetBodyModFileName();
	void SetModPiece(CModelPiece*	pModelPiece)   { m_BodyModelPiece = pModelPiece;}
private:
	string					m_sName;
	float					m_fScaling;
	string					m_sBAniFileName;
	//string					m_sGAniFileName;
	MODELPIECE_MAP			m_mapModelPiece;
	CModelPiece*				m_BodyModelPiece;
};

class CModelPiece
	:public virtual CDynamicObject
{

public:
	void	SetIndexName(const TCHAR* name)		{m_sIndexName = name;};
	string	GetIndexName()						{return m_sIndexName;};
	void	SetModelFileName(const TCHAR* name)	{m_sModelFile = name;};
	string	GetModelFileName()					{return m_sModelFile;};
	void	SetPieceName(const TCHAR* pieceName)	
	{
		if(pieceName)
			m_sPieceName = pieceName;
	};
	string  GetPieceName()						{return m_sPieceName;};
	void    SetRenderStyle(const TCHAR* style)	{m_sRenderStyle = style;};
	string  GetRenderStyle()					{return m_sRenderStyle;};
	void	AddHidePiece(const TCHAR* pieceName);
private:
	string					m_sIndexName;
	string					m_sModelFile;
	string					m_sPieceName;
	string					m_sRenderStyle;
	HIDEPIECE_VECTOR		m_vHidePiece;
};

class CPlayerModelPart
	:public virtual CDynamicObject
{
public:
	void	SetName(const TCHAR* name)		{m_strName = name;}
	string	GetName()						{return m_strName;}
	void	SetModelFileName(const TCHAR* name);
	string	GetModelFileName()					{return m_strModelFile;}
	string	GetModelFileName(string strType);
	void	SetAniFileName(const TCHAR* name)	{m_strAniFile = name;}
	string	GetAniFileName()					{return m_strAniFile;}
	void	SetEffectFileName(const TCHAR* name)	{m_strEffectFile = name;}
	string	GetEffectFileName()					{return m_strEffectFile;}
	void	SetPieceName(const TCHAR* pieceName)	
	{
		if(pieceName)
			m_strPieceName = pieceName;
	};
	string  GetPieceName()						{return m_strPieceName;}
	void    SetRenderStyle(const TCHAR* style)	{m_strRenderStyle = style;}
	string  GetRenderStyle()					{return m_strRenderStyle;}
	void    SetHumanMaleHidePiece(const TCHAR* name)	{m_strHumanMaleHidePiece = name;}
	string  GetHumanMaleHidePiece()					{return m_strHumanMaleHidePiece;}
	void    SetHumanFeMaleHidePiece(const TCHAR* name)	{m_strHumanFeMaleHidePiece = name;}
	string  GetHumanFeMaleHidePiece()					{return m_strHumanFeMaleHidePiece;}
	void    SetDwarfHidePiece(const TCHAR* name)	{m_strDwarfHidePiece = name;}
	string  GetDwarfHidePiece()					{return m_strDwarfHidePiece;}
	void    SetElfHidePiece(const TCHAR* name)	{m_strElfHidePiece = name;}
	string  GetElfHidePiece()					{return m_strElfHidePiece;}
	void    SetOrcHidePiece(const TCHAR* name)	{m_strOrcHidePiece = name;}
	string  GetOrcHidePiece()					{return m_strOrcHidePiece;}
	void    SetDynamic(const TCHAR* name)	{m_strDynamic = name;}
	string  GetDynamic()					{return m_strDynamic;}
	void    SetTransform(const TCHAR* name)	{m_strTransform = name;}
	string  GetTransform()					{return m_strTransform;}
	void   SetEveryModelFileName(const string& strType,const string& strModel);
private:
	string					m_strName;
	string					m_strModelFile;
	string					m_strAniFile;
	string					m_strEffectFile;
	string					m_strPieceName;
	string					m_strRenderStyle;
	string					m_strHumanMaleHidePiece;
	string					m_strHumanFeMaleHidePiece;
	string					m_strDwarfHidePiece;
	string					m_strElfHidePiece;
	string					m_strOrcHidePiece;
	string					m_strDynamic;
	string					m_strTransform;
	map<string,string> m_mapModFile;
};
