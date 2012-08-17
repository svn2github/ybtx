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

	void	SetName(const TCHAR* name);
	string	GetName();
	void	SetScaling(float scaling);
	float	GetScaling();
	void	SetAniFileName(const TCHAR* name);
	string	GetAniFileName();
	//string	
	void	AddModelPiece(CModelPiece*	pModelPiece);
	string  GetBodyModFileName();
	void    SetModPiece(CModelPiece*	pModelPiece);
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
	void	SetIndexName(const TCHAR* name);
	string	GetIndexName();
	void	SetModelFileName(const TCHAR* name);
	string	GetModelFileName();
	void	SetPieceName(const TCHAR* pieceName);
	string  GetPieceName();;
	void    SetRenderStyle(const TCHAR* style);
	string  GetRenderStyle();
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
	CPlayerModelPart(void);
	virtual ~CPlayerModelPart();

	void	SetName(const TCHAR* name);
	string	GetName();
	void	SetModelFileName(const TCHAR* name);
	string	GetModelFileName();
	string	GetModelFileName(string strType);
	void	SetAniFileName(const TCHAR* name);
	string	GetAniFileName();
	void	SetEffectFileName(const TCHAR* name);
	string	GetEffectFileName();
	void	SetPieceName(const TCHAR* pieceName);
	string  GetPieceName();
	void    SetRenderStyle(const TCHAR* style);
	string  GetRenderStyle();
	void    SetHumanMaleHidePiece(const TCHAR* name);
	string  GetHumanMaleHidePiece();
	void    SetHumanFeMaleHidePiece(const TCHAR* name);
	string  GetHumanFeMaleHidePiece();
	void    SetDwarfHidePiece(const TCHAR* name);
	string  GetDwarfHidePiece();
	void    SetElfHidePiece(const TCHAR* name);
	string  GetElfHidePiece();
	void    SetOrcHidePiece(const TCHAR* name);
	string  GetOrcHidePiece();
	void    SetDynamic(const TCHAR* name);
	string  GetDynamic();
	void    SetTransform(const TCHAR* name);
	string  GetTransform();
	void    SetEveryModelFileName(const string& strType,const string& strModel);
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
	map<string,string>		m_mapModFile;
};


